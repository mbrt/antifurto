#include <stdexcept>
#include <cassert>
#include <functional>
#include <thread>

#include "CvCamera.hpp"
#include "MotionDetector.hpp"
#include "PictureArchive.hpp"
#include "RecordingController.hpp"
#include "Config.hpp"
#include "concurrency/TaskScheduler.hpp"
#include "concurrency/TimeUtility.hpp"
#include "text/ToString.hpp"

#define BOOST_TEST_MODULE unit
#include <boost/test/unit_test.hpp>

using namespace antifurto;


struct test_error : public std::runtime_error
{ using std::runtime_error::runtime_error; };

cv::Mat makeBlackImg()
{
    return { cv::Mat::zeros(10, 10, CV_8U) };
}

cv::Mat makeWhiteImg()
{
    cv::Mat result = makeBlackImg();
    cv::bitwise_not(result, result);
    return result;
}


BOOST_AUTO_TEST_CASE(cameraTakePicture)
{
    Picture p;
    CvCamera camera;
    camera.takePicture(p);
}

BOOST_AUTO_TEST_CASE(pictureRefcount)
{
    Picture z { makeBlackImg() };
    Picture o { makeWhiteImg() };

    auto checkRefcount = [](cv::Mat const& m) { BOOST_CHECK_EQUAL(*m.refcount, 1); };
    auto checkEqual = [](cv::Mat const& a, cv::Mat const& b) {
        BOOST_CHECK(std::equal(a.begin<uchar>(), a.end<uchar>(), b.begin<uchar>()));
    };
    auto checkNotEqual = [](cv::Mat const& a, cv::Mat const& b) {
        BOOST_CHECK(!std::equal(a.begin<uchar>(), a.end<uchar>(), b.begin<uchar>()));
    };

    // test value semantics
    Picture def;
    Picture r1 = z;
    Picture r2 = z;

    checkRefcount(z);
    checkRefcount(r1);
    checkRefcount(r2);

    checkEqual(r1, r2);
    checkEqual(z, r1);

    checkNotEqual(z, o);
    checkNotEqual(z, o);
    checkNotEqual(r1, o);

    r2 = o;
    checkEqual(r2, o);
    checkNotEqual(r2, z);

    checkRefcount(o);
}


struct MotionDetectorChecker
{
    MotionDetector::State expected;
    bool called = false;

    void setExpected(MotionDetector::State newExpected)
    {
        called = false;
        expected = newExpected;
    }

    void onStateChange(MotionDetector::State s)
    {
        BOOST_CHECK_EQUAL(s, expected);
        called = true;
    }
};

BOOST_AUTO_TEST_CASE(motionDetection)
{
    using std::placeholders::_1;
    using State = MotionDetector::State;

    Picture w { makeWhiteImg() };
    Picture b { makeBlackImg() };
    MotionDetector detector(3, 4);
    MotionDetectorChecker checker;
    detector.addObserver(std::bind(&MotionDetectorChecker::onStateChange, &checker, _1));

    for (uchar a: w)
        BOOST_CHECK_EQUAL(a, 255);
    for (uchar a: b)
        BOOST_CHECK_EQUAL(a, 0);

    // initialize
    for (int i = 0; i < 5; ++i)
        detector.examinePicture(w);
    BOOST_CHECK_EQUAL(checker.called, false);

    // pre alarm after one different frame
    checker.setExpected(State::PRE_ALARM);
    detector.examinePicture(b); // the first difference is discarded by the algorithm
    BOOST_CHECK_EQUAL(checker.called, false);
    detector.examinePicture(w);
    BOOST_CHECK_EQUAL(checker.called, true);

    // no alarm after one not different
    checker.setExpected(State::NO_ALARM);
    detector.examinePicture(w);
    BOOST_CHECK_EQUAL(checker.called, true);

    // pre alarm after one other different frame
    checker.setExpected(State::PRE_ALARM);
    detector.examinePicture(b);
    detector.examinePicture(w);
    BOOST_CHECK_EQUAL(checker.called, true);

    // alarm after 3 different frames
    checker.setExpected(State::ALARM);
    detector.examinePicture(b);
    BOOST_CHECK_EQUAL(checker.called, false);
    detector.examinePicture(w);
    BOOST_CHECK_EQUAL(checker.called, true);

    // no alarm after 4 equal frames
    checker.setExpected(State::NO_ALARM);
    detector.examinePicture(w);
    detector.examinePicture(w);
    detector.examinePicture(w);
    BOOST_CHECK_EQUAL(checker.called, false);
    detector.examinePicture(w);
    BOOST_CHECK_EQUAL(checker.called, true);
}

BOOST_AUTO_TEST_CASE(pictureArchive)
{
    PictureArchive archive("/tmp", 3);
    CvCamera camera;
    Picture p;
    for (int i = 0; i < 5; ++i)
    {
        camera.takePicture(p);
        archive.addPicture(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    archive.startSaving();
    camera.takePicture(p);
    archive.addPicture(p);
    archive.stopSaving();
}

BOOST_AUTO_TEST_CASE(recordingController)
{
    MotionDetector detector;
    Configuration cfg;
    concurrency::TaskScheduler scheduler;
    RecordingController controller{ cfg, detector, scheduler };
}

BOOST_AUTO_TEST_CASE(taskScheduler)
{
    using namespace std::chrono;

    int counter = 0;
    {
        concurrency::TaskScheduler sched;
        sched.scheduleEvery(seconds(1), [&]{ std::cout << "tick\n"; ++counter; });
        std::this_thread::sleep_for(seconds(5) + milliseconds(10));
    }
    BOOST_CHECK_EQUAL(counter, 5);

    counter = 0;
    {
        concurrency::TaskScheduler sched;
        sched.scheduleEvery(seconds(1), [&]{
            std::this_thread::sleep_for(seconds(1) + milliseconds(500));
            std::cout << "tick\n";
            ++counter; });
        std::this_thread::sleep_for(seconds(3) + milliseconds(10));
    }
    BOOST_CHECK_EQUAL(counter, 2);

    counter = 0;
    {
        concurrency::TaskScheduler sched;
        sched.scheduleAfter(seconds(1), [&]{
            counter = 1;
        });
        std::this_thread::sleep_for(seconds(1) - milliseconds(30));
        BOOST_CHECK_EQUAL(counter, 0);
        std::this_thread::sleep_for(milliseconds(35));
        BOOST_CHECK_EQUAL(counter, 1);
    }

    counter = 0;
    {
        concurrency::TaskScheduler sched;
        sched.scheduleAfter(seconds(2), [&]{
            std::cout << 2 << std::endl;
            counter = 2;
        });
        sched.scheduleAfter(seconds(1), [&]{
            std::cout << 1 << std::endl;
            counter = 1;
        });
        BOOST_CHECK_EQUAL(counter, 0);
        std::this_thread::sleep_for(seconds(1) + milliseconds(50));
        BOOST_CHECK_EQUAL(counter, 1);
        std::this_thread::sleep_for(seconds(1) + milliseconds(50));
        BOOST_CHECK_EQUAL(counter, 2);
    }
}

BOOST_AUTO_TEST_CASE(timeOps)
{
    std::cout << "Now: " << text::toString(std::chrono::system_clock::now())
              << "\nTomorrow: " << text::toString(concurrency::tomorrow())
              << std::endl;
}

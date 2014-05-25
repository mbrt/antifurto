#include <stdexcept>
#include <cassert>
#include <chrono>
#include <thread>

#include "CvCamera.hpp"
#include "MotionDetector.hpp"

#define BOOST_TEST_MODULE unit
#include <boost/test/unit_test.hpp>

struct test_error : public std::runtime_error
{ using std::runtime_error::runtime_error; };


BOOST_AUTO_TEST_CASE(cameraTakePicture)
{
    antifurto::Picture p;
    antifurto::CvCamera camera;
    camera.takePicture(p);
}

BOOST_AUTO_TEST_CASE(pictureRefcount)
{
    antifurto::Picture z { cv::Mat(cv::Mat::zeros(10, 10, CV_8U)) };
    antifurto::Picture o { cv::Mat(cv::Mat::ones(10, 10, CV_8U)) };

    auto checkRefcount = [](cv::Mat const& m) { BOOST_CHECK_EQUAL(*m.refcount, 1); };
    auto checkEqual = [](cv::Mat const& a, cv::Mat const& b) {
        BOOST_CHECK(std::equal(a.begin<uchar>(), a.end<uchar>(), b.begin<uchar>()));
    };
    auto checkNotEqual = [](cv::Mat const& a, cv::Mat const& b) {
        BOOST_CHECK(!std::equal(a.begin<uchar>(), a.end<uchar>(), b.begin<uchar>()));
    };

    // test value semantics
    antifurto::Picture def;
    antifurto::Picture r1 = z;
    antifurto::Picture r2 = z;

    checkRefcount(z);
    checkRefcount(r1);
    checkRefcount(r2);

    checkEqual(r1, r2);
    checkEqual(z, r1);

    checkNotEqual(z, o);
    checkNotEqual(r1, o);

    r2 = o;
    checkEqual(r2, o);
    checkNotEqual(r2, z);

    checkRefcount(o);
}

void dumpState(antifurto::MotionDetector::State s)
{
    using State = antifurto::MotionDetector::State;

    char const* out;
    switch (s)
    {
    case State::ALARM:
        out = "Alarm\n";
        break;
    case State::NO_ALARM:
        out = "No alarm\n";
        break;
    case State::PRE_ALARM:
        out = "Pre alarm\n";
        break;
    default:
        out = "Unknown\n";
        break;
    }
    std::cout << out << std::endl;
}

BOOST_AUTO_TEST_CASE(motionDetection)
{
    antifurto::Picture p;
    antifurto::CvCamera camera;
    antifurto::MotionDetector detector;
    detector.AddObserver(&dumpState);
    for (int i = 0; i < 50; ++i)
    {
        camera.takePicture(p);
        detector.examinePicture(p);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

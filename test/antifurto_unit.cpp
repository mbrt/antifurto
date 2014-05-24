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

#include <stdexcept>
#include <cassert>

#include "CvCamera.hpp"

#define BOOST_TEST_MODULE unit
#include <boost/test/unit_test.hpp>

struct test_error : public std::runtime_error
{ using std::runtime_error::runtime_error; };


BOOST_AUTO_TEST_CASE(shouldPass)
{
    antifurto::Picture p;
    antifurto::CvCamera camera;
    camera.takePicture(p);
}

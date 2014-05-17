#define BOOST_TEST_MODULE cvtest
#include <boost/test/included/unit_test.hpp>
#include <stdexcept>
#include <cassert>

struct test_error : public std::runtime_error
{ using std::runtime_error::runtime_error; };


BOOST_AUTO_TEST_CASE(shouldPass)
{
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(shouldFail)
{
    BOOST_CHECK_EQUAL("one", "two");
}

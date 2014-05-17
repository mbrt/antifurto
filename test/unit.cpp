#include <stdexcept>
#include <cassert>

#define BOOST_TEST_MODULE unit
#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_CASE(shouldFail2)
{
    throw test_error("my unexpected error");
}

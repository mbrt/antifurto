#include "DefaultZmqSerializer.hpp"

#include <vector>
#include <algorithm>
#include <future>

#define BOOST_TEST_MODULE unit
#include <boost/test/unit_test.hpp>

using namespace antifurto::serialization;


BOOST_AUTO_TEST_CASE(serialize)
{
    DefaultZmqSerializer serializer;
    zmq::message_t msg1, msg2;
    serializer.serializeMessage(msg1, MessageType::LIVE_VIEW_REQUEST);
    BOOST_CHECK_GT(msg1.size(), msg2.size());
    MessageType t = serializer.deserializeHeader(msg1);
    BOOST_CHECK(t == MessageType::LIVE_VIEW_REQUEST);
}

BOOST_AUTO_TEST_CASE(send)
{
    const char* addr = "inproc://a";
    zmq::context_t ctx{0};
    zmq::socket_t srv{ctx, ZMQ_REP}, cli{ctx, ZMQ_REQ};
    srv.bind(addr);
    cli.connect(addr);

    DefaultZmqSerializer serializer;
    zmq::message_t msg1, msg2;
    serializer.serializeMessage(msg1, MessageType::LIVE_VIEW_REQUEST);
    auto swork = std::async(std::launch::async, [&]{
        cli.send(msg1);
    });
    srv.recv(&msg2);
    swork.get();

    BOOST_CHECK_EQUAL(msg1.size(), msg2.size());
    const char* m1begin = static_cast<const char*>(msg1.data());
    const char* m1end = m1begin + msg1.size();
    const char* m2begin = static_cast<const char*>(msg2.data());
    const char* m2end = m2begin + msg2.size();
    BOOST_CHECK_EQUAL_COLLECTIONS(m1begin, m1end, m2begin, m2end);
}

#pragma once

#include "Serializer.hpp"
#include <zmq.hpp>
#include <cstring>

namespace antifurto {
namespace serialization {

template <typename M>
class messageTypeTraits
{
    enum { max = 0 };
};

/// This class uses Serializer to serialize and deserialize messages for
/// ZeroMQ transport.
/// @tparam M the MessageType enum to use
template <typename M>
class ZmqSerializer
{
public:
    ZmqSerializer()
        : serializer_(messageTypeTraits<M>::max) { }

    /// Serialize a payload into a zmq message.
    template <typename T>
    void serializeMessage(zmq::message_t& result, M type, const T& msg)
    {
        auto contents = serializer_.serializeMessage(type, msg);
        result.rebuild(contents.payload.second - contents.header.first);
        ::memcpy(result.data(), content.header.first, contents.payload.second);
    }

    /// Deserialize the header, returning the message type.
    M deserializeHeader(const zmq::message_t& msg)
    {
        return static_cast<M>(serializer_.deserializeHeader(
                                  msg.data(), msg.data() + msg.size()));
    }

    /// Deserialize the payload of the message.
    template <typename T>
    T deserializePayload(const zmq::message_t& msg)
    {
        return serializer_.deserializePayload(
                    msg.data(), msg.data() + msg.size());
    }

private:
    Serializer serializer_;
};

}} // namespace antifurto::serialization

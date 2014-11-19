#pragma once

#include "Serializer.hpp"
#include <zmq.hpp>
#include <cstring>

namespace antifurto {
namespace serialization {

template <typename M>
class messageTypeTraits
{
public:
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

    /// Serialize a message with no payload into a zmq message.
    void serializeMessage(zmq::message_t& result, M type)
    {
        auto contents = serializer_.serializeMessage(static_cast<int>(type));
        result.rebuild(contents.header.second - contents.header.first);
        ::memcpy(result.data(), contents.header.first,
                 contents.header.second - contents.header.first);
    }

    /// Serialize a payload into a zmq message.
    template <typename T>
    void serializeMessage(zmq::message_t& result, M type, const T& msg)
    {
        auto contents = serializer_.serializeMessage(static_cast<int>(type), msg);
        result.rebuild(contents.payload.second - contents.header.first);
        ::memcpy(result.data(), contents.header.first,
                 contents.payload.second - contents.header.first);
    }

    /// Deserialize the header, returning the message type.
    M deserializeHeader(zmq::message_t& msg)
    {
        const char* data = static_cast<const char*>(msg.data());
        return static_cast<M>(serializer_.deserializeHeader(
                data, data + msg.size()));
    }

    /// Deserialize the payload of the message.
    template <typename T>
    T deserializePayload(zmq::message_t& msg)
    {
        const char* data = static_cast<const char*>(msg.data());
        return serializer_.deserializePayload<T>(
                data, data + msg.size());
    }

private:
    Serializer serializer_;
};

}} // namespace antifurto::serialization

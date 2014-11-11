#pragma once

#include <utility>
#include <vector>
#include <type_traits>

#include "Exception.hpp"

namespace antifurto {
namespace serialization {

/// This class serializes and deserializes a message.
class Serializer
{
public:
    using BufferRange = std::pair<const char*, const char*>;
    struct MessageContents {
        BufferRange header;
        BufferRange payload;
    };

    Serializer(int maxMessageNumber);

    // serialization
    MessageContents serializeMessage(int msgNumber);

    template <typename T>
    MessageContents serializeMessage(int msgNumber, const T& msg) {
        static_assert(std::has_trivial_copy_constructor<T>::value,
                      "Type is not trivially copyable");
        return serializeMessage(msgNumber,
                                reinterpret_cast<const void*>(&msg),
                                sizeof(msg));
    }

    template <typename T>
    std::size_t getSerializedMessageSize(const T& msg) const
    {
        return getSerializedMessageSizeImpl(sizeof(msg));
    }

    std::size_t getSerializedMessageSizeNoPayload() const;

    // deserialization
    int deserializeHeader(const char* begin, const char* end);

    template <typename T>
    T deserializePayload(const char* begin, const char* end) {
        static_assert(std::has_trivial_copy_constructor<T>::value,
                      "Type is not trivially copyable");
        T result;
        if (!deserializePayload(&result, sizeof(T), begin, end))
            throw Exception("Failed to deserialize payload");
        return result;
    }

private:
    BufferRange serializeHeader(int msgNumber, std::size_t msgSize);
    MessageContents serializeMessage(int msgNumber, const void* msg,
                                     std::size_t msgSize);
    void reserveSize(std::size_t size);
    bool deserializePayload(void* dest, std::size_t destSize,
                            const char* begin, const char* end);
    size_t getSerializedMessageSizeImpl(size_t payloadSize) const;

    const int maxMessageNumber_;
    std::vector<char> buffer_;
};

}} // namespace antifurto::serialization

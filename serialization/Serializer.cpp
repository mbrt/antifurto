#include "Serializer.hpp"

#include <cstdint>
#include <cstring>

namespace antifurto {
namespace serialization {
namespace { // anon

constexpr int32_t getMagicCode() {
    return 0x35f6d3;
}

struct Header
{
    int32_t magicCode;
    int32_t msgNumber;
};

} // anon namespace


Serializer::Serializer(int maxMessageNumber)
    : maxMessageNumber_(maxMessageNumber)
{ }

Serializer::MessageContents
Serializer::serializeMessage(int msgNumber)
{
    MessageContents result;
    result.header = serializeHeader(msgNumber, 0);
    result.payload.first = result.payload.second = nullptr;
    return result;
}

std::size_t Serializer::getSerializedMessageSizeNoPayload() const
{
    return sizeof(Header);
}

int
Serializer::deserializeHeader(const char* begin, const char* end)
{
    if (end < begin
        || static_cast<std::size_t>(end - begin) < sizeof(Header))
        throw Exception("Header is missing");

    Header h;
    ::memcpy(&h, begin, sizeof(Header));
    if (h.magicCode != getMagicCode())
        throw Exception("Corrupt header");
    if (h.msgNumber >= maxMessageNumber_)
        throw Exception("Unknown message number");
    return h.msgNumber;
}


Serializer::BufferRange
Serializer::serializeHeader(int msgNumber, std::size_t msgSize)
{
    Header header{getMagicCode(), msgNumber};
    reserveSize(sizeof(header) + msgSize);
    char* ptr = &buffer_[0];
    ::memcpy(ptr, &header, sizeof(header));
    return BufferRange{ptr, ptr + sizeof(header)};
}

Serializer::MessageContents
Serializer::serializeMessage(int msgNumber, const void* msg, std::size_t msgSize)
{
    MessageContents result;
    result.header = serializeHeader(msgNumber, msgSize);
    result.payload.first = result.header.second;
    ::memcpy(const_cast<char*>(result.payload.first), msg, msgSize);
    result.payload.second = result.payload.first + msgSize;
    return result;
}

void Serializer::reserveSize(std::size_t size)
{
    if (buffer_.size() < size)
        buffer_.resize(size);
}

bool Serializer::
deserializePayload(void* dest, std::size_t destSize,
                   const char* begin, const char* end)
{
    if (end < begin
        || static_cast<std::size_t>(end - begin) < destSize + sizeof(Header))
        return false;

    begin += sizeof(Header);
    ::memcpy(dest, begin, destSize);
    return true;
}

size_t Serializer::getSerializedMessageSizeImpl(size_t payloadSize) const
{
    return sizeof(Header) + payloadSize;
}

}} // namespace antifurto::serialization

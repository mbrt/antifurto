#pragma once

#include "Protocol.hpp"
#include "ZmqSerializer.hpp"

namespace antifurto {
namespace serialization {

template <>
class messageTypeTraits<MessageType>
{
public:
    enum { max = static_cast<int>(MessageType::MAX) };
};

using DefaultZmqSerializer = ZmqSerializer<MessageType>;

}} // namespace antifurto::serialization

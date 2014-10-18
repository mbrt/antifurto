#pragma once

#include <stdexcept>

namespace antifurto {
namespace serialization {

struct Exception : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

}} // namespace antifurto::serialization

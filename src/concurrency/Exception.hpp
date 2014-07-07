#pragma once

#include "../Exception.hpp"

namespace antifurto {
namespace concurrency {

class Exception : public ::antifurto::Exception
{
    using ::antifurto::Exception::Exception;
};

} // namespace concurrency
} // namespace antifurto

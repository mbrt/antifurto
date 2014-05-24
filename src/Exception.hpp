#pragma once

#include <stdexcept>

namespace antifurto {

struct Exception : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

struct UnexpectedException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto

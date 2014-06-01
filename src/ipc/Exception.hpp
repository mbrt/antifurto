#pragma once

#include "../Exception.hpp"

namespace antifurto {
namespace ipc {

class Exception : public ::antifurto::Exception
{
    using ::antifurto::Exception::Exception;
};

} // namespace antifurto
} // namespace antifurto

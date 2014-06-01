#pragma once

#include <string>
#include "Exception.hpp"
#include "ipc/Process.hpp"

namespace antifurto {

/// This class manages notifications using WhatsApp
class WhatsappNotifier
{
public:
    WhatsappNotifier(std::string baseDir, std::string configFile);
    bool send(std::string const& dest, std::string const& msg);

private:
    std::string baseDir_;
    std::string configFile_;
    ipc::Process yowsupProcess_;
};

class WhatsappNotifierException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto

#pragma once

#include <string>
#include "Exception.hpp"
#include "ipc/Process.hpp"

namespace antifurto {

class Configuration;

/// This class manages notifications using WhatsApp
class WhatsappNotifier
{
public:
    WhatsappNotifier(std::string baseDir, std::string configFile);
    void send(std::string const& dest, std::string const& msg);

private:
    std::string baseDir_;
    std::string configFile_;
    ipc::Process yowsupProcess_;
};

/// This function configures a WhatsappNotifier using a template file and
/// variables stored in the given configuration
WhatsappNotifier configureWhatsappNotifier(const Configuration& c,
                                           const std::string& baseDir);

class WhatsappNotifierException : public Exception
{
    using Exception::Exception;
};

} // namespace antifurto

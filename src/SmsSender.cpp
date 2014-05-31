#include "SmsSender.hpp"

#include <iomanip>

namespace antifurto {

/// Example code to send SMS:
/// http://www.codeproject.com/Articles/387567/How-to-send-an-SMS-message-from-an-application
///
/// how to print in hex format:
/// std::cout << std::setw(4) << std::setfill('0') << std::hex()
///
/// send HTTP request using curl:
/// http://makandracards.com/makandra/1145-how-to-send-http-requests-using-curl
void SmsSender::send(const std::string &msg, const std::string &dest)
{
}

} // namespace antifurto

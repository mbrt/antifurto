#include "StreamRedirect.hpp"
#include <iostream>

namespace antifurto {
namespace fcgi {

StreamRedirector::StreamRedirector(FCGX_Request& request)
    : cinFcgi_(request.in)
    , coutFcgi_(request.out)
    , cerrFcgi_(request.err)
{
    std::cin.rdbuf(&cinFcgi_);
    std::cout.rdbuf(&coutFcgi_);
    std::cerr.rdbuf(&cerrFcgi_);
}

StreamRedirector::~StreamRedirector() { }

}} // namespace antifurto::fcgi

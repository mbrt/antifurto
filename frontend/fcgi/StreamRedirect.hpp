#pragma once

#include <fcgio.h>
#include <iosfwd>

namespace antifurto {
namespace fcgi {

/// This class redirects the standard streams to the given request streams.
class StreamRedirector
{
public:
    StreamRedirector(FCGX_Request& request);
    ~StreamRedirector();

private:
    fcgi_streambuf cinFcgi_;
    fcgi_streambuf coutFcgi_;
    fcgi_streambuf cerrFcgi_;
};

}} // namespace antifurto::fcgi

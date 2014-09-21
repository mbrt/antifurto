// see example here:
// https://github.com/toshic/libfcgi/blob/master/examples/echo-cpp.cpp

#include <fcgio.h>
#include <fcgi_config.h>
#include <iostream>

#include "StreamRedirect.hpp"

using namespace antifurto::fcgi;

int main(int, char*[])
{
    FCGX_Request request;
    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0) {
        StreamRedirector redirect{request};
    }
    return 0;
}

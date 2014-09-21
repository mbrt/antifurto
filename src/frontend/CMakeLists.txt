add_executable(antifurto_fcgi
               fcgi/main.cpp
               fcgi/StreamRedirect.hpp
               fcgi/StreamRedirect.cpp)
target_link_libraries(antifurto_fcgi fcgi fcgi++)

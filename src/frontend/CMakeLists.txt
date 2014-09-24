add_executable(antifurto_fcgi
               fcgi/main.cpp
               fcgi/StreamReader.cpp
               fcgi/StreamReader.hpp
               fcgi/StreamRedirect.cpp
               fcgi/StreamRedirect.hpp
               fcgi/ZmqLazyPirateClient.cpp
               fcgi/ZmqLazyPirateClient.hpp
)
target_link_libraries(antifurto_fcgi fcgi fcgi++ zmq)

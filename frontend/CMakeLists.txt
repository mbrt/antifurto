include_directories(..)
add_executable(antifurto.fcgi
               fcgi/main.cpp
               fcgi/StreamReader.cpp
               fcgi/StreamReader.hpp
               fcgi/StreamRedirect.cpp
               fcgi/StreamRedirect.hpp
               fcgi/ZmqLazyPirateClient.cpp
               fcgi/ZmqLazyPirateClient.hpp
)
target_link_libraries(antifurto.fcgi fcgi fcgi++ zmq antifurto_serialization)

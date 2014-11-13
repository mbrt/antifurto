add_library(antifurto_serialization STATIC
            Serializer.cpp
            Serializer.hpp
            Exception.hpp
            Protocol.hpp
            ZmqSerializer.hpp
            DefaultZmqSerializer.hpp
)
target_link_libraries(antifurto_serialization zmq)

add_subdirectory(test)

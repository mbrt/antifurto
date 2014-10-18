add_library(antifurto_serialization STATIC
            Serializer.cpp
            Serializer.hpp
            Exception.hpp
            Protocol.hpp
            ZmqSerializer.hpp
            DefaultZmqSerializer.hpp
)

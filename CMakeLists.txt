CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("Antifurto")

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CROSS_COMPILE true)
else()
    set(CROSS_COMPILE false)
endif()


# main exe
include_directories(src)
add_executable(pitest src/test.cpp)

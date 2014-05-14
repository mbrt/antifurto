CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("Antifurto")

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CROSS_COMPILE true)
else()
    set(CROSS_COMPILE false)
endif()


# options
set(TARGET_HOST "pi@picamera" CACHE STRING "The user and host name of the target")
set(DEPLOY_DIR "${DEVICE_ROOT_FS}/home/pi/antifurto" CACHE PATH "The deploy
directory")

# scripts
configure_file(scripts/setup-sysroot.sh setup-sysroot.sh)
configure_file(scripts/deploy.sh deploy.sh)

# main exe
include_directories(src)
add_executable(pitest src/test.cpp)

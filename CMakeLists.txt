CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("Antifurto" CXX)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CROSS_COMPILE true)
else()
    set(CROSS_COMPILE false)
endif()

# c++11 support
list(APPEND CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

if(CROSS_COMPILE)
    # options
    set(TARGET_HOST "pi@picamera" CACHE STRING "The user and host name of the target")
    set(DEPLOY_DIR "${DEVICE_ROOT_FS}/home/pi/antifurto" CACHE PATH "The deploy
directory")

    # scripts
    configure_file(scripts/setup-sysroot.sh setup-sysroot.sh)
    configure_file(scripts/deploy.sh deploy.sh)
    configure_file(scripts/umount-sysroot.sh umount-sysroot.sh)
endif()


add_subdirectory(test)

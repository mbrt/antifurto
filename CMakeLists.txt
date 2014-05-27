CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("Antifurto" CXX)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CROSS_COMPILE true)
else()
    set(CROSS_COMPILE false)
endif()

# c++11 support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(CROSS_COMPILE)
    # options
    set(TARGET_HOST "pi@picamera" CACHE STRING "The user and host name of the target")
    set(DEPLOY_DIR "${DEVICE_ROOT_FS}/home/pi/antifurto" CACHE PATH "The deploy directory")

    # scripts
    configure_file(scripts/setup-sysroot.sh setup-sysroot.sh)
    configure_file(scripts/deploy.sh deploy.sh)
    configure_file(scripts/umount-sysroot.sh umount-sysroot.sh)
endif()

# libraries
if(NOT CROSS_COMPILE)
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost COMPONENTS unit_test_framework)
    find_package(OpenCV REQUIRED core highgui imgproc)
    set(ANTIFURTO_LIBS ${OpenCV_LIBS})
else()
    # find_package OpenCV does not work well when cross compiling
    set(OpenCV_LIBS opencv_core opencv_highgui opencv_imgproc)
    set(ANTIFURTO_LIBS ${OpenCV_LIBS})
endif()

# third party to build
add_subdirectory(lib)

# common files
set(ANTIFURTO_COMMON_CPP
    src/MotionDetector.cpp
    src/PictureArchive.cpp
)
set(ANTIFURTO_COMMON_HPP
    src/Camera.hpp
    src/Picture.hpp
    src/Exception.hpp
    src/MotionDetector.hpp
    src/PictureArchive.hpp
)
# x86
set(ANTIFURTO_X86_CPP
    src/CvCamera.cpp
)
set(ANTIFURTO_X86_HPP
    src/CvCamera.hpp
)
# raspberry pi
set(ANTIFURTO_PI_CPP
)
set(ANTIFURTO_PI_HPP
)

if(NOT CROSS_COMPILE)
    set(ANTIFURTO_CPP ${ANTIFURTO_COMMON_CPP} ${ANTIFURTO_X86_CPP})
    set(ANTIFURTO_HPP ${ANTIFURTO_COMMON_HPP} ${ANTIFURTO_X86_HPP})
else()
    set(ANTIFURTO_CPP ${ANTIFURTO_COMMON_CPP} ${ANTIFURTO_PI_CPP})
    set(ANTIFURTO_HPP ${ANTIFURTO_COMMON_HPP} ${ANTIFURTO_PI_HPP})
endif()


# main exe and libs
add_library(antifurto_core STATIC ${ANTIFURTO_CPP} ${ANTIFURTO_HPP})
target_link_libraries(antifurto_core ${ANTIFURTO_LIBS})

add_executable(antifurto src/main.cpp ${ANTIFURTO_CPP} ${ANTIFURTO_HPP})
target_link_libraries(antifurto antifurto_core)

# tests
if(NOT CROSS_COMPILE)
    add_subdirectory(test)
endif()

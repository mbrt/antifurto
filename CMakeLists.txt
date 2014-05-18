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
    set(DEPLOY_DIR "${DEVICE_ROOT_FS}/home/pi/antifurto" CACHE PATH "The deploy directory")

    # scripts
    configure_file(scripts/setup-sysroot.sh setup-sysroot.sh)
    configure_file(scripts/deploy.sh deploy.sh)
    configure_file(scripts/umount-sysroot.sh umount-sysroot.sh)
endif()

# libraries
if(NOT ${CROSS_COMPILE})
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost COMPONENTS unit_test_framework)
endif()
find_package(OpenCV REQUIRED core highgui imgproc)

macro(ADD_PREFIX LIST_NAME PREFIX)
    set(${LIST_NAME}_RESULT)
    foreach(ELEMENT ${${LIST_NAME}})
        list(APPEND ${LIST_NAME}_RESULT ${PREFIX}${ELEMENT})
    endforeach()
    set(${LIST_NAME} "${${LIST_NAME}_RESULT}")
    unset(${LIST_NAME}_RESULT})
endmacro()

# main exe
set(ANTIFURTO_CPP
    src/CvCamera.cpp
)
set(ANTIFURTO_HPP
    src/Camera.hpp
    src/CvCamera.hpp
    src/Picture.hpp
    src/Exception.hpp
)

add_executable(antifurto src/main.cpp ${ANTIFURTO_CPP} ${ANTIFURTO_HPP})
target_link_libraries(antifurto ${OpenCV_LIBS})

# tests
add_subdirectory(test)

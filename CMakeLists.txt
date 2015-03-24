CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT("Antifurto" CXX)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CROSS_COMPILE true)
    set(ARCHITECTURE pi)
else()
    set(CROSS_COMPILE false)
    set(ARCHITECTURE ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif()

set(VERSION_MAJOR 3)
set(VERSION_MINOR 4)
set(VERSION_BUILD 0)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD}")

# compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# definitions
if(CROSS_COMPILE)
    add_definitions(-DANTIFURTO_RASPBERRY=1)
endif()
add_definitions(-DANTIFURTO_VERSION_STRING="${VERSION_STRING}")

if(CROSS_COMPILE)
    # options
    set(TARGET_HOST "pi@picamera" CACHE STRING "The user and host name of the target")
    set(DEPLOY_DIR "${DEVICE_ROOT_FS}/home/pi/antifurto" CACHE PATH "The deploy directory")

    # scripts
    configure_file(scripts/setup-sysroot.sh setup-sysroot.sh)
    configure_file(scripts/deploy.sh deploy.sh)
    configure_file(scripts/umount-sysroot.sh umount-sysroot.sh)
    configure_file(scripts/init-script.sh init-script.sh)
endif()

# common scripts
file(COPY scripts/send_mail.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# third party to build
add_subdirectory(lib)

# libraries
if(NOT CROSS_COMPILE)
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost COMPONENTS
                 unit_test_framework filesystem system program_options)
    find_package(OpenCV REQUIRED core highgui imgproc)
    list(APPEND ANTIFURTO_LIBS ${OpenCV_LIBS} ${Boost_LIBRARIES})
else()
    # find_package OpenCV does not work well when cross compiling
    set(OpenCV_LIBS opencv_core opencv_highgui opencv_imgproc)
    set(picam_LIBS
        picam
        rt pthread
        mmal_core mmal mmal_util vcos bcm_host)
    set(picam_LIBRARY_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/picam/build
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/userland/build/lib)
    set(picam_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lib/picam)
    find_package(Boost COMPONENTS
                 filesystem system program_options)

    link_directories(${picam_LIBRARY_DIRS})
    include_directories(. ${picam_INCLUDE_DIRS})

    list(APPEND ANTIFURTO_LIBS ${OpenCV_LIBS} ${picam_LIBS} ${Boost_LIBRARIES})
    list(APPEND ANTIFURTO_DEPENDENCIES libpicam)
endif()
list(APPEND ANTIFURTO_LIBS pthread)
include_directories(lib/folly/include)
# zeromq
list(APPEND ANTIFURTO_LIBS zmq)

# common files
set(ANTIFURTO_COMMON_CPP
    src/Antifurto.cpp
    src/CameraController.cpp
    src/Config.cpp
    src/DropboxUploader.cpp
    src/Environment.cpp
    src/LiveView.cpp
    src/LiveViewController.cpp
    src/MailNotifier.cpp
    src/MailNotificationController.cpp
    src/MonitorController.cpp
    src/MotionDetector.cpp
    src/PictureArchive.cpp
    src/RecordingController.cpp
    src/SignalHandler.cpp
    src/StaticConfig.cpp
    src/WhatsappNotificationController.cpp
    src/WhatsappNotifier.cpp
    src/concurrency/TaskScheduler.cpp
    src/concurrency/TimeUtility.cpp
    src/ipc/Fork.cpp
    src/ipc/NamedPipe.cpp
    src/ipc/PosixSignalHandler.cpp
    src/ipc/Process.cpp
    src/log/Log.cpp
    src/log/Logger.cpp
    src/text/ToString.cpp
    src/text/TextReplace.cpp
)
set(ANTIFURTO_COMMON_HPP
    src/Antifurto.hpp
    src/Camera.hpp
    src/CameraController.hpp
    src/Config.hpp
    src/DropboxUploader.hpp
    src/Environment.hpp
    src/Exception.hpp
    src/LiveView.hpp
    src/LiveViewController.hpp
    src/MailNotifier.hpp
    src/MailNotificationController.hpp
    src/MonitorController.hpp
    src/MotionDetector.hpp
    src/Picture.hpp
    src/PictureArchive.hpp
    src/RecordingController.hpp
    src/SignalHandler.hpp
    src/StaticConfig.hpp
    src/WhatsappNotificationController.hpp
    src/WhatsappNotifier.hpp
    src/concurrency/Exception.hpp
    src/concurrency/SpScQueue.hpp
    src/concurrency/TaskScheduler.hpp
    src/concurrency/TimeUtility.hpp
    src/concurrency/binsem.hpp
    src/concurrency/fwd.hpp
    src/fs/Paths.hpp
    src/ipc/Exception.hpp
    src/ipc/Fork.hpp
    src/ipc/NamedPipe.hpp
    src/ipc/PosixSignalHandler.hpp
    src/ipc/Process.hpp
    src/log/ConcreteLoggerSink.hpp
    src/log/Log.hpp
    src/log/Logger.hpp
    src/log/LoggerSink.hpp
    src/log/NullStream.hpp
    src/meta/Algorithm.hpp
    src/meta/Iterator.hpp
    src/meta/Metronome.hpp
    src/meta/Observer.hpp
    src/meta/SmartPtr.hpp
    src/meta/TypeTraits.hpp
    src/text/ToString.hpp
    src/text/TextReplace.hpp
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
    src/PiCamera.cpp
)
set(ANTIFURTO_PI_HPP
    src/PiCamera.hpp
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
if (ANTIFURTO_DEPENDENCIES)
    add_dependencies(antifurto_core ${ANTIFURTO_DEPENDENCIES})
endif()

add_executable(antifurto src/main.cpp)
target_link_libraries(antifurto antifurto_core)

# frontend
add_subdirectory(frontend)
# tests
add_subdirectory(test)

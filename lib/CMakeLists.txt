include(ExternalProject)

if(CROSS_COMPILE)

ExternalProject_Add(userland
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../toolchain-pi.cmake -DCMAKE_BUILD_TYPE=Release
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/userland
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/userland/build/arm-linux/release
    BUILD_COMMAND make mmal_core mmal mmal_util vcos bcm_host
    INSTALL_COMMAND ""
)

ExternalProject_Add(raspicam_cv
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../toolchain-pi.cmake -DCMAKE_BUILD_TYPE=Release
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/raspicam_cv
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/raspicam_cv/build
    BUILD_COMMAND cmake . && make
    INSTALL_COMMAND ""
)
add_dependencies(raspicam_cv userland)

ExternalProject_Add(libb64
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../toolchain-pi.cmake -DCMAKE_BUILD_TYPE=Release
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libb64
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libb64/build/pi
    BUILD_COMMAND make
    INSTALL_COMMAND ""
)

else(CROSS_COMPILE)

ExternalProject_Add(libb64
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libb64
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libb64/build/${CMAKE_HOST_SYSTEM_PROCESSOR}
    BUILD_COMMAND make
    INSTALL_COMMAND ""
)

endif(CROSS_COMPILE)

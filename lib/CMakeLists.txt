include(ExternalProject)

if(CROSS_COMPILE)

ExternalProject_Add(userland
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../toolchain-pi.cmake -DCMAKE_BUILD_TYPE=Release
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/userland
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/userland/build/arm-linux/release
    BUILD_COMMAND make mmal_core mmal mmal_util vcos bcm_host
    INSTALL_COMMAND ""
)

ExternalProject_Add(libpicam
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../toolchain-pi.cmake -DCMAKE_BUILD_TYPE=Release
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/picam
    BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/picam/build
    BUILD_COMMAND cmake . && make picam
    INSTALL_COMMAND ""
)
add_dependencies(libpicam userland)

else(CROSS_COMPILE)

# no external projects for x86 now

endif(CROSS_COMPILE)

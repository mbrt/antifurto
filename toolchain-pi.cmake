# this one is important
SET(CMAKE_SYSTEM_NAME Linux)

#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(TOOLCHAIN_PATH "/opt/raspberrypi-tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin" CACHE PATH "The path to toolchain bin directory")
SET(DEVICE_ROOT_FS "/opt/toolchain/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi" CACHE PATH "The path to the root filesystem")
SET(TOOLCHAIN_PREFIX arm-poky-linux-gnueabi-)
SET(TOOLCHAIN ${TOOLCHAIN_PATH}/${TOOLCHAIN_PREFIX})

# specify the cross compiler
SET(CMAKE_C_COMPILER ${TOOLCHAIN}gcc)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --sysroot ${DEVICE_ROOT_FS}")
SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --sysroot ${DEVICE_ROOT_FS}")
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN}g++)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot ${DEVICE_ROOT_FS}")
SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --sysroot ${DEVICE_ROOT_FS}")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH ${DEVICE_ROOT_FS})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

cmake_minimum_required(VERSION 2.8)
project(picam)

set(USERLAND_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/../userland CACHE PATH "The userland directory")

set(USERLAND_INCLUDE_DIRS
    ${USERLAND_DIR}
    ${USERLAND_DIR}/host_applications/linux/libs/bcm_host/include
	${USERLAND_DIR}/host_applications/linux/apps/raspicam
	${USERLAND_DIR}/interface/vcos/pthreads
	${USERLAND_DIR}/interface/vmcs_host/linux
	${USERLAND_DIR}/interface/mmal)
set(USERLAND_LIBRARY_DIRS ${USERLAND_DIR}/build/lib)
set(USERLAND_LIBRARIES mmal_core mmal mmal_util vcos bcm_host)

# opencv
set(OpenCV_LIBS opencv_core opencv_imgproc)

link_directories(${USERLAND_LIBRARY_DIRS})
include_directories(. ${USERLAND_INCLUDE_DIRS})

add_library(picam STATIC camera.cpp ${USERLAND_RASPICAM_SRC})
target_link_libraries(picam ${USERLAND_LIBRARIES} ${OpenCV_LIBS})

add_executable(picamtest picamtest.cpp cameracontrol.cpp)
target_link_libraries(picamtest picam)

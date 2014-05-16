# hello world
include_directories(.)
add_executable(hello helloworld.cpp)

add_executable(cvtest cvtest.cpp)
target_link_libraries(cvtest opencv_highgui opencv_core opencv_imgproc)

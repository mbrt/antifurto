find_package(Boost COMPONENTS unit_test_framework)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()


# hello world
include_directories(.)
add_executable(hello helloworld.cpp)

# cv test
add_executable(cvtest cvtest.cpp)
target_link_libraries(cvtest
    opencv_highgui opencv_core opencv_imgproc
    ${Boost_LIBRARIES})

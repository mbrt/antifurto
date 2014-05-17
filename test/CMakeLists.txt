set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS unit_test_framework)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()


# hello world
include_directories(.)
add_executable(hello helloworld.cpp)

# cv test
add_executable(cvcamera cvcamera.cpp)
target_link_libraries(cvcamera
    opencv_highgui opencv_core opencv_imgproc)

# unit test example
add_executable(unit_test_example unit.cpp)
target_link_libraries(unit_test_example ${Boost_LIBRARIES})

include_directories(../src)

if(CROSS_COMPILE)
    # Raspberry Pi TESTS
    add_executable(antifurto_pi_test antifurto_pi_test.cpp)
    target_link_libraries(antifurto_pi_test antifurto_core)
else()
    # x86 TESTS
    include_directories(${Boost_INCLUDE_DIRS})

    # hello world
    include_directories(.)
    add_executable(hello helloworld.cpp)

    # cv test
    add_executable(cvcamera cvcamera.cpp)
    target_link_libraries(cvcamera ${OpenCV_LIBS})

    # unit test example
    add_executable(unit_test_example unit.cpp)
    target_link_libraries(unit_test_example ${Boost_LIBRARIES})

    # main unit test exe
    add_executable(antifurto_unit antifurto_unit.cpp)
    target_link_libraries(antifurto_unit ${Boost_LIBRARIES} antifurto_core)

endif(CROSS_COMPILE)

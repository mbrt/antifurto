include_directories(..)

if(NOT CROSS_COMPILE)
    include_directories(${Boost_INCLUDE_DIRS})

    # main unit test exe
    add_executable(serialization_unit serialization_unit.cpp)
    target_link_libraries(serialization_unit
                          ${Boost_LIBRARIES} antifurto_serialization)
endif()

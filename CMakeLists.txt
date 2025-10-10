cmake_minimum_required(VERSION 3.22)
project(parallel_planets C)

set(CMAKE_C_STANDARD 11)

add_executable(parallel_planets main.c)


find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(parallel_planets PUBLIC ${MATH_LIBRARY})
endif()
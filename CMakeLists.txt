cmake_minimum_required(VERSION 3.22)
project(parallel-planets C)

set(CMAKE_C_STANDARD 11)

add_executable(parallel-planets main.c
        ray-tracing/geometry.c
        ray-tracing/geometry.h
        ray-tracing/ray_trace.c
        ray-tracing/ray_trace.h
)


find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(parallel-planets PUBLIC ${MATH_LIBRARY})
endif()
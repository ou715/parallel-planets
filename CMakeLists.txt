cmake_minimum_required(VERSION 3.22)
project(parallel-planets C)

set(CMAKE_C_STANDARD 11)

find_package(MPI REQUIRED)


add_executable(parallel-planets src/main.c
        src/ray-tracing/geometry.c
        src/ray-tracing/geometry.h
        src/ray-tracing/ray_trace.c
        src/ray-tracing/ray_trace.h
        src/utils/io.c
        src/utils/io.h
        src/utils/scene.c
        src/utils/scene.h
        src/utils/mpi_utils.h
        src/utils/mpi_utils.c
        src/utils/mpi_utils.h
        src/dynamics/move.c
        src/dynamics/move.h
)

target_compile_options(parallel-planets PRIVATE -march=native)

find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(parallel-planets PRIVATE ${MATH_LIBRARY} MPI::MPI_C)
endif()

//
// Created by nemo on 17/10/25.
//

#ifndef MPI_UTILS_H
#define MPI_UTILS_H
#include <mpi/mpi.h>
#include "../ray-tracing/ray_trace.h"
MPI_Datatype create_pixel_colour_mpi_type(void);
void allocate_rows_to_processes_blocks(int image_height, int *rows_to_process);
#endif //MPI_UTILS_H

#ifndef MPI_UTILS_H
#define MPI_UTILS_H
#include <mpi/mpi.h>
#include "../ray-tracing/ray_trace.h"
MPI_Datatype create_pixel_colour_mpi_type(void);
MPI_Datatype create_vector3_mpi_type(void);
MPI_Datatype create_sphere_mpi_type(void);
MPI_Datatype create_solid_colour_sphere_mpi_type(void);
void allocate_rows_to_processes_blocks(int image_height, int *rows_to_process);
#endif //MPI_UTILS_H

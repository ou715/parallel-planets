
#include "mpi_utils.h"

/**
 *
 * @return MPI Data type of uint8_t triple struct. Not sure why isn't there a helper function for this in OpenMPI
 */
MPI_Datatype create_pixel_colour_mpi_type(void) {
    MPI_Datatype pixel_colour_type;
    const int count = 3;
    const int blocklengths[3] = {1, 1, 1};
    const MPI_Datatype types[3] = {MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR};
    MPI_Aint displacements[3], base;

    pixel_colour sample;
    MPI_Get_address(&sample, &base);
    MPI_Get_address(&sample.r, &displacements[0]);
    MPI_Get_address(&sample.g, &displacements[1]);
    MPI_Get_address(&sample.b, &displacements[2]);

    for (int i = 0; i < count; i++) {
        displacements[i] -= base;
    }

    MPI_Type_create_struct(count, blocklengths, displacements, types, &pixel_colour_type);
    MPI_Type_commit(&pixel_colour_type);
    return pixel_colour_type;
}

void allocate_rows_to_processes_blocks(int image_height, int *rows_to_process) {
    for (int i = 0; i < ( image_height); i++) {
        rows_to_process[i] = i;
    }
}

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

    displacements[0] = offsetof(pixel_colour, r);
    displacements[1] = offsetof(pixel_colour, g);
    displacements[2] = offsetof(pixel_colour, b);

    for (int i = 0; i < count; i++) {
        displacements[i] -= base;
    }

    MPI_Type_create_struct(count, blocklengths, displacements, types, &pixel_colour_type);
    MPI_Type_commit(&pixel_colour_type);
    return pixel_colour_type;
}

//TODO create a helper function for this
MPI_Datatype create_vector3_mpi_type(void) {
    MPI_Datatype vector3_type;
    const int blocklengths[3] = {1, 1, 1};
    const MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Aint displacements[3];

    displacements[0] = offsetof(vector3, x);
    displacements[1] = offsetof(vector3, y);
    displacements[2] = offsetof(vector3, z);

    MPI_Type_create_struct(3, blocklengths, displacements, types, &vector3_type);
    MPI_Type_commit(&vector3_type);
    return vector3_type;
}

void allocate_rows_to_processes_blocks(int image_height, int *rows_to_process) {
    for (int i = 0; i < ( image_height); i++) {
        rows_to_process[i] = i;
    }
}
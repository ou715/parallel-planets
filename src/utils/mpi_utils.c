
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

//TODO There has to be a better way
MPI_Datatype create_sphere_mpi_type(void) {
    MPI_Datatype sphere_type;
    MPI_Datatype vector3_type = create_vector3_mpi_type();

    const int blocklengths[4] = {1, 1, 1, 1};
    const MPI_Datatype types[4] = {MPI_DOUBLE, MPI_DOUBLE, vector3_type, vector3_type};
    MPI_Aint displacements[4];

    displacements[0] = offsetof(sphere, radius);
    displacements[1] = offsetof(sphere, mass);
    displacements[2] = offsetof(sphere, position);
    displacements[3] = offsetof(sphere, velocity);

    MPI_Type_create_struct(4, blocklengths, displacements, types, &sphere_type);
    MPI_Type_commit(&sphere_type);
    return sphere_type;
}

//TODO This is madness
MPI_Datatype create_solid_colour_sphere_mpi_type(void) {
    MPI_Datatype solid_sphere;
    MPI_Datatype sphere_type = create_sphere_mpi_type();
    MPI_Datatype pixel_colour_type = create_pixel_colour_mpi_type();

    const int blocklengths[2] = {1, 1};
    const MPI_Datatype types[2] = {sphere_type, pixel_colour_type};
    MPI_Aint displacements[2];

    displacements[0] = offsetof(solid_colour_sphere, sphere);
    displacements[1] = offsetof(solid_colour_sphere, colour);

    MPI_Type_create_struct(2, blocklengths, displacements, types, &solid_sphere);
    MPI_Type_commit(&solid_sphere);
    return solid_sphere;
}

void allocate_rows_to_processes_blocks(int image_height, int *rows_to_process) {
    for (int i = 0; i < ( image_height); i++) {
        rows_to_process[i] = i;
    }
}

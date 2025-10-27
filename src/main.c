// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "dynamics/move.h"
#include "mpi/mpi.h"
#include "ray-tracing/geometry.h"
#include "ray-tracing/ray_trace.h"
#include "utils/io.h"
#include "utils/mpi_utils.h"
#include "utils/scene.h"

/**
 * Useful for prototyping
 * @param spheres List of sphere to update
 * @param t time step
 */
void simple_move_spheres(solid_colour_sphere *spheres, int t) {
    solid_colour_sphere sphere = spheres[1];
    sphere.sphere.position.x += sin(t) + 1;
    sphere.sphere.position.y += sin(t) + 1;
    sphere.sphere.position.z += cos(t) + 1;;
    spheres[1] = sphere;
}

int main(int argc, char **argv) {
    char *output_root = argv[1];

    //MPI RELATED SETUP

    MPI_Init(&argc, &argv);

    int rank, namelen;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &namelen);

    // printf("Processor %s, %d out of %d processors, reporting for duty\n",
    //    processor_name, rank, world_size);

    //Define custom MPI data type; will be used for communicating
    MPI_Datatype pixel_colour_type = create_pixel_colour_mpi_type();
    MPI_Datatype vector3_type = create_vector3_mpi_type();

    //Define communicators
    MPI_Group world_group, ray_tracing_group, dynamics_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int ray_tracing_ranks_number = 4, dynamics_ranks_number = 2;
    int ray_tracing_ranks[4] = { 2, 3, 4, 5}, dynamics_ranks[2] = {0, 1};

    MPI_Comm ray_tracing_comm, dynamics_comm, messenger_com;
    MPI_Group_incl(world_group, ray_tracing_ranks_number, ray_tracing_ranks, &ray_tracing_group);
    MPI_Group_incl(world_group, dynamics_ranks_number, dynamics_ranks, &dynamics_group);

    //MPI_Comm_create_group(MPI_COMM_WORLD, dynamics_group, 0, &dynamics_comm);

    int ray_tracing_rank, dynamics_rank;
    MPI_Group_rank(ray_tracing_group, &ray_tracing_rank);
    MPI_Group_rank(dynamics_group, &dynamics_rank);

    MPI_Barrier(MPI_COMM_WORLD);

    //IMAGE RELATED SETUP

    //TODO read these from an input file or command line
    const double aspect_ratio = 1.6; // almost as good as 4:3
    const int image_height = ray_tracing_ranks_number * 100; //for even division of work; should be generalized
    const int image_width = image_height * aspect_ratio;

    scene scene = scene_init(image_width, image_height);
    pixel_colour *image = malloc(image_width * image_height * sizeof(pixel_colour));

    //Useful information
    //printf("The eye is positioned at x: %.2f, y: %.2f, z: %.2f\n", scene.eye_position.x, scene.eye_position.y, scene.eye_position.z);

    //Setup objects
    //sphere sphere1 = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}};
    //TODO Read object positions from a file at runtime
    //solid_colour_sphere sphere2 = {.sphere = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere3 = {.sphere = {.radius = 5, .position = {.x = 10.0, .y = -5.0, .z = -30.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere4 = {.sphere = {.radius = 5, .position = {.x = -10.0, .y = 5.0, .z = -30.0}}, .colour = {.r = 150, .g = 0, .b = 0}};

    sphere_with_mass sphere3_m = {.mass = 1, .position = sphere3.sphere.position, .velocity = {.x = 0, .y = 0, .z = 0}};
    sphere_with_mass sphere4_m = {.mass = 1, .position = sphere4.sphere.position, .velocity = {.x = 1, .y = 1, .z = 0}};

    //TODO Consider merging dynamic and ray tracing spheres
    solid_colour_sphere spheres[2] = { sphere3, sphere4 };

    //Setup light
    //TODO Consider storing in a list, or a file
    vector3 point_light = {.x = 0.0, .y = 15000.0, .z = -20.0};

    int number_of_rows_per_process = image_height / ray_tracing_ranks_number;

    int rows_to_process[image_height];

    int number_of_time_steps = 30000;
    double dt = 0.05;

    if (rank == 0) {
        printf("START PROGRAM\n");

        printf("Image width is %d and image height is %d\n", image_width, image_height);
        printf("The eye is positioned at x: %.2f, y: %.2f, z: %.2f\n", scene.eye_position.x, scene.eye_position.y, scene.eye_position.z);
        printf("Output root is: %s\n", output_root);
        printf("The number of time steps is: %d\n", number_of_time_steps);
        printf("The size of the time step is: %.5f\n", dt);

        printf("Rows per process:  %d\n", number_of_rows_per_process );
    }

    double start_time, elapsed_time;
    start_time = MPI_Wtime();

    if (ray_tracing_rank != MPI_UNDEFINED) {
        MPI_Comm_create_group(MPI_COMM_WORLD, ray_tracing_group, 0, &ray_tracing_comm);
        MPI_Intercomm_create(ray_tracing_comm, 0, MPI_COMM_WORLD, 0, 0, &messenger_com);
        int received_rows[number_of_rows_per_process];
        int pixels_to_receive = image_width * number_of_rows_per_process; //also the number of total pixels per partial image

        if (ray_tracing_rank == 0) {
            //Sending data in consecutive rows. There are probably more efficient ways to split the work,
            //if more advanced rendering techniques were in use
            //The simple one has all the rays completely independent
            allocate_rows_to_processes_blocks(image_height, rows_to_process);
        }

        printf("World rank %d: ray_tracing_rank = %d\n", rank, ray_tracing_rank);

        pixel_colour *partial_image = malloc(pixels_to_receive * sizeof(pixel_colour));

        MPI_Scatter(rows_to_process, number_of_rows_per_process, MPI_INT, received_rows,
        number_of_rows_per_process, MPI_INT, 0, ray_tracing_comm);

        //Main time loop
        for (int t = 0; t < number_of_time_steps; t++) {
            //printf("Total pixels to be processed by each process:  %d\n", pixels_to_receive );

            if (partial_image == NULL) {
                printf("Failed to allocate memory for partial image\n");
            }
            //printf("All scattered\n");

            //Actual work
            render_pixels(image_width, received_rows, number_of_rows_per_process, scene, spheres,point_light, partial_image, rank);
            //printf("All rendered\n");

            MPI_Gather(partial_image, pixels_to_receive, pixel_colour_type, image, pixels_to_receive, pixel_colour_type, 0, ray_tracing_comm);

            //Output
            if (ray_tracing_rank == 0) {
                //printf("The position of sphere3 is x: %.2f, y: %.2f, z: %.2f\n", sphere3.sphere.position.x, sphere3.sphere.position.y, sphere3.sphere.position.z);
                //printf("The position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere3_m.position.x, sphere3_m.position.y, sphere3_m.position.z);
                char image_output_path[PATH_MAX];
                if (t % 100 == 0) {
                    snprintf(image_output_path, sizeof(image_output_path), "%s/outputs/video/image_%04d.png", output_root, t/100);

                    save_image_png(image, image_width, image_height, image_output_path);
                }

                vector3 new_position;
                MPI_Recv(&new_position, 3, vector3_type, 0, 0, messenger_com, MPI_STATUS_IGNORE);
                sphere4_m.position = new_position;
            }

            //Update the other ranks about the position/velocity of the spheres
            MPI_Bcast(&sphere4_m.position, 3, vector3_type, 0, ray_tracing_comm);
            sphere4.sphere.position = sphere4_m.position;
            spheres[1] = sphere4;

            //simple_move_spheres(spheres,t);
        }
        MPI_Barrier(ray_tracing_comm);
        elapsed_time = MPI_Wtime()  - start_time;
        free(partial_image);

        if (ray_tracing_rank == 0) {
            printf("\nRay tracing CPU time: %f\n", elapsed_time);
        }
    }


    if (dynamics_rank != MPI_UNDEFINED) {
        MPI_Comm_create_group(MPI_COMM_WORLD, dynamics_group, 0, &dynamics_comm);
        MPI_Intercomm_create(dynamics_comm, 0, MPI_COMM_WORLD, 2, 0, &messenger_com);

        printf("World rank %d: dynamics_rank = %d\n", rank, dynamics_rank);
        //printf("World rank %d: messenger_com = %d\n", rank, messenger_com);

        for (int t = 0; t < number_of_time_steps; t++) {
            if (dynamics_rank == 0) {
                vector3 force1 = calculate_force(sphere4_m, sphere3_m);
                vector3 acc1 = calculate_acceleration(sphere4_m, force1);
                update_position_euler_explicit(&sphere4_m, acc1, dt);

                //TEMPORARY ONLY USING 1
                MPI_Send(&sphere4_m.position, 3, vector3_type, 0, 0, messenger_com);
            }
            MPI_Barrier(dynamics_comm);
        }

        MPI_Barrier(dynamics_comm);
        elapsed_time = MPI_Wtime()  - start_time;

        if (dynamics_rank == 0) {
            printf("\nDynamics calculation time: %f\n", elapsed_time);
        }
    }

    free(image);

    MPI_Group_free(&ray_tracing_group);
    MPI_Group_free(&dynamics_group);
    MPI_Type_free(&pixel_colour_type);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    if (rank == 0) {
        printf("END PROGRAM\n");
    }
    return 0;
}

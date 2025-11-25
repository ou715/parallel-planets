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

#define _GNU_SOURCE
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "dynamics/move.h"
#include "mpi/mpi.h"
#include "ray-tracing/geometry.h"
#include "ray-tracing/ray_trace.h"
#include "utils/io.h"
#include "utils/mpi_utils.h"
#include "utils/scene.h"


int main(int argc, char **argv) {

    //MPI RELATED SETUP
    MPI_Init(&argc, &argv);

    option_arguments option_arguments;

    static char doc[] = "N body simulation using OpenMPI";
    struct argp argp = {options, parse_options, 0, doc};
    argp_parse(&argp, argc, argv, 0, 0, &option_arguments);

    char *output_root = option_arguments.output_directory;
    char *sphere_input_path = option_arguments.sphere_input_file;
    char *ranks_config_input_path = option_arguments.ranks_file;
    int number_of_time_steps = option_arguments.number_of_steps;
    double dt = option_arguments.step_size;

    int render_n = 10; //Determines how often does a simulation step gets rendered

    char *input_file_name = basename(sphere_input_path);

    int rank, namelen;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &namelen);

    // printf("Processor %s, %d out of %d processors, reporting for duty\n",
    //processor_name, rank, world_size);

    //Define custom MPI data type; will be used for communicating
    MPI_Datatype pixel_colour_type = create_pixel_colour_mpi_type();
    MPI_Datatype vector3_type = create_vector3_mpi_type();
    MPI_Datatype sphere_type = create_sphere_mpi_type();
    MPI_Datatype solid_colour_sphere_type = create_solid_colour_sphere_mpi_type();

    //Fix padding for nested sphere type
    MPI_Datatype fixed_solid_colour_sphere_type;
    MPI_Aint lower_bound, old_extent;
    MPI_Type_get_extent(solid_colour_sphere_type, &lower_bound, &old_extent);
    MPI_Type_create_resized(solid_colour_sphere_type, lower_bound, sizeof(solid_colour_sphere), &fixed_solid_colour_sphere_type);
    MPI_Type_commit(&fixed_solid_colour_sphere_type);

    //Define communicators
    MPI_Group world_group, ray_tracing_group, dynamics_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    int ray_tracing_ranks_number;
    int dynamics_ranks_number;
    int *ray_tracing_ranks;
    int *dynamics_ranks;
    rank_configuration input_rank_configuration;

    if (rank == 0) {
        printf("START PROGRAM\n\n");
        printf("=================================\n");
        printf("Rank input file path: %s \n", ranks_config_input_path);
        input_rank_configuration = read_rank_configuration(ranks_config_input_path);
        ray_tracing_ranks_number = input_rank_configuration.number_of_ray_tracing_ranks;
        dynamics_ranks_number = input_rank_configuration.number_of_dynamic_ranks;
        ray_tracing_ranks = input_rank_configuration.ray_tracing_ranks;
        dynamics_ranks = input_rank_configuration.dynamic_ranks;

        printf("Cores allocated to dynamic calculations: %i \n", dynamics_ranks_number);
        printf("Cores allocated to ray tracing calculations: %i \n", ray_tracing_ranks_number);

        //printf("Input rank configuration mapped!\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Bcast(&ray_tracing_ranks_number, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dynamics_ranks_number, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // printf("N of dynamic ranks is : %d \n\n", dynamics_ranks_number);
    // printf("N of rt ranks is : %d \n\n", ray_tracing_ranks_number);

    if (rank != 0) {
        ray_tracing_ranks = malloc(ray_tracing_ranks_number * sizeof(int));
        dynamics_ranks = malloc(dynamics_ranks_number * sizeof(int));
    }

    MPI_Bcast(ray_tracing_ranks, ray_tracing_ranks_number, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(dynamics_ranks, dynamics_ranks_number, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Comm ray_tracing_comm, dynamics_comm, messenger_com;

    MPI_Group_incl(world_group, ray_tracing_ranks_number, ray_tracing_ranks, &ray_tracing_group);
    MPI_Group_incl(world_group, dynamics_ranks_number, dynamics_ranks, &dynamics_group);

    //NOTE - world rank of dynamics group is assumed to be 0, and is 1 for ray tracing
    // TODO, make these the first element of the ranks arrays (or even configurable)
    int ray_tracing_rank, dynamics_rank;
    MPI_Group_rank(ray_tracing_group, &ray_tracing_rank);
    MPI_Group_rank(dynamics_group, &dynamics_rank);

    MPI_Barrier(MPI_COMM_WORLD);

    //IMAGE RELATED SETUP
    //TODO read these from an input file or command line
    const double aspect_ratio = 1.6; // almost as good as 4:3
    const int image_height = 72 * 5; //for even division of work; should be generalized
    const int image_width = image_height * aspect_ratio;

    scene scene = scene_init(image_width, image_height);
    pixel_colour *image = malloc(image_width * image_height * sizeof(pixel_colour));

    //Setup objects
    int number_of_spheres;
    if (rank == 0) {
        printf("=================================\n");
        printf("Reading sphere initial values from: %s\n", sphere_input_path);
        number_of_spheres = read_sphere_number(sphere_input_path);
        printf("The number of input spheres: %d\n", number_of_spheres);
    }
    //The number_of_spheres is needed to allocate the correct amount of
    // memory to hold the sphere information

    MPI_Bcast(&number_of_spheres, 1, MPI_INT, 0, MPI_COMM_WORLD);

    solid_colour_sphere *spheres = malloc(number_of_spheres * sizeof(solid_colour_sphere));
    if (!spheres) {
        printf("Spheres arrays is not allocated for rank %d\n", rank);
    }

    if (rank == 0) {
        read_sphere_configuration(sphere_input_path, spheres, number_of_spheres);
    }

    //CAUSES SEGMENTATION FAULT WITHOUT -o3
    MPI_Bcast(spheres, number_of_spheres, fixed_solid_colour_sphere_type, 0, MPI_COMM_WORLD);

    //Setup light
    //TODO Consider storing in a list, or a file
    //TODO Make the lights dynamic
    vector3 point_light = {.x = 0.0, .y = 1500.0, .z = -20.0};

    int number_of_rows_per_process = image_height / ray_tracing_ranks_number;
    int rows_to_process[image_height];

    if (rank == 0) {
        printf("\n============================");
        printf("\nImage rendering information:\n");
        printf("Image width is %d and image height is %d\n", image_width, image_height);
        printf("The eye is positioned at x: %.2f, y: %.2f, z: %.2f\n", scene.eye_position.x, scene.eye_position.y, scene.eye_position.z);
        printf("Rows to be rendered by each process:  %d\n", number_of_rows_per_process );

        printf("\n==============================");
        printf("\nN body simulation information:\n");
        printf("The number of time steps is: %d\n", number_of_time_steps);
        printf("The size of the time step is: %.5f\n", dt);
        printf("\nOutput root is: %s\n", output_root);

        printf("\nSTART SIMULATION\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    //TODO Find out why is a dynamic array causing segmentation fault when finalizing
    //sphere *spheres_g = malloc(number_of_spheres * sizeof(sphere));
    //TODO merge this with the main sphere struct array. Careful not to require rewrite if additional sphere types get added
    sphere spheres_g[4];
    for (int m = 0; m < number_of_spheres; m++) {
        spheres_g[m] = spheres[m].sphere;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double start_time, elapsed_time;
    start_time = MPI_Wtime();

    if (ray_tracing_rank != MPI_UNDEFINED) {
        MPI_Comm_create_group(MPI_COMM_WORLD, ray_tracing_group, 0, &ray_tracing_comm);
        MPI_Intercomm_create(ray_tracing_comm, 0, MPI_COMM_WORLD, 0, 0, &messenger_com);
        int *received_rows = malloc(number_of_rows_per_process * sizeof(int));
        int pixels_to_receive = image_width * number_of_rows_per_process; //also the number of total pixels per partial image
        char image_output_path[PATH_MAX];

        if (ray_tracing_rank == 0) {
            //Sending data in consecutive rows. There are probably more efficient ways to split the work,
            //if more advanced rendering techniques were in use
            //The simple one has all the rays completely independent
            allocate_rows_to_processes_blocks(image_height, rows_to_process);
        }

        //printf("World rank = %d : ray_tracing_rank = %d\n", rank, ray_tracing_rank);

        pixel_colour *partial_image = malloc(pixels_to_receive * sizeof(pixel_colour));

        MPI_Scatter(rows_to_process, number_of_rows_per_process, MPI_INT, received_rows,
                    number_of_rows_per_process, MPI_INT, 0, ray_tracing_comm);
        //printf("All scattered\n");

        //Main time loop
        for (int t = 0; t < number_of_time_steps; t++) {
            //printf("Total pixels to be processed by each process:  %d\n", pixels_to_receive );
            //
            if (t % render_n == 0) {

            //Actual work
            render_pixels(image_width, received_rows, number_of_rows_per_process, scene, spheres, number_of_spheres, point_light, partial_image, rank);
            //printf("All rendered\n");
            //Combine partial images to root rank
            MPI_Gather(partial_image, pixels_to_receive, pixel_colour_type, image, pixels_to_receive, pixel_colour_type, 0, ray_tracing_comm);
            }
            //Output
            if (ray_tracing_rank == 0) {
                //printf("The position of sphere3 is x: %.2f, y: %.2f, z: %.2f\n", sphere3.sphere.position.x, sphere3.sphere.position.y, sphere3.sphere.position.z);
                //printf("The position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere3_m.position.x, sphere3_m.position.y, sphere3_m.position.z);
                if (t % render_n == 0) {
                    snprintf(image_output_path, sizeof(image_output_path), "%s/video/image_%04d.png", output_root, t/render_n);
                    save_image_png(image, image_width, image_height, image_output_path);
                }

                for (int b1 = 0; b1 < number_of_spheres; b1++) {
                    MPI_Recv(&spheres_g[b1].position, 3, vector3_type, 0, 0, messenger_com, MPI_STATUS_IGNORE);
                }
            }
            //Update the other ranks about the position/velocity of the spheres
            for (int b1 = 0; b1 < number_of_spheres; b1++) {
                MPI_Bcast(&spheres_g[b1].position, 3, vector3_type, 0, ray_tracing_comm);
                spheres[b1].sphere.position = spheres_g[b1].position;
            }
        }
        free(partial_image);
        free(image);

        MPI_Barrier(ray_tracing_comm);
        elapsed_time = MPI_Wtime()  - start_time;

        if (ray_tracing_rank == 0) {
            printf("\nRay tracing CPU time: %f\n", elapsed_time);
        }
    }

    if (dynamics_rank != MPI_UNDEFINED) {
        MPI_Comm_create_group(MPI_COMM_WORLD, dynamics_group, 0, &dynamics_comm);
        MPI_Intercomm_create(dynamics_comm, 0, MPI_COMM_WORLD, 1, 0, &messenger_com);

        //printf("World rank = %d : dynamics_rank    = %d\n", rank, dynamics_rank);

        //Holds a list of integers that will be used for indexing spheres
        int *all_sphere_indices = malloc(number_of_spheres * sizeof(int));
        int equal_parts = number_of_spheres / dynamics_ranks_number;

        int local_sphere_count = equal_parts + (number_of_spheres % dynamics_ranks_number) * (dynamics_ranks_number % (dynamics_rank + 1));
        //printf("-local_sphere_count %i\n", local_sphere_count);


        for (int i = 0; i < local_sphere_count; i++) {
            all_sphere_indices[i] = dynamics_rank * equal_parts + i;
            //printf("Dynamics rank %i is being allocated sphere with index %i\n", dynamics_rank, dynamics_rank * equal_parts + i);
        }

        sphere *spheres_to_update = malloc(local_sphere_count * sizeof(sphere));
        MPI_Scatter(spheres_g, local_sphere_count, sphere_type, spheres_to_update,
                    local_sphere_count, sphere_type, 0, dynamics_comm);

        char text_output_path[PATH_MAX];
        FILE **text_output_files, *energy_output_file;
        double kinetic_energy, potential_energy, total_energy;

        if (dynamics_rank == 0) {
            text_output_files = malloc(number_of_spheres * sizeof(FILE));
            snprintf(text_output_path, sizeof(text_output_path), "%s/txt/system_energy_%s.txt", output_root, input_file_name);
            energy_output_file = fopen(text_output_path, "w");
            for (int o = 0; o < number_of_spheres; o++) {
                snprintf(text_output_path, sizeof(text_output_path), "%s/txt/sphere_%s_%03d.txt", output_root, input_file_name, o);
                text_output_files[o] = fopen(text_output_path, "w");
            }
        }

        //printf("World rank %d: messenger_com = %d\n", rank, messenger_com);
        for (int t = 0; t < number_of_time_steps; t++) {
            //TEMPORARY ONLY USING 1
            //printf("Current time is %.1f\n", t*dt);
            if (dynamics_rank == 0) {

                for (int b1 = 0; b1 < number_of_spheres; b1++) {

                    fprintf(text_output_files[b1], "%d %f %lf %lf %lf %lf %lf\n",
                                                            t,
                                                            spheres_g[b1].position.x,
                                                            spheres_g[b1].position.y,
                                                            spheres_g[b1].position.z,
                                                            spheres_g[b1].velocity.x,
                                                            spheres_g[b1].velocity.y,
                                                            spheres_g[b1].velocity.z);

                }
                kinetic_energy = calculate_kinetic_energy(spheres_g, number_of_spheres);
                potential_energy = calculate_potential_energy(spheres_g, number_of_spheres);
                total_energy = kinetic_energy + potential_energy;
                fprintf(energy_output_file, "%lf %lf %lf %lf\n",
                                                        t * dt,
                                                        kinetic_energy,
                                                        potential_energy,
                                                        total_energy);
            }
            for (int b1 = 0; b1 < local_sphere_count; b1++) {
                update_position_velocity_verlet(&spheres_to_update[b1], all_sphere_indices[b1], spheres_g, number_of_spheres, dt);
            }

            MPI_Allgather(spheres_to_update, local_sphere_count, sphere_type, spheres_g,
                                          local_sphere_count, sphere_type, dynamics_comm);

            if (dynamics_rank == 0) {
                //TODO send an array instead
                for (int b1 = 0; b1 < number_of_spheres; b1++) {
                    //printf("Accessing sphere %i\n", b1);
                    //Potential stack-buffer-overflow
                    MPI_Send(&spheres_g[b1].position, 3, vector3_type, 0, 0, messenger_com);
                    //printf("Sending coordinates of sphere %d - x: %.2f y: %.2f z: %.2f\n", all_sphere_indices[b1], spheres_g[all_sphere_indices[b1]].position.x, spheres_g[all_sphere_indices[b1]].position.y, spheres_g[all_sphere_indices[b1]].position.z);
                }
            }
        }

        if (dynamics_rank == 0) {
            for (int o = 0; o < number_of_spheres; o++) {
                fclose(text_output_files[o]);
            }
            free(text_output_files);
            fclose(energy_output_file);
        }

        free(all_sphere_indices);
        MPI_Barrier(dynamics_comm);

        elapsed_time = MPI_Wtime()  - start_time;

        if (dynamics_rank == 0) {
            printf("\nDynamics calculation time: %f\n", elapsed_time);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    free(spheres);
    //free(spheres_g);

    MPI_Group_free(&ray_tracing_group);
    MPI_Group_free(&dynamics_group);

    MPI_Type_free(&pixel_colour_type);
    MPI_Type_free(&vector3_type);
    MPI_Type_free(&sphere_type);
    MPI_Type_free(&solid_colour_sphere_type);
    MPI_Type_free(&fixed_solid_colour_sphere_type);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    if (rank == 0) {
        printf("END PROGRAM\n");
    }
    return 0;
}

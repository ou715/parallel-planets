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

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <linux/limits.h>
#include <libgen.h>
#include <stdlib.h>

#include "ray-tracing/geometry.h"
#include "ray-tracing/ray_trace.h"
#include "utils/io.h"
#include "utils/scene.h"
#include "mpi/mpi.h"

/**
 *
 * @param ray
 * @return
 */
pixel_colour background_colour(const ray ray) {
    pixel_colour colour;

    vector3 ray_unit_direction = unit_vector(ray.direction);

    colour.r = (uint8_t) ((ray_unit_direction.x + 1) * 117.0);
    colour.g = (uint8_t) ((ray_unit_direction.y + 1) * 34 +
               (uint8_t) ((ray_unit_direction.x + 1)  * 34));
    colour.b = (uint8_t) ((ray_unit_direction.y + 1) * 117.0);

    //printf("The ray unit direction is x: %.2f, y: %.2f, z: %.2f\n", ray_unit_direction.x, ray_unit_direction.y, ray_unit_direction.z);

    return colour;
}

void allocate_rows_to_processes_blocks(int image_height, int *rows_to_process) {
    for (int i = 0; i < ( image_height); i++) {
        rows_to_process[i] = i;
    }
}

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

/**
 *
 * @param image_width
 * @param rows_to_process
 * @param number_of_rows
 * @param scene
 * @param spheres
 * @param point_light
 * @param image
 * @param process_index
 */
void render_pixels(
                    const int image_width,
                    const int *rows_to_process,
                    const int number_of_rows,
                    scene scene,
                    const solid_colour_sphere *spheres,
                    vector3 point_light, pixel_colour *image,
                    int process_index) {


    for (int i = 0; i < number_of_rows; i++) {
        for (int j = 0; j < image_width; ++j) {
            if (process_index == 1) {
                //printf("Currently processing row %d\n", rows_to_process[i]);
            }
            //printf("Currently processing column %d\n", j);
            vector3 pixel_location = add_vector3(add_vector3(scene.viewscreen_first_pixel_location,
                                                             vector3_multiply_by_scalar(scene.viewscreen_delta_u, j)),
                                                 vector3_multiply_by_scalar(scene.viewscreen_delta_v, rows_to_process[i]));

            //printf("The calculating pixel at location: x: %.2f, y: %.2f, z: %.2f\n", pixel_location.x, pixel_location.y, pixel_location.z);

            ray ray = {.direction = subtract_second_vector3_from_first(pixel_location, scene.eye_position), .origin = scene.eye_position};

            pixel_colour colour = background_colour(ray);

            hit_sphere potential_sphere_hit = intersected_sphere_index(ray, spheres, 2);
            if (potential_sphere_hit.sphere_index != -1) {
                colour = shade(ray, potential_sphere_hit.ray_intersection, spheres[potential_sphere_hit.sphere_index], point_light);
                //printf("Shading sphere %d\n", potential_sphere_hit.sphere_index);
            }
            // if (process_index == 1) {
            //     printf("Process %d attempting to write to %d\n", process_index, i * image_width + j);
            // }
            image[i * image_width + j] = colour;
        }
    }
    //printf("Process %d finished rendering\n", process_index);

}

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


int main(int argc, char **argv) {
    clock_t begin = clock();
    char *output_root = argv[1];

    //MPI RELATED SETUP

    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Get_processor_name(processor_name, &namelen);
    // printf("Processor %s, %d out of %d processors, reporting for duty\n",
    //    processor_name, rank, world_size);

    //Define custom MPI data type; will be used for communicating
    MPI_Datatype pixel_colour_type = create_pixel_colour_mpi_type();

    //IMAGE RELATED SETUP

    const double aspect_ratio = 1.6; // almost as good as 4:3
    const int image_height = 1200; //for even division of work; should be generalized
    const int image_width = image_height * aspect_ratio; //for even division of work; should be generalized

    ///////

    scene scene = scene_init(image_width, image_height);
    pixel_colour *image = malloc(image_width * image_height * sizeof(pixel_colour));


    //printf("Image width is %d and image height is %d\n", image_width, image_height);

    //Useful information
    //printf("The eye is positioned at x: %.2f, y: %.2f, z: %.2f\n", scene.eye_position.x, scene.eye_position.y, scene.eye_position.z);

    //Setup objects
    //sphere sphere1 = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}};
    //TODO Read object positions from a file at runtime
    //solid_colour_sphere sphere2 = {.sphere = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere3 = {.sphere = {.radius = 5, .position = {.x = 10.0, .y = -5.0, .z = -20.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere4 = {.sphere = {.radius = 5, .position = {.x = -10.0, .y = 5.0, .z = -25.0}}, .colour = {.r = 150, .g = 0, .b = 0}};

    solid_colour_sphere spheres[2] = { sphere3, sphere4 };

    //Setup light
    //Parallel light source (could be a distant spherical light source)
    vector3 light = {.x = 0.0, .y = -1.0, .z = 0.0}; //not used at the moment

    //TODO Consider storing in a list, or a file
    vector3 point_light = {.x = 0.0, .y = 15000.0, .z = -20.0};

    int number_of_rows_per_process = image_height / world_size;

    int rows_to_process[image_height];

    if (rank == 0) {
        printf("START PROGRAM\n");

        printf("Image width is %d and image height is %d\n", image_width, image_height);
        printf("The eye is positioned at x: %.2f, y: %.2f, z: %.2f\n", scene.eye_position.x, scene.eye_position.y, scene.eye_position.z);
        printf("Output root is:  %s\n", output_root);

        printf("Rows per process:  %d\n", number_of_rows_per_process );
    }

    //Main time loop
    int number_of_time_steps = 10;
    for (int t = 0; t < number_of_time_steps; t++) {
        if (rank == 0) {

            //Sending data in consecutive rows. There are probably more efficient ways to split the work,
            //if more advanced rendering techniques were in use
            //The simple one has all the rays completely independent
            allocate_rows_to_processes_blocks(image_height, rows_to_process);
        }

        int received_rows[number_of_rows_per_process];

        int pixels_to_receive = image_width * number_of_rows_per_process; //also the number of total pixels per partial image
        //printf("Total pixels to be processed by each process:  %d\n", pixels_to_receive );

        pixel_colour *partial_image = malloc(pixels_to_receive * sizeof(pixel_colour));
        if (partial_image == NULL) {
            printf("Failed to allocate memory for partial image\n");
        }

        MPI_Scatter(rows_to_process, number_of_rows_per_process, MPI_INT, received_rows,
            number_of_rows_per_process, MPI_INT, 0, MPI_COMM_WORLD);


        // if (rank == 1) {
        //     for (int i = 0; i < number_of_rows_per_process; i++) {
        //         printf("Rows to render for process %d : %d \n", rank, received_rows[i]);
        //     }
        // }

        //printf("All scattered\n");

        //Actual work
        render_pixels(image_width, received_rows, number_of_rows_per_process, scene, spheres,point_light, partial_image, rank);

        //printf("All rendered\n");

        MPI_Gather(partial_image, pixels_to_receive, pixel_colour_type, image, pixels_to_receive, pixel_colour_type, 0, MPI_COMM_WORLD);

        //Output
        if (rank == 0) {
            char image_output_path[PATH_MAX];
            snprintf(image_output_path, sizeof(image_output_path), "%s/outputs/image_%d.png", output_root, t);

            save_image_png(image, image_width, image_height, image_output_path);

        }

        MPI_Barrier(MPI_COMM_WORLD);

        free(partial_image);
        simple_move_spheres(spheres,t);
    }
    free(image);

    MPI_Type_free(&pixel_colour_type);
    MPI_Finalize();
    //Should work okay for a single thread run time
    //TODO Improve time calculation
    const clock_t end = clock();
    const double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    if (rank == 0) {
        printf("END PROGRAM\n");
        printf("\nExecution CPU time: %f\n", time_spent);
    }
    return 0;
}

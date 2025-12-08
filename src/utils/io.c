//
// Created by nemo on 14/10/25.
//
#define STBI_WRITE_NO_COMPRESS 1
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "io.h"
#include <errno.h>
#include "../../dependencies/stb_image_write.h"

/**
 * This function writes an image to the specified path.
 * Note: If the parent directory doesn't exist, this function will fail to save silently.
 * @image The image to be saved
 * @image_width
 * @image_height
 * @file_path The destination path of the .png image
 */
void save_image_png(const pixel_colour *image, int image_width, int image_height, char *file_path) {
    const int row_size_bytes = image_width * sizeof(pixel_colour);
    //printf("Image byte_stride is %d\n", row_size_bytes);

    //printf("Saving image to relative path %s\n", filename);
    stbi_write_png(file_path, image_width, image_height, 3, image, row_size_bytes);
    //stbi_write_bmp(filename, image_width, image_height, 3, image);
    //printf("Saving image to relative path %s\n", filename);
}

/**
 * This function takes a file path string and return an integer from the first line of that file
 * @param file_path File containing information about the spheres
 * @return The number specified in the first line of the input file
 */
int read_sphere_number(char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "File %s cannot be opened!\n", file_path);
            printf("Error %d: %s\n", errno, strerror(errno));
        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        //printf("Input file opened succesfully! Now reading the first line. \n");
    }
    int sphere_number;
    if (fscanf(file, "%d", &sphere_number) != 1) {
        fprintf(stderr, "Number of sphere is incorrectly specified! It should be a string at the start of the file\n");
        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        //printf("The number of input spheres is %d\n", sphere_number);
    }
    fclose(file);
    return sphere_number;
}

//TODO handle printf debugging more gracefully
/**
 * Function which takes the information from a text file of a specific format and maps it into a sequence of solid_colour_sphere
 * x y z vx vy vz rad r g b
 * Where xyz are coordinates, vxvyvz are corresponding velocities, rad is radius and rgb are 0-255 colour values
 * @param file_path File containing information about the spheres
 * @param number_of_spheres The number of spheres in the file
 * @param[out] spheres The variable to write the information from the file
 */
void read_sphere_configuration(char *file_path, solid_colour_sphere *spheres, int number_of_spheres) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "File %s cannot be opened!\n", file_path);
        printf("Error %d: %s\n", errno, strerror(errno));
        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        //printf("Input file opened succesfully! Now reading the remaining %d lines\n", number_of_spheres);
    }
    //TODO handle skipping the number of spheres in a more elegant fashion
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("The first line of the file is empty, while it should contain the number of spheres!\n");
    }

    for (int i = 0; i < number_of_spheres; i++) {
        //printf("Mapping sphere %d\n", i);
        if (fscanf(file, "%lf %lf %lf %lf %lf %lf", &spheres[i].sphere.position.x,
                                                    &spheres[i].sphere.position.y,
                                                    &spheres[i].sphere.position.z,
                                                    &spheres[i].sphere.velocity.x,
                                                    &spheres[i].sphere.velocity.y,
                                                    &spheres[i].sphere.velocity.z) != 6) {
                                                    fprintf(stderr, "Error - first 6 entries in each row should be doubles, denoting position and velocity.\n");
                                                    fprintf(stderr, "Exiting\n");
                                                    exit(EXIT_FAILURE);
                                                } else {
                                                    // printf("The position of sphere %d is x: %.2f, y: %.2f, z: %.2f\n", i, spheres[i].sphere.position.x,
                                                    //  spheres[i].sphere.position.y, spheres[i].sphere.position.z);
                                                    // printf("The velocity of sphere %d is x: %.2f, y: %.2f, z: %.2f\n", i, spheres[i].sphere.velocity.x,
                                                    //  spheres[i].sphere.velocity.y, spheres[i].sphere.velocity.z);
                                                }

        if (fscanf(file, "%lf", &spheres[i].sphere.radius) != 1) {
            fprintf(stderr, "Error - the 7th entry should be the radius of the sphere.\n");
            fprintf(stderr, "Exiting\n");
            exit(EXIT_FAILURE);
        } else {
            //printf("The radius of the sphere %d is %.2f\n", i, spheres[i].sphere.radius);
        }

        if (fscanf(file, "%lf", &spheres[i].sphere.mass) != 1) {
            fprintf(stderr, "Error - the 8th entry should be the mass of the sphere\n");
            fprintf(stderr, "Exiting\n");
            exit(EXIT_FAILURE);
        } else {
            //printf("The mass of the sphere %d is %.2f\n", i, spheres[i].sphere.mass);
        }

        if (fscanf(file, "%hhu %hhu %hhu ", &spheres[i].colour.r,
                                            &spheres[i].colour.g,
                                            &spheres[i].colour.b) != 3) {
                                            fprintf(stderr, "Error - entries 9, 10 and 11 should be integers lower than 256, denoting rgb values\n");
                                            fprintf(stderr, "Exiting\n");
                                            exit(EXIT_FAILURE);
        } else {
            //printf("The colour of the sphere is %i %i %i\n", spheres[i].colour.r, spheres[i].colour.g, spheres[i].colour.b);
        }
        //printf("Done mapping sphere %d\n", i);
    }
    //printf("Succesfully loaded %i spheres!\n", number_of_spheres);
    // printf("The position of the sphere 1 is %i %i %i\n",spheres[0]->colour.r, spheres[0]->colour.g, spheres[0]->colour.b);
    // printf("The velocity of the sphere 1 is %.2f %.2f %.2f\n", spheres[0]->sphere.velocity.x, spheres[0]->sphere.velocity.x, spheres[0]->sphere.velocity.x);
    fclose(file);
}

//TODO handle printf debugging more gracefully
/**
 * Function which takes the information from a text file of a specific format and maps it into a struct rank_configuration
 * Example input
 * 4
 * 0 1 2 3
 * 2
 * 4 5
 * Where xyz are coordinates, vxvyvz are corresponding velocities, rad is radius and rgb are 0-255 colour values
 * @param file_path File containing information about the spheres
 * @return A struct containing the count and arrays of dynamic and ray tracing ranks
 */
rank_configuration read_rank_configuration(char *file_path) {
    FILE *file = fopen(file_path, "r");
    rank_configuration input_rank_configuration;

    if (fscanf(file, "%i", &input_rank_configuration.number_of_ray_tracing_ranks) != 1) {
        fprintf(stderr, "Number of ray tracing ranks is incorrectly specified! It should be an integer on line 1\n");
        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        //printf("The number of dynamic ranks is %d\n", input_rank_configuration.number_of_ray_tracing_ranks);
    }

    int *ray_tracing_ranks = malloc(input_rank_configuration.number_of_ray_tracing_ranks * sizeof(int));

    for (int i = 0; i < input_rank_configuration.number_of_ray_tracing_ranks; i++) {
        if (fscanf(file, "%i", &ray_tracing_ranks[i]) != 1) {
            fprintf(stderr, "Dynamic rank incorrectly specified! It should be an integer\n");
            fprintf(stderr, "Exiting\n");
            exit(EXIT_FAILURE);
        } else {
            //printf("The %d th ray tracing rank is - %d\n", i, ray_tracing_ranks[i]);
        }
    }

    if (fscanf(file, "%i", &input_rank_configuration.number_of_dynamic_ranks) != 1) {
        fprintf(stderr, "Number of ray tracing ranks is incorrectly specified! It should be an integer on line 3\n");
        fprintf(stderr, "Exiting\n");
        exit(EXIT_FAILURE);
    } else {
        //printf("The number of ray tracing ranks is %d\n", input_rank_configuration.number_of_dynamic_ranks);
    }

    int *dynamic_ranks = malloc(input_rank_configuration.number_of_dynamic_ranks * sizeof(int));

    for (int i = 0; i < input_rank_configuration.number_of_dynamic_ranks; i++) {
        if (fscanf(file, "%i", &dynamic_ranks[i]) != 1) {
            fprintf(stderr, "Dynamic rank incorrectly specified! It should be an integer\n");
            fprintf(stderr, "Exiting\n");
            exit(EXIT_FAILURE);
        } else {
           // printf("The %d dynamic th rank is - %d\n", i, dynamic_ranks[i]);
        }
    }
    input_rank_configuration.ray_tracing_ranks = ray_tracing_ranks;
    input_rank_configuration.dynamic_ranks = dynamic_ranks;
    //printf("Returning ranking configuration - read_rank_configuration\n");

    return input_rank_configuration;
}

error_t parse_options(int key, char *arg, struct argp_state *state) {
   	struct option_arguments *arguments = state->input;

    switch (key) {
        case 'o':
            arguments->output_directory = arg;
            break;
        case 'i':
            arguments->sphere_input_file = arg;
            break;
        case 'n':
            arguments->number_of_steps = atoi(arg);
            break;
        case 'h':
            arguments->step_size = atof(arg);
            break;
        case 'r':
            arguments->ranks_file = arg;
            break;
        case 'f':
            arguments->file_output = atoi(arg);
            break;
        case 'k':
            arguments->render_step_ratio = atoi(arg);
            break;
        case 'v':
            arguments->image_height = atoi(arg);
            break;
    }
    return 0;
}

void output_benchmark(const char *output_root,
                      int number_of_time_steps,
                      double dt,
                      int ray_tracing_ranks_number,
                      int dynamics_ranks_number,
                      int number_of_spheres,
                      int image_height,
                      int render_n,
                      double ray_tracing_elapsed_time,
                      double dynamic_elapsed_time,
                      double ray_tracing_time_no_intercom,
                      double dynamic_time_no_intercom) {

    char benchmark_file_path[PATH_MAX];
    char benchmark_row[200];

    snprintf(benchmark_file_path, sizeof(benchmark_file_path), "%s/benchmarks/benchmark_%i_%.3f.csv", output_root, number_of_time_steps, dt);
    FILE *benchmark_file;
    if((benchmark_file = fopen(benchmark_file_path,"r")) != NULL) {
        benchmark_file = fopen(benchmark_file_path,"a");
    }
    else {
        benchmark_file = fopen(benchmark_file_path,"w");
        fprintf(benchmark_file, "rt_ranks,d_ranks,n_bodies,image_height,render_n,rt_time,d_time,rt_time_elapsed,d_time_elapsed");
    }
    fprintf(benchmark_file, "\n%d,%d,%d,%d,%d,%.3f,%.3f,%.3f,%.3f", ray_tracing_ranks_number,
                                                                    dynamics_ranks_number,
                                                                    number_of_spheres,
                                                                    image_height,
                                                                    render_n,
                                                                    ray_tracing_elapsed_time,
                                                                    dynamic_elapsed_time,
                                                                    ray_tracing_time_no_intercom,
                                                                    dynamic_time_no_intercom);
    fclose(benchmark_file);
}

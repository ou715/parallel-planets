#ifndef IO_H
#define IO_H
#define STBIW_ZLIB_COMPRESSION_LEVEL 1

#include "../ray-tracing/ray_trace.h"
#include <unistd.h>
#include <argp.h>

static struct argp_option options[] = {
    {"output_root_path", 'o', "OUTPUT_ROOT_PATH", 0, "Root for output files"},
    {"sphere_input_file", 'i', "INPUT_FILE_PATH", 0, "Path to file containing sphere coordinates"},
    {"n_steps", 'n', "NUMBER_OF_STEPS", 0, "Number of steps to run the simulation for"},
    {"step_size", 'h', "STEP SIZE", 0, "Size of a single step"},
    {"ranks_file", 'r', "VALUE", 0, "Path to rank configuration file"},
    {"image_output", 'f', "VALUE", 0, "Flag for saving file outputs"},
    {"render_step_ratio", 'k', "VALUE", 0, "Determines the ratio of time steps calculated to rendered"},
    {0}
};

typedef struct option_arguments {
    char *output_directory;
    char *sphere_input_file;
    int number_of_steps;
    double step_size;
    char *ranks_file;
    int file_output;
    int render_step_ratio;
} option_arguments;

typedef struct rank_configuration {
    int number_of_ray_tracing_ranks;
    int *ray_tracing_ranks;
    int number_of_dynamic_ranks;
    int *dynamic_ranks;
} rank_configuration;

void save_image_png(const pixel_colour *image, int image_width, int image_height, char *filename);
int read_sphere_number(char *file_path);
void read_sphere_configuration(char *file_path, solid_colour_sphere *spheres, int number_of_spheres);
rank_configuration read_rank_configuration(char *file_path);
error_t parse_options(int key, char *arg, struct argp_state *state);
void output_benchmark(const char *output_root,
                      int number_of_time_steps,
                      double dt,
                      int ray_tracing_ranks_number,
                      int dynamics_ranks_number,
                      int number_of_spheres,
                      int image_height,
                      int render_n,
                      double ray_tracing_elapsed_time,
                      double dynamic_elapsed_time);

#endif //IO_H

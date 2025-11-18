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
    //{"rank_file", 'r', "VALUE", 0, "Size of a single step"},
    {0}
};

typedef struct option_arguments {
    char *output_directory;
    char *sphere_input_file;
    int number_of_steps;
    double step_size;
    char *ranks_file;
} option_arguments;

void save_image_png(const pixel_colour *image, int image_width, int image_height, char *filename);
int read_sphere_number(char *file_path);
void read_sphere_configuration(char *file_path, solid_colour_sphere *spheres, int number_of_spheres);
error_t parse_options(int key, char *arg, struct argp_state *state);
#endif //IO_H

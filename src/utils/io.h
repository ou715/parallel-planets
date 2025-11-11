#ifndef IO_H
#define IO_H
#define STBIW_ZLIB_COMPRESSION_LEVEL 1

#include "../ray-tracing/ray_trace.h"
#include <unistd.h>

void save_image_png(const pixel_colour *image, int image_width, int image_height, char *filename);
int read_sphere_number(char *file_path);
void read_sphere_configuration(char *file_path, solid_colour_sphere *spheres, int number_of_spheres);
#endif //IO_H

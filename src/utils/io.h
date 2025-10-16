//
// Created by nemo on 14/10/25.
//

#ifndef IO_H
#define IO_H
#define STBIW_ZLIB_COMPRESSION_LEVEL 1

#include "../ray-tracing/ray_trace.h"
#include <unistd.h>

void save_image_png(const pixel_colour *image, int image_width, int image_height, char *filename);

#endif //IO_H

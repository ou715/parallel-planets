//
// Created by nemo on 14/10/25.
//

#ifndef IO_H
#define IO_H

#include "../../dependencies/stb_image_write.h"
#include "../ray-tracing/ray_trace.h"

void save_image_png(const pixel_colour *image, int image_width, int image_height, const char *filename);

#endif //IO_H

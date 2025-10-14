//
// Created by nemo on 14/10/25.
//

#include "io.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../dependencies/stb_image_write.h"


void save_image_png(const pixel_colour *image, int image_width, int image_height, const char *filename) {
    const int row_size_bytes = image_width * sizeof(pixel_colour);
    //printf("Image byte_stride is %d\n", row_size_bytes);
    printf("Saving image to relative path %s\n", filename);
    stbi_write_png("../outputs/image.png", image_width, image_height, 3, image, row_size_bytes);
}

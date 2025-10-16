//
// Created by nemo on 14/10/25.
//
#define STBI_WRITE_NO_COMPRESS 1
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "io.h"

#include "../../dependencies/stb_image_write.h"

void save_image_png(const pixel_colour *image, int image_width, int image_height, char *filename) {
    const int row_size_bytes = image_width * sizeof(pixel_colour);
    //printf("Image byte_stride is %d\n", row_size_bytes);

    printf("Saving image to relative path %s\n", filename);
    stbi_write_png(filename, image_width, image_height, 3, image, row_size_bytes);
    //stbi_write_bmp(filename, image_width, image_height, 3, image);
    printf("Saving image to relative path %s\n", filename);

}

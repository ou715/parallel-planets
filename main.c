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

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stdint.h>
#include <stdio.h>
#include "dependencies/stb_image_write.h"


int main(void) {
    printf("START PROGRAM\n");

    const int image_width = 600;
    const int image_height = 400;

    struct pixel {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    struct pixel *image = calloc(image_width * image_height, sizeof(struct pixel));
    //uint8_t image[image_width * image_height * 3];

    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; ++j) {
            //printf("Currently processing row %d\n", i);
            //printf("Currently processing column %d\n", j);

            image[i * image_width + j].r = (uint8_t)(((float) j / (float)image_width) * 255.0 );
            image[i * image_width + j].g = (uint8_t)(((float) i / (float)image_height) * 17.5 ) + (uint8_t)(((float) i / (float)image_height) * 17.5 );
            image[i * image_width + j].b = (uint8_t)(((float) i / (float)image_height) * 255.0 );
        }
    }

    const int row_size_bytes = image_width * sizeof(struct pixel);

    printf("Image byte_stride is %d\n", row_size_bytes);


    stbi_write_png("../outputs/image.png",image_width ,image_height, 3, image, row_size_bytes);

    printf("END PROGRAM\n");

    return 0;
}

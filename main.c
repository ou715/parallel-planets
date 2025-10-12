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
#include <time.h>

#include "dependencies/stb_image_write.h"
#include "ray-tracing/geometry.h"


typedef struct pixel_colour {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_colour;

void draw_simple_image(pixel_colour *image, int image_width, int image_height) {
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; ++j) {
            //printf("Currently processing row %d\n", i);
            //printf("Currently processing column %d\n", j);

            image[i * image_width + j].r = (uint8_t) (((float) j / (float) image_width) * 255.0);
            image[i * image_width + j].g = (uint8_t) (((float) i / (float) image_height) * 17.5) + (uint8_t) (
                                               ((float) i / (float) image_height) * 17.5);
            image[i * image_width + j].b = (uint8_t) (((float) i / (float) image_height) * 255.0);
        }
    }
}

pixel_colour simple_ray_trace(const ray ray) {
    pixel_colour colour;

    vector3 ray_unit_direction = unit_vector(ray.direction);

    colour.r = (uint8_t) ((ray_unit_direction.x + 1) * 117.0);
    colour.g = (uint8_t) ((ray_unit_direction.y + 1) * 34 +
               (uint8_t) ((ray_unit_direction.x + 1)  * 34));
    colour.b = (uint8_t) ((ray_unit_direction.y + 1) * 117.0);

    //printf("The ray unit direction is x: %.2f, y: %.2f, z: %.2f\n", ray_unit_direction.x, ray_unit_direction.y, ray_unit_direction.z);

    return colour;
}

void save_image_png(const pixel_colour *image, int image_width, int image_height, const char *filename) {
    const int row_size_bytes = image_width * sizeof(pixel_colour);
    //printf("Image byte_stride is %d\n", row_size_bytes);
    printf("Saving image to relative path %s\n", filename);
    stbi_write_png("../outputs/image.png", image_width, image_height, 3, image, row_size_bytes);
}


int main(void) {
    clock_t begin = clock();
    printf("START PROGRAM\n");

    const int image_width = 600;
    const double aspect_ratio = 1.6; // almost as good as 4:3
    const int image_height = image_width / aspect_ratio;

    //The units are not pixels; this is in world coordinates, image variables relate to resolution that fits into this size
    //Changing these settings changes the field of vision
    const double viewscreen_height = 2.0; //eyes could be rectangles
    const double viewscreen_width = viewscreen_height * ((double) image_width / image_height);

    const double eye_to_viewscreen = 1.0;
    vector3 eye_position = {.x = 0.0, .y = 0.0, .z = 0.0};

    pixel_colour *image = calloc(image_width * image_height, sizeof(pixel_colour));

    vector3 viewscreen_u = {.x = viewscreen_width, .y = 0, .z = 0.0};
    vector3 viewscreen_v = {.x = 0, .y = -viewscreen_height, .z = 0.0};

    vector3 half_viewscreen_u = vector3_multiply_by_scalar(viewscreen_u, 0.5);
    vector3 half_viewscreen_v = vector3_multiply_by_scalar(viewscreen_v, 0.5);

    vector3 viewscreen_centre = {.x = 0.0, .y = 0.0, .z = eye_to_viewscreen};

    //should probably add more utility functions for vector operations; I really miss operator overloading

    //eye_position - viewscreen_centre - 0.5 * viewscreen_u - 0.5 * viewscreen_v
    vector3 viewscreen_upper_left = subtract_second_vector3_from_first(
                                        subtract_second_vector3_from_first(
                                            subtract_second_vector3_from_first(eye_position,
                                                                                    viewscreen_centre),
                                                                                        half_viewscreen_u),
                                                                                            half_viewscreen_v);


    //Distance between pixels in viewscreen coordinates; u aligns with x and v aligns with -y
    vector3 viewscreen_delta_u = vector3_multiply_by_scalar(viewscreen_u, 1.0 / image_width);
    vector3 viewscreen_delta_v = vector3_multiply_by_scalar(viewscreen_v, 1.0 / image_height);

    vector3 half_viewscreen_delta_u = vector3_multiply_by_scalar(viewscreen_delta_u, 0.5);
    vector3 half_viewscreen_delta_v = vector3_multiply_by_scalar(viewscreen_delta_v, 0.5);

    vector3 first_pixel_location = add_vector3(viewscreen_upper_left,
        add_vector3(half_viewscreen_delta_u, half_viewscreen_delta_v));


    //Useful information
    printf("The eye is positioned at x: %.2f, y: %.2f, z: %.2f\n", eye_position.x, eye_position.y, eye_position.z);

    //Draw
    //draw_simple_image(image, image_width, image_height);

    //Draw more
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; ++j) {
            //printf("Currently processing row %d\n", i);
            //printf("Currently processing column %d\n", j);

            vector3 pixel_location = add_vector3(add_vector3(first_pixel_location,
                                                             vector3_multiply_by_scalar(viewscreen_delta_u, j)),
                                                 vector3_multiply_by_scalar(viewscreen_delta_v, i));

            //printf("The calculating pixel at location: x: %.2f, y: %.2f, z: %.2f\n", pixel_location.x, pixel_location.y, pixel_location.z);

            ray ray = {.direction = subtract_second_vector3_from_first(pixel_location, eye_position), .origin = eye_position};

            pixel_colour colour = simple_ray_trace(ray);

            image[i * image_width + j] = colour;

        }
    }

    //Output
    save_image_png(image, image_width, image_height, "../outputs/image.png");

    printf("END PROGRAM\n");

    //Should work okay for a single thread run time
    const clock_t end = clock();
    const double time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("\nExecution CPU time: %f\n", time_spent);

    return 0;
}

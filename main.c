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
#include <stdio.h>
#include <time.h>

#include "dependencies/stb_image_write.h"
#include "ray-tracing/geometry.h"
#include "ray-tracing/ray_trace.h"

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


pixel_colour sphere_ray_trace(const ray ray) {
    pixel_colour colour;

    vector3 ray_unit_direction = unit_vector(ray.direction);

    colour.r = (uint8_t) ((ray_unit_direction.x + 1) * 117.0);
    colour.g = (uint8_t) ((ray_unit_direction.y + 1) * 34 +
               (uint8_t) ((ray_unit_direction.x + 1) * 34));
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

    const int image_width = 1200;
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

    //Setup objects
    //sphere sphere1 = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}};
    //TODO Store the spheres in an array
    //TODO Read object positions from a file at runtime
    //solid_colour_sphere sphere2 = {.sphere = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere3 = {.sphere = {.radius = 5, .position = {.x = 10.0, .y = -5.0, .z = -20.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere4 = {.sphere = {.radius = 5, .position = {.x = -10.0, .y = 5.0, .z = -15.0}}, .colour = {.r = 150, .g = 0, .b = 0}};

    //Setup light
    //Parallel light source (could be a distant spherical light source)
    vector3 light = {.x = 0.0, .y = -1.0, .z = 0.0}; //not used at the moment

    //TODO Consider storing in a list, or a file
    vector3 point_light = {.x = 0.0, .y = 5.0, .z = -20.0};

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

            ray_intersection intersection = sphere_intersect(sphere3.sphere, ray);
            ray_intersection intersection2 = sphere_intersect(sphere4.sphere, ray);

            if (intersection.intersects) {
                vector3 normal = subtract_second_vector3_from_first(ray_at_t(ray, intersection.t), sphere3.sphere.position);
                vector3 unit_normal = unit_vector(normal);
                double projection = vector3_dot(unit_normal, light);

                vector3 intersection_to_light_source = subtract_second_vector3_from_first(point_light,ray_at_t(ray, intersection.t));
                vector3 intersection_to_light_source_normal = unit_vector(intersection_to_light_source);
                double normal_light_projection = vector3_dot(unit_normal, intersection_to_light_source_normal);

                colour.r = (normal_light_projection + 1)/2 * sphere3.colour.r;
                colour.g = (normal_light_projection + 1)/2 * sphere3.colour.g;
                colour.b = (normal_light_projection + 1)/2 * sphere3.colour.b;
                //printf("The projection value is: %.2f, and the green value is: %d\n", normal_light_projection, colour.g);

            };

            if (intersection2.intersects) {
                //TODO move to a separate function
                vector3 normal2 = subtract_second_vector3_from_first(ray_at_t(ray, intersection2.t), sphere4.sphere.position);
                vector3 unit_normal2 = unit_vector(normal2);
                double projection2 = vector3_dot(unit_normal2, light);

                vector3 intersection_to_light_source = subtract_second_vector3_from_first(point_light,ray_at_t(ray, intersection2.t));
                vector3 intersection_to_light_source_normal = unit_vector(intersection_to_light_source);
                double normal_light_projection = vector3_dot(unit_normal2, intersection_to_light_source_normal);

                colour.r = (normal_light_projection + 1)/2 * sphere4.colour.r;
                colour.g = (normal_light_projection + 1)/2 * sphere4.colour.g;
                colour.b = (normal_light_projection + 1)/2 * sphere4.colour.b;
                //printf("The projection value is: %.2f, and the green value of the second sphere is is: %d\n", normal_light_projection, colour.g);
                //printf("The projection value is: %.2f, the colour coefficient is %.2f and the red value of the second sphere is is: %d\n", normal_light_projection, (normal_light_projection - 1)/2, colour.r);
                //printf("The projection value is: %.2f, and the blue value of the second sphere is is: %d\n", normal_light_projection, colour.b);
            };

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

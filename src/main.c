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

#include <stdio.h>
#include <time.h>
#include "ray-tracing/geometry.h"
#include "ray-tracing/ray_trace.h"
#include "utils/io.h"
#include "utils/scene.h"

pixel_colour background_colour(const ray ray) {
    pixel_colour colour;

    vector3 ray_unit_direction = unit_vector(ray.direction);

    colour.r = (uint8_t) ((ray_unit_direction.x + 1) * 117.0);
    colour.g = (uint8_t) ((ray_unit_direction.y + 1) * 34 +
               (uint8_t) ((ray_unit_direction.x + 1)  * 34));
    colour.b = (uint8_t) ((ray_unit_direction.y + 1) * 117.0);

    //printf("The ray unit direction is x: %.2f, y: %.2f, z: %.2f\n", ray_unit_direction.x, ray_unit_direction.y, ray_unit_direction.z);

    return colour;
}

//TODO write into image directly
pixel_colour shade(const ray ray, ray_intersection intersection, solid_colour_sphere hit_sphere, vector3 point_light) {
    pixel_colour colour;

    vector3 normal = subtract_second_vector3_from_first(ray_at_t(ray, intersection.t), hit_sphere.sphere.position);
    vector3 unit_normal2 = unit_vector(normal);

    vector3 intersection_to_light_source = subtract_second_vector3_from_first(point_light,ray_at_t(ray, intersection.t));
    vector3 intersection_to_light_source_normal = unit_vector(intersection_to_light_source);
    double normal_light_projection = vector3_dot(unit_normal2, intersection_to_light_source_normal);

    colour.r = (normal_light_projection + 1)/2 * hit_sphere.colour.r;
    colour.g = (normal_light_projection + 1)/2 * hit_sphere.colour.g;
    colour.b = (normal_light_projection + 1)/2 * hit_sphere.colour.b;

    colour.r = ((normal_light_projection + 1)/2) * ((normal_light_projection + 1)/2) * hit_sphere.colour.r;
    colour.g = ((normal_light_projection + 1)/2) * ((normal_light_projection + 1)/2) * hit_sphere.colour.g;
    colour.b = ((normal_light_projection + 1)/2) * ((normal_light_projection + 1)/2) * hit_sphere.colour.b;

    return colour;
}


int main(void) {
    clock_t begin = clock();
    printf("START PROGRAM\n");

    const int image_width = 1600;
    const double aspect_ratio = 1.6; // almost as good as 4:3
    const int image_height = image_width / aspect_ratio;

    //The units are not pixels; this is in world coordinates, image variables relate to resolution that fits into this size
    //Changing these settings changes the field of vision
    const double viewscreen_height = 2.0; //eyes could be rectangles
    const double viewscreen_width = viewscreen_height * ((double) image_width / image_height);

    scene scene = scene_init(image_width, image_height);
    pixel_colour *image = calloc(image_width * image_height, sizeof(pixel_colour));

    //Useful information
    printf("The eye is positioned at x: %.2f, y: %.2f, z: %.2f\n", scene.eye_position.x, scene.eye_position.y, scene.eye_position.z);

    //Setup objects
    //sphere sphere1 = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}};
    //TODO Read object positions from a file at runtime
    //solid_colour_sphere sphere2 = {.sphere = {.radius = 5, .position = {.x = -12.0, .y = 3.0, .z = -20.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere3 = {.sphere = {.radius = 5, .position = {.x = 10.0, .y = -5.0, .z = -20.0}}, .colour = {.r = 0, .g = 255, .b = 0}};
    solid_colour_sphere sphere4 = {.sphere = {.radius = 5, .position = {.x = -10.0, .y = 5.0, .z = -25.0}}, .colour = {.r = 150, .g = 0, .b = 0}};

    solid_colour_sphere spheres[2] = { sphere3, sphere4 };

    //Setup light
    //Parallel light source (could be a distant spherical light source)
    vector3 light = {.x = 0.0, .y = -1.0, .z = 0.0}; //not used at the moment

    //TODO Consider storing in a list, or a file
    vector3 point_light = {.x = 0.0, .y = 15000.0, .z = -20.0};

    //Draw
    //draw_simple_image(image, image_width, image_height);

    //Draw more
    for (int i = 0; i < image_height; i++) {
        for (int j = 0; j < image_width; ++j) {
            //printf("Currently processing row %d\n", i);
            //printf("Currently processing column %d\n", j);
            vector3 pixel_location = add_vector3(add_vector3(scene.viewscreen_first_pixel_location,
                                                             vector3_multiply_by_scalar(scene.viewscreen_delta_u, j)),
                                                 vector3_multiply_by_scalar(scene.viewscreen_delta_v, i));

            //printf("The calculating pixel at location: x: %.2f, y: %.2f, z: %.2f\n", pixel_location.x, pixel_location.y, pixel_location.z);

            ray ray = {.direction = subtract_second_vector3_from_first(pixel_location, scene.eye_position), .origin = scene.eye_position};

            pixel_colour colour = background_colour(ray);

            hit_sphere potential_sphere_hit = intersected_sphere_index(ray, spheres, 2);
            if (potential_sphere_hit.sphere_index != -1) {
                colour = shade(ray, potential_sphere_hit.ray_intersection, spheres[potential_sphere_hit.sphere_index], point_light);
                //printf("Shading sphere %d\n", potential_sphere_hit.sphere_index);
            }
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

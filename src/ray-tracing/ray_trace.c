#include "ray_trace.h"

ray_intersection sphere_intersect(const sphere sphere, const ray ray) {

    ray_intersection intersection;
    intersection.intersects = 0;
    intersection.t = INFINITY;

    vector3 ray_to_sphere_centre = subtract_second_vector3_from_first(sphere.position, ray.origin);

    const double a = vector3_dot(ray.direction, ray.direction);
    const double b = -2.0 * vector3_dot(ray.direction, ray_to_sphere_centre);
    const double c = vector3_length_squared(ray_to_sphere_centre) - (sphere.radius * sphere.radius);

    const double discriminant = b * b - 4 * a * c;

    if (discriminant > 0) {
        const double numerator = -b - sqrt(discriminant);
        if (numerator >= 0) {
            intersection.intersects = 1;
            const double denominator = 2 * a;
            intersection.t = numerator / denominator;
        }
    }

    return intersection;
}

hit_sphere intersected_sphere_index(const ray ray, const solid_colour_sphere *spheres, int number_of_spheres ) {
    hit_sphere potential_hit_sphere = {.sphere_index = -1};

    for (int i = 0; i < number_of_spheres; i++) {
        ray_intersection intersection = sphere_intersect(spheres[i].sphere, ray);
        if (intersection.intersects == 1) {
            potential_hit_sphere.sphere_index = i;
            potential_hit_sphere.ray_intersection = intersection;
        }
    }
    return potential_hit_sphere;
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

/**
 *
 * @param ray
 * @return A background colour. Not guaranteed to stay stable.
 */
pixel_colour background_colour(const ray ray) {
    pixel_colour colour;

    vector3 ray_unit_direction = unit_vector(ray.direction);

    colour.r = (uint8_t) (((ray_unit_direction.x + 1) * 117.0));
    colour.g = (uint8_t) (((ray_unit_direction.y + 1) * 34) +
               (uint8_t) ((ray_unit_direction.x + 1)  * 34));
    colour.b = (uint8_t) ((ray_unit_direction.y + 1) * 117.0);

    return colour;
}

/**
 *
 * @param image_width
 * @param rows_to_process
 * @param number_of_rows
 * @param scene
 * @param spheres
 * @param point_light
 * @param image
 * @param process_index
 */
void render_pixels(
                    const int image_width,
                    const int *rows_to_process,
                    const int number_of_rows,
                    scene scene,
                    const solid_colour_sphere *spheres,
                    const int number_of_spheres,
                    vector3 point_light,
                    pixel_colour *image,
                    int process_index) {

    pixel_colour colour;
    vector3 eye_to_corner = subtract_second_vector3_from_first(scene.viewscreen_first_pixel_location, scene.eye_position);
    for (int i = 0; i < number_of_rows; i++) {
        vector3 eye_to_vertical_offset = add_vector3(eye_to_corner,
                                                vector3_multiply_by_scalar(scene.viewscreen_delta_v, rows_to_process[i]));
        for (int j = 0; j < image_width; ++j) {
            if (process_index == 1) {
                //printf("Currently processing row %d\n", rows_to_process[i]);
            }
            //printf("Currently processing column %d\n", j);
            vector3 pixel_location = add_vector3(vector3_multiply_by_scalar(scene.viewscreen_delta_u, j), eye_to_vertical_offset);

            //printf("The calculating pixel at location: x: %.2f, y: %.2f, z: %.2f\n", pixel_location.x, pixel_location.y, pixel_location.z);

            ray ray = {.direction = pixel_location, .origin = scene.eye_position};

            hit_sphere potential_sphere_hit = intersected_sphere_index(ray, spheres, number_of_spheres);
            if (potential_sphere_hit.sphere_index != -1) {
                colour = shade(ray, potential_sphere_hit.ray_intersection, spheres[potential_sphere_hit.sphere_index], point_light);
                //printf("Shading sphere %d\n", potential_sphere_hit.sphere_index);
            } else {
                colour = background_colour(ray);
            }
            // if (process_index == 1) {
            //     printf("Process %d attempting to write to %d\n", process_index, i * image_width + j);
            // }
            image[i * image_width + j] = colour;
        }
    }
    //printf("Process %d finished rendering\n", process_index);
}

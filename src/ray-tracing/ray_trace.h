#pragma once

#ifndef RAY_TRACE_H
#define RAY_TRACE_H
#include "geometry.h"
#include <stdint.h>
#include "../utils/scene.h"
#include <stdio.h>
typedef struct pixel_colour {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} pixel_colour;

typedef struct solid_colour_sphere {
    sphere sphere;
    pixel_colour colour;
} solid_colour_sphere;

typedef struct hit_sphere {
    int sphere_index;
    ray_intersection ray_intersection;
} hit_sphere;



ray_intersection sphere_intersect(sphere sphere, ray ray);
hit_sphere intersected_sphere_index(ray ray, const solid_colour_sphere *spheres, int number_of_spheres );
pixel_colour shade(ray ray, ray_intersection intersection, solid_colour_sphere hit_sphere, vector3 point_light);
pixel_colour background_colour(ray ray);
void render_pixels(int image_width, const int *rows_to_process, int number_of_rows, scene scene, const solid_colour_sphere *spheres,
                    const int number_of_spheres, vector3 point_light, pixel_colour *image, int process_index);
#endif //RAY_TRACE_H

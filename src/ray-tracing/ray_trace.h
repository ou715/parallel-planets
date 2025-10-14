#pragma once

#ifndef RAY_TRACE_H
#define RAY_TRACE_H
#include "geometry.h"
#include <stdint.h>

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
hit_sphere intersected_sphere_index(ray ray, solid_colour_sphere *spheres, int number_of_spheres );
#endif //RAY_TRACE_H

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

hit_sphere intersected_sphere_index(const ray ray, solid_colour_sphere *spheres, int number_of_spheres ) {
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


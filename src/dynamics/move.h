#ifndef MOVE_H
#define MOVE_H

#define gamma_const 0.1
#include "../ray-tracing/geometry.h"
#include <stdio.h>

typedef struct sphere_with_mass {
    double mass;
    vector3 position;
    vector3 velocity;
} sphere_with_mass;

vector3 calculate_force(sphere_with_mass sphere_a, sphere_with_mass sphere_b);
vector3 calculate_acceleration(sphere_with_mass sphere, vector3 force);
void update_position_euler_semi_implicit(sphere_with_mass *sphere, vector3 acceleration, double dt);
vector3 calculate_total_acceleration_verlet(int subject_sphere_position, sphere_with_mass spheres[], int number_of_spheres);
void update_position_velocity_verlet(int subject_sphere_position, sphere_with_mass spheres[], int number_of_spheres, double dt);

#endif //MOVE_H

#ifndef MOVE_H
#define MOVE_H

#define gamma_const 0.1
#include "../ray-tracing/geometry.h"
#include <stdio.h>

vector3 calculate_force(sphere sphere_a, sphere sphere_b);
vector3 calculate_acceleration(sphere sphere, vector3 force);
void update_position_euler_semi_implicit(sphere *sphere, vector3 acceleration, double dt);
vector3 calculate_total_acceleration_verlet(int subject_sphere_position, sphere spheres[], int number_of_spheres);
void update_position_velocity_verlet(int subject_sphere_position, sphere spheres[], int number_of_spheres, double dt);

#endif //MOVE_H

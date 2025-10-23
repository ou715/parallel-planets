#ifndef MOVE_H
#define MOVE_H

#define gamma_const 1
#include "../ray-tracing/geometry.h"

typedef struct sphere_with_mass {
    double mass;
    vector3 position;
    vector3 velocity;
} sphere_with_mass;

vector3 calculate_force(sphere_with_mass sphere_a, sphere_with_mass sphere_b);
vector3 calculate_acceleration(sphere_with_mass sphere, vector3 force);
void update_position_euler_explicit(sphere_with_mass *sphere, vector3 acceleration, double dt);

#endif //MOVE_H

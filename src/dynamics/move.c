
#include "move.h"

#include <stdio.h>

/**
 * Calculates the force on sphere a by sphere b
 * @param sphere_a
 * @param sphere_b
 * @return A vector from sphere b to sphere a with the calculated intensity
 */
vector3 calculate_force(const sphere_with_mass sphere_a, const sphere_with_mass sphere_b) {
    const vector3 vector_b_to_a = subtract_second_vector3_from_first(sphere_b.position, sphere_a.position);

    const double distance_squared = vector3_length_squared(
        subtract_second_vector3_from_first(sphere_a.position, sphere_b.position));

    const double force_magnitude = gamma_const * (sphere_a.mass * sphere_b.mass) / distance_squared;

    return vector3_multiply_by_scalar(vector_b_to_a, force_magnitude);
}

vector3 calculate_acceleration(const sphere_with_mass sphere, const vector3 force) {
    return vector3_multiply_by_scalar(force, 1/sphere.mass);
}

/**
 *
 * @param sphere Object
 * @param acceleration
 * @param dt Time step
 * @return
 */
void update_position_euler_explicit(sphere_with_mass *sphere, const vector3 acceleration, const double dt) {

    //printf("Time step dt = %.3f\n", dt);
    //printf("The acceleration of sphere3_m is x: %.3f, y: %.3f, z: %.3f\n", acceleration.x, acceleration.y, acceleration.z);

    const vector3 dv = vector3_multiply_by_scalar(acceleration, dt);

    //printf("The dv of sphere3_m is x: %.5f, y: %.5f, z: %.5f\n", dv.x, dv.y, dv.z);

    //printf("The position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere->position.x, sphere->position.y, sphere->position.z);

    sphere->velocity.x += dv.x;
    sphere->velocity.y += dv.y;
    sphere->velocity.z += dv.z;

    //printf("The velocity of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere->velocity.x, sphere->velocity.y, sphere->velocity.z);
    //const vector3 dr = vector3_multiply_by_scalar(dv, dt);

    sphere->position.x += dt * sphere->velocity.x;
    sphere->position.y += dt * sphere->velocity.y;
    sphere->position.z += dt * sphere->velocity.z;
    //printf("The new position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere->position.x, sphere->position.y, sphere->position.z);

}
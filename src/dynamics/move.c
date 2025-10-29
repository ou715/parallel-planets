
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

    const double distance_squared = vector3_length_squared(vector_b_to_a);

    const double distance_cubed = vector3_length(vector_b_to_a) * distance_squared;


    const double force_magnitude = gamma_const * (sphere_a.mass * sphere_b.mass) / distance_cubed;


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
void update_position_euler_semi_implicit(sphere_with_mass *sphere, const vector3 acceleration, const double dt) {

    //printf("Time step dt = %.3f\n", dt);
    //printf("The acceleration of sphere3_m is x: %.3f, y: %.3f, z: %.3f\n", acceleration.x, acceleration.y, acceleration.z);

    const vector3 dv = vector3_multiply_by_scalar(acceleration, dt);

    //printf("The dv of sphere3_m is x: %.5f, y: %.5f, z: %.5f\n", dv.x, dv.y, dv.z);

    //printf("The position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere->position.x, sphere->position.y, sphere->position.z);

    sphere->velocity.x += dv.x;
    sphere->velocity.y += dv.y;
    sphere->velocity.z += dv.z;

    //printf("The velocity of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere->velocity.x, sphere->velocity.y, sphere->velocity.z);

    sphere->position.x += dt * sphere->velocity.x;
    sphere->position.y += dt * sphere->velocity.y;
    sphere->position.z += dt * sphere->velocity.z;
    //printf("The new position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere->position.x, sphere->position.y, sphere->position.z);

}

/*
 * @param sphere_a_position
 * @param sphere_a_mass
 * @param sphere_b_position
 * @param sphere_b_mass
 * @return Acceleration caused by the the gravitational force between two
 *         spheres in given positions with the given mass experienced by
 *         the sphere_a
 */
vector3 calculate_acceleration_verlet(vector3 sphere_a_position,
                                      double sphere_a_mass,
                                      vector3 sphere_b_position,
                                      double sphere_b_mass) {

  const vector3 vector_b_to_a = subtract_second_vector3_from_first(sphere_b_position, sphere_a_position);
  const double distance_cubed = pow(vector3_length(vector_b_to_a), 3);
  const double force_magnitude = gamma_const * (sphere_a_mass * sphere_b_mass) / distance_cubed;

  const vector3 force = vector3_multiply_by_scalar(vector_b_to_a, force_magnitude);
  return vector3_multiply_by_scalar(force, 1 / sphere_a_mass);
}

/**
 *
 * @param sphere Object
 * @param acceleration
 * @param dt Time step
 * @return
 */
void update_position_velocity_verlet(sphere_with_mass *sphere_a, sphere_with_mass sphere_b, double dt) {

    const vector3 acceleration_t0 = calculate_acceleration_verlet(sphere_a->position, sphere_a->mass, sphere_b.position, sphere_b.mass);

    //printf("Time step dt = %.3f\n", dt);
    //printf("The acceleration of sphere3_m is x: %.3f, y: %.3f, z: %.3f\n", acceleration_t0.x, acceleration_t0.y, acceleration_t0.z);

    const double dx = sphere_a->velocity.x * dt + 0.5 * (dt * dt) * acceleration_t0.x;
    const double dy = sphere_a->velocity.y * dt + 0.5 * (dt * dt) * acceleration_t0.y;
    const double dz = sphere_a->velocity.z * dt + 0.5 * (dt * dt) * acceleration_t0.z;
    const vector3 position_1 = {.x = sphere_a->position.x + dx, .y = sphere_a->position.y + dy, .z = sphere_a->position.z + dz};

    const vector3 acceleration_t1 = calculate_acceleration_verlet(position_1, sphere_a->mass, sphere_b.position, sphere_b.mass);

    const double dv_x = 0.5 * dt * (acceleration_t0.x + acceleration_t1.x);
    const double dv_y = 0.5 * dt * (acceleration_t0.y + acceleration_t1.y);
    const double dv_z = 0.5 * dt * (acceleration_t0.z + acceleration_t1.z);

    const vector3 v_1 = {.x = sphere_a->velocity.x + dv_x, .y = sphere_a->velocity.y + dv_y, .z = sphere_a->velocity.z + dv_z};

    //printf("The velocity of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", v_1.x, v_1.y, v_1.z);

    sphere_a->velocity.x += dv_x;
    sphere_a->velocity.y += dv_y;
    sphere_a->velocity.z += dv_z;

    sphere_a->position = position_1;
    //printf("The new position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", sphere_a->position.x, sphere_a->position.y, sphere_a->position.z);
}

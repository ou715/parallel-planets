
#include "move.h"

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
vector3 calculate_total_acceleration_verlet(int subject_sphere_position,
                                            sphere_with_mass spheres[],
                                            int number_of_spheres) {

  vector3 total_force = {.x = 0, .y = 0, .z = 0};
  vector3 force = {.x = 0, .y = 0, .z = 0};
  for (int i = 0; i < number_of_spheres; i++) {
    if (subject_sphere_position != i) {

        const vector3 vector_a_to_b = subtract_second_vector3_from_first(spheres[i].position, spheres[subject_sphere_position].position);
        const double distance = vector3_length(vector_a_to_b);
        if (distance > 10) {
            const double distance_cubed = pow(distance, 3);
            const double force_magnitude = gamma_const * (spheres[subject_sphere_position].mass * spheres[i].mass) / distance_cubed;

            force = vector3_multiply_by_scalar(vector_a_to_b, force_magnitude);
            if (subject_sphere_position == 1 && i == 0) {
                //printf("The vector from sphere %i to sphere %i is x: %.3f, y: %.3f, z: %.3f\n", subject_sphere_position, i, vector_a_to_b.x, vector_a_to_b.y, vector_a_to_b.z);

                //printf("Distance cubed is %.2f \n", distance_cubed);
                //printf("The force acting on sphere %i by sphere %i is x: %.3f, y: %.3f, z: %.3f\n", subject_sphere_position, i, force.x, force.y, force.z);
                //printf("The position of sphere %i is x: %.3f, y: %.3f, z: %.3f\n", subject_sphere_position, spheres[subject_sphere_position].position.x, spheres[subject_sphere_position].position.y, spheres[subject_sphere_position].position.z);

            }
        } else {
            //printf("The spheres %i and %i are too close, the distance id %.1f\n", subject_sphere_position, i, distance);
            force.x = 0;
            force.y = 0;
            force.z = 0;
        }
        total_force = add_vector3(total_force, force);
    }
  }
  //printf("The forcing acting on sphere %i is x: %.3f, y: %.3f, z: %.3f\n", subject_sphere_position, total_force.x, total_force.y, total_force.z);

  return vector3_multiply_by_scalar(total_force, 1 / spheres[subject_sphere_position].mass);
}

/**
 *
 * @param sphere Object
 * @param acceleration
 * @param dt Time step
 * @return
 */
void update_position_velocity_verlet(int subject_sphere_position, sphere_with_mass spheres[], int number_of_spheres, double dt) {

    const vector3 acceleration_t0 = calculate_total_acceleration_verlet(subject_sphere_position,
                                                                        spheres,
                                                                        number_of_spheres);

    //printf("Time step dt = %.3f\n", dt);
    //printf("The acceleration of sphere3_m is x: %.3f, y: %.3f, z: %.3f\n", acceleration_t0.x, acceleration_t0.y, acceleration_t0.z);

    const double dx = spheres[subject_sphere_position].velocity.x * dt + 0.5 * (dt * dt) * acceleration_t0.x;
    const double dy = spheres[subject_sphere_position].velocity.y * dt + 0.5 * (dt * dt) * acceleration_t0.y;
    const double dz = spheres[subject_sphere_position].velocity.z * dt + 0.5 * (dt * dt) * acceleration_t0.z;
    const vector3 position_1 = {.x = spheres[subject_sphere_position].position.x + dx,
                                .y = spheres[subject_sphere_position].position.y + dy,
                                .z = spheres[subject_sphere_position].position.z + dz};

    spheres[subject_sphere_position].position = position_1;

    const vector3 acceleration_t1 = calculate_total_acceleration_verlet(subject_sphere_position,
                                                                        spheres,
                                                                        number_of_spheres);

    const double dv_x = 0.5 * dt * (acceleration_t0.x + acceleration_t1.x);
    const double dv_y = 0.5 * dt * (acceleration_t0.y + acceleration_t1.y);
    const double dv_z = 0.5 * dt * (acceleration_t0.z + acceleration_t1.z);

    const vector3 v_1 = {.x = spheres[subject_sphere_position].velocity.x + dv_x,
                         .y = spheres[subject_sphere_position].velocity.y + dv_y,
                         .z = spheres[subject_sphere_position].velocity.z + dv_z};

    //printf("The velocity of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", v_1.x, v_1.y, v_1.z);

    spheres[subject_sphere_position].velocity.x += dv_x;
    spheres[subject_sphere_position].velocity.y += dv_y;
    spheres[subject_sphere_position].velocity.z += dv_z;

    //printf("The new position of sphere3_m is x: %.2f, y: %.2f, z: %.2f\n", spheres[subject_sphere_position].position.x, spheres[subject_sphere_position].position.y, spheres[subject_sphere_position].position.z);
}

#include "geometry.h"

#include <math.h>

double square(const double x) {
    return x * x;
}

//Vector related code
vector3 add_vector3(const vector3 a, const vector3 b) {
    vector3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

vector3 subtract_second_vector3_from_first(const vector3 a, const vector3 b) {
    vector3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

vector3 vector3_multiply_by_scalar(const vector3 v, const double scalar) {
    vector3 result;
    result.x = v.x * scalar;
    result.y = v.y * scalar;
    result.z = v.z * scalar;
    return result;
}

double vector3_length_squared(const vector3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

double vector3_length(const vector3 v) {
    return sqrt(vector3_length_squared(v));
}

double vector3_dot(const vector3 a, const vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vector3 unit_vector(const vector3 v) {
    vector3 result;
    double starting_vector3_length = vector3_length(v);
    result.x = v.x / starting_vector3_length;
    result.y = v.y / starting_vector3_length;
    result.z = v.z / starting_vector3_length;
    return result;
}

//Ray code

vector3 ray_at_t(const ray r, const double t) {
    return add_vector3(r.origin, vector3_multiply_by_scalar(r.direction, t));
}
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <math.h>

/**
  *
 * @param x Double to be squared
 * @return A new double which is x * x
 */
double square(double x);

//Vector related code

typedef struct vector3 {
    double x, y, z;
} vector3;

vector3 add_vector3(vector3 a, vector3 b);
vector3 subtract_second_vector3_from_first(vector3 a, vector3 b);
vector3 vector3_multiply_by_scalar(vector3 v, double scalar);
double vector3_length_squared(vector3 v);
double vector3_length(vector3 v);
double vector3_dot(vector3 a, vector3 b);
vector3 unit_vector(vector3 v);

//Geometric bodies
typedef struct sphere {
    double radius;
    double mass;
    vector3 position;
    vector3 velocity;
} sphere;

//Ray code
typedef struct ray {
    vector3 origin;
    vector3 direction;
} ray;

vector3 ray_at_t(ray r, double t);

typedef struct ray_intersection {
    int intersects;
    double t;
} ray_intersection;

#endif //GEOMETRY_H

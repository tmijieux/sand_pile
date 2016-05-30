#ifndef MATH2_H
#define MATH2_H

#include "vec.h"

void vec3_normalize(vec3 *v);
double radian_to_degree(double radian);

double *rotation_matrix(vec3 ax, double angle);
void matrix_multiply(vec3 *P, double mat[9]);

#endif //MATH2_H

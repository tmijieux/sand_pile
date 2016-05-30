#define __USE_XOPEN 1

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "math2.h"

void vec3_normalize(vec3 *v)
{
    double n = sqrt(v->x*v->x+v->y*v->y+v->z*v->z);
    v->x /= n;  v->y /= n;  v->z /= n;
}

double radian_to_degree(double radian)
{
    return radian * 180. / M_PI;
}

double *rotation_matrix(vec3 ax, double angle)
{
    double co = cos(angle); double co2 = 1.-co; double si = sin(angle);
    double xx = ax.x*ax.x, yy = ax.y*ax.y, zz = ax.z*ax.z;
    double xy = ax.x*ax.y, xz = ax.x*ax.z, yz = ax.y*ax.z; 
    double r[9] = {
	co+xx*co2, xy*co2-ax.z*si, xz*co2+ax.y*si,
        xy*co2+ax.z*si, co+yy*co2, yz*co2-ax.x*si,
        xz*co2-ax.y*si, yz*co2+ax.x*si, co+zz*co2
    };
    double *rot = malloc(9 * sizeof(*rot));
    memcpy(rot, r, 9 * sizeof(*rot));
    return rot;
}

void matrix_multiply(vec3 *P, double mat[9])
{
    vec3 p = *P;
    P->x = mat[0]*p.x + mat[1]*p.y+mat[2]*p.z;
    P->y = mat[3]*p.x + mat[4]*p.y+mat[5]*p.z;
    P->z = mat[6]*p.x + mat[7]*p.y+mat[8]*p.z;
}

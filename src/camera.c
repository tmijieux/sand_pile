#include <string.h>
#include <math.h>
#include <stdio.h>

#include <GL/glut.h>
#include <GL/glu.h>

#include "camera.h"
#include "vec.h"
#include "math2.h"

struct camera {
    // Point observé 
    vec3 look;

    /** Position dans la base (look, x, y ,z)  
     * c-à-d position relative au point observé 
     **/
    vec3 rpos;
    vec3 up, right; // vecteur de direction haut et droite pour la camera

    // Distance par rapport au point observé
    double dis;

    /** ay: angle autour de y (hauteur)
     *  ar: angle autour de right (direction droite de la camera) */
     double ay, ar;
};

struct camera *camera_create(vec3 look, double dis, int ay, int ar)
{
    struct camera *c = malloc(sizeof(*c));
    c->ar = ar;
    c->ay = ay;
    c->look = look;
    c->dis = dis;
    camera_set_rotate(c, ay, ar);
    return c;
}

void camera_free(struct camera *c)
{
    free(c);
}

void camera_set_look(struct camera *c, vec3 look)
{
    c->look = look;
}

void camera_set_distance(struct camera *c, double d)
{
    c->dis = d;
}

void camera_set_rotate(struct camera *c, double ay, double ar)
{
    c->rpos = (vec3) {0., 0., 1.};
    c->up = (vec3) {0., 1., 0.};
    c->right = (vec3) {1., 0., 0.};
    double *rot = rotation_matrix(c->right, ar);
    matrix_multiply(&c->up, rot);
    matrix_multiply(&c->rpos, rot);
    free(rot);
    rot = rotation_matrix((vec3) {0., 1., 0.}, ay);
    matrix_multiply(&c->up, rot);
    matrix_multiply(&c->rpos, rot);
    matrix_multiply(&c->right, rot);
    free(rot);
}

void camera_translate(struct camera *c, double dw, double dh)
{
    vec3 t = {
	dw * c->right.x + dh * c->up.x,
	dw * c->right.y + dh * c->up.y,
	dw * c->right.z + dh * c->up.z
    };
    c->look.x += t.x;
    c->look.y += t.y;
    c->look.z += t.z;
}

void camera_add_distance(struct camera *c, double d)
{
    c->dis = (c->dis + d <= 0.01) ? 0.0 : c->dis + d;
}

void camera_rotate(struct camera *c, double ay, double ar)
{
    c->ar += ar;
    c->ay += ay;
}

void camera_update(struct camera *c)
{
    camera_set_rotate(c, c->ay, c->ar);
    gluLookAt(c->look.x + c->dis * c->rpos.x,
	      c->look.y + c->dis * c->rpos.y,
	      c->look.z + c->dis * c->rpos.z,
	      c->look.x, c->look.y, c->look.z,
	      c->up.x, c->up.y, c->up.z);
}


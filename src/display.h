#ifndef DISPLAY_H
#define DISPLAY_H

#include "camera.h"

typedef unsigned (*get_func_t) (unsigned x, unsigned y);
typedef float * (*compute_func_t) (unsigned iterations);

#define STATIC_COLORING    ((float *)0)
#define DYNAMIC_COLORING   ((float *)1)

void display_init(int *argc, char **argv,
                  unsigned dim, unsigned max_height,
                  get_func_t get_func,
                  compute_func_t compute_func);

void display_main_loop(void);

#endif //DISPLAY_H

#ifndef COLORER_H
#define COLORER_H

#include "sand_pile.h"

struct color {
    float R, G, B;
};

float *sand_color(struct sand_pile *sand, struct color *colors);

#endif //COLORER_H

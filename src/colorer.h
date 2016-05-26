#ifndef COLORER_H
#define COLORER_H

struct color {
    float R, G, B;
};

float * sand_color(struct sand_heap * sand, struct color * colors);

#endif //COLORER_H

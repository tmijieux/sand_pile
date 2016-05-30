#ifndef SAND_BUILDER_H
#define SAND_BUILDER_H

#include "sand_pile.h"

void sand_build_column(struct sand_pile *sand, uint height);
void sand_build_5columns(struct sand_pile *sand, uint height);
void sand_build_wall(struct sand_pile *sand, uint height);
void sand_build_ground(struct sand_pile *sand, uint height);

#endif //SAND_BUILDER_H

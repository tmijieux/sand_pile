#ifndef SP_OMP_H
#define SP_OMP_H

#include "sand_pile.h"

struct sp_omp;

static inline sand_pile get_sand_pile(struct sp_omp *sp)
{
    return (sand_pile) sp;
}

#endif //SP_OMP_H

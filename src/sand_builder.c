#include <stdio.h>
#include <stdlib.h>

#include "sand_pile.h"

void sand_build_column(sand_pile sp, uint height)
{
    uint size = sp->op.get_size(sp);
    sp->op.set(sp, size / 2, size / 2, height);
}

void sand_build_5columns(sand_pile sp, uint height)
{
    uint size = sp->op.get_size(sp);
    sp->op.set(sp, size / 2, size / 2, height);
    uint h = height / 2;
    uint s1 = size / 4;
    uint s3 = size * 3 / 4;
    sp->op.set(sp, s1, s1, h);
    sp->op.set(sp, s1, s3, h);
    sp->op.set(sp, s3, s1, h);
    sp->op.set(sp, s3, s3, h);
}

void sand_build_wall(sand_pile sp, uint height)
{
    uint size = sp->op.get_size(sp);
    for (uint i = 0; i < size; i++)
	sp->op.set(sp, i, size / 2, height);
}

void sand_build_ground(sand_pile sp, uint height)
{
    uint size = sp->op.get_size(sp);
    for (uint i = 1; i < size-1; i++)
	for (uint j = 1; j < size-1; j++)
	    sp->op.set(sp, i, j, height);
}

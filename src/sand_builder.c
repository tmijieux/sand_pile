#include <stdio.h>
#include <stdlib.h>

#include "sand_heap.h"

void sand_build_column(struct sand_heap * sand, uint height)
{
    uint size = sand_get_size(sand);
    sand_set(sand, size / 2, size / 2, height);
}

void sand_build_5columns(struct sand_heap * sand, uint height)
{
    uint size = sand_get_size(sand);
    sand_set(sand, size / 2, size / 2, height);
    uint h = height / 2;
    uint s1 = size / 4;
    uint s3 = size * 3 / 4;
    sand_set(sand, s1, s1, h);
    sand_set(sand, s1, s3, h);
    sand_set(sand, s3, s1, h);
    sand_set(sand, s3, s3, h);
}

void sand_build_wall(struct sand_heap * sand, uint height)
{
    uint size = sand_get_size(sand);
    for (uint i = 0; i < size; i++)
	sand_set(sand, i, size / 2, height);    
}

void sand_build_ground(struct sand_heap * sand, uint height)
{
    uint size = sand_get_size(sand);
    for (uint i = 0; i < size; i++)
	for (uint j = 0; j < size; j++)
	    sand_set(sand, i, j, height);
}

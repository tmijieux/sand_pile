#include <stdio.h>
#include <stdlib.h>

#include "sand_heap.h"

struct sand_heap * sand_new(uint size)
{
    struct sand_heap * sand = malloc(sizeof(struct sand_heap));
    sand->size = size;
    sand->table = calloc(sizeof(*(sand->table)), size);  
    for (uint i = 0; i < size; i++) 
	sand->table[i] = calloc(sizeof(*(sand->table[i])), size);
    return sand;
}

void sand_free(struct sand_heap * sand)
{
    if (sand == NULL)
	return;
    for (uint i = 0; i < sand->size; i++) 
	free(sand->table[i]);
    free(sand->table);
    free(sand);
}

static inline uint sand_get_copy(struct sand_heap * sand, uint i, uint j)
{
    return sand->table[i][j].copy;
}

static inline void sand_set_copy(struct sand_heap * sand, uint i, uint j, uint value)
{
    sand->table[i][j].copy = value;
}

static inline void sand_reverse(struct sand_heap * sand, uint i, uint j)
{
    sand_set(sand, i, j, sand_get_copy(sand, i, j));
}

static inline void sand_set_stable(struct sand_heap * sand, uint i, uint j, int stable)
{
    sand->table[i][j].stable = stable;
}

static inline void sand_compute_one_tile(struct sand_heap * sand, uint i, uint j)
{
    uint val = sand_get(sand, i, j) % 4;
    val += sand_get(sand, i+1, j) / 4;
    val += sand_get(sand, i-1, j) / 4;
    val += sand_get(sand, i, j+1) / 4;
    val += sand_get(sand, i, j-1) / 4;

    int stable = (val == sand_get(sand, i, j));
    sand_set_stable(sand, i, j, stable);

    sand_set_copy(sand, i, j, val);
}

static inline void sand_compute_one_tile_more(struct sand_heap * sand, uint i, uint j, uint nb)
{
    uint val = sand_get(sand, i, j) % 4;
    val += sand_get(sand, i+1, j) / 4;
    val += sand_get(sand, i-1, j) / 4;
    val += sand_get(sand, i, j+1) / 4;
    val += sand_get(sand, i, j-1) / 4;

    for (uint k = 0; k < nb; k++) {
	i + nb - k, j + k;
	// +3,0 && +2,+1 && +1,+2 // i+2, j && i+1, j+1 // UR
	i - nb + k, j - k;
	// -3,0 && -2,-1 && -1,-2 // i-2, j && i-1, j-1 // BL
	i - k, j + nb - k;
	// 0,+3 && -1,+2 && -2,+1 // i, j+2 && i-1, j+1 // UL
	i + k, j - nb + k;
	// 0,-3 && +1,-2 && +2,-1 // i, j-2 && i+1, j-1 // BR
    }
	
    
}

static inline void sand_compute_one_step(struct sand_heap * sand)
{
    #pragma omp for
    for (uint i = 1; i < sand->size-1; i++)
	for (uint j = 1; j < sand->size-1; j++)
	    sand_compute_one_tile(sand, i, j);
    #pragma omp for
    for (uint i = 1; i < sand->size-1; i++)
	for (uint j = 1; j < sand->size-1; j++)
	    sand_reverse(sand, i, j);
}

void sand_compute(struct sand_heap * sand, uint nb)
{
    #pragma omp parallel
    for (uint k = 0; k < nb; k++)
	sand_compute_one_step(sand);
}

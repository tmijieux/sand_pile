#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sand_heap.h"

/* ---------------- --------- ---------------- */
/* ---------------- Sand Tile ---------------- */
/* ---------------- --------- ---------------- */

static inline struct sand_tile ** sand_tile_table_new(uint size)
{
    struct sand_tile ** table = calloc(sizeof(*table), size);
    for (uint i = 0; i < size; i++)
	table[i] = calloc(sizeof(*(table[i])), size);
    return table;
}

static inline struct sand_tile ** sand_tile_table_copy(struct sand_tile ** table, uint size)
{
    struct sand_tile ** copy = sand_tile_table_new(size);
    for (uint i = 0; i < size; i++)
	memcpy(copy[i], table[i], size * sizeof(*table[i]));
    return copy;
}

static inline void sand_tile_table_free(struct sand_tile ** table, uint size)
{
    if (table == NULL)
	return;
    for (uint i = 0; i < size; i++)
	free(table[i]);
    free(table);
}

/* ---------------- ---- ---------------- */
/* ---------------- Sand ---------------- */
/* ---------------- ---- ---------------- */

struct sand_heap * sand_new(uint size)
{
    struct sand_heap * sand = malloc(sizeof(struct sand_heap));
    sand->size  = size;
    sand->table = sand_tile_table_new(size);
    sand->copy  = sand_tile_table_new(size);
    return sand;
}

struct sand_heap * sand_copy(struct sand_heap * sand)
{
    struct sand_heap * copy = malloc(sizeof(struct sand_heap));
    copy->size  = sand->size;
    copy->table = sand_tile_table_copy(sand->table, sand->size);
    copy->copy  = sand_tile_table_new(sand->size);
    return copy;
}

void sand_free(struct sand_heap * sand)
{
    if (sand == NULL)
	return;
    sand_tile_table_free(sand->table, sand_get_size(sand));
    sand_tile_table_free(sand->copy,  sand_get_size(sand));
    free(sand);
}

/* ---------------- --------- ---------------- */
/* ---------------- Set & Get ---------------- */
/* ---------------- --------- ---------------- */

static inline uint sand_get_copy(struct sand_heap * sand, uint i, uint j)
{
    return sand->copy[i][j].value;
}

static inline void sand_set_copy(struct sand_heap * sand, uint i, uint j, uint value)
{
    sand->copy[i][j].value = value;
}

static inline void sand_reverse(struct sand_heap * sand)
{
    struct sand_tile ** tmp = sand->table;
    sand->table = sand->copy;
    sand->copy  = tmp;
}

static inline void sand_set_stable(struct sand_heap * sand, uint i, uint j, int stable)
{
    sand->table[i][j].stable = stable;
}

/* ---------------- --------------- ---------------- */
/* ---------------- Compute Synchro ---------------- */
/* ---------------- --------------- ---------------- */

static inline void sand_compute_one_tile(struct sand_heap * sand, uint i, uint j)
{
    if (sand_is_out(sand, i, j))
	return;

    uint val = sand_get(sand, i, j) % 4;
    val += sand_get(sand, i+1, j) / 4;
    val += sand_get(sand, i-1, j) / 4;
    val += sand_get(sand, i, j+1) / 4;
    val += sand_get(sand, i, j-1) / 4;

    int stable = (val == sand_get(sand, i, j));
    sand_set_stable(sand, i, j, stable);

    sand_set_copy(sand, i, j, val);
}

static inline void sand_compute_one_step_synchronous(struct sand_heap * sand)
{
    uint size = sand_get_size(sand);
    #pragma omp for collapse(2)
    for (uint i = 0; i < size; i++)
	for (uint j = 0; j < size; j++)
	    sand_compute_one_tile(sand, i, j);
    #pragma omp single
    sand_reverse(sand);
}

void sand_compute_n_step_synchronous(struct sand_heap * sand, uint nb)
{
    #pragma omp parallel
    for (uint k = 0; k < nb; k++)
	sand_compute_one_step_synchronous(sand);
}

/* ---------------- ---------------- ---------------- */
/* ---------------- Compute Asynchro ---------------- */
/* ---------------- ---------------- ---------------- */

static inline void sand_compute_diamond(struct sand_heap * sand, uint i, uint j, uint dist)
{
    /*
      Going through each "level" of the diamond except the center
      - - 2 - -
      - 2 1 2 -
      2 1 x 1 2
      - 2 1 2 -
      - - 2 - -
    */
    for (uint nb = 1; nb < dist; nb++) {
	for (uint k = 0; k < nb; k++) {
	    sand_compute_one_tile(sand, i + nb - k, j + k); // UR
	    sand_compute_one_tile(sand, i - nb + k, j - k); // BL
	    sand_compute_one_tile(sand, i - k, j + nb - k); // UL
	    sand_compute_one_tile(sand, i + k, j - nb + k); // BR
	}
    }
    sand_compute_one_tile(sand, i, j);
    sand_reverse(sand);
}

static inline void sand_compute_one_tile_asynchronous(struct sand_heap * sand, uint i, uint j, uint nb)
{
    struct sand_heap * sandbox = sand_copy(sand);
    for (uint k = 0; k < nb; k++)
	sand_compute_diamond(sandbox, i, j, k);

    uint stable = sand_get_stable(sandbox, i, j);
    sand_set_stable(sand, i, j, stable);
    uint val = sand_get(sandbox, i, j);
    sand_set_copy(sand, i, j, val);

    sand_free(sandbox);
}

/*
  TODO: Not working yet
 */
void sand_compute_n_step_asynchronous(struct sand_heap * sand, uint nb)
{
    uint size = sand_get_size(sand);
    #pragma omp parallel
    #pragma omp for collapse(2)
    for (uint i = 0; i < size; i++)
	for (uint j = 0; j < size; j++)
	    sand_compute_one_tile_asynchronous(sand, i, j, nb);
    sand_reverse(sand);
}

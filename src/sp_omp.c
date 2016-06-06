#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sand_pile.h"
#include "sand_builder.h"
#include "sp_omp.h"

/*
 * Here just for static inline functions. 
 * Do not access directly the structure.
 */
struct sand_tile {
    uint value;
    bool stable;
};

struct sp_omp {
    struct sand_pile super;
    struct sand_tile **table;
    struct sand_tile **copy;
    uint size;
};

static struct sp_operations sp_omp_op;
static struct sp_operations sp_omp_sync_op;
static struct sp_operations sp_omp_async_op;


/* ---------------- --------- ---------------- */
/* ---------------- Set & Get ---------------- */
/* ---------------- --------- ---------------- */

static inline uint sand_get_size(struct sp_omp *sand)
{
    return sand->size;
}

static inline bool sand_get_stable(struct sp_omp *sand, uint i, uint j)
{
    return sand->table[i][j].stable;
}

static inline bool sand_is_out(struct sp_omp *sand, uint i, uint j)
{
    return (i <= 0 || sand_get_size(sand)-1 <= i ||
	    j <= 0 || sand_get_size(sand)-1 <= j);
}

static inline uint sand_get(struct sp_omp* sand, uint i, uint j)
{
    if (sand_is_out(sand, i, j))
	return 0;
    return sand->table[i][j].value;
}

static inline void sand_set(struct sp_omp* sand, uint i, uint j, uint value)
{
    if (sand_is_out(sand, i, j))
	return;
    sand->table[i][j].value = value;
}

static inline uint sand_get_copy(struct sp_omp * sand, uint i, uint j)
{
    return sand->copy[i][j].value;
}

static inline void sand_set_copy(
    struct sp_omp * sand, uint i, uint j, uint value)
{
    sand->copy[i][j].value = value;
}

static inline void sand_reverse(struct sp_omp * sand)
{
    struct sand_tile **tmp = sand->table;
    sand->table = sand->copy;
    sand->copy  = tmp;
}

static inline void sand_set_stable(
    struct sp_omp * sand, uint i, uint j, bool stable)
{
    sand->table[i][j].stable = stable;
}

/* ---------------- --------- ---------------- */
/* ---------------- Sand Tile ---------------- */
/* ---------------- --------- ---------------- */

static inline struct sand_tile **sand_tile_table_new(uint size)
{
    struct sand_tile ** table = calloc(sizeof(*table), size);
    for (uint i = 0; i < size; i++)
	table[i] = calloc(sizeof(*(table[i])), size);
    return table;
}

static inline struct sand_tile **sand_tile_table_copy(
    struct sand_tile **table, uint size)
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

static sand_pile sand_new(uint size)
{
    struct sp_omp *sand = malloc(sizeof(struct sp_omp));
    sand->size  = size;
    sand->table = sand_tile_table_new(size);
    sand->copy  = sand_tile_table_new(size);
    return get_sand_pile(sand);
}

static sand_pile sand_sync_new(size_t size)
{
    sand_pile sand = sand_new(size);
    sand->op = sp_omp_sync_op;
    return sand;
}

static sand_pile sand_async_new(size_t size)
{
    sand_pile sand = sand_new(size);
    sand->op = sp_omp_async_op;
    return sand;
}

static struct sp_omp *sand_copy(struct sp_omp *sand)
{
    struct sp_omp * copy = malloc(sizeof(struct sp_omp));
    copy->size  = sand->size;
    copy->table = sand_tile_table_copy(sand->table, sand->size);
    copy->copy  = sand_tile_table_new(sand->size);
    return copy;
}

static void sand_free(struct sp_omp *sand)
{
    if (sand == NULL)
	return;
    sand_tile_table_free(sand->table, sand_get_size(sand));
    sand_tile_table_free(sand->copy,  sand_get_size(sand));
    free(sand);
}


/* ---------------- ----- ---------------- */
/* ---------------- Build ---------------- */
/* ---------------- ----- ---------------- */

static void build_1(sand_pile sp, uint height)
{
    sand_build_ground(sp, 5);
}

static void build_2(sand_pile sp, uint height)
{
    sand_build_column(sp, 100000);
}

static void build_custom(sand_pile sp, uint height)
{
    sand_build_column(sp, height);
}

/* ---------------- --------------- ---------------- */
/* ---------------- Compute Synchro ---------------- */
/* ---------------- --------------- ---------------- */

static inline void sand_compute_one_tile(
    struct sp_omp *sand, uint i, uint j)
{
    if (sand_is_out(sand, i, j))
	return;

    uint val = sand_get(sand, i, j) % 4;
    val += sand_get(sand, i+1, j) / 4;
    val += sand_get(sand, i-1, j) / 4;
    val += sand_get(sand, i, j+1) / 4;
    val += sand_get(sand, i, j-1) / 4;

    bool stable = (val == sand_get(sand, i, j));
    sand_set_stable(sand, i, j, stable);
    sand_set_copy(sand, i, j, val);
}

static inline void sand_compute_one_step_async(struct sp_omp *sand)
{
    uint size = sand_get_size(sand);
    #pragma omp for collapse(2)
    for (uint i = 0; i < size; i++)
	for (uint j = 0; j < size; j++)
	    sand_compute_one_tile(sand, i, j);
    #pragma omp single
    sand_reverse(sand);
}

static void sand_compute_n_step_async(sand_pile sp, uint nb_iterations)
{
    struct sp_omp *sand = (struct sp_omp*) sp;
    #pragma omp parallel
    for (uint k = 0; k < nb_iterations; k++)
	sand_compute_one_step_async(sand);
}

/* ---------------- ---------------- ---------------- */
/* ---------------- Compute Asynchro ---------------- */
/* ---------------- ---------------- ---------------- */

static inline void sand_compute_diamond(
    struct sp_omp *sand, uint i, uint j, uint dist)
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

static inline void sand_compute_one_tile_nsync(
    struct sp_omp *sand, uint i, uint j, uint nb)
{
    struct sp_omp *sandbox = sand_copy(sand);
    for (uint k = 0; k < nb; k++)
	sand_compute_diamond(sandbox, i, j, nb - k);

    bool stable = sand_get_stable(sandbox, i, j);
    sand_set_stable(sand, i, j, stable);
    uint val = sand_get(sandbox, i, j);
    sand_set_copy(sand, i, j, val);

    sand_free(sandbox);
}

static void sand_compute_n_step_nsync(sand_pile sp, uint nb)
{
    struct sp_omp *sand = (struct sp_omp*) sp;
    uint size = sand_get_size(sand);
    #pragma omp parallel
    #pragma omp for collapse(2)
    for (uint i = 0; i < size; i++)
	for (uint j = 0; j < size; j++)
	    sand_compute_one_tile_nsync(sand, i, j, nb);
    sand_reverse(sand);
}

static struct sp_operations sp_omp_op = {
    .name = "omp_generic_noinst",
    .get = (void*) sand_get,
    .set = (void*) sand_set,

    .get_stable = (void*) sand_get_stable,
    .get_size   = (void*) sand_get_size,

    .build_1 = (void*) build_1,
    .build_2 = (void*) build_2,
    .build_3 = (void*) build_custom,
};

inherits(sp_omp_sync_op, sp_omp_op, 1);
override(sp_omp_sync_op, name, "sp_omp_async", 2);
override(sp_omp_sync_op, new, sand_sync_new, 3);
override(sp_omp_sync_op, compute, sand_compute_n_step_async, 4);
register_sand_pile_type(sp_omp_sync_op, 5);

inherits(sp_omp_async_op, sp_omp_op, 6);
override(sp_omp_async_op, name, "sp_omp_nsync", 7);
override(sp_omp_async_op, new, sand_async_new, 8);
override(sp_omp_async_op, compute, sand_compute_n_step_nsync, 9);
register_sand_pile_type(sp_omp_async_op, 10);

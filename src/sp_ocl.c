#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <OpenCL/opencl.h>

#include "sand_pile.h"
#include "sand_builder.h"

struct sp_ocl {
    struct sand_pile super;
    uint size;
    uint *table;
    uint *copy;
    bool *stable;
};

static struct sp_operations sp_ocl_op;

/* ---------------- ------ ---------------- */
/* ---------------- Inline ---------------- */
/* ---------------- ------ ---------------- */

static inline uint sp_ocl_get_size(struct sp_ocl *sand)
{
    return sand->size;
}

static inline bool sp_ocl_get_stable(struct sp_ocl *sand, uint i, uint j)
{
    return sand->stable[i * sp_ocl_get_size(sand) + j];
}

static inline bool sp_ocl_is_out(struct sp_ocl *sand, uint i, uint j)
{
    return (i <= 0 || sp_ocl_get_size(sand)-1 <= i ||
	    j <= 0 || sp_ocl_get_size(sand)-1 <= j);
}

static inline uint sp_ocl_get(struct sp_ocl* sand, uint i, uint j)
{
    if (sp_ocl_is_out(sand, i, j))
	return 0;
    return sand->table[i * sp_ocl_get_size(sand) + j];
}

static inline void sp_ocl_set(struct sp_ocl* sand, uint i, uint j, uint value)
{
    if (sp_ocl_is_out(sand, i, j))
	return;
    sand->table[i * sp_ocl_get_size(sand) + j] = value;
}

/* ---------------- ------ ---------------- */
/* ---------------- sp_ocl ---------------- */
/* ---------------- ------ ---------------- */

struct sp_ocl *sp_ocl_new(uint size)
{
    struct sp_ocl *sand = malloc(sizeof*sand);
    sand->size   = size;
    sand->table  = malloc(sizeof(uint) * size * size);
    sand->copy   = malloc(sizeof(uint) * size * size);
    sand->stable = malloc(sizeof(bool) * size * size);
    sand->super.op = sp_ocl_op;
    return sand;
}

struct sp_ocl *sand_copy(struct sp_ocl *sand)
{
    uint size = sp_ocl_get_size(sand);
    struct sp_ocl * copy = sp_ocl_new(size);
    memcpy(copy->table,  sand->table,  sizeof(uint) * size * size);
    memcpy(copy->copy,   sand->copy,   sizeof(uint) * size * size);
    memcpy(copy->stable, sand->stable, sizeof(bool) * size * size);
    return copy;
}

void sand_free(struct sp_ocl *sand)
{
    if (sand == NULL)
	return;
    free(sand->table);
    free(sand->copy);
    free(sand->stable);
    free(sand);
}

/* ---------------- --------------- ---------------- */
/* ---------------- Compute Synchro ---------------- */
/* ---------------- --------------- ---------------- */

static void sp_ocl_compute_n_step_sync(struct sp_ocl *sand, uint nb_iterations)
{
    
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

/* ---------------- ----- ---------------- */
/* ---------------- sp_op ---------------- */
/* ---------------- ----- ---------------- */

static struct sp_operations sp_ocl_op = {
    .new = (void*) sp_ocl_new,
    .get = (void*) sp_ocl_get,
    .set = (void*) sp_ocl_set,
    .get_stable = (void*) sp_ocl_get_stable,
    .get_size = (void*) sp_ocl_get_size,
    
    .build_1 = build_1,
    .build_2 = build_2,
    .build_3 = build_custom,
    
    .compute = (void*) sp_ocl_compute_n_step_sync,
    .name = "sp_ocl",
};

register_sand_pile_type(sp_ocl_op, 1);

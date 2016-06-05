#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "sp_seq.h"

struct sp_operations sp_seq_op_generic;


static sand_pile sp_seq_new(size_t size)
{
    struct sp_seq *sp = malloc(sizeof*sp);

    sp->size = size;
    sp->change = true;
    sp->v1 = malloc(sizeof *sp->v1 * size);
    sp->v2 = malloc(sizeof *sp->v2 * size);
    sp->nochange = malloc(sizeof *sp->nochange * size);
    for (uint i = 0; i < size; ++i) {
        sp->v1[i] = calloc(size, sizeof*sp->v1[i]);
        sp->v2[i] = calloc(size, sizeof*sp->v2[i]);
        sp->nochange[i] = calloc(size, sizeof*sp->nochange);
    }
    
    return get_sand_pile(sp);
}

static void sp_build_custom(sand_pile opaque_sp, uint height)
{
    struct sp_seq *sp = get_sp_seq(opaque_sp);
    size_t s = sp->size / 2;
    sp->v1[s][s] = height;
}

static void sp_build_1(sand_pile opaque_sp, uint height)
{
    struct sp_seq *sp = get_sp_seq(opaque_sp);
    for (uint i = 1; i < sp->size-1; ++i) {
        for (uint j = 1; j < sp->size-1; ++j) {
            sp->v1[i][j] = 5;
            sp->v2[i][j] = 0;
        }
    }
}

static void sp_build_2(sand_pile opaque_sp, uint height)
{
    sp_build_custom(opaque_sp, 100000);
}

static uint sp_seq_get(sand_pile sp, uint i, uint j)
{
    return get_sp_seq(sp)->v1[i][j];
}

static void sp_seq_set(sand_pile sp, uint i, uint j, uint height)
{
     get_sp_seq(sp)->v1[i][j] = height;
}

static size_t sp_seq_get_size(sand_pile sp)
{
    return get_sp_seq(sp)->size;
}

static bool sp_seq_get_stable(sand_pile sp, uint i, uint j)
{
    return get_sp_seq(sp)->nochange[i][j];
}

struct sp_operations sp_seq_op_generic = {
    .new = sp_seq_new,
    .get = sp_seq_get,
    .set = sp_seq_set,

    .name = "sp_seq_generic",

    .get_stable = sp_seq_get_stable,
    .get_size = sp_seq_get_size,

    .build_1 = sp_build_1,
    .build_2 = sp_build_2,
    .build_3 = sp_build_custom
};

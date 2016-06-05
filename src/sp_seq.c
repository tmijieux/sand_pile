#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "sand_pile.h"

struct sp_operations sp_seq_op_generic;

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
    .get = sp_seq_get,
    .set = sp_seq_set,

    .get_stable = sp_seq_get_stable,
    .get_size = sp_seq_get_size,

    .build_1 = sp_build_1,
    .build_2 = sp_build_2,
    .build_3 = sp_build_custom
};

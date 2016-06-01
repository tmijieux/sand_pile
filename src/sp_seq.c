#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "sand_pile.h"

struct sp_seq {
    struct sand_pile super;
    size_t size;
    uint **v1;
    uint **v2;
    bool **nochange;
    bool change;
};
static struct sp_operations sp_seq_op;

static inline void __pointer_swap(void *a, void *b)
{
    void *tmp;
    tmp = *(void**)a;
    *(void**)a = *(void**)b;
    *(void**)b = tmp;
}

#define POINTER_SWAP(a, b)                      \
    do {                                        \
        __pointer_swap(&a, &b);                 \
    } while (0)


static inline sand_pile get_sand_pile(struct sp_seq *sp)
{
    return (sand_pile) sp;
}

static inline struct sp_seq *get_sp_seq(sand_pile opaque_sp)
{
    return (struct sp_seq*) opaque_sp;
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
    sp->super.op = sp_seq_op;
    return (sand_pile) sp;
}

static void sp_seq_compute_sync_1_step(struct sp_seq *sp, uint nb_iterations)
{
    size_t size = sp->size;
    sp->change = false;
    for (uint i = 1; i < size-1; i++) {
        for (uint j = 1; j < size-1; j++) {
            sp->nochange[i][j] = true;
                
            if (sp->v1[i][j] >= 4) {
                sp->v1[i][j] -= 4;
                sp->nochange[i][j] = false;
                ++ sp->v2[i+1][j];
                ++ sp->v2[i][j+1];
                ++ sp->v2[i-1][j];
                ++ sp->v2[i][j-1];
                sp->change = true;
            }
        }
    }
    for (uint i = 1; i < size-1; i++) {
        for (uint j = 1; j < size-1; j++) {
            sp->v1[i][j] += sp->v2[i][j];
            if (!sp->nochange[i][j] && sp->v2[i][j] == 4)
                sp->nochange[i][j] = true;
            else if (sp->nochange[i][j] && sp->v2[i][j] != 0)
                sp->nochange[i][j] = false;
                
            sp->v2[i][j] = 0;
        }
    }
}

static void sp_seq_compute_sync(sand_pile sand, uint nb_iterations)
{
    struct sp_seq *sp = (struct sp_seq*) sand;
    
    for (int it = 0; it < nb_iterations; ++it) {
        if (sp->change == false) {
            static int announced = false;
            if (!announced) {
                printf("\nC'est fini\n");
                announced = true;
            }
            return;
        }
        sp_seq_compute_sync_1_step(sp, nb_iterations);
    }
}

static void sp_seq_compute_sync2_1_step(struct sp_seq *sp, uint nb_iterations)
{
    uint size = sp->size;
    for (uint i = 1; i < size-1; i++) {
        for (uint j = 1; j < size-1; j++) {
            uint v = sp->v1[i][j];
            v -= 4 * (v > 3);
            v += sp->v1[i+1][j] > 3;
            v += sp->v1[i][j+1] > 3;
            v += sp->v1[i-1][j] > 3;
            v += sp->v1[i][j-1] > 3;
            sp->nochange[i][j] = sp->v1[i][j] == v;
            sp->v2[i][j] = v;
        }
    }
    POINTER_SWAP(sp->v1, sp->v2);
}

static void sp_seq_compute_sync2(sand_pile sand, uint nb_iterations)
{
    struct sp_seq *sp = get_sp_seq(sand);
    
    for (int it = 0; it < nb_iterations; ++it) {
        if (sp->change == false) {
            static int announced = false;
            if (!announced) {
                printf("\nC'est fini\n");
                announced = true;
            }
            return;
        }
        sp_seq_compute_sync2_1_step(sp, nb_iterations);
    }
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

static struct sp_operations sp_seq_op = {
    .new  = sp_seq_new,
    .get = sp_seq_get,
    .set = sp_seq_set,
    .get_stable = sp_seq_get_stable,
    .get_size = sp_seq_get_size,
    .compute_sync = sp_seq_compute_sync,
    .compute_async = sp_seq_compute_sync2,
    
    .build_1 = sp_build_1,
    .build_2 = sp_build_2,
    .build_3 = sp_build_custom
};

register_sand_pile_type(sp_seq_thomas, &sp_seq_op);


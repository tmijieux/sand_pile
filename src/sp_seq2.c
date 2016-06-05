#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "sand_pile.h"

static struct sp_operations sp_seq2_op;

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

static void sp_seq_compute_sync2_1_step(struct sp_seq *sp)
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
        sp_seq_compute_sync2_1_step(sp);
    }
}


inherits(sp_seq2_op, sp_seq);
override(sp_seq2_op, compute, sp_seq2_compute_sync2);
override(sp_seq2_op, new, sp_seq2_new);

register_sand_pile_type_base(sp_seq2_op, sp_seq_op_generic);
    

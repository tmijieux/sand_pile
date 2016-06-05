#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "sand_pile.h"

static struct sp_operations sp_seq1_op;

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


__attribute__ ((constructor))
static void register_sand_pile_seq(void)
{
    sp_seq_1_op = sp_seq_op;
    sp_seq_1_op.name = "sp_seq_sync1";
    sp_seq_1_op.new     = sp_seq_1_new;
    sp_seq_1_op.compute = sp_seq_compute_sync;
    register_sand_pile_type(&sp_seq_1_op);
}

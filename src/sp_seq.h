#ifndef SP_SEQ_H
#define SP_SEQ_H

#include "sand_pile.h"

struct sp_seq {
    struct sand_pile super;
    size_t size;
    uint **v1;
    uint **v2;
    bool **nochange;
    bool change;
};

extern struct sp_operations sp_seq_op_generic;

static inline sand_pile get_sand_pile(struct sp_seq *sp)
{
    return (sand_pile) sp;
}

static inline struct sp_seq *get_sp_seq(sand_pile opaque_sp)
{
    return (struct sp_seq*) opaque_sp;
}

#endif //SP_SEQ_H

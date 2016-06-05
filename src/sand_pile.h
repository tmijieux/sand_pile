#ifndef SAND_PILE_H
#define SAND_PILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef unsigned int uint;
struct sand_pile;
typedef struct sand_pile *sand_pile;

struct sp_operations {
    sand_pile (*new)(size_t size);
    uint (*get)(sand_pile sp, uint i, uint j);
    void (*set)(sand_pile sp, uint i, uint j, uint height);
    
    bool (*get_stable)(sand_pile sp, uint i, uint j);
    size_t (*get_size)(sand_pile sp);
    
    void (*compute)(sand_pile sp, uint nb_iterations);
    
    void (*build_1)(sand_pile sp, uint height); // ground
    void (*build_2)(sand_pile sp, uint height); // column
    void (*build_3)(sand_pile sp, uint height); // custom

    const char *name;
};

struct sand_pile {
    struct sp_operations op;
};

struct op_list {
    struct sp_operations *op;
    struct op_list *next;
};

extern struct op_list *global_op_list;

#define register_sand_pile_type(sp_op, order)   \
    __attribute__((constructor(101+order) ))    \
    static void register_##sp_op##_init(void)   \
    {                                           \
        static struct op_list opl;              \
        struct op_list opl_ = {                 \
            .op = &sp_op,                       \
            .next = global_op_list              \
        };                                      \
        opl = opl_;                             \
        global_op_list = &opl;                  \
    }

#define CONCAT(x, y) x##y
#define CONCAT_MACRO(x, y) CONCAT(x, y) 

#define inherits(sp_op, sp_base, order)                 \
    __attribute__((constructor(101+order) ))            \
    static void inherits__##sp_op##sp_base(void) {      \
        sp_op = sp_base;                                \
    }

#define override(sp_op, field, value, order)            \
    __attribute__((constructor(101+order) ))            \
    static void override__##sp_op##__##field(void) {    \
        sp_op.field = value;                            \
    }


void sand_fprint(FILE *file, sand_pile sp);

#endif //SAND_PILE_H

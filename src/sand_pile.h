#ifndef SAND_PILE_H
#define SAND_PILE_H

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
    
    void (*compute_sync)(sand_pile sp, uint nb_iterations);
    void (*compute_async)(sand_pile sp, uint nb_iterations);
    
    void (*build_1)(sand_pile sp, uint height); // ground
    void (*build_2)(sand_pile sp, uint height); // column
    void (*build_3)(sand_pile sp, uint height); // custom

    const char * name;
};

struct sand_pile {
    struct sp_operations op;
};

struct op_list {
    struct sp_operations *op;
    struct op_list *next;
};

extern struct op_list *global_op_list;

#define register_sand_pile_type(sp_name_, sp_op)        \
    __attribute__((constructor))                        \
    static void __init_##sp_name_##_register(void)      \
    {                                                   \
	(sp_op)->name = #sp_name_;			\
        static struct op_list opl;                      \
        struct op_list opl_ = {                         \
            .op = sp_op,                                \
            .next = global_op_list                      \
        };                                              \
        opl = opl_;                                     \
        global_op_list = &opl;                          \
    }                                                   \


#endif //SAND_PILE_H

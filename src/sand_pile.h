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

static inline bool sp_op_has_null_field(struct sp_operations * sp_op)
{
    void ** ptr = (void **) sp_op;
    for (uint i = 0; i < sizeof(*sp_op) / 8; i++)
	if (ptr[i] == NULL) {
	    printf("sp_op has a NULL pointer in %d.\n", i);
	    return true;
	}
    return false;
}

#define register_sand_pile_type(sp_op)				\
    do {							\
	if (sp_op_has_null_field(sp_op))			\
	    break;						\
	static struct op_list opl;				\
        struct op_list opl_ = {					\
            .op = sp_op,					\
            .next = global_op_list				\
        };							\
        opl = opl_;						\
        global_op_list = &opl;					\
    } while(0)

#endif //SAND_PILE_H

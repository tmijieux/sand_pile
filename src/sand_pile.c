#include <stdio.h>
#include "sand_pile.h"

struct op_list *global_op_list = NULL;

void sand_fprint(FILE * file, sand_pile sp)
{
    fprintf(file, "Final sand pile:\n");
    uint size = sp->op.get_size(sp);
    for (uint i = 0; i < size; i++) {
	for (uint j = 0; j < size; j++) {
	    fprintf(file, "%u ", sp->op.get(sp, i, j));
	}
	fprintf(file, "\n");
    }	
}

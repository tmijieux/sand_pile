#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sp_ocl.h"
#include "sand_builder.h"

static struct sp_operations sp_ocl_op;

static inline uint sp_ocl_get_size(struct sp_ocl *sand)
{
    return sand->size;
}

static inline bool sp_ocl_get_stable(struct sp_ocl *sand, uint i, uint j)
{
    return sand->stable[i * sp_ocl_get_size(sand) + j];
}

static inline bool sp_ocl_is_out(struct sp_ocl *sand, uint i, uint j)
{
    return (i <= 0 || sp_ocl_get_size(sand)-1 <= i ||
	    j <= 0 || sp_ocl_get_size(sand)-1 <= j);
}

static inline uint sp_ocl_get(struct sp_ocl* sand, uint i, uint j)
{
    if (sp_ocl_is_out(sand, i, j))
	return 0;
    return sand->table[i * sp_ocl_get_size(sand) + j];
}

static inline void sp_ocl_set(struct sp_ocl* sand, uint i, uint j, uint value)
{
    if (sp_ocl_is_out(sand, i, j))
	return;
    sand->table[i * sp_ocl_get_size(sand) + j] = value;
}

struct sp_ocl *sp_ocl_new(uint size)
{
    struct sp_ocl *sp;

    sp = malloc(sizeof*sp);
    sp->size   = size;
    sp->buf_size   = sizeof(uint) * size * size;
    sp->table  = malloc(sp->buf_size);
    sp->copy   = malloc(sp->buf_size);
    sp->stable = malloc(sizeof(bool) * size * size);
    sp->super.op = sp_ocl_op;

    setup_opencl(sp, KERNEL_FILE, "compute_n_step");
    return sp;
}

struct sp_ocl *sand_copy(struct sp_ocl *sand)
{
    uint size = sp_ocl_get_size(sand);
    struct sp_ocl * copy = sp_ocl_new(size);
    memcpy(copy->table,  sand->table,  sand->buf_size);
    memcpy(copy->copy,   sand->copy,   sand->buf_size);
    memcpy(copy->stable, sand->stable, sizeof(bool) * size * size);
    return copy;
}

static void sp_ocl_free(sand_pile sp__)
{
    struct sp_ocl *sp = (struct sp_ocl*) sp__;
    if (sp == NULL)
	return;
    free(sp->table);
    free(sp->copy);
    free(sp->stable);
 
    clReleaseCommandQueue(sp->queue);
    clReleaseKernel(sp->kernel);
    clReleaseProgram(sp->program);
    clReleaseContext(sp->context);

    clReleaseMemObject(sp->cl_table);
    clReleaseMemObject(sp->cl_copy);

    free(sp);
}

static void sp_ocl_compute_n_step_sync(struct sp_ocl *sp, uint nb_iterations)
{
    send_input(sp->queue, sp->cl_table, sp->table, sp->buf_size);
    execute_kernel(sp->queue, sp->kernel, sp->dev, nb_iterations, sp->size);
    retrieve_output(sp->queue, sp->cl_table, sp->table, sp->buf_size);
}

static void build_1(sand_pile sp, uint height)
{
    sand_build_ground(sp, 5);
}

static void build_2(sand_pile sp, uint height)
{
    sand_build_column(sp, 100000);
}

static struct sp_operations sp_ocl_op = {
    .new = (void*) sp_ocl_new,
    .get = (void*) sp_ocl_get,
    .set = (void*) sp_ocl_set,
    .get_stable = (void*) sp_ocl_get_stable,
    .get_size = (void*) sp_ocl_get_size,
    
    .build_1 = build_1,
    .build_2 = build_2,
    .build_3 = sand_build_column,
    
    .compute = (void*) sp_ocl_compute_n_step_sync,
    .name = "sp_ocl",
    .free = sp_ocl_free
};

register_sand_pile_type(sp_ocl_op, 1);

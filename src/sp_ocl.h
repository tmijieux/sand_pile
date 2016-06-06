#ifndef SP_OCL_H
#define SP_OCL_H

#include "sand_pile.h"

#ifdef __APPLE__
#  include <OpenCL/opencl.h>
#else
#  include <CL/opencl.h>
#endif

struct sp_ocl {
    struct sand_pile super;
    size_t size;
    size_t buf_size;
    
    cl_platform_id pf;
    cl_device_id dev;
    cl_context context;
    cl_kernel kernel;
    cl_program program;
    cl_command_queue queue;

    cl_mem cl_table;
    cl_mem cl_copy;
    uint *table;
    uint *copy;
    bool *stable;
};

void setup_opencl(struct sp_ocl *sp,
                  const char *filename, const char *kernel_name);
void send_input(cl_command_queue queue, cl_mem mem, void *data, size_t size);
void execute_kernel(cl_command_queue queue, cl_kernel kernel,
                    cl_device_id dev, uint nb_iterations, size_t size);
void retrieve_output(cl_command_queue queue, cl_mem mem, void *data, size_t size);
#endif //SP_OCL_H

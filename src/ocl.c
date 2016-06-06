#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/time.h>

#include "./util.h"
#include "sp_ocl.h"

extern char *optarg;
extern int optopt;
extern int optind;

static struct option option[] = {
    { "gpu-only", 0, NULL, 'g' },
    { "cpu-only", 0, NULL, 'c' },
    { "size", 1, NULL, 's' },
    { 0 }
};

#define MAX_PLATFORMS 3
#define MAX_DEVICES   5

cl_device_type device_type = CL_DEVICE_TYPE_ALL;
unsigned SIZE = 1024 * 1024; // ne pas utiliser
unsigned TILE = 16;           // idem

void send_input(cl_command_queue queue, cl_mem mem, void *data, size_t size)
{
    cl_int err;
    err = clEnqueueWriteBuffer(queue, mem, CL_TRUE, 0,
                               size, data, 0, NULL, NULL);
    check(err, "Failed to write to input array");
    clFinish(queue);
}

void retrieve_output(cl_command_queue queue, cl_mem mem, void *data, size_t size)
{
    cl_int err;
    err = clEnqueueReadBuffer(queue, mem, CL_TRUE, 0,
                              size, data, 0, NULL, NULL );  
    check(err, "Failed to read output array");
    clFinish(queue);
}

size_t parse_size(const char *str)
{
    char c;
    size_t s;
    int r = sscanf(str, "%zu%[mMkK]", &s, &c);
    
    if (r == 2) {
        if (c == 'k' || c == 'K')
            s *= 1024;
        else if (c == 'm' || c == 'M')
            s *= 1024 * 1024;
    }
    return s;
}

int ocl_parse_options(int *argc, char **argv)
{
    int c;
    while ((c = getopt_long(*argc, argv, "+gps:", option, NULL)) != -1) {
	switch (c) {
	case 'g':
            device_type = CL_DEVICE_TYPE_GPU;
	    break;
	case 'c':
	    break;
            device_type = CL_DEVICE_TYPE_CPU;
        case 's':
            SIZE = parse_size(argv[1]);
            break;
        default:
	case '?':
	    exit(EXIT_FAILURE);
	    break;
	}
    }
    *argc += optind ; argv += optind;
    return 0;
}

void compile_program(cl_program program, cl_device_id dev)
{
    char flags[1024];
    cl_int err;
    
    sprintf(flags, "-cl-mad-enable -cl-fast-relaxed-math "
            "-DSIZE=%d -DTILE=%d -DTYPE=%s", SIZE, TILE, "float");
    err = clBuildProgram(program, 0, NULL, flags, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        // Display compiler log
        clGetProgramBuildInfo(program, dev,
                              CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        {
            char buffer[len+1];

            fprintf(stderr, "--- Compiler log ---\n");
            clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                                  sizeof(buffer), buffer, NULL);
            fprintf(stderr, "%s\n", buffer);
            fprintf(stderr, "--------------------\n");
        }
        if (err != CL_SUCCESS)
            error("Failed to build program!\n");
    }
}

void execute_kernel(cl_command_queue queue, cl_kernel kernel,
                    cl_device_id dev, uint nb_iterations, size_t size)
{
    cl_int err;
    cl_event prof_event;
    size_t global[2] = { size, size };// global domain size for our calculation
    size_t local[2]  = { TILE, TILE };// local domain size for our calculation

    err = clSetKernelArg(kernel, 2, sizeof(uint), &nb_iterations);
    check(err, "Failed to set kernel arguments");
    err = clEnqueueNDRangeKernel(
        queue, kernel, 2, NULL,
        global, local, 0, NULL,
        &prof_event
    );
    check(err, "Failed to execute kernel");
    clFinish(queue);
    clReleaseEvent(prof_event);
}

cl_context create_context(cl_platform_id pf)
{
    cl_int err;
    cl_uint nb_devices = 0;
    cl_context context;
    cl_device_id devices[MAX_DEVICES];

    err = clGetDeviceIDs(pf, device_type, MAX_DEVICES, devices, &nb_devices);
    context = clCreateContext(0, nb_devices, devices, NULL, NULL, &err);
    check(err, "Failed to create context");
    return context;
}

cl_program create_program(const char *filename,
                          cl_context context, cl_device_id dev)
{
    char *opencl_prog;
    cl_program program;
    cl_int err;

    opencl_prog = file_load(filename);
    program = clCreateProgramWithSource(
        context, 1, (const char**) &opencl_prog, NULL, &err);
    check(err, "Failed to create program");
    compile_program(program, dev);
    free(opencl_prog);
    return program;
}

cl_kernel create_kernel(cl_program program, const char *kernel_name)
{
    cl_int err;
    cl_kernel kernel;
    
    kernel = clCreateKernel(program, kernel_name, &err);
    check(err, "Failed to create '%s' kernel", kernel_name);
    return kernel;
}

cl_platform_id detect_platform(int *argc, char **argv)
{
    cl_platform_id pf[MAX_PLATFORMS], pf_r = NULL;
    cl_uint nb_platforms = 0;
    cl_int err, p;

    ocl_parse_options(argc, argv);
    err = clGetPlatformIDs(MAX_PLATFORMS, pf, &nb_platforms);
    check(err, "Failed to get platform IDs");
    
    for (p = 0; p < nb_platforms; ++p) {
        char name[1024], vendor[1024];
        cl_uint nb_devices = 0;

        err = clGetPlatformInfo(pf[p], CL_PLATFORM_NAME, 1024, name, NULL);
        check(err, "Failed to get Platform Info 'name'");
        err = clGetPlatformInfo(pf[p], CL_PLATFORM_VENDOR, 1024, vendor, NULL);
        check(err, "Failed to get Platform Info 'vendor'");
        printf("Platform %d: %s - %s\n", p, name, vendor);

        err = clGetDeviceIDs(pf[p], device_type, 0, NULL, &nb_devices);
        printf("nb devices = %d\n", nb_devices);
        if (nb_devices != 0) {
            pf_r = pf[p];
            break;
        }
    }

    return pf_r;
}

cl_device_id get_device(cl_platform_id pf)
{
    cl_device_id dev;
    cl_uint nb_devices = 0;
    cl_int err;
    
    err = clGetDeviceIDs(pf, device_type, 1, &dev, &nb_devices);
    check(err, "Error getting device");
    return dev;
}


cl_mem create_mem(cl_context context, size_t size)
{
    cl_mem mem;
    mem = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, NULL);
    if (!mem)
        error("Failed to allocate buffer");
    return mem;
}

cl_command_queue create_queue(cl_context context, cl_device_id dev)
{
    cl_int err;
    cl_command_queue queue;
    
    queue = clCreateCommandQueue(context, dev, CL_QUEUE_PROFILING_ENABLE, &err);
    check(err, "Failed to create command queue");
    return queue;
}

void set_kernel_args(cl_kernel kernel, cl_mem table, cl_mem copy)
{
    cl_int err;
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &table);
    check(err, "Failed to set kernel arguments");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &copy);
    check(err, "Failed to set kernel arguments");
}

void setup_opencl(struct sp_ocl *sp,
                  const char *filename, const char *kernel_name)
{
    int argc = 1;
    char *argv[] = { "-g", NULL };
    
    sp->pf = detect_platform(&argc, argv);
    if (sp->pf == NULL)
        error("No platform were found\n");
    sp->dev = get_device(sp->pf);
    sp->context = create_context(sp->pf);
    sp->program = create_program(filename, sp->context, sp->dev);
    sp->kernel = create_kernel(sp->program, kernel_name);
    sp->queue = create_queue(sp->context, sp->dev);
    sp->cl_table = create_mem(sp->context, sp->buf_size);
    sp->cl_copy = create_mem(sp->context, sp->buf_size);
    set_kernel_args(sp->kernel, sp->cl_table, sp->cl_copy);
}

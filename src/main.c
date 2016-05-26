#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "display.h"
#include "sand_heap.h"
#include "sand_builder.h"
#include "colorer.h"

#define DIM_DEFAULT 128
#define MAX_HEIGHT_DEFAULT 128 * 128

static struct color * colors;
static struct sand_heap * sand;

static uint get(uint x, uint y)
{
    return sand_get(sand, x, y);
}

static float * compute(uint iterations)
{
    sand_compute(sand, iterations);
    return sand_color(sand, colors);
}

int main(int argc, char **argv)
{
    uint DIM = DIM_DEFAULT;
    uint MAX_HEIGHT = MAX_HEIGHT_DEFAULT;

    colors = calloc(sizeof(struct color), DIM * DIM);
    sand = sand_new(DIM);
    sand_build_5columns(sand, MAX_HEIGHT);
    display_init(argc, argv,
		 DIM,              // dimension ( = x = y) du tas
		 MAX_HEIGHT,       // hauteur maximale du tas
		 get,              // callback func
		 compute);         // callback func
    printf("Error: glutMainLoop() returned!\n");
    return EXIT_FAILURE;
}

__attribute__ ((constructor))
static void free_main(void)
{
    sand_free(sand);
    free(colors);
}

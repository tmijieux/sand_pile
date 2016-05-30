#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "display.h"
#include "sand_pile.h"
#include "sand_builder.h"
#include "colorer.h"

#define DIM_DEFAULT 32
#define MAX_HEIGHT_DEFAULT DIM_DEFAULT * DIM_DEFAULT

static struct color *colors;
static struct sand_pile *sand;

static uint get(uint x, uint y)
{
    return sand_get(sand, x, y);
}

static float *compute(uint iterations)
{
    sand_compute_n_step_sync(sand, iterations);
    return sand_color(sand, colors);
}

int main(int argc, char *argv[])
{
    uint DIM = DIM_DEFAULT;
    uint MAX_HEIGHT = MAX_HEIGHT_DEFAULT;

    colors = calloc(sizeof(struct color), DIM * DIM);
    sand = sand_new(DIM);
    //sand_build_column(sand, MAX_HEIGHT);
    sand_build_wall(sand, 100);
    
    display_init(&argc, argv,
		 DIM,              // dimension ( = x = y) du tas
		 MAX_HEIGHT,       // hauteur maximale du tas
		 get,              // callback func
		 compute);         // callback func
    display_main_loop();
    
    printf("salut\n");
    fprintf(stderr, "Error: glutMainLoop() returned!\n");
    return EXIT_FAILURE;
}

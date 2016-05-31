#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "display.h"
#include "sand_pile.h"
#include "sand_builder.h"
#include "colorer.h"
#include "options.h"

static struct color *colors;
static struct sand_pile *sand;

static struct config * conf;

static uint get(uint x, uint y)
{
    return sand_get(sand, x, y);
}

static float *compute(uint iterations)
{
    conf->sand_compute_fun(sand, iterations);
    return sand_color(sand, colors);
}

int main(int argc, char *argv[])
{
    conf = get_config(argc, argv);

    colors = calloc(sizeof(struct color), conf->dim * conf->dim);
    sand = sand_new(conf->dim);
    conf->sand_build_fun(sand, conf->max_height);
    
    display_init(&argc, argv,
		 conf->dim,        // dimension ( = x = y) du tas
		 conf->max_height, // hauteur maximale du tas
		 get,              // callback func
		 compute);         // callback func
    display_main_loop();
    
    fprintf(stderr, "Error: glutMainLoop() returned!\n");
    return EXIT_FAILURE;
}

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
static sand_pile sp;

static struct config *conf;

static uint get(uint x, uint y)
{
    return conf->sp_op->get(sp, x, y);
}

static float *compute(uint iterations)
{
    void (*compute)(sand_pile sp, uint nb_it);
    compute = *(void(**)(sand_pile,uint))
        ((char*)conf->sp_op + conf->sp_compute_offset);
    compute(sp, iterations);
    return sand_color(sp, colors);
}

static void build(void)
{
    void (*build_)(sand_pile sp, uint height);
    build_ = *(void(**)(sand_pile,uint))
        ((char*)conf->sp_op + conf->sp_build_offset);
    build_(sp, conf->max_height);
}

int main(int argc, char *argv[])
{
    conf = get_config(argc, argv);

    colors = calloc(conf->dim * conf->dim, sizeof*colors);
    sp = conf->sp_op->new(conf->dim);
    build();
    
    display_init(&argc, argv,
		 conf->dim,        // dimension ( = x = y) du tas
		 conf->max_height, // hauteur maximale du tas
		 get,              // callback func
		 compute);         // callback func
    display_main_loop();
    
    fprintf(stderr, "Error: glutMainLoop() returned!\n");
    return EXIT_FAILURE;
}

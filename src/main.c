#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "display.h"
#include "sand_pile.h"
#include "sand_builder.h"
#include "colorer.h"
#include "options.h"

#define MAX_ITERATION 10000

static struct color *colors;
static sand_pile sp;
static struct config *conf;
void (*compute_fun)(sand_pile sp, uint nb_it);

static uint get(uint x, uint y)
{
    return conf->sp_op->get(sp, x, y);
}

static float *compute(uint iterations)
{
    compute_fun(sp, iterations);
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
    compute_fun = *(void(**)(sand_pile,uint))
        ((char*)conf->sp_op + conf->sp_compute_offset);
    if (compute_fun == NULL) {
        fprintf(stderr, "Compute method not implemented\n");
        exit(EXIT_FAILURE);
    }
    config_print(conf);
    colors = calloc(conf->dim * conf->dim, sizeof*colors);
    sp = conf->sp_op->new(conf->dim);
    build();
    
    if (conf->display) {
	display_init(&argc, argv,
		     conf->dim,        // dimension ( = x = y) du tas
		     conf->max_height, // hauteur maximale du tas
		     get,              // callback func
		     compute);         // callback func
	display_main_loop();
	fprintf(stderr, "Error: glutMainLoop() returned!\n");
	exit(EXIT_FAILURE);
    } else {
	printf("nb_iterations time\n");
	uint it = 0;
	struct timeval t1, t2;
	gettimeofday(&t1, NULL);
	while (true) {
	    conf->sp_op->compute(sp, conf->iterations);
	    gettimeofday(&t2, NULL);
	    uint time = ((t2.tv_sec - t1.tv_sec) * 1000000 + 
			 (t2.tv_usec - t1.tv_usec));
	    it += conf->iterations;
	    printf("%u %u\n", it, time);
	    if (it > MAX_ITERATION)
		break;
	}
    }
    
    sand_fprint(stderr, sp); // separate from other data

    return EXIT_SUCCESS;
}

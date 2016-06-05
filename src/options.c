#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "sand_pile.h"
#include "sand_builder.h"
#include "options.h"

#include "util/hash_table.h"

#define OPTIONS_PREFIX '-'

#define ARG_OPT_DIM     "-d"
#define ARG_OPT_DISPLAY "-D"
#define ARG_OPT_HEIGHT  "-h"
#define ARG_OPT_HELP_1  "--help"
#define ARG_OPT_HELP_2  "-H"
#define ARG_OPT_ITERATIONS "-i"
#define ARG_OPT_LIST    "-l"
#define ARG_OPT_SP_TYPE "-m"
#define ARG_OPT_BUILD_TYPE "-b"

#define DIM_DEFAULT              32
#define MAX_HEIGHT_DEFAULT       DIM_DEFAULT * DIM_DEFAULT

#define SP_OP_OFFSET(field) ((uintptr_t) &((struct sp_operations*)0)->field)
#define SAND_COMPUTE_FUN_DEFAULT_OFF SP_OP_OFFSET(compute)
#define SAND_BUILD_FUN_DEFAULT_OFF   SP_OP_OFFSET(build_3)

#define ITERATIONS_DEFAULT 5

static struct hash_table * ht_opt;

static void handle_options(struct config *conf, int argc, char *argv[]);

/* ---------------- ------ ---------------- */
/* ---------------- Config ---------------- */
/* ---------------- ------ ---------------- */

static struct config * config_new(void)
{
    struct config * conf = malloc(sizeof(*conf));
    conf->dim              = DIM_DEFAULT;
    conf->max_height       = MAX_HEIGHT_DEFAULT;
    conf->sp_op            = global_op_list ? global_op_list->op : NULL;
    conf->sp_compute_offset = SAND_COMPUTE_FUN_DEFAULT_OFF;
    conf->sp_build_offset   = SAND_BUILD_FUN_DEFAULT_OFF;
    conf->display           = true;
    conf->iterations        = ITERATIONS_DEFAULT;
    return conf;
}

void config_free(struct config * conf)
{
    free(conf);
}

struct config *get_config(int argc, char *argv[])
{
    struct config * conf = config_new();
    handle_options(conf, argc, argv);
    return conf;
}

void config_print(struct config * conf)
{
    printf("Configuration:\n"
	   "\top:     %s\n"
	   "\tdim:    %d\n"
	   "\theight: %d\n"
	   "\tdisplay:    %d\n"
	   "\titerations: %d\n",
	   conf->sp_op->name, 
	   conf->dim, conf->max_height, 
	   conf->display, conf->iterations);
}

/* ---------------- ------- ---------------- */
/* ---------------- Options ---------------- */
/* ---------------- ------- ---------------- */

#define OPT_ARGC_ERR(argc, n, opt)				\
    if (argc < n) {						\
	fprintf(stderr,						\
		"Option '%s' needs %d arguments.\n", opt, n);	\
        exit(EXIT_FAILURE);                                     \
    }

static int opt_display(struct config * conf, int argc, char * argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_DISPLAY);
    conf->display = (bool) atoi(argv[0]);
    return 1;
}

static int opt_iterations(struct config * conf, int argc, char * argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_DISPLAY);
    conf->iterations = atoi(argv[0]);
    return 1;
}

static int opt_dim(struct config * conf, int argc, char * argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_DIM);
    conf->dim = atoi(argv[0]);
    return 1;
}

static int opt_height(struct config *conf, int argc, char *argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_HEIGHT);
    conf->max_height = atoi(argv[0]);
    return 1;
}

static int opt_print_list(struct config *conf, int argc, char *argv[])
{
    struct op_list *l = global_op_list;
    while (l != NULL) {
        printf("%s\n", l->op->name);
        l = l->next;
    }

    exit(EXIT_SUCCESS);
    return 0;
}

static int opt_set_build_type(struct config *conf, int argc, char *argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_BUILD_TYPE);
    int type = atoi(argv[0]);
    switch (type) {
    case 1: conf->sp_build_offset = SP_OP_OFFSET(build_1); break;
    case 2: conf->sp_build_offset = SP_OP_OFFSET(build_2); break;
    case 3: conf->sp_build_offset = SP_OP_OFFSET(build_3); break;
    default:
        fprintf(stderr, "invalid build type %d\n", type);
        exit(EXIT_FAILURE);
    }
    return 1;
}

static int opt_set_sp_type(struct config *conf, int argc, char *argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_SP_TYPE);
    struct op_list *l = global_op_list;
    while (l != NULL) {
        if (!strcmp(argv[0], l->op->name)) {
            conf->sp_op = l->op;
            return 1;
        }
        l = l->next;
    }
    fprintf(stderr, "Invalid sand pile type '%s'\n", argv[0]);
    exit(EXIT_FAILURE);
    return 0;
}

static int opt_print_help(struct config *conf, int argc, char *argv[])
{
    printf("available option:\n");
    printf("-h X : set max height (when supported)\n");
    printf("-d X : set dimension (when supported)\n");
    printf("-l : print compute type list (when supported)\n");
    printf("-D X : enable or disable display mode\n");
    printf("-i X : set iterations\n");
    printf("-m X : select sand pile type\n\t(available type can be "
           "retrieved with '-l' option)\n");
    printf("-b X : select build type: 1: 5 ground; 2: 100000 tower; "
           "3: custom (default)\n");

    exit(EXIT_SUCCESS);
    return 0;
}

/* ---------------- --------------- ---------------- */
/* ---------------- Options Handler ---------------- */
/* ---------------- --------------- ---------------- */

static int set_option(struct config *conf, int argc, char *argv[])
{
    int (*opt)(struct config*, int, const char**) = NULL;

    ht_get_entry(ht_opt, argv[0], &opt);
    if (opt == NULL) {
	fprintf(stderr, "Unknown option: '%s'.\n", argv[0]);
	return 0;
    }
    return opt(conf, argc-1, (const char **) &argv[1]);
}

static void handle_options(struct config * conf, int argc, char *argv[])
{
    for (int i = 0; i < argc; i++) {
	if (argv[i][0] == OPTIONS_PREFIX)
	    i += set_option(conf, argc - i, &argv[i]);
    }
}

__attribute__ ((constructor))
static void config_init(void)
{
    ht_opt = ht_create(0, NULL);

    ht_add_entry(ht_opt, ARG_OPT_DIM,    opt_dim);
    ht_add_entry(ht_opt, ARG_OPT_HEIGHT, opt_height);

    ht_add_entry(ht_opt, ARG_OPT_DISPLAY,    opt_display);
    ht_add_entry(ht_opt, ARG_OPT_ITERATIONS, opt_iterations);

    ht_add_entry(ht_opt, ARG_OPT_LIST, opt_print_list);
    ht_add_entry(ht_opt, ARG_OPT_HELP_1, opt_print_help);
    ht_add_entry(ht_opt, ARG_OPT_HELP_2, opt_print_help);
    ht_add_entry(ht_opt, ARG_OPT_SP_TYPE, opt_set_sp_type);
    ht_add_entry(ht_opt, ARG_OPT_BUILD_TYPE, opt_set_build_type);
}

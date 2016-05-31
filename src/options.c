#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sand_pile.h"
#include "sand_builder.h"
#include "options.h"

#include "util/hash_table.h"

#define OPTIONS_PREFIX '-'

#define ARG_OPT_DIM    "-d"
#define ARG_OPT_HEIGHT "-h"
#define ARG_OPT_HELP "--help"
#define ARG_OPT_LIST   "-l"

#define ARG_OPT_COMPUTE_SYNC  "-cs"
#define ARG_OPT_COMPUTE_ASYNC "-ca"

#define DIM_DEFAULT              32
#define MAX_HEIGHT_DEFAULT       DIM_DEFAULT * DIM_DEFAULT

#define SP_OP_OFFSET(field) ((uintptr_t) &((struct sp_operations*)0)->field)
#define SAND_COMPUTE_FUN_DEFAULT_OFF SP_OP_OFFSET(compute_sync)
#define SAND_BUILD_FUN_DEFAULT_OFF   SP_OP_OFFSET(build_3)

static struct hash_table * ht_opt;

static void handle_options(struct config * conf, int argc, char *argv[]);

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

/* ---------------- ------- ---------------- */
/* ---------------- Options ---------------- */
/* ---------------- ------- ---------------- */

#define OPT_ARGC_ERR(argc, n, opt)				\
    if (argc < n) {						\
	fprintf(stderr,						\
		"Option '%s' needs %d arguments.\n", opt, n);	\
        exit(EXIT_FAILURE);                                     \
    }

static int opt_dim(struct config * conf, int argc, char * argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_DIM);
    conf->dim = atoi(argv[0]);
    return 1;
}

static int opt_height(struct config * conf, int argc, char * argv[])
{
    OPT_ARGC_ERR(argc, 1, ARG_OPT_HEIGHT);
    conf->max_height = atoi(argv[0]);
    return 1;
}

static int opt_compute_sync(struct config * conf, int argc, char * argv[])
{
    OPT_ARGC_ERR(argc, 0, ARG_OPT_COMPUTE_SYNC);
    conf->sp_compute_offset = SP_OP_OFFSET(compute_sync);
    return 0;
}

static int opt_compute_async(struct config * conf, int argc, char * argv[])
{
    OPT_ARGC_ERR(argc, 0, ARG_OPT_COMPUTE_ASYNC);
    conf->sp_compute_offset = SP_OP_OFFSET(compute_async);
    return 0;
}

static int opt_print_list(struct config *conf, int argc, char *argv[])
{
    OPT_ARGC_ERR(argc, 0, ARG_OPT_LIST);
    struct op_list *l = global_op_list;
    while (l != NULL) {
        printf("%s\n", l->sp_name);
        l = l->next;
    }

    exit(EXIT_SUCCESS);
    return 0;
}

static int opt_print_help(struct config *conf, int argc, char *argv[])
{
    printf("available option:\n");
    printf("-h X : set max height (when supported)\n");
    printf("-d X : set dimension (when supported)\n");
    printf("-l : print compute type list (when supported)\n");
    printf("-cs : synchronous mode\n");
    printf("-ca : asynchronous mode\n");

    exit(EXIT_SUCCESS);
    return 0;
}

/* ---------------- --------------- ---------------- */
/* ---------------- Options Handler ---------------- */
/* ---------------- --------------- ---------------- */

static int set_option(struct config * conf, int argc, char * argv[])
{
    int (* opt)(struct config *, int, const char **) = NULL;
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

    ht_add_entry(ht_opt, ARG_OPT_COMPUTE_SYNC,  opt_compute_sync);
    ht_add_entry(ht_opt, ARG_OPT_COMPUTE_ASYNC, opt_compute_async);
    ht_add_entry(ht_opt, ARG_OPT_LIST, opt_print_list);
    ht_add_entry(ht_opt, ARG_OPT_HELP, opt_print_help);
}

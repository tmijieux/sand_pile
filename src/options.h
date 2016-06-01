#ifndef OPTIONS_H
#define OPTIONS_H

struct config {
    uint dim;
    uint max_height;

    struct sp_operations *sp_op;
    off_t sp_compute_offset;
    off_t sp_build_offset;

    bool sync;
    bool display;
    uint iterations;
};

struct config * get_config(int argc, char *argv[]);
void config_free(struct config * conf);
void config_print(struct config * conf);

#endif //OPTIONS_H

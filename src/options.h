#ifndef OPTIONS_H
#define OPTIONS_H

struct config {
    uint dim;
    uint max_height;
    void (*sand_compute_fun)(struct sand_pile*, uint);
    void (*sand_build_fun)(struct sand_pile*, uint);
};

struct config * get_config(int argc, char *argv[]);
void config_free(struct config * conf);

#endif //OPTIONS_H

#ifndef SAND_HEAP
#define SAND_HEAP

typedef unsigned int uint;

/*
 * Here just for static inline functions. 
 * Do not access directly the structure.
 */
struct sand_tile {
    uint value;
    int stable;
};

/*
 * Here just for static inline functions. 
 * Do not access directly the structure.
 * Use the get and set functions below.
 */
struct sand_heap {
    struct sand_tile ** table;
    struct sand_tile ** copy;
    uint size;
};

struct sand_heap * sand_new(uint size);
struct sand_heap * sand_copy(struct sand_heap * sand);
void sand_free(struct sand_heap * sand);

void sand_compute_n_step_synchronous(struct sand_heap * sand, uint nb);
void sand_compute_n_step_asynchronous(struct sand_heap * sand, uint nb);

static inline uint sand_get_size(struct sand_heap * sand) {
    return sand->size;
}
static inline uint sand_get_stable(struct sand_heap * sand, uint i, uint j) {
    return sand->table[i][j].stable;
}
static inline uint sand_get(struct sand_heap * sand, uint i, uint j) {
    return sand->table[i][j].value;
}
static inline void sand_set(struct sand_heap * sand, uint i, uint j, uint value) {
    sand->table[i][j].value = value;
}

#endif

#ifndef SAND_HEAP
#define SAND_HEAP

typedef unsigned int uint;

/*
 * Do not access directly the struct.
 * Use the get and set functions below.
 */
struct sand_tile {
    uint value;
    uint copy;
    int stable;
};

struct sand_heap {
    struct sand_tile ** table;
    uint size;
};

struct sand_heap * sand_new(uint size);
void sand_free(struct sand_heap * sand);

void sand_compute(struct sand_heap * sand, uint nb);

inline uint sand_get_size(struct sand_heap * sand) {
    return sand->size;
}
inline uint sand_get_stable(struct sand_heap * sand, uint i, uint j) {
    return sand->table[i][j].stable;
}
inline uint sand_get(struct sand_heap * sand, uint i, uint j) {
    return sand->table[i][j].value;
}
inline void sand_set(struct sand_heap * sand, uint i, uint j, uint value) {
    sand->table[i][j].value = value;
}

#endif

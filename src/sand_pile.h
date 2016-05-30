#ifndef SAND_PILE_H
#define SAND_PILE_H

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
struct sand_pile {
    struct sand_tile **table;
    struct sand_tile **copy;
    uint size;
};

struct sand_pile *sand_new(uint size);
struct sand_pile *sand_copy(struct sand_pile *sand);
void sand_free(struct sand_pile *sand);

void sand_compute_n_step_sync(struct sand_pile *sand, uint nb);
void sand_compute_n_step_async(struct sand_pile *sand, uint nb);

static inline uint sand_get_size(struct sand_pile *sand)
{
    return sand->size;
}

static inline uint sand_get_stable(struct sand_pile *sand, uint i, uint j)
{
    return sand->table[i][j].stable;
}

static inline int sand_is_out(struct sand_pile *sand, uint i, uint j)
{
    return (i < 0 || sand_get_size(sand) <= i ||
	    j < 0 || sand_get_size(sand) <= j);
}

static inline uint sand_get(struct sand_pile* sand, uint i, uint j)
{
    if (sand_is_out(sand, i, j))
	return 0;
    return sand->table[i][j].value;
}

static inline void sand_set(struct sand_pile* sand, uint i, uint j, uint value)
{
    if (sand_is_out(sand, i, j))
	return;
    sand->table[i][j].value = value;
}

#endif //SAND_PILE_H

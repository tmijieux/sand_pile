#include <stdio.h>
#include <stdlib.h>

#include "sand_pile.h"
#include "colorer.h"

static struct color RED   = {255, 0,   0};
static struct color GREEN = {0,   255, 0};
static struct color BLUE  = {0,   0,   255};
static struct color BLACK = {0,   0,   0};

float *sand_color(sand_pile sp, struct color *colors)
{
    uint size = sp->get_size(sp);
    for (uint i = 0; i < size; i++) {
	for (uint j = 0; j < size; j++) {
	    if (sp->get(sp, i, j) == 0)
		colors[i + size * j] = BLACK;
	    else if (sp->get(sp, i, j) < 4)
		colors[i + size * j] = GREEN;
	    else if (sp->get_stable(sp, i, j))
		colors[i + size * j] = BLUE;
	    else
		colors[i + size * j] = RED;
	}
    }
    return (float *) colors;
}

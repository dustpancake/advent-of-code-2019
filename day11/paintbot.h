#ifndef PAINTBOT_H
#define PAINTBOT_H

#include <opcomp.h>
#include "panelfloor.h"

// CONVENIENCE MACROS
#define PB_UP       0
#define PB_RIGHT    1
#define PB_DOWN     2
#define PB_LEFT     3

// DEFINITION

struct paintbot {
    // current location
    coord_t x;
    coord_t y;

    // direction facing
    int dir;

    // the "brain"
    OpComp *oc;

    // the floor
    PanelFloor *pf;
};

typedef struct paintbot PaintBot;

// CONSTRUCTORS
PaintBot pb_new(OpComp *oc, PanelFloor *pf);

// DESTRUCTORS
void pb_free(PaintBot *pb);

// MACROS
#define PB_SET_LOC(pb, x_, y_) pb->x = x_, pb->y = y_
#define PB_SET_DIR(pb, dir_) pb->dir = dir_

// METHODS
int pb_step(PaintBot *pb);
int pb_run(PaintBot *pb);

#endif

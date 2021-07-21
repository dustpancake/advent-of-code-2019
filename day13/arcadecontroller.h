#ifndef ARCADECONTROLLER_H
#define ARCADECONTROLLER_H

// FORWARD DECLARE
struct arcadeworld;
typedef struct arcadeworld ArcWorld;

// DEFINITION
struct arcade_controller {
    int     p_bx,    p_by;       // previous ball x and y positions
    int     x,      y;           // paddle x and y positions
};

typedef struct arcade_controller ArcControl;

// CONSTRUCTOR
ArcControl acont_new();

// METHODS
int acont_get_next_input(ArcControl *acont, const ArcWorld *aw);

#endif
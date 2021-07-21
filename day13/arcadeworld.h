#ifndef ARCADEWORLD_H
#define ARCADEWORLD_H

#include <opcomp.h>
#include "arcadecontroller.h"

typedef int tile_t;

enum tiles {
    AW_EMPTY             =0, 
    AW_WALL, 
    AW_BLOCK, 
    AW_HORIZONTAL_PADDLE, 
    AW_BALL
};

typedef enum tiles AWTile;

struct arcadeworld {
    const int   height;
    const int   width;
    
    tile_t      **world;
};

typedef struct arcadeworld ArcWorld;

// MACROS
#define AW_SET_TILE(aw, x, y, v)    aw->world[y][x] = v
#define AW_GET_TILE(aw, x, y)       aw->world[y][x]

// CONSTRUCTORS
ArcWorld aw_new(int width, int height);

// DESTRUCTORS
void aw_free(ArcWorld *aw);

// METHODS
int aw_build_from_prog(ArcWorld *aw, OpComp *oc);
int aw_run(ArcWorld *aw, OpComp *oc, ArcControl *acont);
int aw_play_game_with_controller(ArcWorld *aw, OpComp *oc, ArcControl *acont);
int aw_play_game(ArcWorld *aw, OpComp *oc);
void aw_visualise(ArcWorld aw);

#endif
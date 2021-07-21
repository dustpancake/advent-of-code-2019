#include "arcadecontroller.h"

// forward declarations defined here
#include "arcadeworld.h"

#include <opcomp.h> // for error and success codes

#define COORD_INIT -1

#define P_LEFT     -1
#define P_NEUTRAL   0
#define P_RIGHT     1

struct coordinate {
    int x;
    int y;
};
typedef struct coordinate Coord;

static Coord coord_new() {
    Coord a = {
        COORD_INIT, COORD_INIT
    };
    return a;
}

// PRIVATE METHOD DECLARATIONS
// side effect return 
static int acont_find_paddle_and_ball(const ArcWorld *aw, Coord *paddle, Coord *ball);

static int acont_ai_next_intput(ArcControl *acont, const ArcWorld *aw);

// CONSTRUCTOR 
ArcControl acont_new() {
    ArcControl acont = {
        .p_bx   = COORD_INIT,  // everything inits to -1, as
        .p_by   = COORD_INIT,  // this is a value we never see in the game

        .x      = COORD_INIT,
        .y      = COORD_INIT
    };
    return acont;
}

int acont_get_next_input(ArcControl *acont, const ArcWorld *aw) {

    // aw_visualise(*aw);
    
    // dispatch between ai methods or human here
    // only AI at the moment
    return acont_ai_next_intput(acont, aw);
}


static int acont_find_paddle_and_ball(const ArcWorld *aw, Coord *paddle, Coord *ball) {
    int i, j;
    int found       = 0b00;
    tile_t tile;

    for (i = 0; i < aw->height; ++i) {
        for (j = 0; j < aw->width; ++j) {
            tile = aw->world[i][j];

            if (tile == AW_BALL) {
                ball->x = j;
                ball->y = i;
                
                // update flags
                found |= 0b01;

            } else if (tile == AW_HORIZONTAL_PADDLE) {
                paddle->x = j;
                paddle->y = i;

                // update flags
                found |= 0b10;
            }

            // early exit
            if (found == 0b11) {
                goto exit;
            }
        }
    }
    
    ERROR("PADDLE OR BALL NOT FOUND %d", found);
    return ERROR_CODE;

exit:
    return SUCCESS_CODE;
}

static int acont_ai_next_intput(ArcControl *acont, const ArcWorld *aw) {
    int dx; // x difference in coords

    Coord ball = coord_new();
    Coord paddle = coord_new();

    if (acont_find_paddle_and_ball(aw, &paddle, &ball) != SUCCESS_CODE) {
        // just don't move
        return 0;
    }

    DEBUG("Ball (%d, %d)  Paddle (%d, %d)", ball.x, ball.y, paddle.x, paddle.y);

    // negative number means ball is further left
    dx = ball.x - paddle.x;

    if (dx > 0) {
        DEBUG("Paddle going right.\n");
        return P_RIGHT;
    
    } else if (dx < 0) {
        DEBUG("Paddle going left.\n");
        return P_LEFT;
    }
    
    return P_NEUTRAL;
}
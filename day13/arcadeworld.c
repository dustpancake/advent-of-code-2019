#include "arcadeworld.h"

#define SCORE_OUTPUT    13

static int aw_step(ArcWorld *aw, OpComp *oc);

ArcWorld aw_new(int width, int height) {
    tile_t **world;
    ArcWorld aw = {
        .width = width,
        .height = height,
        .world = NULL,
    };

    // allocate world
    world = (tile_t **) malloc(height * sizeof(tile_t *));
    for (int i = 0; i < height; ++i) {
        world[i] = (tile_t *) malloc(width * sizeof(tile_t));

        // initalize to zero
        for (int j = 0; j < width; ++j) {
            world[i][j] = 0;
        }
    }

    aw.world = world; 
    return aw;
}

void aw_free(ArcWorld *aw) {
    for (int i = 0; i < aw->height; ++i) {
        free(aw->world[i]);
        aw->world[i] = NULL;
    }
    free(aw->world);
    aw->world = NULL;
}

// METHODS
int aw_run(ArcWorld *aw, OpComp *oc, ArcControl *acont) {
    int n_input;                // next input
    int retval = SUCCESS_CODE;
    
    while (retval == SUCCESS_CODE) {
        retval = aw_step(aw, oc);
        // reset output pointer
        oc->outp = oc->outp == 3 ? 0 : oc->outp;

        if (retval == HALT_ON_INP) {
            // this is where the controller takes over and determines the next input
            if (acont != NULL) {
                // get the next input
                
                n_input = acont_get_next_input(acont, aw);
                opcomp_single_input(oc, n_input);

            }

            // execute next instruction: let opcomp read input

            retval = opcomp_continue(oc);
            if (retval < SUCCESS_CODE) {
                return retval;
            }
            
            // reset the input pointer
            oc->inpp = 0;

            // continue
            retval = SUCCESS_CODE;
        }
    }

    return retval;
}

int aw_build_from_prog(ArcWorld *aw, OpComp *oc) {
    return aw_run(aw, oc, NULL);
}

int aw_play_game_with_controller(ArcWorld *aw, OpComp *oc, ArcControl *acont) {
    int retcode = SUCCESS_CODE;

    // ensure money set
    oc->prog[0] = 2;

    // put joystick neutral
    opcomp_single_input(oc, 0);
    // configure input interrupts
    opcomp_set_interrupt(oc, SET_INT_INP);

    // run until we get a score
    while (retcode == SUCCESS_CODE) {
        retcode = aw_run(aw, oc, acont);
        
        if (retcode == SCORE_OUTPUT) {
            // print score to screen
            printf("Score is %d\n", oc->output[2]);
            // and continue
            retcode = SUCCESS_CODE;
        }

    }
    return retcode;
}

int aw_play_game(ArcWorld *aw, OpComp *oc) {
    return aw_play_game_with_controller(aw, oc, NULL);
}


void aw_visualise(ArcWorld aw) {
    tile_t tile;
    char *row = (char *) malloc((aw.width + 1) * sizeof(char));
    

    for (int i = 0; i < aw.height; ++i) {
        for (int j = 0; j < aw.width; ++j) {
            
            tile = aw.world[i][j];

            switch (tile)
            {
            case AW_EMPTY:
                row[j] = ' ';
                break;

            case AW_WALL:
                row[j] = '#';
                break;
            
            case AW_BLOCK:
                row[j] = '+';
                break;

            case AW_HORIZONTAL_PADDLE:
                row[j] = '~';
                break;

            case AW_BALL:
                row[j] = 'o';
                break;
            
            default:
                ERROR("Unknown tile %d: painting as X", tile);
                row[j] = 'X';
                break;
            }
        }
        row[aw.width] = '\0';
        // show
        printf("%s\n", row);
    }

    free(row);
}

static int aw_step(ArcWorld *aw, OpComp *oc) {
    int retval;
    int x, y;
    tile_t pv;

    // need three items of output
    while (oc->outp < 3) {

        // run until interrupt
        retval = opcomp_run(oc);
        if (retval < SUCCESS_CODE) {
            return retval;
        }

        // check if input interrupt triggered
        if (retval == HALT_ON_INP) {
            return retval;
        }

        // do the output step
        retval = opcomp_continue(oc);
        if (retval < SUCCESS_CODE) {
            return retval;
        }
    }
    // read output
    x = (int)       oc->output[0];
    y = (int)       oc->output[1];
    pv = (tile_t)   oc->output[2];

    DEBUG_BOT(" (%d, %d)=%d\n", x, y, pv);

    if (x == -1 && y == 0) {
        // we got a score back
        return SCORE_OUTPUT;
    }

    // check we're inbounds
    if (
        x < 0 || y < 0 ||
        x > aw->width || y > aw->height
    ) {
        ERROR("Tried to paint x=%d, y=%d (world is %dx%d)", x, y, aw->width, aw->height);
        return ERROR_CODE;
    }
    // paint
    AW_SET_TILE(aw, x, y, pv);

    return SUCCESS_CODE;
}
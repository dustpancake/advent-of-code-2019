#include "paintbot.h"

#define ERROR_CODE -1
#define SUCCESS_CODE 0

static int pb_decode_direction(int cur_dir, opc_t d);
static void pb_read_oc_output(PaintBot *pb);
static int pb_advance(PaintBot *pb);

PaintBot pb_new(OpComp *oc, PanelFloor *pf) {
    PaintBot pb = {
        .x = pf->width / 2,
        .y = pf->height / 2,

        .dir = PB_UP,

        .oc = oc,
        .pf = pf
    };

    return pb;
}

void pb_free(PaintBot *pb) {
    pb->oc = NULL;
    pb->pf = NULL;
}

// METHODS
int pb_step(PaintBot *pb) {
    int retval;
    panel_t pv;

    pv = PF_GET_PANEL((pb->pf), pb->x, pb->y);
    // -1 is empty (black), 0 is painted once (black), 1 is painted once (white)
    pv = pv < 1 ? 0 : 1;
    
    DEBUG_BOT(" input -> %d\n", pv);
    
    opcomp_single_input(pb->oc, (opc_t) pv);

    // consume input
    retval = opcomp_continue(pb->oc);
    if (retval < SUCCESS_CODE) {
        return retval;
    }

    // run until next input
    retval = opcomp_run(pb->oc);
    if (retval < SUCCESS_CODE) {
        return retval;
    }

    // read output array
    pb_read_oc_output(pb);

    // advance
    if (pb_advance(pb) != 0) {
        return ERROR_CODE;
    }

    DEBUG_BOT(" advanced to x=%d, y=%d\n", pb->x, pb->y);

    // all good
    return SUCCESS_CODE;
}

int pb_run(PaintBot *pb) {
    int retval = SUCCESS_CODE;
    int count = 0;

    // set the right interrupts: input
    opcomp_set_interrupt(pb->oc, SET_INT_INP);

    while (retval == SUCCESS_CODE) {
        DEBUG_BOT("\nTURN %d:\n", ++count);
        retval = pb_step(pb);
    }

    return retval;
}

static int pb_decode_direction(int cur_dir, opc_t d) {
    int new_dir;

    // recast to -1 1
    d = (d * 2) - 1;

    // calculate new direction
    new_dir = cur_dir + d;

    if (new_dir < 0) {
        new_dir = 4 + new_dir;
    }

    new_dir = new_dir % 4;

    return new_dir;
}

static void pb_read_oc_output(PaintBot *pb) {
    DEBUG_BOT(" output : (%d, %d)\n", pb->oc->output[0], pb->oc->output[1]);

    // read output 1: colour and set
    PF_SET_PANEL(pb->pf, pb->x, pb->y, pb->oc->output[0]);

    DEBUG_BOT(" current pannel : %d\n", PF_GET_PANEL((pb->pf), pb->x, pb->y));

    // read output 2: direction and set
    PB_SET_DIR(pb, pb_decode_direction(pb->dir, pb->oc->output[1]));
    
    DEBUG_BOT(" now facing %d\n", pb->dir);

    // reset output pointer
    pb->oc->outp = 0;
}

static int pb_advance(PaintBot *pb) {
    switch (pb->dir) {
        
        case PB_UP:
            --(pb->y);
            break;
        
        case PB_RIGHT:
            ++(pb->x);
            break;

        case PB_DOWN:
            ++(pb->y);
            break;

        case PB_LEFT:
            --(pb->x);
            break;

        default:
            ERROR("Unknown direction code %d", pb->dir);
            return ERROR_CODE;
    }

    // check we're not out of bounds
    if (
        pb->x < 0 || pb -> y < 0 ||
        pb->x > pb->pf->width ||
        pb->y > pb->pf->height
    ) {
        ERROR(
            "Position (%d, %d) is out of bounds (mapsize: %d, %d)",
            pb->x, pb->y, pb->pf->width, pb->pf->height
        );
        return ERROR_CODE;
    }

    // otherwise we're dandy
    return SUCCESS_CODE;
}
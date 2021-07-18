#ifndef PANELFLOOR_H
#define PANELFLOOR_H

typedef int coord_t;
typedef int panel_t;

// DEFINITION
struct panelfloor {
    const coord_t width;
    const coord_t height;

    panel_t **floor;        // indexed floor[y][x]
};

typedef struct panelfloor PanelFloor;

// CONSTRUCTORS
PanelFloor pf_new(coord_t width, coord_t height);

// DESTRUCTORS
void pf_free(PanelFloor *pf);

// MACROS
#define PF_SET_PANEL(pf, x, y, v) pf->floor[y][x] = (coord_t) v
#define PF_INC_PANEL(pf, x, y, v) pf->floor[y][x] = pf->floor[y][x] + (coord_t) v
#define PF_GET_PANEL(pf, x, y) pf->floor[y][x]

// METHODS
void pf_visualize(PanelFloor pf);

#endif
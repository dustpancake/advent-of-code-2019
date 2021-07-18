#include <stdlib.h>
#include "panelfloor.h"

PanelFloor pf_new(coord_t width, coord_t height) {
    panel_t **floor;
    PanelFloor pf = {
        .width = width,
        .height = height,
        .floor = NULL
    };

    // allocate memory
    floor = (panel_t **) malloc(height * sizeof(panel_t *));
    for (int i = 0; i < height; ++i) {
        floor[i] = (panel_t *) malloc(width * sizeof(panel_t));

        // zero init
        for (int j = 0; j < width; ++j) {
            floor[i][j] = -1;
        }
    }

    pf.floor = floor;
    return pf;
}

void pf_free(PanelFloor *pf) {
    for (int i = 0; i < pf->height; ++i) {
        free(pf->floor[i]);
        pf->floor[i] = NULL;
    }
    free(pf->floor);
    pf->floor = NULL;
}


// METHODS
void pf_visualize(PanelFloor pf) {
    char *row = (char *) malloc((pf.width + 1) * sizeof(char));

    for (int i = 0; i < pf.height; ++i) {
        for (int j = 0; j < pf.width; ++j) {
            if (pf.floor[i][j] >= 1) {
                // white
                row[j] = '#';
            }
            else {
                row[j] = ' ';
            }
        }
        row[pf.width] = '\0';
        // show
        printf("%s\n", row);
    }

    free(row);
}
#include "paintbot.h"

int count_panels(PanelFloor pf) {
    panel_t panel;
    int count = 0;


    for (int i = 0; i < pf.width; ++i) {
        for (int j = 0; j < pf.height; ++j) {
            panel = PF_GET_PANEL((&pf), i, j);
            if (panel != -1) {
                ++count;
            }
        }
    }

    return count;
}

void part1() {
    printf("\n\n");
    int retval;
    PanelFloor pf = pf_new(1024, 1024);
    OpComp oc = comp_from_file("input.txt");

    PaintBot pb = pb_new(&oc, &pf);

    opcomp_reserve_memory(&oc, 2048 - oc.plen);
    opcomp_reserve_output(&oc, 2);

    printf("Bot Initialized.\n");
    printf("Floor size %d, %d\n", pf.width, pf.height);

    retval = pb_run(&pb);
    printf("Bot returned %d\n", retval);
    printf("Bot painted %d\n", count_panels(pf));

    pb_free(&pb);
    opcomp_free(&oc);
    pf_free(&pf);
    return 0;
}

int main() {
    printf("\n\n");
    int retval;
    PanelFloor pf = pf_new(128, 32);
    OpComp oc = comp_from_file("input.txt");

    PaintBot pb = pb_new(&oc, &pf);
    // start on white
    PF_SET_PANEL((&pf), pb.x, pb.y, 1);

    opcomp_reserve_memory(&oc, 2048 - oc.plen);
    opcomp_reserve_output(&oc, 2);

    printf("Bot Initialized.\n");
    printf("Floor size %d, %d\n", pf.width, pf.height);

    retval = pb_run(&pb);
    printf("Bot returned %d\n", retval);

    // reveal
    pf_visualize(pf);

    pb_free(&pb);
    opcomp_free(&oc);
    pf_free(&pf);
    return 0;
}


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

int main() {
    printf("\n\n");
    int retval;

    PanelFloor pf1 = pf_new(1024, 1024);
    PanelFloor pf2 = pf_new(45, 16);

    // create bot
    OpComp oc1 = comp_from_file("input.txt");
    opcomp_reserve_memory(&oc1, 2048 - oc1.plen);
    opcomp_reserve_output(&oc1, 2);

    // copy for part 2
    OpComp oc2 = opcomp_copy(oc1);

    // create paint bots
    PaintBot pb1 = pb_new(&oc1, &pf1);
    PaintBot pb2 = pb_new(&oc2, &pf2);

    // part 1
    retval = pb_run(&pb1);
    printf("Part1: Bot returned %d\n", retval);
    printf("Part1: Bot painted %d\n", count_panels(pf1));


    // part 2
    // move starting loc on grid
    PB_SET_LOC((&pb2), 2, 2);

    // start on white
    PF_SET_PANEL((&pf2), pb2.x, pb2.y, 1);

    retval = pb_run(&pb2);
    printf("Part2: Bot returned %d\n", retval);

    // reveal
    pf_visualize(pf2);

    // free memory
    pb_free(&pb2);
    pb_free(&pb1);
    opcomp_free(&oc2);
    opcomp_free(&oc1);
    pf_free(&pf2);
    pf_free(&pf1);

    return 0;
}


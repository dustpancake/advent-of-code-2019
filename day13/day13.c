#include "arcadeworld.h"

int count_blocks(ArcWorld aw) {
    int blockcount = 0;
    tile_t tile;

    for (int i = 0; i < aw.height; ++i) {
        for (int j = 0; j < aw.width; ++j) {
            
            tile = aw.world[i][j];

            if (tile == AW_BLOCK) {
                ++blockcount;
            }

        }
    }

    return blockcount;
}

void part1() {
    int retval;
    OpComp oc = comp_from_file("input.txt");
    ArcWorld aw = aw_new(35, 20);

    // ensure bot has sufficient output
    opcomp_reserve_output(&oc, 3);
    // ensure output interrupt is set
    opcomp_set_interrupt(&oc, SET_INT_OUT);

    // reserve memory
    opcomp_reserve_memory(&oc, 4096 - oc.plen);

    retval = aw_build_from_prog(&aw, &oc);

    printf("Bot returned %d\n", retval);
    printf("Number of blocks = %d\n", count_blocks(aw));

    aw_visualise(aw);


    aw_free(&aw);
    opcomp_free(&oc);
}

void part2() {
    int retval;
    OpComp oc = comp_from_file("input.txt");
    ArcWorld aw = aw_new(35, 20);
    ArcControl acont = acont_new();

    // ensure bot has sufficient output
    opcomp_reserve_output(&oc, 3);

    // ensure output interrupt is set
    opcomp_set_interrupt(&oc, SET_INT_OUT);

    // reserve memory
    opcomp_reserve_memory(&oc, 4096 - oc.plen);

    // run
    retval = aw_play_game_with_controller(&aw, &oc, &acont);

    printf("Retval = %d\n", retval);

    aw_free(&aw);
    opcomp_free(&oc);
}

int main() {
    // part1();
    part2();
    return 0;
}
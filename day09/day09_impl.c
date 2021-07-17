#include "day09_impl.h"

opc_t part1() {
    OpComp oc = comp_from_file("input.txt");

    opc_t input[1] = {1};
    opcomp_copy_to_input(&oc, input, 1);
    opcomp_reserve_output(&oc, 256);
    opcomp_reserve_memory(&oc, 2048 - oc.plen);     // reserve 2048 slots of memory

    int retval = opcomp_run(&oc);
    
    printf("1: Program exited with retval %d\n", retval);
    
    opcomp_print_output(oc);

    opc_t out1 = oc.output[0];

    opcomp_free(&oc);

    return out1;
}

opc_t part2() {
    OpComp oc = comp_from_file("input.txt");

    opc_t input[1] = {2};
    opcomp_copy_to_input(&oc, input, 1);
    opcomp_reserve_output(&oc, 1);
    opcomp_reserve_memory(&oc, 2048 - oc.plen);     // reserve 2048 slots of memory

    int retval = opcomp_run(&oc);
    
    printf("1: Program exited with retval %d\n", retval);
    
    opcomp_print_output(oc);

    opc_t out1 = oc.output[0];

    opcomp_free(&oc);

    return out1;
}
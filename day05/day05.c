#include <opcomp.h>

void part1() {
    OpComp oc = comp_from_file("input.txt");

    opc_t input[1] = {1};
    opcomp_copy_to_input(&oc, input, 1);
    opcomp_reserve_output(&oc, 256);

    int retval = opcomp_run(&oc);
    printf("1: Program exited with retval %d\n", retval);
    
    opcomp_print_output(oc);

   
    opcomp_free(&oc);
}

void part2() {
    OpComp oc = comp_from_file("input.txt");

    opc_t input[1] = {5};
    opcomp_copy_to_input(&oc, input, 1);
    opcomp_reserve_output(&oc, 256);

    int retval = opcomp_run(&oc);
    printf("2: Program exited with retval %d\n", retval);
    
    opcomp_print_output(oc);

   
    opcomp_free(&oc);
}

int main() {
    part1();
    part2();
    return 0;
}


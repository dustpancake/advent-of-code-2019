#include <opcomp.h>

void bruteforce(const OpComp oc) {
    int retval;
    // create copy
    OpComp runner = opcomp_copy(oc);
    
    for (opc_t i = 0; i <= 99; ++i) {
        for (opc_t j = 0; j <= 99; ++j) {
            // copy in the program code
            runner.prog = memcpy(runner.prog, oc.prog, oc.plen * sizeof(opc_t));

            // modify noun and verb
            runner.prog[1] = i;
            runner.prog[2] = j;
            
            // execute
            retval = opcomp_run(&runner);

            if (retval != -99) {
                ERROR("i=%ld, j=%ld : return value = %d\n", i, j, retval);
            }
            // check output
            if (runner.prog[0] == 19690720) {
                printf("Part 2: noun = %ld, verb = %ld, hash = %ld\n", i, j, 100 * i + j);
                goto exit;
            }
            
            // reset
            opcomp_reset(&runner);
        }
    }

    printf("No solution found.\n");

exit:
    opcomp_free(&runner);
    return;
}

int main() {
    OpComp opcomp = comp_from_file("input.txt");

    // create copy for part 2
    OpComp opcopy = opcomp_copy(opcomp);

    opcomp.prog[1] = 12;
    opcomp.prog[2] = 2;

    int retval = opcomp_run(&opcomp);
    printf("Program exited with retval %d\n", retval);
    printf("Part 1: value at position 0 is %ld\n", opcomp.prog[0]);

    bruteforce(opcopy);

    opcomp_free(&opcopy);
    opcomp_free(&opcomp);
    return 0;
}


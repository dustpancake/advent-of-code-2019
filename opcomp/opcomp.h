#ifndef OPCOMP_H
#define OPCOMP_H

#include <inputio.h>
#include <logger.h>

// TYPE DEFS
typedef u_int32_t pmode_t;
typedef long opc_t;

// DEFINITION
struct opcomp {
    opc_t   *prog;      // program code
    int     plen;       // program length

    // input
    opc_t   *input;
    int     inplen;
    int     inpp;
    // output
    opc_t   *output;
    int     outlen;
    int     outp;

    // memory: added to the end of the prog array and zeroed
    int     memsize;

    int     ip;         // instruction pointer
    int     rbo;        // relative base offset pointer
};

typedef struct opcomp OpComp;

// INIT FUNCTIONS
OpComp opcomp_new();
OpComp comp_from_file(const char *filename);
OpComp opcomp_copy(const OpComp oc);

// DESTRUCTOR
void opcomp_free(OpComp *oc);

// METHODS
int opcomp_handle_operation(OpComp *oc, opc_t op);
int opcomp_run(OpComp *oc);
int opcomp_run_blocking(OpComp *oc); // stop on I/O operations
int opcomp_run_continue(OpComp *oc);
void opcomp_reset(OpComp *oc);

void opcomp_print_prog(OpComp oc);
void opcomp_print_output(OpComp oc);

void opcomp_clear_input(OpComp *oc);
void opcomp_clear_output(OpComp *oc);

void opcomp_reserve_output(OpComp *oc, int len);

void opcomp_copy_to_input(OpComp *oc, const opc_t *input, int len);
void opcomp_single_input(OpComp *oc, opc_t input);

void opcomp_reserve_memory(OpComp *oc, int amount);

#endif
#include "opcomp.h"

// CONVENIENCE MACROS
#define P_MODE_FIRST(param_modes) param_modes & 15
#define P_MODE_SECOND(param_modes) (param_modes >> 4) & 15
#define P_MODE_THIRD(param_modes) (param_modes >> 8) & 15

// PROGRAM CODES
#define ADD     1
#define MULT    2

#define INP     3
#define OUT     4

#define JMP     5
#define JMPF    6

#define LT      7
#define EQ      8

#define RBO     9

#define HLT     99

#define POSITION_MODE   0
#define IMMEDIATE_MODE  1
#define RELATIVE_MODE   2

// INTERNAL METHODS
static int opcomp_add(OpComp *oc, pmode_t param_modes);
static int opcomp_mult(OpComp *oc, pmode_t param_modes);

static int opcomp_inp(OpComp *oc, pmode_t param_modes);
static int opcomp_out(OpComp *oc, pmode_t param_modes);

static int opcomp_jmp(OpComp *oc, pmode_t param_modes);
static int opcomp_jmpf(OpComp *oc, pmode_t param_modes);

static int opcomp_lt(OpComp *oc, pmode_t param_modes);
static int opcomp_eq(OpComp *oc, pmode_t param_modes);

static int opcomp_rbo(OpComp *oc, pmode_t param_modes);

static int opcomp_param_modes(opc_t op, pmode_t *param_modes);

static int opcomp_read_params(OpComp *oc, pmode_t param_modes, opc_t *params, int num);
static int opcomp_read_dest(OpComp *oc, opc_t *dest, pmode_t pmode);


// MACRO WRAPPERS
#define OPCOMP_CHECK_DEST(oc, dest) \
        if (dest < 0 || dest >= (oc->plen + oc->memsize)) { ERROR("dest = %ld (plen = %d)", dest, oc->plen + oc->memsize); return ERROR_CODE; }

#define OPCOMP_READ_PARAMS(oc, param_modes, params, num) \
        if (opcomp_read_params(oc, param_modes, params, num) != 0) { return ERROR_CODE; }

#define OPCOMP_READ_DEST(oc, dest, pmode) \
        if (opcomp_read_dest(oc, dest, pmode) != 0) { return ERROR_CODE; } \
        else { OPCOMP_CHECK_DEST(oc, *dest); }

// INIT AND DESTRUCTION DEFINITIONS
OpComp opcomp_new() {
    OpComp oc = {
        .prog       = NULL,
        .plen       = 0,

        .input      = NULL,
        .inplen     = 0,
        .inpp       = 0,

        .output     = NULL,
        .outlen     = 0,
        .outp       = 0,

        .memsize    = 0,

        .ip         = 0,
        .rbo        = 0,

        .interrupts = 0
    };
    return oc;
}

OpComp opcomp_copy(const OpComp oc) {
    // create copy
    OpComp runner = {
        .prog = (opc_t *) malloc((oc.plen + oc.memsize) * sizeof(opc_t)),
        .plen = oc.plen,

        .input = (opc_t *) malloc(oc.inplen * sizeof(opc_t)),
        .inplen = oc.inplen,
        .inpp = oc.inpp,

        .output = (opc_t *) malloc(oc.outlen * sizeof(opc_t)),
        .outlen = oc.outlen,
        .outp = oc.outp,

        .memsize = oc.memsize,

        .ip = oc.ip,
        .rbo = oc.rbo,

        .interrupts = oc.interrupts
    };

    // copy memory
    runner.prog = (opc_t *) memcpy(runner.prog, oc.prog, (oc.plen + oc.memsize) * sizeof(opc_t));
    runner.input = (opc_t *) memcpy(runner.input, oc.input, oc.inplen * sizeof(opc_t));
    runner.output = (opc_t *) memcpy(runner.output, oc.output, oc.outlen * sizeof(opc_t));

    return runner;
}

void opcomp_reset(OpComp *oc) {
    // reset registers
    oc->ip = 0;

    oc->inpp = 0;
    oc->outp = 0;

    // clear output buffer
    for (int i = 0; i < oc->outlen; ++i) {
        oc->output[i] = 0;
    }

}

void opcomp_free(OpComp *oc) {
    free(oc->prog);
    free(oc->input);
    free(oc->output);
    oc->prog = NULL;
    oc->input = NULL;
    oc->output = NULL;
}

// METHOD DEFINITIONS
OpComp comp_from_file(const char *filename) {
    int len;
    opc_t *arr;
    
    arr = f_read_bytecode_program(filename, &len);

    // heap allocate for int ptr conversion
    // create data structure
    OpComp oc = opcomp_new();
    oc.prog = arr;
    oc.plen = len;
    oc.ip = 0;

    return oc;
}

void opcomp_print_prog(OpComp oc) {
    printf("PROG: ");
    for (int i = 0; i < oc.plen; ++i) {
        printf("%ld, ", oc.prog[i]);
    }
    printf("\n");
}

void opcomp_print_output(OpComp oc) {
    printf("OUTPUT: ");
    for (int i = 0; i < oc.outp; ++i) {
        printf("%ld, ", oc.output[i]);
    }
    printf("\n");
}

void opcomp_clear_input(OpComp *oc) {
    if (oc->input != NULL) {
        free(oc->input);
    }
    oc->inplen = 0;
    oc->inpp = 0;
}

void opcomp_clear_output(OpComp *oc) {
    if (oc->output != NULL) {
        free(oc->output);
    }
    oc->outlen = 0;
    oc->outp = 0;
}

void opcomp_reserve_output(OpComp *oc, int len) {
    // if we already have enough output, do nothing
    if (oc->outlen == len) {
        return;
    }
    
    if (oc->output != NULL) {
        free(oc->output);
    }
    oc->output = (opc_t *) malloc(len * sizeof(opc_t));
    oc->outlen = len;
    oc->outp = 0;

    // zero initialize
    for (int i = 0; i < len; ++i) {
        oc->output[i] = 0;
    }
}

void opcomp_copy_to_input(OpComp *oc, const opc_t *input, int len) {
    // check if there is already input
    if (oc->input != NULL) {
        free(oc->input);
    }
    oc->input = (opc_t *) malloc(len * sizeof(opc_t));
    oc->input = (opc_t *) memcpy(oc->input, input, len * sizeof(opc_t));
    oc->inplen = len;
    oc->inpp = 0;
}

void opcomp_single_input(OpComp *oc, opc_t input) {
    if (oc->inplen != 1) {
        // free current memory and realloc
        free(oc->input);
        oc->input = (opc_t *) malloc(sizeof(opc_t));
        oc->inplen = 1;
    }
    // assign value
    oc->input[0] = input;
    // reset input pointer
    oc->inpp = 0;
}

void opcomp_reserve_memory(OpComp *oc, int amount) {
    opc_t *arr;
    
    // create a new memory pool: ignore any existing memory
    arr = (opc_t *) malloc((oc->plen + amount) * sizeof(opc_t));
    
    // copy and initialize
    arr = (opc_t *) memcpy(arr, oc->prog, oc->plen * sizeof(opc_t));
    for (int i = oc->plen; i < oc->plen + amount; ++i) {
        arr[i] = 0;
    }
    // free old pointer
    free(oc->prog);
    // assign new
    oc->prog = arr;
    oc->memsize = amount;
}

void opcomp_set_interrupt(OpComp *oc, int sig) {
    if (sig == SET_INT_NONE) {
        oc->interrupts = SET_INT_NONE;   
    } else {
        oc->interrupts = oc->interrupts | sig;
    }
}

void opcomp_clear_interrupts(OpComp *oc) { 
    oc->interrupts = SET_INT_NONE;
}

// RUNTIME
int opcomp_run(OpComp *oc) {
    opc_t op = 0;
    int retval = SUCCESS_CODE;

    while (oc->ip < oc->plen) {
        op = oc->prog[oc->ip];

        if (oc->interrupts) {
            // check if we need to return
            switch (op % 10) {
            case INP:
                if (oc->interrupts & SET_INT_INP) {
                    DEBUG_OP("\n::INPUT INTERRUPT\n\n");
                    return HALT_ON_INP;
                }
                break;
            
            case OUT:
                if (oc->interrupts & SET_INT_OUT) {
                    DEBUG_OP("\n::OUTPUT INTERRUPT\n\n");
                    return HALT_ON_OUT;
                }
                break;

            default:
                // no interrupts
                break;
            }
        }

        retval = opcomp_handle_operation(oc, op);
        
        if (retval < SUCCESS_CODE) {
            break;
        }
    }
    return retval;
}

int opcomp_continue(OpComp *oc) {
    int retval = SUCCESS_CODE;
    opc_t op = oc->prog[oc->ip];

    // handle last blocked operation
    retval = opcomp_handle_operation(oc, op);

    if (retval < SUCCESS_CODE) {
        ERROR("Error continuing.");
        return ERROR_CODE;
    }

    return retval;
}

int opcomp_handle_operation(OpComp *oc, opc_t op) {
    pmode_t param_modes;

    // print next 4 instructions
    DEBUG_OP("ip=%d :: (%ld %ld %ld %ld)\n  ", oc->ip, oc->prog[oc->ip], oc->prog[oc->ip + 1], oc->prog[oc->ip + 2], oc->prog[oc->ip + 3]);

    // digest operation
    op = opcomp_param_modes(op, &param_modes);

    switch (op)
    {
    case HLT:
        DEBUG_OP("HLT\n");
        return -99;

    case ADD:
        return opcomp_add(oc, param_modes);

    case MULT:
        return opcomp_mult(oc, param_modes);

    case INP:
        return opcomp_inp(oc, param_modes);

    case OUT:
        return opcomp_out(oc, param_modes);

    case JMP:
        return opcomp_jmp(oc, param_modes);

    case JMPF:
        return opcomp_jmpf(oc, param_modes);

    case LT:
        return opcomp_lt(oc, param_modes);

    case RBO: 
        return opcomp_rbo(oc, param_modes);

    case EQ:
        return opcomp_eq(oc, param_modes);

    default:
        ERROR("Unknown operation code %ld", op);
        return ERROR_CODE;
    }
}

static int opcomp_param_modes(opc_t op, pmode_t *param_modes) {
    opc_t op_c;
    *param_modes = 0;
    
    op_c = op % 100;
    op /= 100;

    for (int i = 0; i < 3; ++i) {
        *param_modes = *param_modes | ((op % 10) << (i * 4));
        op /= 10;
    }
    
    return op_c;
}

static int opcomp_read_dest(OpComp *oc, opc_t *dest, pmode_t pmode) {
    switch (pmode) {
        
        case POSITION_MODE:
        case IMMEDIATE_MODE:
            // act the same for destination
            *dest = oc->prog[++(oc->ip)];
            break;
        
        case RELATIVE_MODE:
            *dest = oc->prog[++(oc->ip)] + oc->rbo;
            DEBUG_OP(" (dest=%ld+(%d)", oc->prog[(oc->ip)], oc->rbo);
            break;

        default:
            ERROR("Unkown parameter mode %d", pmode);
            return ERROR_CODE;
    }

    return SUCCESS_CODE;
}

static int opcomp_read_params(OpComp *oc, pmode_t param_modes, opc_t *params, int num) {
    opc_t p;
    pmode_t pmode;
    for (int i = 0; i < num; ++i) {
        
        p = oc->prog[++(oc->ip)];

        pmode = (param_modes >> (i * 4)) & 15;

        switch (pmode) {

        case POSITION_MODE:

            if (p < 0 || p > (oc->plen + oc->memsize)) {
                ERROR("Position mode trying to index %ld (lprog + memsize = %d)", p, (oc->plen + oc->memsize));
                return ERROR_CODE;
            }

            DEBUG_OP(" [%ld]=%ld,", p, oc->prog[p]);
            
            params[i] = oc->prog[p];

            break;

        case IMMEDIATE_MODE:
            
            DEBUG_OP(" %ld,", p);

            params[i] = p;
            
            break;

        case RELATIVE_MODE:
            
            DEBUG_OP(" [%ld+(%d)]=%ld,", p, oc->rbo, oc->prog[p + oc->rbo]);

            params[i] = oc->prog[p + oc->rbo];

            break;
        
        default:
            
            ERROR("Unkown parameter mode %d", pmode);

            return ERROR_CODE;
        }
    }

    return SUCCESS_CODE;
}

// OPERATIONS
static int opcomp_add(OpComp *oc, pmode_t param_modes) {
    opc_t dest;
    opc_t args[2];

    DEBUG_OP("ADD");
    
    OPCOMP_READ_PARAMS(oc, param_modes, args, 2);

    OPCOMP_READ_DEST(oc, &dest, P_MODE_THIRD(param_modes));

    DEBUG_OP(" (%ld) -> [%ld]\n", args[0] + args[1], dest);

    oc->prog[dest] = args[0] + args[1];

    // increment instruction pointer
    ++(oc->ip);

    return SUCCESS_CODE;
}

static int opcomp_mult(OpComp *oc, pmode_t param_modes) {
    opc_t dest;
    opc_t args[2];
    
    DEBUG_OP("MULT");

    OPCOMP_READ_PARAMS(oc, param_modes, args, 2);

    OPCOMP_READ_DEST(oc, &dest, P_MODE_THIRD(param_modes));

    DEBUG_OP(" (%ld) -> [%ld]\n", args[0] * args[1], dest);

    oc->prog[dest] = args[0] * args[1];

    // increment instruction pointer
    ++(oc->ip);

    return SUCCESS_CODE;
}

static int opcomp_inp(OpComp *oc, pmode_t param_modes) {
    opc_t dest;
    
    DEBUG_OP("INP");

    // check there is space
    if (oc->inpp >= oc->inplen) {
        ERROR("No input available.");
        return ERROR_CODE;
    }
    
    OPCOMP_READ_DEST(oc, &dest, P_MODE_FIRST(param_modes));
    
    // read from input buffer, store, and inc pointer
    oc->prog[dest] = oc->input[(oc->inpp)++];

    DEBUG_OP(" (%ld) -> [%ld]\n", oc->prog[dest], dest);

    // increment instruction pointer
    ++(oc->ip);

    return SUCCESS_CODE;
}

static int opcomp_out(OpComp *oc, pmode_t param_modes) {
    opc_t arg;

    DEBUG_OP("OUT");

    OPCOMP_READ_PARAMS(oc, param_modes, &arg, 1);
    
    // check there is space
    if (oc->outp >= oc->outlen) {
        ERROR("No more space left in output buffer.");
        return ERROR_CODE;
    }

    DEBUG_OP(" -> buf[%d]\n", oc->outp);

    // write to output buffer and inc pointer
    oc->output[(oc->outp)++] = arg;

    // increment instruction pointer
    ++(oc->ip);

    return SUCCESS_CODE;
}

static int opcomp_jmp(OpComp *oc, pmode_t param_modes) {
    opc_t args[2];

    DEBUG_OP("JMP");

    OPCOMP_READ_PARAMS(oc, param_modes, args, 2);

    if (args[0] != 0) {
        DEBUG_OP(" -> ip=%ld\n", args[1]);
        OPCOMP_CHECK_DEST(oc, args[1]);
        oc->ip = (int) args[1];
    } else {
        DEBUG_OP("\n");
        // advance instruction pointer
        ++(oc->ip);
    }

    return SUCCESS_CODE;
}

static int opcomp_jmpf(OpComp *oc, pmode_t param_modes) {
    opc_t args[2];

    DEBUG_OP("JMPF");

    OPCOMP_READ_PARAMS(oc, param_modes, args, 2);

    if (args[0] == 0) {
        DEBUG_OP(" -> ip=%ld\n", args[1]);
        OPCOMP_CHECK_DEST(oc, args[1]);
        oc->ip = (int) args[1];
    } else {
        DEBUG_OP("\n");
        // advance instruction pointer
        ++(oc->ip);
    }

    return SUCCESS_CODE;
}

static int opcomp_lt(OpComp *oc, pmode_t param_modes) {
    opc_t args[2];
    opc_t dest;

    DEBUG_OP("LT");

    OPCOMP_READ_PARAMS(oc, param_modes, args, 2);
    
    OPCOMP_READ_DEST(oc, &dest, P_MODE_THIRD(param_modes));

    if (args[0] < args[1]) {
        DEBUG_OP(" (1) -> [%ld]\n", dest);
        oc->prog[dest] = 1;
    } else {
        DEBUG_OP(" (0) -> [%ld]\n", dest);
        oc->prog[dest] = 0;
    }

    // increment instruction pointer
    ++(oc->ip);

    return SUCCESS_CODE;
}

static int opcomp_eq(OpComp *oc, pmode_t param_modes) {
    opc_t args[2];
    opc_t dest;

    DEBUG_OP("EQ");

    OPCOMP_READ_PARAMS(oc, param_modes, args, 2);
    
    OPCOMP_READ_DEST(oc, &dest, P_MODE_THIRD(param_modes));

    if (args[0] == args[1]) {
        DEBUG_OP(" (1) -> [%ld]\n", dest);
        oc->prog[dest] = 1;
    } else {
        DEBUG_OP(" (0) -> [%ld]\n", dest);
        oc->prog[dest] = 0;
    }

    // increment instruction pointer
    ++(oc->ip);

    return SUCCESS_CODE;
}

static int opcomp_rbo(OpComp *oc, pmode_t param_modes) {
    opc_t arg; 

    DEBUG_OP("RBO");
    OPCOMP_READ_PARAMS(oc, param_modes, &arg, 1);

    DEBUG_OP(" rbo=%d (prev: %d)\n", oc->rbo + (int) arg, oc->rbo);
    oc->rbo += (int) arg;

    // increment instruction pointer
    ++(oc->ip);

    return SUCCESS_CODE;
}
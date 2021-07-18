#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>


#define DEBUG_BOT_ON
// #define DEBUG_OP_ON


#ifdef DEBUG_ON
    #define DEBUG(...) fprintf(stdout, "D :: %s :: ", __FUNCTION__); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n")
#else
    #define DEBUG(...)
#endif

#ifdef DEBUG_OP_ON
    #define DEBUG_OP(...) printf(__VA_ARGS__)
#else
    #define DEBUG_OP(...)
#endif

#ifdef DEBUG_BOT_ON
    #define DEBUG_BOT(...) printf(__VA_ARGS__)
#else
    #define DEBUG_BOT(...)
#endif

#define ERROR(...) fprintf(stderr, "\033[1;31mE :: %s :: ", __FUNCTION__); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n\033[0m")

#endif
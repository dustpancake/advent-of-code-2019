#ifndef INPUTIO_H
#define INPUTIO_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>

#include <glib.h>

static char *f_next_line(FILE *fp) {
    char ch;
    char *line = NULL;
    fpos_t pos;
    int line_length;
    
    // get current file position
    if (fgetpos(fp, &pos) != 0) {
        fprintf(stderr, "Error in determining file position.");
        return line;
    }

    // how many characters needed
    for (line_length = 0; !feof(fp); ++line_length) {
        ch = fgetc(fp);
        if (ch == '\n') {
            break;
        }
    }

    // reseek position
    fsetpos(fp, &pos);
    
    // read into pointer
    line = (char *) malloc((line_length + 1) * sizeof(char));
    for (int i = 0; i < line_length; ++i) {
        line[i] = fgetc(fp);
    }

    // if last character was a newline, consume it
    if (ch == '\n') {
        fgetc(fp); 
    }

    // null terminate
    line[line_length] = '\0';

    return line;
}

static int f_line_count(FILE *fp) {
    int lines = 0;

    // seek start
    fseek(fp, 0, SEEK_SET);

    while (!feof(fp)) {
        if (fgetc(fp) == '\n') {
            ++lines;
        }
    }
    // check if last line is empty
    fseek(fp, -2, SEEK_CUR);
    if (fgetc(fp) == '\n') {
        --lines;
    }

    // seek start again
    fseek(fp, 0, SEEK_SET);

    return lines;
}

static GPtrArray *f_read_lines(const char *filename)
{     
    int line_count = 1; // start at 1 since always at least one line
    FILE   *fp;
    GPtrArray *ptr_arr = NULL;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Failed to open input file - %s\n", filename);
        return ptr_arr;
    }

    line_count = f_line_count(fp);

    // allocate memory
    ptr_arr = g_ptr_array_new();
    g_ptr_array_set_size(ptr_arr, line_count);

    // read in file
    for (int i = 0; i < line_count; ++i) {
        ptr_arr->pdata[i] = f_next_line(fp);    
    }

    fclose(fp);

    return ptr_arr;
}

static long *f_read_bytecode_program(const char *filename, int *len) {
    int comma_count;
    int digits;
    int index;
    char buf[64];
    char ch = 'A';      // initialize with random
    long *arr = NULL;
    FILE *fp;

    // init len to 0 incase of errors
    *len = 0;

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Failed to open input file - %s\n", filename);
        return arr;
    }

    // count number of commas, assuming no trailing comma and only one line of data
    // also count the number of digits in each number
    comma_count = 0;
    while (!feof(fp) && ch != '\n') {
        ch = getc(fp);
        if (ch == ',') {
            ++comma_count;
        }
    }
    if (comma_count == 0) {
        fprintf(stderr, "No comma seperated values read.");
        return arr;
    }

    fseek(fp, 0, SEEK_SET);

    // allocate, need one more than number of commas for trailing value
    arr = (long *) malloc(++comma_count * sizeof(long));

    // read in the data
    index = 0;
    digits = 0;
    do {
        ch = getc(fp);
        if (isdigit(ch) || ch == '-') {
            buf[digits++] = ch;
        } else if (ch == ',') {
            buf[digits] = '\0';
            arr[index++] = strtol(buf, NULL, 10);
            digits = 0;
        }
    } while (!feof(fp) && ch != '\n');

    // trailing value after last comma
    buf[digits] = '\0';
    arr[index] = strtol(buf, NULL, 10);

    // return length information
    *len = comma_count;

    return arr;
}

#endif
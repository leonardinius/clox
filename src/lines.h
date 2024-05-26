#ifndef clox_line_h
#define clox_line_h

#include "common.h"

typedef uint32_t line_pos_t;

typedef struct
{
    int capacity;
    int count;
    // line[i] is a run encoded line information
    // [byte]   [3 bytes]
    // count    line
    // the sum of all counts is the current offset.
    line_pos_t *lines;
    // Running total offset
    int offset;
} Lines;

void initLines(Lines *lines);
void freeLines(Lines *lines);
void writeLines(Lines *lines, int offset, int line);
int getLineByOffset(const Lines *lines, int offset);

#endif
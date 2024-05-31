#include "lines.h"

#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

void initLines(Lines *lines) {
    lines->count = 0;
    lines->capacity = 0;
    lines->offset = -1;
    lines->lines = NULL;
}

void appendLinePos(Lines *lines, line_pos_t linePos) {
    if (lines->capacity < lines->count + 1) {
        int oldCapacity = lines->capacity;
        lines->capacity = GROW_CAPACITY(oldCapacity);
        lines->lines =
            GROW_ARRAY(line_pos_t, lines->lines, oldCapacity, lines->capacity);
    }

    lines->lines[lines->count++] = linePos;
}

void writeLines(Lines *lines, int offset, int line) {
    if (offset <= lines->offset) {
        printf("Error: try to add offset %d before last observed offset %d.\n",
               offset, lines->offset);
        exit(1);
    }

    // empty lines array
    if (lines->count == 0) {
        // init
        lines->offset = offset;
        line_pos_t linePos = line & 0x00ffffff;
        appendLinePos(lines, linePos);
        return;
    }

    line_pos_t lastPos = lines->lines[lines->count - 1];
    int lastCount = lastPos >> 24;
    int lastLine = lastPos & 0x00ffffff;
    if (lastLine == line) {
        // increment the count by 1
        lines->lines[lines->count - 1] = ((lastCount + 1) << 24) | line;
        lines->offset = offset;
        return;
    }

    if (line < lastLine) {
        printf("Error: try to add line %d before last observed line %d.\n",
               line, lastLine);
        exit(1);
    }

    lastCount = offset - lines->offset;
    while (lastCount > 0) {
        int count = lastCount > 255 ? 255 : lastCount;
        lastPos = ((count) << 24) | line;
        appendLinePos(lines, lastPos);
        lastCount -= count;
    }
    lines->offset = offset;
}

void freeLines(Lines *lines) {
    FREE_ARRAY(line_pos_t, lines->lines, lines->capacity);
    initLines(lines);
}

int getLineByOffset(const Lines *lines, int offset) {
    if (offset > lines->offset) {
        return -1;
    }

    int runningOffset = 0;
    for (int i = 0; i < lines->count; i++) {
        line_pos_t linePos = lines->lines[i];
        int count = linePos >> 24;
        int line = linePos & 0x00ffffff;
        if (runningOffset + count >= offset) {
            return line;
        }

        runningOffset += count;
    }

    return -1;
}
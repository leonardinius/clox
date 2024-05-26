#include <stdlib.h>

#include "lines.h"
#include "memory.h"

void initLines(Lines *lines)
{
    lines->count = 0;
    lines->capacity = 0;
    lines->offset = -1;
    lines->lines = NULL;
}

void appendLinePos(Lines *lines, line_pos_t line_pos)
{
    if (lines->capacity < lines->count + 1)
    {
        int oldCapacity = lines->capacity;
        lines->capacity = GROW_CAPACITY(oldCapacity);
        lines->lines = GROW_ARRAY(line_pos_t, lines->lines, oldCapacity, lines->capacity);
    }

    lines->lines[lines->count++] = line_pos;
}

void writeLines(Lines *lines, int offset, int line)
{
    if (offset <= lines->offset)
    {
        printf("Error: try to add offset %d before last observed offset %d.\n", offset, lines->offset);
        exit(1);
    }

    // empty lines array
    if (lines->count == 0)
    {
        // init
        lines->offset = offset;
        line_pos_t line_pos = line & 0x00ffffff;
        appendLinePos(lines, line_pos);
        return;
    }

    line_pos_t last_pos = lines->lines[lines->count - 1];
    int last_count = last_pos >> 24;
    int last_line = last_pos & 0x00ffffff;
    if (last_line == line)
    {
        // increment the count by 1
        lines->lines[lines->count - 1] = ((last_count + 1) << 24) | line;
        lines->offset = offset;
        return;
    }

    if (line < last_line)
    {
        printf("Error: try to add line %d before last observed line %d.\n", line, last_line);
        exit(1);
    }

    last_count = offset - lines->offset;
    while (last_count > 0)
    {
        int count = last_count > 255 ? 255 : last_count;
        last_pos = ((count) << 24) | line;
        appendLinePos(lines, last_pos);
        last_count -= count;
    }
    lines->offset = offset;
}

void freeLines(Lines *lines)
{
    FREE_ARRAY(line_pos_t, lines->lines, lines->capacity);
    initLines(lines);
}

int getLineByOffset(const Lines *lines, int offset)
{
    if (offset > lines->offset)
    {
        return -1;
    }

    int running_offset = 0;
    for (int i = 0; i < lines->count; i++)
    {
        line_pos_t line_pos = lines->lines[i];
        int count = line_pos >> 24;
        int line = line_pos & 0x00ffffff;
        if (running_offset + count >= offset)
        {
            return line;
        }

        running_offset += count;
    }

    return -1;
}
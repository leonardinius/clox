#include <stdio.h>

#include "debug.h"

void dissassembleChunk(const Chunk *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count; offset++)
    {
        offset = dissassembleInstruction(chunk, offset);
    }
}

int dissassembleInstruction(const Chunk *chunk, int offset)
{
    printf("%04d ", offset);

    uint8_t instruction = chunk->code[offset];
    switch (instruction)
    {
    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
        break;

    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
        break;
    }
}

int simpleInstruction(const char *name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}

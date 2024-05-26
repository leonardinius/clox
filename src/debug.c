#include <stdio.h>

#include "debug.h"
#include "value.h"

void dissassembleChunk(const Chunk *chunk, const char *name)
{
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;)
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
    case OP_CONSTANT:
        return constantInstruction("OP_CONSTANT", chunk, offset);
        break;

    case OP_RETURN:
        return simpleInstruction("OP_RETURN", offset);
        break;

    default:
        printf("Unknown opcode %d\n", instruction);
        return offset + 1;
        break;
    }
}

int constantInstruction(const char *name, const Chunk *chunk, int offset)
{
    uint8_t constant = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

int simpleInstruction(const char *name, int offset)
{
    printf("%s\n", name);
    return offset + 1;
}
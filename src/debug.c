#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

#include "value.h"

int constantInstruction(const char *name, const Chunk *chunk, int offset) {
    uint8_t constantOffset = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constantOffset);
    printValue(chunk->constants.values[constantOffset]);
    printf("'\n");
    return offset + 2;
}

int simpleInstruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int byteInstruction(const char *name, const Chunk *chunk, int offset) {
    uint8_t slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2;
}

void disassembleChunk(const Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);

    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

int disassembleInstruction(const Chunk *chunk, int offset) {
    printf("%04d ", offset);
    int line = getLine(chunk, offset);
    int prevLine = getLine(chunk, offset - 1);
    if (offset > 0 && line == prevLine) {
        printf("   | ");
    } else {
        printf("%4d ", line);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", chunk, offset);
            break;

        case OP_NIL:
            return simpleInstruction("OP_NIL", offset);
            break;

        case OP_TRUE:
            return simpleInstruction("OP_TRUE", offset);
            break;

        case OP_FALSE:
            return simpleInstruction("OP_FALSE", offset);
            break;

        case OP_POP:
            return simpleInstruction("OP_POP", offset);
            break;

        case OP_GET_LOCAL:
            return byteInstruction("OP_GET_LOCAL", chunk, offset);

        case OP_SET_LOCAL:
            return byteInstruction("OP_SET_LOCAL", chunk, offset);

        case OP_GET_GLOBAL:
            return constantInstruction("OP_GET_GLOBAL", chunk, offset);

        case OP_DEFINE_GLOBAL:
            return constantInstruction("OP_DEFINE_GLOBAL", chunk, offset);

        case OP_SET_GLOBAL:
            return constantInstruction("OP_SET_GLOBAL", chunk, offset);

        case OP_EQUAL:
            return simpleInstruction("OP_EQUAL", offset);
            break;

        case OP_BANG_EQUAL:
            return simpleInstruction("OP_BANG_EQUAL", offset);
            break;

        case OP_LESS:
            return simpleInstruction("OP_LESS", offset);
            break;

        case OP_LESS_EQUAL:
            return simpleInstruction("OP_LESS_EQUAL", offset);
            break;

        case OP_GREATER:
            return simpleInstruction("OP_GREATER", offset);
            break;

        case OP_GREATER_EQUAL:
            return simpleInstruction("OP_GREATER_EQUAL", offset);
            break;

        case OP_ADD:
            return simpleInstruction("OP_ADD", offset);
            break;

        case OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", offset);
            break;

        case OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", offset);
            break;

        case OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", offset);
            break;

        case OP_NOT:
            return simpleInstruction("OP_NOT", offset);
            break;

        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
            break;

        case OP_PRINT:
            return simpleInstruction("OP_PRINT", offset);
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

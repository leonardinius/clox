#include "debug.h"

#include <stdio.h>

#include "value.h"

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

        case OP_CONSTANT_LONG:
            return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
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

        case OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
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

int constantInstruction(const char *name, const Chunk *chunk, int offset) {
    uint8_t constantOffset = chunk->code[offset + 1];
    printf("%-16s %4d '", name, constantOffset);
    printValue(chunk->constants.values[constantOffset]);
    printf("'\n");
    return offset + 2;
}

int constantLongInstruction(const char *name, const Chunk *chunk, int offset) {
    uint8_t constantOffsetByte3 = chunk->code[offset + 1];
    uint8_t constantOffsetByte2 = chunk->code[offset + 2];
    uint8_t constantOffsetByte1 = chunk->code[offset + 3];
    uint32_t constantOffset = (constantOffsetByte3 << 16) |
                              (constantOffsetByte2 << 8) |
                              (constantOffsetByte1);
    printf("%-16s %4d '", name, constantOffset);
    printValue(chunk->constants.values[constantOffset]);
    printf("'\n");
    return offset + 4;
}

int simpleInstruction(const char *name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

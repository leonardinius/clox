#include "chunk.h"

#include <stdlib.h>

#include "memory.h"

void initChunk(Chunk *chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    initLines(&chunk->lines);
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code =
            GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    writeLines(&chunk->lines, chunk->count, line);
    chunk->count++;
}

void writeConstant(Chunk *chunk, Value value, int line) {
    writeValueArray(&chunk->constants, value);
    int constantOffset = chunk->constants.count - 1;
    if (constantOffset < 256) {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, (uint8_t)(constantOffset), line);
        return;
    }
    writeChunk(chunk, OP_CONSTANT_LONG, line);
    uint8_t byte1 = constantOffset & 0xff;
    uint8_t byte2 = (constantOffset >> 8) & 0xff;
    uint8_t byte3 = (constantOffset >> 16) & 0xff;
    writeChunk(chunk, byte3, line);
    writeChunk(chunk, byte2, line);
    writeChunk(chunk, byte1, line);
}

void freeChunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    freeLines(&chunk->lines);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

int getLine(const Chunk *chunk, int offset) {
    return getLineByOffset(&chunk->lines, offset);
}
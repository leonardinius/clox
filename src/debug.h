#ifndef clox_debug_h
#define clox_debug_h

#include "chunk.h"

void dissassembleChunk(const Chunk *chunk, const char *name);

int dissassembleInstruction(const Chunk *chunk, int offset);

#endif
#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char *argv[])
{
	Chunk chunk;
	initChunk(&chunk);

	for (int i = 0; i < 300; i++)
	{
		writeConstant(&chunk, i + .2, i);
	}
	writeChunk(&chunk, OP_RETURN, 524);
	writeChunk(&chunk, OP_RETURN, 524);
	writeChunk(&chunk, OP_RETURN, 525);
	writeChunk(&chunk, OP_RETURN, 526);
	writeChunk(&chunk, OP_RETURN, 526);
	writeChunk(&chunk, OP_RETURN, 526);

	dissassembleChunk(&chunk, "test chunk");
	freeChunk(&chunk);
	return 0;
}
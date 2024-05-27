#include "common.h"
#include "chunk.h"
#include "vm.h"
#include "debug.h"

int main(int argc, char *argv[])
{
	initVM();
	Chunk chunk;
	initChunk(&chunk);

	writeConstant(&chunk, 1.3, 1);
	writeChunk(&chunk, OP_NEGATE, 2);
	writeConstant(&chunk, 4, 2);
	writeChunk(&chunk, OP_MULTIPLY, 3);
	writeChunk(&chunk, OP_RETURN, 3);

	printf("== interpret chunk==\n");
	interpret(&chunk);
	freeVM();
	freeChunk(&chunk);
	return 0;
}
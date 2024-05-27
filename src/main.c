#include "common.h"
#include "chunk.h"
#include "vm.h"
#include "debug.h"

int main(int argc, char *argv[])
{
	initVM();
	Chunk chunk;
	initChunk(&chunk);

	writeConstant(&chunk, 1.2, 1);
	writeChunk(&chunk, OP_RETURN, 2);
	writeChunk(&chunk, OP_RETURN, 3);

	dissassembleChunk(&chunk, "test chunk");
	printf("\n\n");
	printf("== interpret chunk==\n");
	interpret(&chunk);
	freeVM();
	freeChunk(&chunk);
	return 0;
}
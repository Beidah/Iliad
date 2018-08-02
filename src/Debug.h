#pragma once

#include "Chunk.h"

// TODO: write assert macros.
#if _DEBUG
#define ASSERT(x)
#else
#define ASSERT(x)
#endif

class Debugger {
public:

	static void DisassembleChunk(Chunk* chunk, const char* name);
	static int DisassembleInstruction(Chunk* chunk, int i);
	static int ConstantInstruction(const char* name, Chunk* chunk, int offset);
	static int SimpleInstruction(const char* name, int offset);
};
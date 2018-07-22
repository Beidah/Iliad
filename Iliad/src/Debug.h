#pragma once

#include "Chunk.h"

class Debugger {
public:

	static void DisassembleChunk(Chunk* chunk, const char* name);
	static int DisassembleInstruction(Chunk* chunk, int i);
	static int ConstantInstruction(const char* name, Chunk* chunk, int offset);
	static int SimpleInstruction(const char* name, int offset);
};

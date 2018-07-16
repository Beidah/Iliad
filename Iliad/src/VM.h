#pragma once

#include "Chunk.h"
#include "Value.h"

#define STACK_MAX 256

enum class InterpretResults {
	OK, 
	CompileError,
	RuntimeError
};

class VM {
private:
	Chunk* m_Chunk;
	Byte* m_IP;
	Value m_Stack[STACK_MAX];
	Value* m_StackTop;

public:
	VM();

	InterpretResults Interpret(Chunk* chunk);

private:
	InterpretResults run();

	void push(Value value);
	Value pop();
	Value peek(int distance) const { return m_StackTop[-1 - distance]; }
};
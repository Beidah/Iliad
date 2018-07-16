#pragma once

#include "stdafx.h"
#include "Value.h"

enum class OpCode : Byte {
	Constant,
	Add, Subtract,
	Multiply, Divide,
	Negate,
	Return
};

/**
 * A Chunk of bytecode.
 */
class Chunk {
private:
	std::vector<Byte> m_Code;
	std::vector<int> m_Lines;

public:
	std::vector<Value> m_Constants;

	Chunk() = default;

	void writeByte(Byte byte);
	void writeByte(OpCode opCode) { writeByte(static_cast<Byte>(opCode)); }
	int addConstant(Value constant);

	Byte* getStart() { return m_Code.data(); };
};
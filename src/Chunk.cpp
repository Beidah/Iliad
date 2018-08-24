#include "stdafx.h"
#include "Chunk.h"

void Chunk::writeByte(byte byte, int line) { 
	m_Code.push_back(byte);
	m_Lines.push_back(line);

}

int Chunk::addConstant(Value constant) { 
	m_Constants.push_back(constant);
	return static_cast<int>(m_Constants.size()) - 1;
}

OpCode valueTypeToOpCode(ValueType type) {
	switch (type) {
	case ValueType::Int32: return OpCode::IntLiteral;
	case ValueType::Float: return OpCode::FloatLiteral;
	case ValueType::Char: return OpCode::CharLiteral;
	case ValueType::String: return OpCode::StringLiteral;
	default: return OpCode::Return;
	}
}

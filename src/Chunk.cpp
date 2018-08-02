#include "stdafx.h"
#include "Chunk.h"

void Chunk::writeByte(Byte byte, int line) { 
	m_Code.push_back(byte);
	m_Lines.push_back(line);

}

int Chunk::addConstant(Value constant) { 
	m_Constants.push_back(constant);
	return static_cast<int>(m_Constants.size()) - 1;
}
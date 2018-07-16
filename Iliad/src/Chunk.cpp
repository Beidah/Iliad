#include "stdafx.h"
#include "Chunk.h"

void Chunk::writeByte(Byte byte) { 
	m_Code.push_back(byte);

}

int Chunk::addConstant(Value constant) { 
	m_Constants.push_back(constant);
	return m_Constants.size() - 1;
}
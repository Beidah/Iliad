#include "stdafx.h"
#include "Debug.h"

#include <iomanip>

#include "Chunk.h"

void Debugger::DisassembleChunk(Chunk* chunk, const char* name) {
	std::cout << "= " << name << " =" << std::endl;

	for (int i = 0; i < chunk->m_Code.size();) {
		i = DisassembleInstruction(chunk, i);
	}
}

int Debugger::DisassembleInstruction(Chunk* chunk, int offset) {
	std::cout << std::setw(4) << offset << " ";

	if (offset > 0 && chunk->m_Lines[offset - 1] == chunk->m_Lines[offset]) {
		std::cout << "   | ";
	} else {
		std::cout << std::setw(4) << chunk->m_Lines[offset] << " ";
	}

	byte instruction = chunk->m_Code[offset];
	switch (static_cast<OpCode>(instruction)) {
	case OpCode::IntLiteral: return ConstantInstruction("OP Int", chunk, offset);
	case OpCode::FloatLiteral: return ConstantInstruction("OP Float", chunk, offset);
	case OpCode::CharLiteral: return ConstantInstruction("OP Char", chunk, offset);
	case OpCode::StringLiteral: return ConstantInstruction("Op String", chunk, offset);
	case OpCode::TrueLiteral: return SimpleInstruction("OP True", offset);
	case OpCode::FalseLiteral: return SimpleInstruction("OP False", offset);
	case OpCode::VarDeclar: return DeclarationInstruction("Var declaration", chunk, offset);
	case OpCode::VarAssign: return ConstantInstruction("Assign var", chunk, offset);
	case OpCode::VarDeclarAndAssign: return ConstantInstruction("Var declaration", chunk, offset);
	case OpCode::Var: return ConstantInstruction("Var", chunk, offset);
	case OpCode::Equal: return SimpleInstruction("OP Equal", offset);
	case OpCode::NotEqual: return SimpleInstruction("OP Not Equal", offset);
	case OpCode::Greater: return SimpleInstruction("OP Greater", offset);
	case OpCode::GreaterEqual: return SimpleInstruction("OP Greater Equal", offset);
	case OpCode::Less: return SimpleInstruction("OP Less", offset);
	case OpCode::LessEqual: return SimpleInstruction("OP Less Equal", offset);
	case OpCode::Add: return SimpleInstruction("OP Add", offset);
	case OpCode::Subtract: return SimpleInstruction("OP Subtract", offset);
	case OpCode::Multiply: return SimpleInstruction("OP Multiply", offset);
	case OpCode::Divide: return SimpleInstruction("OP Divide", offset);
	case OpCode::Concatenate: return SimpleInstruction("Op Concatenate", offset);
	case OpCode::Not: return SimpleInstruction("OP Not", offset);
	case OpCode::Negate: return SimpleInstruction("OP Negate", offset);
	case OpCode::Null: return SimpleInstruction("OP Null", offset);
	case OpCode::Return: return SimpleInstruction("OP Return", offset);
	default:
		std::cout << "Unkown opcode " << instruction << std::endl;
		return offset + 1;
	}
}

int Debugger::DeclarationInstruction(const std::string& name, Chunk* chunk, int offset) {
	ValueType type = static_cast<ValueType>(chunk->m_Code[offset + 1]);
	std::cout << name << " type:  " << ValueTypeToString(type) << std::endl;
	std::cout << std::setw(8) << " ";
	ConstantInstruction("Var name: ", chunk, offset + 1);
	return offset + 3;
}

int Debugger::ConstantInstruction(const std::string& name, Chunk* chunk, int offset) {
	byte constant = chunk->m_Code[offset + 1];
	std::cout << std::left << std::setw(16) << name << std::right << (int)constant;
	std::cout << " | " << chunk->m_Constants[constant].ToString() << " |" << std::endl;
	return offset + 2;
}

int Debugger::SimpleInstruction(const std::string& name, int offset) {
	std::cout << name << std::endl;
	return offset + 1;
}

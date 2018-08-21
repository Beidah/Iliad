//! \file Debug.h
//! \brief Functions and classes useful for debugging.

#pragma once

#include "Chunk.h"

//! \todo write assert macros.
#if _DEBUG
#define ASSERT(x)
#else
#define ASSERT(x)
#endif


//! A class used to disassemble a chunk and print it's values in a human readable code.
class Debugger {
public:
	//! Reads the chunk and translate it's bytecode into human readable instructions.
	/*!
	  \param chunk Chunk to be disassembled.
	  \param name Name of the chunk.
	*/
	static void DisassembleChunk(Chunk* chunk, const char* name);

	//! Disassembles instruction and prints out human readable opcodes
	/*!
	  \param chunk Chunk containing the instruction.
	  \param i Index of bytearray for the instruction.
	  \return Index of bytearray the next instruction is in (skips over operands).
	*/
	static int DisassembleInstruction(Chunk* chunk, int i);

	//! Disassembles number literals and prints out the type and value of the literal.
	/*!
	  \param name The name of the Op Code (e.g. "OP Int").
	  \param chunk Chunk containing the instruction.
	  \param offset Index of bytearray for the instruction.
	  \return Index of bytearray the next instruction is in (skips over operands).
	*/
	static int ConstantInstruction(const char* name, Chunk* chunk, int offset);

	//! Disassembles simpler instructions (without operands) into a human readable format.
	/*!
	  \param name The name of the Op Code (e.g. "OP Add").
	  \param offset Index of bytearray for the instruction.
	  \return Index of bytearray the next instruction is in.
	*/
	static int SimpleInstruction(const char* name, int offset);
};
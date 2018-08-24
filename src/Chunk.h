//! \file Chunk.h
//! \brief Details the bytecode that the program compiles to.
#pragma once

#include "stdafx.h"
#include "Value.h"


//! Representation of opcodes to write into a Chunk.
/*!
  The opcodes that source files are compiled to, which are then read by a VM.
  Certain opcodes also take one or more operand, which are bytes that detail 
  the values to be operated on.
*/
enum class OpCode : byte {
	//!@{
	//! Literal values
	IntLiteral, FloatLiteral,
	TrueLiteral, FalseLiteral,
	CharLiteral, StringLiteral,
	//!@}

	//!@{
	//! Binary Operators
	Equal, NotEqual,
	Greater, GreaterEqual,
	Less, LessEqual,
	Add, Subtract,
	Multiply, Divide,
	//!@}

	//!@{
	//! Unary Operators
	Not,
	Negate,
	//!@}

	//! Return
	Return
};

OpCode valueTypeToOpCode(ValueType type);

//! A chunk of byte code.
/*!
  A Chunk is a class with a resizable array of unsigned 8-bit values that represent bytecode. A
  Compiler writes each opcode, with the needed operands, to the chunk in sequential order, then
  passes it to a VM to be interpreted.
*/
class Chunk {
private:
	std::vector<byte> m_Code; //!< Byte representation of code to be interpreted.
	std::vector<int> m_Lines; //!< Line at which each byte of code occured on.

#ifdef _DEBUG
	friend class Debugger;
#endif // _DEBUG

public:
	std::vector<Value> m_Constants; //!< An array of constants.

	//! Write byte of code to m_Code.
	/*!
	  \param byte Either the opcode or operand to write to m_Code.
	  \param line Line on which the code occurred on.
	  */
	void writeByte(byte byte, int line);
	//! \copybrief writeByte(Byte byte, int line)
	/*!
	 \param opCode OpCode representation of byte to write to m_Code.
	 \param line Line on which the code occurred on.
	*/
	void writeByte(OpCode opCode, int line) { writeByte(static_cast<byte>(opCode), line); }
	//! Add a constant Value to m_Constants.
	/*!
	  \param constant Value to add to m_Constants
	  \return Index of m_Constants where value was added.
	*/
	int addConstant(Value constant);

	/*!
	  \return Pointer to the beginning of the bytecode
	*/
	const byte* getStart() { return m_Code.data(); };
};
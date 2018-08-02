//! \file VM.h
//! \brief Details the VM that interprets the bytecode and executes the program.
#pragma once

#include <memory>

#include "Chunk.h"
#include "Value.h"

//! The maximum number of Value the VM can hold in its statck.
#define STACK_MAX 256

//! Results to be given by VM as it interprets and runs the code.
/*!
  An enum representaion of the results of compiling and interpreting the bytecode. If an
  error is found, different values are given depending on the stage the error occured.
*/
enum class InterpretResults {
	OK, //!< No error was found.
	CompileError, //!< Error occurred during compiling.
	RuntimeError //!< Error occurred during interpreting.
};

//! A small virtual machine to run generated bytecode.
/*!
  The VM takes the source code and hands it off to the Compiler to be converted to bytecode.
  When the Compiler is finished, the VM then reads through the bytecode Chunk and executes
  the commands instructed by the program source code.
*/
class VM {
private:
	std::shared_ptr<Chunk> m_Chunk; //!< Current Chunk of bytecode being interpreted. Shared with Compiler to generate bytecode.
	const Byte* m_IP; //!< Instruction Pointer. Pointer to current instruction the VM is running from the Chunk.
	std::vector<Value> m_Stack; //!< A statck of Values.
	size_t m_StackTop = 0; //!< A pointer to where in m_Stack the next Value will be written to.

public:
	//! Default compiler
	VM();

	//! Interprets source code and runs it.
	/*!
	  Takes in a string of source code, creates a Compiler, and let's it compile the source coude into a
	  bytecode Chunk, then runs the bytecode and processes it.
	  \param source A string of source code to be compiled and interpreted.
	  \return InterpretResults representation of compiling and interpreting the source code.
		- InterpretResults::OK if there were no errors.
		- InterpretResults::CompileError if there was an error in the compiling phase.
		- InterpretResults::RuntimeError if there was an error in the running phase.
	*/
	InterpretResults Interpret(const std::string* source);

private:
	//! Runs the bytecode from m_Chunk.
	/*!
	  \return
		- InterpretResults::OK if there were no errors.
		- InterpretResults::RuntimeError if an error was encountered.
	*/
	InterpretResults run();

	//! Pushes a Value onto the top of m_Stack
	/*!
	  \param value Value to be written to m_Stack
	*/
	void push(Value& value);

	//! Pops the Value off the top of m_Stack and sets it to be overwritten on the next push.
	/*!
	  \return Value from the top of the stack.
	*/
	Value pop();

	//! Checks a Value in the stack without removing it.
	/*!
	  \param distance Distance from the top of the stack. 0 is the current stack top.
	  \return The value located at (m_Stacktop - distance) index in m_Stack.
	*/
	Value peek(int distance) const { return m_Stack[m_StackTop - 1 - distance]; }

	//! Returns the byte at m_IP and increments the pointer.
	Byte ReadByte() { return *m_IP++; }
};
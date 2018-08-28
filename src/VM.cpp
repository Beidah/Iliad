#include "stdafx.h"
#include "VM.h"

#include <cstdarg>

#include "Compiler.h"
#include "Debug.h"

VM::VM() {
	m_Stack.reserve(STACK_MAX);
}

InterpretResults VM::Interpret(const std::string& source) {
	Compiler compiler;
	m_Chunk = std::make_shared<Chunk>();

	if (!compiler.Compile(source, m_Chunk)) {
		return InterpretResults::CompileError;
	}

	m_IP = m_Chunk->getStart();

	return run();
}

InterpretResults VM::run() {
#define READ_CONSTANT() (m_Chunk->m_Constants[ReadByte()])
#define BINARY_OP(op) do { \
		Value b = pop(); \
		Value a = pop(); \
		Value val(a op b); \
		push(val); \
	} while(false)

	while (true) {
#ifdef DEBUG_TRACE_EXCEPTION
		std::cout << "        ";
		for (size_t slot = 0; slot < m_StackTop; slot++) {
			std::cout << "[ " << m_Stack[slot].ToString() << " ]";
		}
		std::cout << std::endl;
		Debugger::DisassembleInstruction(m_Chunk.get(), static_cast<int>(m_IP - m_Chunk->getStart()));
#endif
		OpCode instruction;
		switch (instruction = static_cast<OpCode>(ReadByte())) {
		case OpCode::IntLiteral:
		case OpCode::FloatLiteral:
		case OpCode::StringLiteral:
		case OpCode::CharLiteral: push(READ_CONSTANT()); break;
		case OpCode::TrueLiteral:
		{
			Value val(true);
			push(val); break;
		}
		case OpCode::FalseLiteral: 
		{ 
			Value val(false);
			push(val); break;
		}
		case OpCode::Equal: BINARY_OP(== ); break;
		case OpCode::NotEqual: BINARY_OP(!= ); break;
		case OpCode::Greater: BINARY_OP(> ); break;
		case OpCode::GreaterEqual: BINARY_OP(>= ); break;
		case OpCode::Less: BINARY_OP(< ); break;
		case OpCode::LessEqual: BINARY_OP(<= ); break;
		case OpCode::Add: BINARY_OP(+); break;
		case OpCode::Subtract: BINARY_OP(-); break;
		case OpCode::Multiply: BINARY_OP(*); break;
		case OpCode::Divide: BINARY_OP(/ ); break;
		case OpCode::Concatenate:
		{
			auto b = pop();
			auto a = pop();
			std::string newString = a.AsValue<std::string>() + b.AsValue<std::string>();
			Value value(FWD(newString));
			push(value);
			break;
		}
		case OpCode::Not: 
		{
			bool not = !pop();
			Value val(FWD(not));
			push(val); break; 
		}
		case OpCode::Negate:
		{
			auto val = -pop();
			push(val);
			break;
		}
		case OpCode::Return:
			pop();
			return InterpretResults::OK;
		}
	}

#undef BINARY_OP
#undef READ_CONSTANT
}

void VM::push(Value& value) {
	m_Stack.push_back(value);
	m_StackTop++;
}

Value VM::pop() {
	m_StackTop--;
	Value value = m_Stack.back();
	m_Stack.pop_back();
	return value;
}

void VM::runtimeError(const char * format, ...) {
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fputs("\n", stderr);

	size_t instruction = m_IP - m_Chunk->getStart();
	std::cerr << "[line " << instruction << "] in script\n";

	// reset stack
	m_StackTop = 0;
}

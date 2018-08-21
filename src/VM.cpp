#include "stdafx.h"
#include "VM.h"

#include <cstdarg>

#include "Compiler.h"
#include "Debug.h"

VM::VM() {
	m_Stack.reserve(STACK_MAX);
}

InterpretResults VM::Interpret(const std::string* source) {
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
		if (!a.IsNumber() || !b.IsNumber()) { \
			runtimeError("Operands must be numbers."); \
			return InterpretResults::RuntimeError;\
		} \
		push(Value(a op b)); \
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
		{
			Value constant = READ_CONSTANT();
			push(constant);
			break;
		}
		case OpCode::FloatLiteral:
		{
			push(READ_CONSTANT());
			break;
		}
		case OpCode::TrueLiteral:
			push(Value(true));
			break;
		case OpCode::FalseLiteral:
			push(Value(false));
			break;
		case OpCode::Equal: BINARY_OP(== ); break;
		case OpCode::NotEqual: BINARY_OP(!= ); break;
		case OpCode::Greater: BINARY_OP(> ); break;
		case OpCode::GreaterEqual: BINARY_OP(>= ); break;
		case OpCode::Less: BINARY_OP(< ); break;
		case OpCode::LessEqual: BINARY_OP(<= ); break;
		case OpCode::Add:
			BINARY_OP(+); break;
		case OpCode::Subtract:
			BINARY_OP(-); break;
		case OpCode::Multiply:
			BINARY_OP(*); break;
		case OpCode::Divide:
			BINARY_OP(/ ); break;
		case OpCode::Not:
			push(Value(!static_cast<bool>(pop()))); break;
		case OpCode::Negate:
			if (peek(0).IsNumber()) {
				runtimeError("Operand must take a number.");
				return InterpretResults::RuntimeError;
			}
			push(-pop());
			break;
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

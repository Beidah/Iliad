#include "stdafx.h"
#include "VM.h"

#include "Compiler.h"
#include "Debug.h"

VM::VM() {
	m_StackTop = m_Stack;
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
#define READ_BYTE() (*m_IP++)
#define READ_CONSTANT() (m_Chunk->m_Constants[READ_BYTE()])
#define BINARY_OP(op) do { \
		Value a = pop(); \
		Value b = pop(); \
		push ((a op b)); \
	} while(false)

	while (true) {
#ifdef DEBUG_TRACE_EXCEPTION
		std::cout << "        ";
		for (Value *slot = m_Stack; slot < m_StackTop; slot++) {
			std::cout << "[ " << slot->toString() << " ]";
		}
		std::cout << std::endl;
		Debugger::DisassembleInstruction(m_Chunk.get(), (m_IP - m_Chunk->getStart()));
#endif
		OpCode instruction;
		switch (instruction = static_cast<OpCode>(READ_BYTE())) {
		case OpCode::Constant:
			Value constant = READ_CONSTANT();
			push(constant);
			break;
		case OpCode::Add:
			BINARY_OP(+); break;
		case OpCode::Subtract:
			BINARY_OP(-); break;
		case OpCode::Multiply:
			BINARY_OP(*); break;
		case OpCode::Divide:
			BINARY_OP(/ ); break;
		case OpCode::Negate:
			if (!IS_NUMBER(peek(0))) {
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
#undef READ_BYTE
}

void VM::push(Value value) {
	*m_StackTop = value;
	m_StackTop++;
}

Value VM::pop() {
	m_StackTop--;
	return *m_StackTop;
}

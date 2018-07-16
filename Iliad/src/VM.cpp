#include "stdafx.h"
#include "VM.h"

VM::VM() {
	m_StackTop = m_Stack;
}

InterpretResults VM::Interpret(Chunk * chunk) {
	m_Chunk = chunk;
	m_IP = chunk->getStart();
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

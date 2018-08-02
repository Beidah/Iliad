#include "stdafx.h"
#include "Compiler.h"

#ifdef DEBUG_PRINT_CODE
#include "Debug.h"
#endif // DEBUG_PRINT_CODE


#define NO_FUNC &Compiler::emptyFunction

const std::array<Compiler::ParseRule, static_cast<size_t>(TokenType::EoF) + 1> Compiler::m_Rules = {
	ParseRule(&Compiler::grouping, NO_FUNC, ParsePrecedence::Call),
	ParseRule(),       // TOKEN_RIGHT_PAREN
	ParseRule(),       // TOKEN_LEFT_BRACE
	ParseRule(),       // TOKEN_RIGHT_BRACE
	ParseRule(),      // TOKEN_COMMA
	ParseRule(),       // TOKEN_DOT
	ParseRule(&Compiler::unary, &Compiler::binary, ParsePrecedence::Term),       // TOKEN_MINUS
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Term),       // TOKEN_PLUS
	ParseRule(),       // TOKEN_SEMICOLON
	ParseRule(NO_FUNC, &Compiler::binary,  ParsePrecedence::Factor),     // TOKEN_SLASH
	ParseRule(NO_FUNC, &Compiler::binary,  ParsePrecedence::Factor),     // TOKEN_STAR
	ParseRule(),       // TOKEN_BANG
	ParseRule(ParsePrecedence::Equality),   // TOKEN_BANG_EQUAL
	ParseRule(),       // TOKEN_EQUAL
	ParseRule(ParsePrecedence::Equality),   // TOKEN_EQUAL_EQUAL
	ParseRule(ParsePrecedence::Comparison), // TOKEN_GREATER
	ParseRule(ParsePrecedence::Comparison), // TOKEN_GREATER_EQUAL
	ParseRule(ParsePrecedence::Comparison), // TOKEN_LESS
	ParseRule(ParsePrecedence::Comparison), // TOKEN_LESS_EQUAL
	ParseRule(ParsePrecedence::And),        // TOKEN_AND
	ParseRule(ParsePrecedence::Or),         // TOKEN_OR
	ParseRule(ParsePrecedence::And),			// Token_BitAnd
	ParseRule(ParsePrecedence::Or),
	ParseRule(),       // TOKEN_IDENTIFIER
	ParseRule(),      // TOKEN_STRING
	ParseRule(&Compiler::integer, NO_FUNC), // Token Integer
	ParseRule(&Compiler::_float, NO_FUNC),       // Token Float
	ParseRule(),       // TOKEN_CLASS
	ParseRule(),       // TOKEN_ELSE
	ParseRule(),       // TOKEN_FALSE
	ParseRule(),       // TOKEN_FOR
	ParseRule(),       // TOKEN_IF
	ParseRule(),       // TOKEN_RETURN
	ParseRule(),       // TOKEN_SUPER
	ParseRule(),       // TOKEN_THIS
	ParseRule(),       // TOKEN_TRUE
	ParseRule(),       // TOKEN_VAR
	ParseRule(),       // TOKEN_WHILE
	ParseRule(),       // TOKEN_ERROR
	ParseRule(),       // TOKEN_EOF
};

bool Compiler::Compile(const std::string* source, std::shared_ptr<Chunk> chunk) {
	m_Scanner = std::make_unique<Scanner>(source);
	m_CompilingChunk = chunk;

	advance();
	expression();
	consume(TokenType::EoF, "Expeceded end of file.");
	endCompiler();
	return !m_Parser.hadError;
}

void Compiler::advance() {
	m_Parser.previousToken = m_Parser.currentToken;

	while (true) {
		m_Parser.currentToken = m_Scanner->ScanToken();

		if (m_Parser.currentToken.type != TokenType::Error) break;

		errorAtCurrent(m_Parser.currentToken.lexeme.c_str());
	}
}

void Compiler::consume(TokenType expectedToken, const char * message) {
	if (m_Parser.currentToken.type == expectedToken) {
		advance();
		return;
	}

	errorAtCurrent(message);
}

void Compiler::unary() {
	// Get the operator
	TokenType op = m_Parser.previousToken.type;

	// Compile the operand
	parsePrecedence(ParsePrecedence::Unary);

	switch (op) {
	case TokenType::Minus: emitByte(OpCode::Negate); break;
	default:
		// Unreachable
		return;
	}
}

void Compiler::binary() {
	// Get the operator
	TokenType op = m_Parser.previousToken.type;

	// Compile the right operand
	const ParseRule* rule = getRule(op);
	parsePrecedence(static_cast<ParsePrecedence>(static_cast<int>(rule->precedence) + 1));

	switch (op) {
	case TokenType::Minus: emitByte(OpCode::Subtract); break;
	case TokenType::Plus: emitByte(OpCode::Add); break;
	case TokenType::Star: emitByte(OpCode::Multiply); break;
	case TokenType::Slash: emitByte(OpCode::Divide); break;
	default:
		return; // Unreachable
	}
}

void Compiler::grouping() {
	expression();
	consume(TokenType::RightParen, "Expected ')' after expression.");
}

void Compiler::expression() {
	parsePrecedence(ParsePrecedence::Assignment);
}

void Compiler::integer() {
	int32_t value = std::stoi(m_Parser.previousToken.lexeme, nullptr);
	emitConstant(Value(value));
}

void Compiler::_float() {
	float value = std::stof(m_Parser.previousToken.lexeme);
	emitConstant(Value(value));
}

void Compiler::parsePrecedence(ParsePrecedence precedence) {
	advance();
	ParseFun prefix = getRule(m_Parser.previousToken.type)->prefixRule;
	if (prefix == NO_FUNC || prefix == nullptr) {
		error("Expected expression.");
		return;
	}

	(*this.*prefix)();

	while (precedence < getRule(m_Parser.currentToken.type)->precedence) {
		advance();
		ParseFun infix = getRule(m_Parser.previousToken.type)->infixRule;
		(*this.*infix)();
	}

}



uint8_t Compiler::makeConstant(Value value) {
	int constant = m_CompilingChunk->addConstant(value);

	if (constant > UINT8_MAX) {
		error("Too many constants in one chunk.");
		return 0;
	}

	return static_cast<uint8_t>(constant);
}

void Compiler::endCompiler() { 
	emitReturn();
#ifdef DEBUG_PRINT_CODE
	if (!m_Parser.hadError) {
		Debugger::DisassembleChunk(m_CompilingChunk.get(), "Code");
	}
#endif // DEBUG_PRINT_CODE

}

void Compiler::errorAt(Token token, const char* message) {
	if (m_Parser.panicMode) return;
	m_Parser.panicMode = true;

	std::cerr << "[line " << token.line << "] Error";

	if (token.type == TokenType::EoF) {
		std::cerr << " at end";
	} else if (token.type == TokenType::Error) {
		// Nothing
	} else {
		std::cerr << " at " << token.lexeme;
	}

	std::cerr << ": " << message << std::endl;
	m_Parser.hadError = true;
}

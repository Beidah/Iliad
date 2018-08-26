#include "stdafx.h"
#include "Compiler.h"

#ifdef DEBUG_PRINT_CODE
#include "Debug.h"
#endif // DEBUG_PRINT_CODE


#define NO_FUNC &Compiler::emptyFunction

const std::array<Compiler::ParseRule, Token::NUMBER_OF_TOKENS> Compiler::m_Rules = {
	ParseRule(&Compiler::grouping, NO_FUNC, ParsePrecedence::Call),			// Token LeftParen
	ParseRule(),															// Token RightParen
	ParseRule(),															// TOKEN_LEFT_BRACE
	ParseRule(),															// TOKEN_RIGHT_BRACE
	ParseRule(),															// TOKEN_COMMA
	ParseRule(),															// TOKEN_DOT
	ParseRule(&Compiler::unary, &Compiler::binary, ParsePrecedence::Term),	// TOKEN_MINUS
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Term),			// TOKEN_PLUS
	ParseRule(),															// TOKEN_SEMICOLON
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Factor),			// TOKEN_SLASH
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Factor),			// TOKEN_STAR
	ParseRule(&Compiler::unary, NO_FUNC),									// TOKEN_BANG
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Equality),		// TOKEN_BANG_EQUAL
	ParseRule(),															// TOKEN_EQUAL
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Equality),		// TOKEN_EQUAL_EQUAL
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		// TOKEN_GREATER
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		// TOKEN_GREATER_EQUAL
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		// TOKEN_LESS
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		// TOKEN_LESS_EQUAL
	ParseRule(&Compiler::binary, NO_FUNC, ParsePrecedence::And),			// Token And
	ParseRule(&Compiler::binary, NO_FUNC, ParsePrecedence::Or),				// Token Or
	ParseRule(),															// Token Identifier
	ParseRule(&Compiler::character, NO_FUNC),								// Token Character
	ParseRule(&Compiler::string, NO_FUNC),									// Token String
	ParseRule(&Compiler::integer, NO_FUNC),									// Token Integer
	ParseRule(&Compiler::_float, NO_FUNC),									// Token Float
	ParseRule(),															// Token Class
	ParseRule(),															// TOKEN_ELSE
	ParseRule(&Compiler::literals, NO_FUNC),								// TOKEN_FALSE
	ParseRule(),															// TOKEN_FOR
	ParseRule(),															// TOKEN_IF
	ParseRule(),															// TOKEN_RETURN
	ParseRule(),															// TOKEN_SUPER
	ParseRule(),															// TOKEN_THIS
	ParseRule(&Compiler::literals, NO_FUNC),								// TOKEN_TRUE
	ParseRule(),															// TOKEN_VAR
	ParseRule(),															// TOKEN_WHILE
	ParseRule(),															// TOKEN_ERROR
	ParseRule(),															// TOKEN_EOF
};

bool Compiler::Compile(const std::string& source, std::shared_ptr<Chunk> chunk) {
	m_Scanner = std::make_unique<Scanner>(source);
	m_CompilingChunk = chunk;

	advance();
	expression();
	consume(TokenType::EoF, "Expeceded end of file.");
	endCompiler();
	return !m_Parser.hadError;
}

void Compiler::advance() {

	while (true) {
		Token token = m_Scanner->ScanToken();
		m_Parser.tokens.push_back(token);

		if (CurrentToken().type != TokenType::Error) break;

		errorAtCurrent(CurrentToken().lexeme.c_str());
	}
}

void Compiler::consume(TokenType expectedToken, const char * message) {
	if (CurrentToken().type == expectedToken) {
		advance();
		return;
	}

	errorAtCurrent(message);
}

void Compiler::unary() {
	// Get the operator
	TokenType op = PreviousToken().type;

	// Compile the operand
	parsePrecedence(ParsePrecedence::Unary);

	switch (op) {
	case TokenType::Bang: emitByte(OpCode::Not); break;
	case TokenType::Minus: emitByte(OpCode::Negate); break;
	default:
		return;	// Unreachable
	}
}

void Compiler::binary() {
	// Get the operator
	TokenType op = PreviousToken().type;
	Token previous = TokenAt(2);
	Token next = CurrentToken();

	// Compile the right operand
	const ParseRule* rule = getRule(op);
	parsePrecedence(static_cast<ParsePrecedence>(static_cast<int>(rule->precedence) + 1));

	switch (op) {
	case TokenType::EqualEqual: emitByte(OpCode::Equal); break;
	case TokenType::BangEqual: emitByte(OpCode::NotEqual); break;
	case TokenType::Greater: emitByte(OpCode::Greater); break;
	case TokenType::GreaterEqual: emitByte(OpCode::GreaterEqual); break;
	case TokenType::Less: emitByte(OpCode::Less); break;
	case TokenType::LessEqual: emitByte(OpCode::LessEqual); break;
	case TokenType::Minus: emitByte(OpCode::Subtract); break;
	case TokenType::Plus:
		if (previous.type == TokenType::String) 
		{ 
			emitByte(OpCode::Concatenate); 
		}
		else {
			emitByte(OpCode::Add);
		}
		break;
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

void Compiler::character() {
	std::string lexeme = PreviousToken().lexeme;
	char c = lexeme[1];
	if (lexeme[1] == '\'') c = 0;
	else if (lexeme[1] == '\\') {
		switch (lexeme[2]) {
		case '\\': c = '\\'; break;
		case 'n': c = '\n'; break;
		case 'r': c = '\r'; break;
		case '0': c = '\0'; break;
		case '\'': c = '\''; break;
		case '\"': c = '\"'; break;
		default:
			break;
		}
	}

	Value value(FWD(c));
	emitByte(OpCode::CharLiteral);
	emitByte(makeConstant(value));
}

void Compiler::integer() {
	int32_t value = std::stoi(PreviousToken().lexeme, nullptr);
	emitConstant(Value(FWD(value)));
}

void Compiler::_float() {
	float value = std::stof(PreviousToken().lexeme);
	emitConstant(Value(FWD(value)));
}

void Compiler::string() {
	auto lexeme = PreviousToken().lexeme;
	std::string valueString = lexeme.substr(1, lexeme.length() - 2);
	Value value(FWD(valueString));
	emitConstant(value);
}

void Compiler::literals() {
	switch (PreviousToken().type) {
	case TokenType::True: emitByte(OpCode::TrueLiteral); break;
	case TokenType::False: emitByte(OpCode::FalseLiteral); break;
	default:
		return; // unreachable.
	}
}

void Compiler::parsePrecedence(ParsePrecedence precedence) {
	advance();
	ParseFun prefix = getRule(PreviousToken().type)->prefixRule;
	if (prefix == NO_FUNC || prefix == nullptr) {
		error("Expected expression.");
		return;
	}

	(*this.*prefix)();

	while (precedence < getRule(CurrentToken().type)->precedence) {
		advance();
		ParseFun infix = getRule(PreviousToken().type)->infixRule;
		(*this.*infix)();
	}

}



uint8_t Compiler::makeConstant(const Value& value) {
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

void Compiler::errorAt(Token token, const std::string& message) {
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

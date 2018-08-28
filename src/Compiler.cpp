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

	m_Parser.StartParser(*m_Scanner);
	expression();
	consume(TokenType::EoF, "Expeceded end of file.");
	endCompiler();
	return !m_Parser.hadError;
}

void Compiler::advance() {

	if (CurrentToken().type == TokenType::EoF) return;

	while (true) {

		m_Parser.currentToken++;

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
	Token opToken = PreviousToken();
	TokenType op = opToken.type;

	// Compile the operand
	parsePrecedence(ParsePrecedence::Unary);

	ValueType rhs = m_Parser.currentExpression;

	switch (op) {
	case TokenType::Bang: emitByte(OpCode::Not); break;
	case TokenType::Minus:
		if (!IsNumber(rhs)) {
			errorAt(opToken, "Incorrect value. Expected number, found " + ValueTypeToString(rhs) + ".");
		}
		emitByte(OpCode::Negate); break;
	default:
		return;	// Unreachable
	}
}

void Compiler::binary() {
	// Get the operator
	Token opToken = PreviousToken();
	TokenType op = opToken.type;
	ValueType lhs = m_Parser.currentExpression;

	// Compile the right operand
	const ParseRule* rule = getRule(op);
	parsePrecedence(static_cast<ParsePrecedence>(static_cast<int>(rule->precedence) + 1));

	ValueType rhs = m_Parser.currentExpression;

	switch (op) {
	case TokenType::EqualEqual: 
		emitByte(OpCode::Equal); 
		m_Parser.currentExpression = ValueType::Bool;
		break;
	case TokenType::BangEqual: 
		emitByte(OpCode::NotEqual);
		m_Parser.currentExpression = ValueType::Bool;
		break;
	case TokenType::Greater:
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected numbers, found" + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
			break;
		}
		emitByte(OpCode::Greater);
		m_Parser.currentExpression = ValueType::Bool;
		break;
	case TokenType::GreaterEqual:
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected numbers, found" + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
			break;
		}
		emitByte(OpCode::GreaterEqual);
		m_Parser.currentExpression = ValueType::Bool;
		break;
	case TokenType::Less: 
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected two numbers, found " + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
			break;
		}
		emitByte(OpCode::Less);
		m_Parser.currentExpression = ValueType::Bool;
		break;
	case TokenType::LessEqual: 
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected two numbers, found " + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
			break;
		}
		emitByte(OpCode::LessEqual);
		m_Parser.currentExpression = ValueType::Bool;
		break;
	case TokenType::Minus:
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected two numbers, found " + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) +".");
			break;
		}
		emitByte(OpCode::Subtract);
		m_Parser.currentExpression = smallestTypeNeeded(lhs, rhs);
		break;
	case TokenType::Plus: 
		if (IsString(lhs)) {
			if (!IsString(rhs)) {
				errorAt(opToken, "Cannot convert " + ValueTypeToString(rhs) + " explicity to string.");
				break;
			}
			emitByte(OpCode::Concatenate);
			m_Parser.currentExpression = ValueType::String;
			break;
		}
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected two numbers, found " + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
			break;
		}
		emitByte(OpCode::Add);
		m_Parser.currentExpression = smallestTypeNeeded(lhs, rhs);
		break;
	case TokenType::Star:
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected two numbers, found " + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
			break;
		}
		emitByte(OpCode::Multiply);
		m_Parser.currentExpression = smallestTypeNeeded(lhs, rhs);
		break;
	case TokenType::Slash: 
		if (!IsNumber(lhs) || !IsNumber(rhs)) {
			errorAt(opToken, "Invalid operands. Expected two numbers, found " + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
			break;
		}
		emitByte(OpCode::Divide);
		m_Parser.currentExpression = smallestTypeNeeded(lhs, rhs);
		break;
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
	emitConstant(value);
	m_Parser.currentExpression = ValueType::Char;
}

void Compiler::integer() {
	int32_t numValue = std::stoi(PreviousToken().lexeme, nullptr);
	Value value(FWD(numValue));
	emitConstant(value);
	m_Parser.currentExpression = ValueType::Int32;
}

void Compiler::_float() {
	float numValue = std::stof(PreviousToken().lexeme);
	Value value(FWD(numValue));
	emitConstant(value);
	m_Parser.currentExpression = ValueType::Float;
}

void Compiler::string() {
	auto lexeme = PreviousToken().lexeme;
	std::string valueString = lexeme.substr(1, lexeme.length() - 2);
	Value value(FWD(valueString));
	emitConstant(value);
	m_Parser.currentExpression = ValueType::String;
}

void Compiler::literals() {
	switch (PreviousToken().type) {
	case TokenType::True: 
		emitByte(OpCode::TrueLiteral);
		m_Parser.currentExpression = ValueType::Bool;
		break;
	case TokenType::False: 
		emitByte(OpCode::FalseLiteral); 
		m_Parser.currentExpression = ValueType::Bool;
		break;
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

void Compiler::Parser::StartParser(Scanner& scanner) {
	tokensToBeParsed = scanner.ScanAllTokens();
	currentToken = tokensToBeParsed.begin();
}

#include "stdafx.h"
#include "Compiler.h"

#ifdef DEBUG_PRINT_CODE
#include "Debug.h"
#endif // DEBUG_PRINT_CODE


#define NO_FUNC &Compiler::emptyFunction

const std::array<Compiler::ParseRule, Token::NUMBER_OF_TOKENS> Compiler::m_Rules = {
	ParseRule(&Compiler::grouping, NO_FUNC, ParsePrecedence::Call),			//!< Token LeftParen
	ParseRule(),															//!< Token RightParen
	ParseRule(),															//!< Token LeftBrace
	ParseRule(),															//!< Token RightBrace
	ParseRule(),															//!< Token Comma
	ParseRule(),															//!< Token Dot
	ParseRule(&Compiler::unary, &Compiler::binary, ParsePrecedence::Term),	//!< Token Minus
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Term),			//!< Token Plus
	ParseRule(),															//!< Token Semicolon
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Factor),			//!< Token Slash
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Factor),			//!< Token Star
	ParseRule(&Compiler::unary, NO_FUNC),									//!< Token Bang
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Equality),		//!< Token BangEquals
	ParseRule(&Compiler::binary, NO_FUNC),									//!< Token Equals
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Equality),		//!< Token EqualEquals
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		//!< Token Greater
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		//!< Token GreaterEqual
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		//!< Token Less
	ParseRule(NO_FUNC, &Compiler::binary, ParsePrecedence::Comparison),		//!< Token LessEqual
	ParseRule(&Compiler::binary, NO_FUNC, ParsePrecedence::And),			//!< Token And
	ParseRule(&Compiler::binary, NO_FUNC, ParsePrecedence::Or),				//!< Token Or
	ParseRule(&Compiler::variable, NO_FUNC),								//!< Token Identifier
	ParseRule(&Compiler::character, NO_FUNC),								//!< Token Character
	ParseRule(&Compiler::string, NO_FUNC),									//!< Token String
	ParseRule(&Compiler::integer, NO_FUNC),									//!< Token Integer
	ParseRule(&Compiler::_float, NO_FUNC),									//!< Token Float
	ParseRule(),															//!< Token DecInt8
	ParseRule(),															//!< Token DecInt16
	ParseRule(),															//!< Token DecInt32
	ParseRule(),															//!< Token DecInt64
	ParseRule(),															//!< Token DecFloat
	ParseRule(),															//!< Token DecDouble
	ParseRule(),															//!< Token DecChar
	ParseRule(),															//!< Token DecString
	ParseRule(),															//!< Token DecBool
	ParseRule(),															//!< Token Var
	ParseRule(),															//!< Token Class
	ParseRule(),															//!< Token Else
	ParseRule(&Compiler::literals, NO_FUNC),								//!< Token False
	ParseRule(),															//!< Token For
	ParseRule(),															//!< Token If
	ParseRule(),															//!< Token Return
	ParseRule(),															//!< Token Super
	ParseRule(),															//!< Token This
	ParseRule(&Compiler::literals, NO_FUNC),								//!< Token True
	ParseRule(),															//!< Token While
	ParseRule(),															//!< Token Error
	ParseRule(),															//!< Token EoF
};

bool Compiler::Compile(const std::string& source, std::shared_ptr<Chunk> chunk) {
	m_Scanner = std::make_unique<Scanner>(source);
	m_CompilingChunk = chunk;

	m_Parser.StartParser(*m_Scanner);
	do {
		declaration();
	} while (CurrentToken().type != TokenType::EoF);
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

bool Compiler::match(TokenType expectedToken) {
	if ((m_Parser.currentToken)->type == expectedToken) {
		advance();
		return true;
	}
	return false;
}

void Compiler::consume(TokenType expectedToken, const std::string& message) {
	if (CurrentToken().type == expectedToken) {
		advance();
		return;
	}

	errorAtCurrent(message);
}

void Compiler::unary(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;
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

void Compiler::binary(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;
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
			errorAt(opToken, "Invalid operands. Expected two numbers, found " + ValueTypeToString(lhs) + ", and " + ValueTypeToString(rhs) + ".");
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

void Compiler::grouping(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;
	expression();
	consume(TokenType::RightParen, "Expected ')' after expression.");
}

void Compiler::expression() {
	parsePrecedence(ParsePrecedence::Assignment);
}

void Compiler::character(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;
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

void Compiler::integer(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;

	int32_t numValue = std::stoi(PreviousToken().lexeme, nullptr);
	Value value(FWD(numValue));
	emitConstant(value);
	m_Parser.currentExpression = ValueType::Int32;
}

void Compiler::_float(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;

	float numValue = std::stof(PreviousToken().lexeme);
	Value value(FWD(numValue));
	emitConstant(value);
	m_Parser.currentExpression = ValueType::Float;
}

void Compiler::string(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;

	auto lexeme = PreviousToken().lexeme;
	std::string valueString = lexeme.substr(1, lexeme.length() - 2);
	Value value(FWD(valueString));
	emitConstant(value);
	m_Parser.currentExpression = ValueType::String;
}

void Compiler::literals(bool canAssign) {
	if (canAssign) canAssign = canAssign && true;

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

void Compiler::declaration() {
	if (CurrentToken().type >= TokenType::DecInt8 && CurrentToken().type <= TokenType::Var) {
		varDeclaration();
	} else {
		statement();
	}
}

void Compiler::varDeclaration() {
	Token declType = CurrentToken();
	advance();
	consume(TokenType::Identifier, "Expected identifier.");
	Token name = PreviousToken();

	switch (declType.type) {
	case TokenType::DecInt8: m_Parser.currentExpression = ValueType::Int8; break;
	case TokenType::DecInt16: m_Parser.currentExpression = ValueType::Int16; break;
	case TokenType::DecInt32: m_Parser.currentExpression = ValueType::Int32; break;
	case TokenType::DecInt64: m_Parser.currentExpression = ValueType::Int64; break;
	case TokenType::DecFloat: m_Parser.currentExpression = ValueType::Float; break;
	case TokenType::DecDouble: m_Parser.currentExpression = ValueType::Double; break;
	case TokenType::DecChar: m_Parser.currentExpression = ValueType::Char; break;
	case TokenType::DecString: m_Parser.currentExpression = ValueType::String; break;
	case TokenType::DecBool: m_Parser.currentExpression = ValueType::Bool; break;
	case TokenType::Var: m_Parser.currentExpression = ValueType::Null; break;
	}

	ValueType varType = m_Parser.currentExpression;

	if (m_Variables.find(name.lexeme) != m_Variables.end()) {
		error("Variable " + name.lexeme + " already declared.");
		return;
	}

	m_Variables.insert({ name.lexeme, m_Parser.currentExpression });

	if (match(TokenType::Equal)) {
		AssignVar(varType, name);
		emitByte(OpCode::VarDeclarAndAssign);
	} else {
		if (varType == ValueType::Null) {
			errorAtCurrent("Variables declared with 'var' keyword must be assigned at declaration.");
		}
		emitByte(OpCode::VarDeclar);
		emitByte(static_cast<uint8_t>(varType));
	}

	consume(TokenType::Semicolon, "Expected ';'.");

	m_Parser.currentExpression = ValueType::Invalid;

	Value id(FWD(name.lexeme));
	emitByte(makeConstant(id));
}

void Compiler::AssignVar(ValueType varType, Token &name) {

	parsePrecedence(ParsePrecedence::Assignment);

	ValueType expType = m_Parser.currentExpression;

	// If declaration was made with the "Var" keyword.
	if (varType == ValueType::Null) varType = expType;

	if (varType != expType) {
		switch (varType) {
		case ValueType::Int8:
		case ValueType::Int16:
		case ValueType::Int32:
		case ValueType::Int64:
		case ValueType::Float:
		case ValueType::Double:
			if (!IsNumber(expType)) {
				errorAt(name, "Cannot assign " + ValueTypeToString(expType) + " to " + ValueTypeToString(varType) + ".");
			}
			if (varType < expType) {
				warningAt(name, "Possible loss of data in conversion of " + ValueTypeToString(expType) + " to " + ValueTypeToString(varType) + ".");
			}
			break;
		case ValueType::Char:
			errorAt(name, "Cannot assign " + ValueTypeToString(expType) + " to char.");
			break;
		case ValueType::String:
			errorAt(name, "Cannot assign " + ValueTypeToString(expType) + " to string.");
			break;
		case ValueType::Bool:
			errorAt(name, "Cannot assign " + ValueTypeToString(expType) + " to bool.");
			break;
		}
	}
}

void Compiler::variable(bool canAssign) {
	Token nameTok = PreviousToken();
	auto name = nameTok.lexeme;
	if (m_Variables.find(name) == m_Variables.end()) {
		errorAtCurrent("Unknown variable '" + name + "'.");
	} else {
		m_Parser.currentExpression = (*m_Variables.find(name)).second;
	}

	if (canAssign && match(TokenType::Equal)) {
		AssignVar(m_Parser.currentExpression, nameTok);
		emitByte(OpCode::VarAssign);
	} else {
		emitByte(OpCode::Var);
	}

	Value stringVal(FWD(name));
	emitByte(makeConstant(stringVal));
}

void Compiler::statement() {
	expression();
	consume(TokenType::Semicolon, "Expected ';'.");
	m_Parser.currentExpression = ValueType::Invalid;
}

void Compiler::parsePrecedence(ParsePrecedence precedence) {
	advance();
	ParseFun prefix = getRule(PreviousToken().type)->prefixRule;
	if (prefix == NO_FUNC || prefix == nullptr) {
		error("Expected expression.");
		return;
	}

	bool canAssign = precedence <= ParsePrecedence::Assignment;

	(*this.*prefix)(canAssign);

	while (precedence < getRule(CurrentToken().type)->precedence) {
		advance();
		ParseFun infix = getRule(PreviousToken().type)->infixRule;
		(*this.*infix)(canAssign);
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

void Compiler::warningAt(Token token, const std::string & message) {
	std::clog << "[line " << token.line << "] Warning";

	if (token.type == TokenType::EoF) {
		std::clog << " at end";
	} else if (token.type == TokenType::Error) {
		// Nothing
	} else {
		std::clog << " at " << token.lexeme;
	}

	std::cerr << ": " << message << std::endl;
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

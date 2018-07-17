#include "stdafx.h"
#include "Scanner.h"

Scanner::Scanner(std::string source) : m_Source(source) {
	m_Line = 1;
	m_CurrentChar = m_Source.begin();
}

Token Scanner::ScanToken() {
	skipWhitespace();

	m_CurrentToken.str(std::string());
	
	if (isAtEnd()) {
		m_CurrentToken << "\0";
		return makeToken(TokenType::EoF);
	}

	char c = advance();
	m_CurrentToken << c;

	if (isdigit(c)) return number();
	if (isalpha(c)) return identifier();

	switch (c) {
	case '(': return makeToken(TokenType::LeftParen);
	case ')': return makeToken(TokenType::RightParen);
	case '{': return makeToken(TokenType::LeftBrace);
	case '}': return makeToken(TokenType::RightBrace);
	case ';': return makeToken(TokenType::Semicolon);
	case ',': return makeToken(TokenType::Comma);
	case '.': return makeToken(TokenType::Dot);
	case '-': return makeToken(TokenType::Minus);
	case '+': return makeToken(TokenType::Plus);
	case '*': return makeToken(TokenType::Star);
	case '/': return makeToken(TokenType::Slash);
	case '!': return makeToken(match('=') ? TokenType::BangEqual : TokenType::Bang);
	case '=': return makeToken(match('=') ? TokenType::EqualEqual : TokenType::Equal);
	case '<': return makeToken(match('=') ? TokenType::LessEqual : TokenType::Less);
	case '>': return makeToken(match('=') ? TokenType::GreaterEqual : TokenType::Greater);
	case '&': return makeToken(match('&') ? TokenType::And : TokenType::BitAnd);
	case '|': return makeToken(match('|') ? TokenType::Or : TokenType::BitOr);
	case '"': return string();
	}
	
	return errorToken("Unexpected character.");
}

void Scanner::skipWhitespace() {
	while (true) {
		char c = peek();
		switch (c) {
		case ' ':
		case '\r':
		case '\t':
			advance();
			break;

		case '\n':
			m_Line++;
			advance();
			break;

		case '/':
			if (peekNext() == '/') {
				while (peek() != '\n' && !isAtEnd()) advance();
			} else {
				return;
			}
			break;
			
		default:
			return;
		}
	}
}

bool Scanner::match(char expected) {
	if (isAtEnd()) return false;

	if (expected != *m_CurrentChar) return false;

	m_CurrentChar++;
	m_CurrentToken << *m_CurrentChar;
	return true;
}

Token Scanner::string() {
	while (peek() != '"' && !isAtEnd()) {
		if (peek() == '\n') m_Line++;
		m_CurrentToken << advance();
	}

	if (isAtEnd()) return errorToken("Unterminated string.");

	m_CurrentToken << advance();
	return makeToken(TokenType::String);
}

Token Scanner::number() {
	TokenType type = TokenType::Integer;

	while (isDigit(peek())) {
		m_CurrentToken << advance();
	}

	// If theres's a decimal, consume and make the type a float
	if (peek() == '.') {
		type = TokenType::Float;
		m_CurrentToken << advance();

		// Continue eating more numbers
		while (isdigit(peek())) m_CurrentToken << advance();
	}

	return makeToken(type);
}

Token Scanner::identifier() {
	while (isAlpha(peek()) || isDigit(peek()))
		m_CurrentToken << advance();

	return makeToken(identifierType());
}

TokenType Scanner::identifierType() {
	const std::string token = m_CurrentToken.str();
	
	switch (token[0]) {
	case 'a': return checkKeyword(token, "and", TokenType::And);
	case 'c': return checkKeyword(token, "class", TokenType::Class);
	case 'e': return checkKeyword(token, "else", TokenType::Else);
	case 'f':
		if (token.length() > 1) {
			switch (token[1]) {
			case 'a': return checkKeyword(token, "false", TokenType::False);
			case 'o': return checkKeyword(token, "for", TokenType::For);
			case 'u': return checkKeyword(token, "fun", TokenType::Fun);
			}
		}
		break;
	case 'i': return checkKeyword(token, "if", TokenType::If);
	case 'o': return checkKeyword(token, "or", TokenType::Or);
	case 'r': return checkKeyword(token, "return", TokenType::Return);
	case 's': return checkKeyword(token, "super", TokenType::Super);
	case 't':
		if (token.length() > 1) {
			switch (token[1]) {
			case 'h': return checkKeyword(token, "this", TokenType::This);
			case 'r': return checkKeyword(token, "true", TokenType::True);
			}
		}
		break;
	case 'v': return checkKeyword(token, "var", TokenType::Var);
	case 'w': return checkKeyword(token, "while", TokenType::While);
	}

	return TokenType::Identifier;
}

Token Scanner::makeToken(TokenType type) {
	Token token;
	token.type = type;
	token.line = m_Line;
	token.token = m_CurrentToken.str();
	return token;
}

Token Scanner::errorToken(std::string message) {
	Token token;
	token.type = TokenType::Error;
	token.token = message;
	token.line = m_Line;
	return token;
}

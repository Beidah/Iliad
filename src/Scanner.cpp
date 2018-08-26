#include "stdafx.h"
#include "Scanner.h"

Scanner::Scanner(const std::string& source) : m_Source(source) {
	m_Line = 1;
	m_CurrentChar = m_Source.begin();
}

Token Scanner::ScanToken() {
	// Whitespace is ignored by the compiler
	skipWhitespace();

	// Reset the current token
	m_CurrentToken.str(std::string());
	
	// Check if at the end of the file.
	if (isAtEnd()) {
		m_CurrentToken << "\0";
		return makeToken(TokenType::EoF);
	}

	// Feed the next character into the scanner
	char c = advance();
	m_CurrentToken << c;

	// If the character is a number, return either a float or int
	if (isdigit(c)) return number();
	// If the character is a letter, find out if the token is a keyword or return an "Identifier"
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
	case '&': return match('&') ? makeToken(TokenType::And) : errorToken("Expected another '&' for 'and' operator.");
	case '|': return match('|') ? makeToken(TokenType::Or) : errorToken("Expected another '|' for 'or' operator.");
	case '\'': return character();
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
			// Comments begin with a "//" and end with a linebreak.
			// Comments aren't technically whitespace, but the scanner should treat them the same way.
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

Token Scanner::character() {
	// Handle escape/control character
	if (peek() == '\\') {
		m_CurrentToken << advance();
		switch (peek()) {
		case '\\':
		case 'n':
		case 'r':
		case '0':
		case '\'':
		case '\"':
			m_CurrentToken << advance(); break;
		default: return errorToken("Invalid escape character.");
		}
	} else m_CurrentToken << advance();

	if (isAtEnd() || peek() != '\'') return errorToken("Unterminated char literal.");

	m_CurrentToken << advance();
	return makeToken(TokenType::Character);
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

	// While the next character is a number, add it to the token.
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
	// Identifiers are to be made with Alphanumerical values or a "_"
	while (isAlpha(peek()) || isDigit(peek()))
		m_CurrentToken << advance();

	return makeToken(identifierType());
}

TokenType Scanner::identifierType() {
	const std::string token = m_CurrentToken.str();
	
	switch (token[0]) {
	case 'c': return checkKeyword(token, "class", TokenType::Class);
	case 'e': return checkKeyword(token, "else", TokenType::Else);
	case 'f':
		if (token.length() > 1) {
			switch (token[1]) {
			case 'a': return checkKeyword(token, "false", TokenType::False);
			case 'o': return checkKeyword(token, "for", TokenType::For);
			}
		}
		break;
	case 'i': return checkKeyword(token, "if", TokenType::If);
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

	// If none of the keywords match, return Identifier
	return TokenType::Identifier;
}

Token Scanner::makeToken(TokenType type) {
	Token token(type, m_CurrentToken.str(), m_Line);
	return token;
}

Token Scanner::errorToken(const std::string& message) {
	Token token(TokenType::Error, message, m_Line);
	return token;
}

#pragma once

#include <string>
#include <sstream>

enum class TokenType {
	// Single-character tokens.
	LeftParen, RightParen,
	LeftBrace, RightBrace,
	Comma, Dot, Minus, Plus,
	Semicolon, Slash, Star,

	// One or two character tokens.
	Bang, BangEqual,
	Equal, EqualEqual,
	Greater, GreaterEqual,
	Less, LessEqual,
	And, Or, BitAnd, BitOr,

	// Literals.
	Identifier, String, 
	Integer, Float,

	// Keywords.
	Class, Else, False,
	Fun, For, If,
	Return, Super, This,
	True, Var, While,

	Error,
	EoF
};

struct Token {
	TokenType type;
	std::string token;
	int line;

private:
	friend class Scanner;
};

class Scanner {
	std::string m_Source;
	int m_Line;
	std::string::iterator m_CurrentChar;
	std::stringstream m_CurrentToken;
public:
	Scanner(const std::string* source);
	
	Token ScanToken();

private:
	char advance() { m_CurrentChar++; return *(m_CurrentChar-1); }
	void skipWhitespace();

	bool isAtEnd() const { return m_CurrentChar == m_Source.end(); }

	// Check ahead
	bool match(char expected);
	char peek() const { return isAtEnd() ? '\0' : *m_CurrentChar; }
	char peekNext() const { return *(m_CurrentChar + 1); }

	// Literals
	Token string();
	Token number();
	Token identifier();

	// Identifiers
	TokenType identifierType();
	TokenType checkKeyword(const std::string& token, const std::string& keyword, TokenType type) { return token.compare(keyword) == 0 ? type : TokenType::Identifier; }

	// utility
	inline bool isDigit(char c) const { return c >= '0' && c <= '9'; }
	inline bool isAlpha(char c) const {
		return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_';
	}

	// tokens
	Token makeToken(TokenType type);
	Token errorToken(std::string message);
};


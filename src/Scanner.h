//! \file Scanner.h
//! \brief Details the tokens and tokenizer of the program.
#pragma once

#include <string>
#include <sstream>


//! An enumeration of all the different types of token to be scanned from the source code
enum class TokenType {
	// Single-character tokens
	LeftParen, //!< {
	RightParen, //!< }
	LeftBrace,  //!< [
	RightBrace,//!< ]
	Comma, //!< ,
	Dot, //!< \.
	Minus, //!< \-
	Plus,//!< \+
	Semicolon, //!< ;
	Slash, //!< /
	Star, //!< "*"

	// Comparison operators
	Bang, //!< !
	BangEqual, //!< !=
	Equal, //!< =
	EqualEqual, //!< =
	Greater, //!< >
	GreaterEqual, //!< >=
	Less, //!< <
	LessEqual, //!< <=
	And, //!< &&
	Or, //!< ||

	// Literals
	Identifier, //!< A set of alphanumerical characters with underscore representing a variable, class, or function name
	Character, //!< A single character enclosed with single quotemarks ' '
	String, //!< A string of characters enclosed with double quotemarks " "
	Integer, //!< A whole number
	Float, //!< A decimal number

	// Keywords
	Class, //!< Instatiates a class
	Else, //!< else
	False, //!< false
	For,  //!< for
	If, //!< if
	Return, //!< return
	Super, //!< super
	This, //!< this
	True, //!< true
	Var, //!< var
	While, //!< while

	Error, //!< A Token representing a compile-time error.
	EoF //!< The end of file.
};


//! A struct representing a token giving by the compiler

struct Token {
	TokenType type; //!< The type of token found
	std::string lexeme; //!< The string that produced the token.
	int line; //!< The line the token was found on.

	Token() : type(TokenType::EoF), lexeme(""), line(0) {}

	//! Full constructor
	Token(TokenType type, const std::string& lexeme, int line) : type(type), lexeme(lexeme), line(line) {}

	//! The total amount of token types that can be scanned.
	static const int NUMBER_OF_TOKENS = static_cast<int>(TokenType::EoF) + 1;
};

//! A class to scan through the source code and tokenize it.
/*!
  The scanner class is initilized with a string containing the code to be tokenize. After initilization,
  ScanToken can be called to recieve each token in order of occurance, one at a time. When the scanner reaches
  the end of the file, ScanToken will continue to produce an EoF token each time it's called.
*/

class Scanner {
	const std::string m_Source; //!< A string containing the source code to be tokenized.
	int m_Line; //!< The current line of the source code the Scanner is tokenizing.
	std::string::const_iterator m_CurrentChar; //!< The current character being looked at by the Scanner.
	std::stringstream m_CurrentToken; //!< A stringstream to store each character of the current token as the Scanner reconizes it.

public:
	Scanner() = delete;

	//! Initilizes the scanner with the source code provided.
	Scanner(const std::string& source);
	
	//! Retrieve the next token.
	/*!
	  Retrieve each token from the source code, one at a time, in the order they occur in.
	  \return The next Token from the sequence.
	*/
	Token ScanToken();

private:
	//! Move the Scanner forward by one character
	/*!
	  \return The next character of the sequence.
	*/
	char advance() { m_CurrentChar++; return *(m_CurrentChar-1); }
	//! Whitespace has no meaning other than as a seperator of tokens.
	void skipWhitespace();

	//! Check to see if the Scanner has reached the end of the file.
	/*!
	  \return Returns true if the Scanner has scanned the entire file, else
	*/
	bool isAtEnd() const { return m_CurrentChar == m_Source.end(); }

	//!@{ \name CheckFunctions

	//! Check if the next character is one that is expected
	/*! Checks one character ahead of the current one, and consumes it if it is the predicted char.
	    If the char is not the predicted one, the scanner goes back to it's original place.
		\param expected The char that is predicted to come next in the sequence.
		\return Returns true if the next char in the source is expected, else false
	*/
	bool match(char expected);

	//! Check the next character without consuming it
	/*!
	  \return The next char from the source code, without it being added into the current token.
	*/
	char peek() const { return isAtEnd() ? '\0' : *m_CurrentChar; }

	//! Check two characters ahead with consuming it.
	/*!
	  \return The char two ahead of the Scanner from the source code, without it being added into the current token.
	*/
	char peekNext() const { return *(m_CurrentChar + 1); }
	//!@}

	//!@{ \name Literals

	//! Matches a set of single quotes to a charcater token.
	Token character();

	//! Matches a set of quotes to a String token.
	/*!
	  \return A token of type String with the lexeme encompassing the quotemarkss, if both a begining and end quotemark are found
	  A error token is returned instead if the ending quotemarks are not found.
	*/
	Token string();

	//! Matches a set of digits to a Integer or Float Token
	/*!
	  \return A token of type Integer if the token does not have a decimal point, or a token of type Float if the token does have a decimal
	*/
	Token number();

	//! Checks if the token is a keyword, or an Identifier
	/*!
	  \return A token with type of a keyword if a keyword is matched, or a Identifier token.
	*/
	Token identifier();
	//!@}

	//!@{ \name Identifiers

	//! Checks if the current token matches with an keyword.
	/*!
	  \return The TokenType of a keyword if the current token matches one, or the Identifier TokenType
	*/
	TokenType identifierType();

	//! Checks the current token against a specific keywork.
	/*! A helper function to identifierType(), this function checks a token against a provided keyword and returns the appropriate TokenType
	  \param token Token currently being scanned.
	  \param keyword Keyword to match against the current token
	  \param type The TokenType of keyword
	  \return The TokenType provided if the token and keyword match, else TokenType::Identifier
	*/
	TokenType checkKeyword(const std::string& token, const std::string& keyword, TokenType type) const { return token.compare(keyword) == 0 ? type : TokenType::Identifier; }
	//!@}

	//!@{ \name UtilityFunctions

	//! Checks if the character is a Digit
	/*!
	  \param c Character to check.
	  \return Returns true if c is a digit, else false.
	*/
	inline bool isDigit(char c) const { return c >= '0' && c <= '9'; }

	//! Check if the character is a alphabet or underscore character
	/*!
	  \param c Character to check.
	  \return Returns true if c is a alphabet or underscore character, else false.
	*/
	inline bool isAlpha(char c) const {
		return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_';
	}
	//!@}

	//!@{ \name Tokens

	//! Creates a token of a given type
	/*! 
	  \param type The TokenType of the return Token.
	  \return A Token with the type provided, the lexeme taken from m_CurrentToken, and line number taken from m_Line.
	*/
	Token makeToken(TokenType type);

	//! Creates an error token with a supplied message.
	/*!
	 \param message Message to attach to the error.
	 \return A Token with TokenType::Error, the current line, and the message in place of a lexeme.
	*/
	Token errorToken(const std::string& message);
	//!@}
};


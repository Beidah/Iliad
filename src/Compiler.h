//! \file Compiler.h
//! \brief Details the Compiler and ParseRules of the program.
#pragma once

#include <string>
#include <memory>
#include <array>
#include <unordered_map>

#include "Chunk.h"
#include "Scanner.h"



//! Precedence of order of operations.
enum class ParsePrecedence {
	None,
	Assignment,
	Or, And,
	Equality,
	Comparison,
	Term, Factor,
	Unary, Call,
	Primary,
};

//! Converts text to byte code.
/*!
  The compiler is takes the source code and hands it off to the Scanner to be tokenized. As
  the Scanner does this, the compiler takes the tokens and makes sure they occur in a grammatical
  sequence that can be understood by the VM. It then writes a bytecode representation of the source
  code to a Chunk provided by the VM.
*/
class Compiler {
private:
	
	//! Utility struct to keep track of tokens generated by the Scanner.
	struct Parser {
		std::vector<Token> tokensToBeParsed; //!< A list of all tokens that are currently being parsed.
		std::vector<Token>::iterator currentToken; //!< An iterator to the current token.
		ValueType currentExpression; //!< The type of value of current expression. Used for type-checking.
		bool hadError = false; //!< If the compiler has found a error.
		bool panicMode = false; //!< If the compiler is currently sorting out an error.

		void StartParser(Scanner& scanner); //!< Get all tokens from the scanner.
	};


	std::unique_ptr<Scanner> m_Scanner; //!< \brief Scans the text in order to tokenize it.
	Parser m_Parser; //!< \brief Contains the current token to parse, and the previous token, as well as info on whether an error has occured.

	std::shared_ptr<Chunk> m_CompilingChunk; //!< \brief A Chunk shared with by the VM that is currently being written to.

	//! Function pointer for Parsing functions, which are used for ParseRule
	typedef void(Compiler::*ParseFun)(bool canAssign);

	//! Stores functions on how to parse different tokens, as well as the precedence in parsing.
	struct ParseRule {
		ParseFun prefixRule; //!< \brief Function to parse a prefix token
		ParseFun infixRule; //!< \brief Function to parse a infix token 
		ParsePrecedence precedence; //!< \brief Precedence of the token to be parsed

		//! Default rule with no functions or precedence.
		ParseRule() = default;
		//! Constructor to initilize every member value.
		ParseRule(ParseFun prefix, ParseFun infix, ParsePrecedence prec = ParsePrecedence::None) {
			prefixRule = prefix;
			infixRule = infix;
			precedence = prec;
		}
		//! Constructor to set precedence, but keep the parsing function null.
		ParseRule(ParsePrecedence prec) : precedence(prec) {}
	};

	static const std::array<ParseRule, Token::NUMBER_OF_TOKENS> m_Rules; //!< Rules for parsing each individual token.

	std::unordered_map<std::string, ValueType> m_Variables; //!< A hash map containing the Value and their ValueType for type-checking.

public:

	//! Default constructor
	Compiler() = default;

	//! Compiles text into bytecode.
	/*!
	  \param source A text string to be compiled.
	  \param [out] chunk A chunk that is shared by the VM to write bytecode to.
	  \return True if source successfuly compiled, false if error occurred.
	*/
	bool Compile(const std::string& source, std::shared_ptr<Chunk> chunk);


	//!@{ \name Token Getters

	//! Current Token
	const Token& CurrentToken() const { return *m_Parser.currentToken; }
	//! Previous Token
	const Token& PreviousToken() const { return *(m_Parser.currentToken - 1); }

	//! Gets token from specified location.
	/*!
	  Retrieves the token a set distance back from current one being parsed.
	  \param distance How many tokens before the current one. 0 = current token, 1 = previous token.
	  \return Token
	*/
	const Token& TokenAt(size_t distance) const { return *(m_Parser.currentToken - distance); }
	//!@}

private:

	
	//!@{ \name Advance
	//! Functions used for advancing the parser.

	//! Gets the next token from the scanner to parse.
	void advance();

	//! Check if the next Token is one that is expected
	/*! Checks one Token ahead of the current one, and consumes it if it is the predicted Token.
		If the Token is not the predicted one, the scanner goes back to it's original place.
		\param expectedToken The Token that is predicted to come next in the sequence.
		\return Returns true if the next Token in the source is expected, else false
	*/
	bool match(TokenType expectedToken);

	//! Takes an expected token and throws an error if it isn't found.
	/*!
	  \param expectedToken Token type that is expected to come next.
	  \param message Message to attach to error if expectedToken isn't found.
	*/
	void consume(TokenType expectedToken, const std::string& message);
	//!@}

	//! Add a token to the parser.
	void addToken(const Token& token) { m_Parser.tokensToBeParsed.push_back(token); }

	//!@{ \name ParseFun
	//! A set of functions to be given to ParseRule as its ParseFun members.

	//! Function for parsing unary operators.
	void unary(bool canAssign);
	//! Function for parsing binary operators.
	void binary(bool canAssign);
	//! Function for parsing parentheses.
	void grouping(bool canAssign);
	//! Function for parsing a character token.
	void character(bool canAssign);
	//! Function for parsing integers.
	void integer(bool canAssign);
	//! Function for parsing floats.
	void _float(bool canAssign);
	//! Function for parsing strings.
	void string(bool canAssign);
	//! Function for parsing literals.
	void literals(bool canAssign);
	//! Function for parsing variables.
	void variable(bool canAssign);
	//! An empty function, meant for parse rules with nothing to parse
	void emptyFunction(bool canAssign) { canAssign = canAssign && true; }
	//!@}

	//!@{ \name Statements and Expressions.

	//! Function for parsing expressions.
	void expression();
	//! Function for declarations.
	void declaration();
	//! Function for variable declaration.
	void varDeclaration();
	//! Function to assign a variable.
	void AssignVar(ValueType varType, Token &name);
	//! Function for parsing statements.
	void statement();
	//!@}


	//! Get the ParseRule associated with a given token
	/*!
	  \param token The type of token associated with the return rule.
	  \return The rule for the given token.
	*/
	const ParseRule* getRule(TokenType token) const { return &m_Rules[static_cast<int>(token)]; }

	//! Gets the next token and parses the rules for it.
	/*!
	  \param precedence The precedence of the rule being parsed. The compiler will continue to parse as long as the next token is of a lower precedence
	  and has a infix rule.
	*/
	void parsePrecedence(ParsePrecedence precedence);


	//!@{ \name Writing
	//! A set of functions to write data to the current Chunk.

	//! Writes a byte to the current Chunk
	/*!
	  \param byte A byte of data to be written to the current Chunk
	*/
	void emitByte(uint8_t byte) { m_CompilingChunk->writeByte(byte, PreviousToken().line); }

	//! \copybrief emitByte(uint8_t byte)
	/*!
	  \param opCode The opcode representation of the byte to be written to the current Chunk.
	*/
	void emitByte(OpCode opCode) { emitByte(static_cast<uint8_t>(opCode)); }

	//! Write two bytes to the current Chunk.
	/*!
	  \param byte1 Byte to be written to the Chunk first.
	  \param byte2 Byte to be wrttien to the Chunk second.
	*/
	void emitBytes(uint8_t byte1, uint8_t byte2) { emitByte(byte1); emitByte(byte2); }

	//! Writes a constant value into the current chunk
	/*!
	  \param value The value to be written to the Chunk
	*/
	void emitConstant(const Value& value) { emitByte(valueTypeToOpCode(value.Type())); emitByte(makeConstant(value)); }

	//! Writes the "Return" opcode into the Chunk.
	void emitReturn() { emitByte(static_cast<uint8_t>(OpCode::Return)); }
	
	//! Adds a value into the compiling Chunk's constants array
	/*! A utility function to append a \a Value into the current Chunk
	    and retrieve the location of said value. If the current Chunk has
		the max amount of constants already, this will trigger an error.
	  \param value The value to place into the Chunk.
	  \return The index of the value in the current Chunk's constant array.
	*/
	uint8_t makeConstant(const Value& value);

	//! Gets the compiler ready to end.
	void endCompiler();
	//!@}

	//!@{ \name Warnings
	//! Generate compiler warnings, but otherwise keeps the process going.

	//! Warning at current token.
	//! \param message Message to attach to error.
	void warningAtCurrent(const std::string& message) { warningAt(CurrentToken(), message); }

	//! Warning at previous token.
	//! \param message Message to attach to error.
	void warning(const std::string& message) { warningAt(PreviousToken(), message); }

	//! Warning at specified token.
	//! \param token Token that generated the error.
	//! \param message Message to attach to error.
	void warningAt(Token token, const std::string& message);

	//!@{ \name Errors
	//! Functions to generate errors.

	//! Error at current token.
	/*!
	  \param message Message to attach to error.
	*/
	void errorAtCurrent(const std::string& message) { errorAt(CurrentToken(), message); }

	//! Error at previous token.
	/*!
	  /param message Message to attach to error.
	  */
	void error(const std::string& message) { errorAt(PreviousToken(), message); }

	//! Error at specified token.
	/*
	 \param token Token at which the error occured.
	 \param message Message to attach to error.
	 */
	void errorAt(Token token, const std::string& message);
	//!@}

};
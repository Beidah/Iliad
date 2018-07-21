#pragma once

#include <string>
#include <memory>
#include <array>

#include "Chunk.h"
#include "Scanner.h"

struct ParseRule;

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


class Compiler {
private:
	struct Parser {
		Token currentToken;
		Token previousToken;
		bool hadError = false;
		bool panicMode = false;
	};


	std::unique_ptr<Scanner> m_Scanner;
	Parser m_Parser;

	std::shared_ptr<Chunk> m_CompilingChunk;

	typedef void(Compiler::*ParseFun)();


	struct ParseRule {
		ParseFun prefixRule;
		ParseFun infixRule;
		ParsePrecedence precedence;

		ParseRule() = default;
		ParseRule(ParseFun prefix, ParseFun infix, ParsePrecedence prec = ParsePrecedence::None) {
			prefixRule = prefix;
			infixRule = infix;
			precedence = prec;
		}
		ParseRule(ParsePrecedence prec) : precedence(prec) {}
	};

	static const std::array<ParseRule, static_cast<size_t>(TokenType::EoF) + 1> m_Rules;

public:
	bool Compile(const std::string* source, std::shared_ptr<Chunk> chunk);

private:
	// Traverse AST
	void advance();
	void consume(TokenType expectedToken, const char* message);

	// Parsing tokens
	void unary();
	void binary();
	void grouping();
	void expression();
	void integer();
	void _float();
	void emptyFunction() {}

	const ParseRule* getRule(TokenType token) const { return &m_Rules[static_cast<int>(token)]; }
	void parsePrecedence(ParsePrecedence precedence);


	// Writing
	void emitByte(uint8_t byte) { m_CompilingChunk->writeByte(byte, m_Parser.previousToken.line); }
	void emitByte(OpCode opCode) { emitByte(static_cast<uint8_t>(opCode)); }
	void emitBytes(uint8_t byte1, uint8_t byte2) { emitByte(byte1); emitByte(byte2); }
	void emitConstant(Value value) { emitBytes(static_cast<uint8_t>(OpCode::Constant), makeConstant(value)); }
	void emitReturn() { emitByte(static_cast<uint8_t>(OpCode::Return)); }
	
	uint8_t makeConstant(Value value);
	void endCompiler() { emitReturn(); }

	// Errors
	void errorAtCurrent(const char* message) { errorAt(m_Parser.currentToken, message); }
	void error(const char* message) { errorAt(m_Parser.previousToken, message); }
	void errorAt(Token token, const char* message);

};
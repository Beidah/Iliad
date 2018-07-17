#include "stdafx.h"
#include "Compiler.h"

#include "Scanner.h"

void Compile(std::string source) {
	Scanner scanner(source);
	int line = -1;

	while (true) {
		Token token = scanner.ScanToken();
		if (line != token.line) {
			std::cout << token.line << " ";
			line = token.line;
		} else {
			std::cout << "    | ";
		}

		std::cout << (int)token.type << " " << token.token << std::endl;

		if (token.type == TokenType::EoF) break;
	}
}
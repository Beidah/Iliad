//! \file Iliad.cpp 
//! \brief Defines the entry point for the console application.

#include "stdafx.h"
#include "VM.h"

#include <string>

//! Interpreter virtual machine for the repl
VM vm;

//! Creates a repl enviroment with vm interpreter.
static void repl() {
	std::string input;

	while (true) {
		std::cout << "IL> ";

		std::getline(std::cin, input);

		vm.Interpret(&input);
	}
}

//! Entry point of the program.
int main(int argc, char** argv) {
	std::cout << "Illiad programming language 0.1" << std::endl;

	if (argc == 1) {
		repl();
	} else if (argc == 2) {
		// TODO: Run file
	} else {
		std::cerr << "Usage: Illiad [path]" << std::endl;
		exit(1);
	}
	
	return 0;
}

// Iliad.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "VM.h"

int main(int argc, char** argv) {
  std::cout << "Iliad programming language 0.1\n";

  VM vm;

  Chunk *chunk = new Chunk;

  chunk->writeByte(OpCode::Constant);
  int constant = chunk->addConstant(4);
  chunk->writeByte(constant);

  chunk->writeByte(OpCode::Constant);
  constant = chunk->addConstant(4);
  chunk->writeByte(constant);
  chunk->writeByte(OpCode::Add);

  chunk->writeByte(OpCode::Return);

  vm.Interpret(chunk);

  while (1) {
    std::cout << "IL> ";
    char* input = new char[255];

    std::cin >> input;

    std::cout << "Hello: " << input << std::endl;
    delete input;
  }
  return 0;
}

cmake_minimum_required(VERSION 3.11)

project(Iliad)

add_executable(Iliad src/iliad.cpp
                         src/Chunk.cpp
                         src/Compiler.cpp
                         src/Debug.cpp
                         src/Scanner.cpp
                         src/stdafx.cpp
                         src/Value.cpp
                         src/ValueType.cpp
                         src/VM.cpp)


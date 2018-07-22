#pragma once

#include <vector>


enum class ValueType {
	Int, Float,
	Bool,
};

class Value {
public:
	ValueType m_Type;

public:
	union {
		bool Bool;
		float Float;
		int Int;
	} as;

	Value() = default;
	Value(int value) { as.Int = value; m_Type = ValueType::Int; }
	Value(float value) { as.Float = value; m_Type = ValueType::Float; }
	Value(bool value) { as.Bool = value; m_Type = ValueType::Bool; }

	// For printing
	std::string toString() const;

	// Unary operators
	Value operator-() const;

	// Binary operators
	Value operator+(Value value) const;
	Value operator-(Value value) const;
	Value operator*(Value value) const;
	Value operator/(Value value) const;

	// Logical operators
	// operator bool() const;
};

typedef std::vector<Value> ValueArray;

#define IS_NUMBER(value) ((value).m_Type == ValueType::Int || (value).m_Type == ValueType::Float)



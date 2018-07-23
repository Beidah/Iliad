//! \file Value.h
//! \brief Details the data representation used by the program.
#pragma once

#include <vector>


//! The type of data the value is meant to represent.
/*!
  Currently supported types of values are:
  - int
  - float
  - bool
  
  TODO:: Add other value types.
*/
enum class ValueType {
	Int, Float,
	Bool,
};

//! Represents data in the code.
class Value {
public:
	ValueType m_Type; //!< The type the value represents

public:
	//! Union to store the actual value data in whichever type is needed.
	union {
		bool Bool;
		float Float;
		int Int;
	} as;

	//!@{ \name Constructors
	//! Default constructor, value and type not specified.
	Value() = default;
	//! Constructs the Value as an int with given value.
	Value(int value) { as.Int = value; m_Type = ValueType::Int; }
	//! Constructs the Value as a float with given value
	Value(float value) { as.Float = value; m_Type = ValueType::Float; }
	//! Constructs the Value as a bool with given value.
	Value(bool value) { as.Bool = value; m_Type = ValueType::Bool; }
	//!@}

	//! A string representation of the value for printing.
	std::string toString() const;


	//!@{ \name Operators
	//!@{ \name Unary
	//! Multiplies the value by -1 to get its negative
	Value operator-() const;
	//!@}

	//!@{ \name Binary
	Value operator+(Value value) const;
	Value operator-(Value value) const;
	Value operator*(Value value) const;
	Value operator/(Value value) const;
	//!@}
	//!@}

	// TODO: Logical operators
	// operator bool() const;
};

//! A resizable array of Values
typedef std::vector<Value> ValueArray;

//! Checks if the value is of ValueType Float or Int
#define IS_NUMBER(value) ((value).m_Type == ValueType::Int || (value).m_Type == ValueType::Float)
//! Checks if the value is of ValueType Bool
#define IS_BOOL(value) ((value).m_Type == ValueType::Bool)


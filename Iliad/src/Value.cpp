#include "stdafx.h"
#include "Value.h"

#include <sstream>

std::string Value::toString() const {
	std::stringstream valueString;

	if (m_Type == ValueType::Bool) {
		valueString << as.Bool ? "true" : "false";
	} else if (m_Type == ValueType::Int) {
		valueString << as.Int;
	} else if (m_Type == ValueType::Float) {
		valueString << as.Float;
	} else {
		valueString << "Unknown value type.";
	}

	return valueString.str();
}

Value Value::operator-() const {
	if (m_Type == ValueType::Int) {
		return Value(-as.Int);
	} else if (m_Type == ValueType::Float) {
		return Value(-as.Float);
	}

	// Should never happen.
	return Value(false);
}

Value Value::operator+(Value value) const {
	if (m_Type == ValueType::Int && value.m_Type == ValueType::Int) {
		int c = as.Int + value.as.Int;
		return Value(c);
	}

	float a, b, c;

	if (m_Type == ValueType::Float) {
		a = as.Float;
	} else if (m_Type == ValueType::Int) {
		a = as.Int;
	}

	if (value.m_Type == ValueType::Float) {
		b = value.as.Float;
	} else if (value.m_Type == ValueType::Int) {
		b = value.as.Int;
	}

	c = a + b;

	return Value(c);
}

Value Value::operator-(Value value) const {
	if (m_Type == ValueType::Int && value.m_Type == ValueType::Int) {
		int c = as.Int - value.as.Int;
		return Value(c);
	}

	float a, b, c;

	if (m_Type == ValueType::Float) {
		a = as.Float;
	} else if (m_Type == ValueType::Int) {
		a = as.Int;
	}

	if (value.m_Type == ValueType::Float) {
		b = value.as.Float;
	} else if (value.m_Type == ValueType::Int) {
		b = value.as.Int;
	}

	c = a - b;

	return Value(c);
}

Value Value::operator*(Value value) const {
	if (m_Type == ValueType::Int && value.m_Type == ValueType::Int) {
		int c = as.Int * value.as.Int;
		return Value(c);
	}

	float a, b, c;

	if (m_Type == ValueType::Float) {
		a = as.Float;
	} else if (m_Type == ValueType::Int) {
		a = as.Int;
	}

	if (value.m_Type == ValueType::Float) {
		b = value.as.Float;
	} else if (value.m_Type == ValueType::Int) {
		b = value.as.Int;
	}

	c = a * b;

	return Value(c);
}

Value Value::operator/(Value value) const {
	if (m_Type == ValueType::Int && value.m_Type == ValueType::Int) {
		return Value(as.Int / value.as.Int);
	}

	float a, b, c;

	if (m_Type == ValueType::Float) {
		a = as.Float;
	} else if (m_Type == ValueType::Int) {
		a = as.Int;
	}

	if (value.m_Type == ValueType::Float) {
		b = value.as.Float;
	} else if (value.m_Type == ValueType::Int) {
		b = value.as.Int;
	}

	c = a / b;

	return Value(c);
}

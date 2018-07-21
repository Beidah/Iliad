#include "stdafx.h"
#include "Value.h"

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

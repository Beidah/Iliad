#include "stdafx.h"
#include "ValueType.h"

std::string ValueTypeToString(ValueType type) {
	switch (type) {
	case ValueType::Invalid: return "Invalid";
	case ValueType::Int8: return "int8";
	case ValueType::Int16: return "int16";
	case ValueType::Int32: return "int32";
	case ValueType::Int64: return "int64";
	case ValueType::Float: return "float";
	case ValueType::Double: return "double";
	case ValueType::Char: return "char";
	case ValueType::String: return "string";
	case ValueType::Bool: return "bool";
	default:
		return "Unknown value type";
	}
}

size_t ValueTypeSize(ValueType type) {
	switch (type) {
	case ValueType::Invalid: return 0;
	case ValueType::Int8: return sizeof(int8_t);
	case ValueType::Int16: return sizeof(int16_t);
	case ValueType::Int32: return sizeof(int32_t);
	case ValueType::Int64: return sizeof(int64_t);
	case ValueType::Float: return sizeof(float);
	case ValueType::Double: return sizeof(double);
	case ValueType::Char: return sizeof(char);
	case ValueType::String: return 0;
	case ValueType::Bool: return sizeof(bool);
	case ValueType::Null: return 0;
	default:
		return 0; // Unreachable.
	}
}

ValueType smallestTypeNeeded(ValueType a, ValueType b) {
	return a > b ? a : b;
}

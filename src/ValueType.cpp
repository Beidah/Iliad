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

ValueType smallestTypeNeeded(ValueType a, ValueType b) {
	return a > b ? a : b;
}

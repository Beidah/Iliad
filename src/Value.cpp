#include "stdafx.h"
#include "Value.h"

#include <sstream>
#include <iomanip>

Value::Value(const Value & value) : m_Type(value.Type()), m_Size(value.Size()) {
	m_Data = value.AsBytes();
}

Value::Value(ByteArray bytes, ValueType type) : m_Type(type), m_Size(bytes.size()) {
	m_Data = bytes;
}

const ByteArray& Value::AsBytes() const {
	return m_Data;
}

std::string Value::ToString() const {
	std::stringstream valueString;

	switch (m_Type) {
	case ValueType::Int8: valueString << static_cast<int>(AsValue<int8_t>()); break;
	case ValueType::Int16: valueString << AsValue<int16_t>(); break;
	case ValueType::Int32: valueString << AsValue<int32_t>(); break;
	case ValueType::Int64: valueString << AsValue<int64_t>(); break;
	case ValueType::Float:
	{
		float val = AsValue<float>();
		if (val == floor(val))
			valueString << std::setprecision(1) << std::fixed;
		valueString << val;
		break;
	}
	case ValueType::Double: 
	{
		double val = AsValue<double>();
		if (val == floor(val))
			valueString << std::setprecision(1) << std::fixed;
		valueString << val;
		break;
	}
	case ValueType::Char: valueString << "'" << AsValue<char>() << "'"; break;
	case ValueType::String: valueString << "\"" << AsValue<std::string>() << "\""; break;
	case ValueType::Bool: valueString << (static_cast<bool>(*this) ? "true" : "false"); break;
	default: return "Unknown value type.";
	}

	return valueString.str();
}

Value Value::operator-() const {
	if (!IsNumber()) return *this;

	switch (m_Type) {
	case ValueType::Int8: return Value(-AsValue<int8_t>());
	case ValueType::Int16: return Value(-AsValue<int16_t>());
	case ValueType::Int32: return Value(-AsValue<int32_t>());
	case ValueType::Int64: return Value(-AsValue<int64_t>());
	case ValueType::Float: return Value(-AsValue<float>());
	case ValueType::Double: return Value(-AsValue<double>());
	default: return Value();
	}
}

Value Value::operator+(Value value) const {
	if (!(IsNumber() && value.IsNumber())) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Char: return Value(AsValue<char>() + AsValue<char>());
	case ValueType::Int8: return Value(AsValue<int8_t>() + value.AsValue<int8_t>());
	case ValueType::Int16: return Value(AsValue<int16_t>() + value.AsValue<int16_t>());
	case ValueType::Int32: return Value(AsValue<int32_t>() + value.AsValue<int32_t>());
	case ValueType::Int64: return Value(AsValue<int64_t>() + value.AsValue<int64_t>());
	case ValueType::Float: return Value(AsValue<float>() + value.AsValue<float>());
	case ValueType::Double: return Value(AsValue<double>() + value.AsValue<double>());
	default: return Value();
	}
}

Value Value::operator-(Value value) const {
	if (!IsNumber() && !value.IsNumber()) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Int8: return Value(AsValue<int8_t>() - value.AsValue<int8_t>());
	case ValueType::Int16: return Value(AsValue<int16_t>() - value.AsValue<int16_t>());
	case ValueType::Int32: return Value(AsValue<int32_t>() - value.AsValue<int32_t>());
	case ValueType::Int64: return Value(AsValue<int64_t>() - value.AsValue<int64_t>());
	case ValueType::Float: return Value(AsValue<float>() - value.AsValue<float>());
	case ValueType::Double: return Value(AsValue<double>() - value.AsValue<double>());
	default: return Value();
	}
}


Value Value::operator*(Value value) const {
	if (!IsNumber() && !value.IsNumber()) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Int8: return Value(AsValue<int8_t>() * value.AsValue<int8_t>());
	case ValueType::Int16: return Value(AsValue<int16_t>() * value.AsValue<int16_t>());
	case ValueType::Int32: return Value(AsValue<int32_t>() * value.AsValue<int32_t>());
	case ValueType::Int64: return Value(AsValue<int64_t>() * value.AsValue<int64_t>());
	case ValueType::Float: return Value(AsValue<float>() * value.AsValue<float>());
	case ValueType::Double: return Value(AsValue<double>() * value.AsValue<double>());
	default: return Value();
	}
}


Value Value::operator/(Value value) const {
	if (!IsNumber() && !value.IsNumber()) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Int8: return Value(AsValue<int8_t>() / value.AsValue<int8_t>());
	case ValueType::Int16: return Value(AsValue<int16_t>() / value.AsValue<int16_t>());
	case ValueType::Int32: return Value(AsValue<int32_t>() / value.AsValue<int32_t>());
	case ValueType::Int64: return Value(AsValue<int64_t>() / value.AsValue<int64_t>());
	case ValueType::Float: return Value(AsValue<float>() / value.AsValue<float>());
	case ValueType::Double: return Value(AsValue<double>() / value.AsValue<double>());
	default: return Value();
	}
}

Value & Value::operator=(const Value & value) {
	
	if (m_Type != value.Type()) return *this;
	m_Data = value.AsBytes();
	return *this;
}

template <typename T>
bool equalTo(T valueA, Value valueB) {
	static_assert(std::is_arithmetic<T>::value, "Type must be a number.");

	switch (valueB.Type()) {
	case ValueType::Int8: return valueA == valueB.AsValue<int8_t>();
	case ValueType::Int16: return valueA == valueB.AsValue<int16_t>();
	case ValueType::Int32: return valueA == valueB.AsValue<int32_t>();
	case ValueType::Int64: return valueA == valueB.AsValue<int64_t>();
	case ValueType::Float: return valueA == valueB.AsValue<float>();
	case ValueType::Double: return valueA == valueB.AsValue<double>();
	default: return false;
	}
}

bool Value::operator==(const Value& value) const {
	// Invalid types are always false compared to others. Even other invalids.
	if (!IsValid() || !value.IsValid()) return false;

	// If they're the same type, just compare the bits directly.
	if (m_Type == value.Type()) {
		return m_Data == value.AsBytes();
	}

	switch (m_Type) {
	case ValueType::Int8: return equalTo(AsValue<int8_t>(), value);
	case ValueType::Int16: return equalTo(AsValue<int16_t>(), value);
	case ValueType::Int32: return equalTo(AsValue<int32_t>(), value);
	case ValueType::Int64: return equalTo(AsValue<int64_t>(), value);
	case ValueType::Float: return equalTo(AsValue<float>(), value);
	case ValueType::Double: return equalTo(AsValue<double>(), value);
	case ValueType::Bool: return AsValue<bool>() == value.AsValue<bool>();
	}

	return false;
}

template <typename T>
bool lessThan(T valueA, Value valueB) {
	switch (valueB.Type()) {
	case ValueType::Int8: return valueA < valueB.AsValue<int8_t>();
	case ValueType::Int16: return valueA < valueB.AsValue<int16_t>();
	case ValueType::Int32: return valueA < valueB.AsValue<int32_t>();
	case ValueType::Int64: return valueA < valueB.AsValue<int64_t>();
	case ValueType::Float: return valueA < valueB.AsValue<float>();
	case ValueType::Double: return valueA < valueB.AsValue<double>();
	default: return false;
	}
}

bool Value::operator<(const Value& value) const {
	// If the values are not numbers, they really shouldn't be compared.
	// Right now, this situation returns false, but when partial_ordering is
	// supported, it'll return partial_ordering::unordered
	if (!IsNumber() || !IsNumber()) return false;

	switch (m_Type) {
	case ValueType::Int8: return lessThan(AsValue<int8_t>(), value);
	case ValueType::Int16: return lessThan(AsValue<int16_t>(), value);
	case ValueType::Int32: return lessThan(AsValue<int32_t>(), value);
	case ValueType::Int64: return lessThan(AsValue<int64_t>(), value);
	case ValueType::Float: return lessThan(AsValue<float>(), value);
	case ValueType::Double: return lessThan(AsValue<double>(), value);
	}

	return false;
}

Value::operator bool() const {
	switch (m_Type) {
	case ValueType::Invalid: return false;
	case ValueType::Bool: return m_Data[0] == 1 ? true : false;
	default: return true;
	}
}



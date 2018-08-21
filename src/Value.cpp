#include "stdafx.h"
#include "Value.h"

#include <sstream>
#include <iomanip>

Value::Value(const Value & value) : m_Type(value.Type()), m_Size(value.Size()) {
	m_Data = value.AsBytes();
}

//Value::Value(const Value* value) : Value(value->AsBytes(), value->Type()) {}

Value::Value(ByteArray bytes, ValueType type) : m_Type(type), m_Size(getTypeSize(type)) {
	m_Data = bytes;
}

const ByteArray& Value::AsBytes() const {
	return m_Data;
}

std::string Value::ToString() const {
	std::stringstream valueString;

	switch (m_Type) {
	case ValueType::Byte: valueString << static_cast<int>(AsValue<int8_t>()); break;
	case ValueType::Short: valueString << AsValue<int16_t>(); break;
	case ValueType::Int: valueString << AsValue<int32_t>(); break;
	case ValueType::Long: valueString << AsValue<int64_t>(); break;
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
	case ValueType::Bool: valueString << (static_cast<bool>(*this) ? "true" : "false"); break;
	default: return "Unknown value type.";
	}

	return valueString.str();
}

Value Value::operator-() const {
	if (!IsNumber()) return *this;

	switch (m_Type) {
	case ValueType::Byte: return Value(-AsValue<int8_t>());
	case ValueType::Short: return Value(-AsValue<int16_t>());
	case ValueType::Int: return Value(-AsValue<int32_t>());
	case ValueType::Long: return Value(-AsValue<int64_t>());
	case ValueType::Float: return Value(-AsValue<float>());
	case ValueType::Double: return Value(-AsValue<double>());
	default: return Value();
	}
}

Value Value::operator+(Value value) const {
	if (!(IsNumber() && value.IsNumber())) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Byte: return Value(AsValue<int8_t>() + value.AsValue<int8_t>());
	case ValueType::Short: return Value(AsValue<int16_t>() + value.AsValue<int16_t>());
	case ValueType::Int: return Value(AsValue<int32_t>() + value.AsValue<int32_t>());
	case ValueType::Long: return Value(AsValue<int64_t>() + value.AsValue<int64_t>());
	case ValueType::Float: return Value(AsValue<float>() + value.AsValue<float>());
	case ValueType::Double: return Value(AsValue<double>() + value.AsValue<double>());
	default: return Value();
	}
}

Value Value::operator-(Value value) const {
	if (!IsNumber() && !value.IsNumber()) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Byte: return Value(AsValue<int8_t>() - value.AsValue<int8_t>());
	case ValueType::Short: return Value(AsValue<int16_t>() - value.AsValue<int16_t>());
	case ValueType::Int: return Value(AsValue<int32_t>() - value.AsValue<int32_t>());
	case ValueType::Long: return Value(AsValue<int64_t>() - value.AsValue<int64_t>());
	case ValueType::Float: return Value(AsValue<float>() - value.AsValue<float>());
	case ValueType::Double: return Value(AsValue<double>() - value.AsValue<double>());
	default: return Value();
	}
}


Value Value::operator*(Value value) const {
	if (!IsNumber() && !value.IsNumber()) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Byte: return Value(AsValue<int8_t>() * value.AsValue<int8_t>());
	case ValueType::Short: return Value(AsValue<int16_t>() * value.AsValue<int16_t>());
	case ValueType::Int: return Value(AsValue<int32_t>() * value.AsValue<int32_t>());
	case ValueType::Long: return Value(AsValue<int64_t>() * value.AsValue<int64_t>());
	case ValueType::Float: return Value(AsValue<float>() * value.AsValue<float>());
	case ValueType::Double: return Value(AsValue<double>() * value.AsValue<double>());
	default: return Value();
	}
}


Value Value::operator/(Value value) const {
	if (!IsNumber() && !value.IsNumber()) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Byte: return Value(AsValue<int8_t>() / value.AsValue<int8_t>());
	case ValueType::Short: return Value(AsValue<int16_t>() / value.AsValue<int16_t>());
	case ValueType::Int: return Value(AsValue<int32_t>() / value.AsValue<int32_t>());
	case ValueType::Long: return Value(AsValue<int64_t>() / value.AsValue<int64_t>());
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

bool Value::operator==(const Value& value) const {
	// If they're the same type, just compare the bits directly.
	if (m_Type == value.Type()) {
		return m_Data == value.AsBytes();
	}

	// If one type is a number, but the other isn't, then they are not comparable.
	if ((IsNumber() && !IsNumber()) ||
		(!IsNumber() && IsNumber())) return false;

	// Check if value is integral or float
	if (IsIntegral()) {
		// For now, get the biggest integral possible. \todo Optimize later
		int64_t num = AsValue<int64_t>();

		// Compare it to the integral or float of the other value.
		switch (value.IsIntegral()) {
		case true: return num == value.AsValue<int32_t>();
		case false: return num == value.AsValue<float>();
		}
	} else {
		// Same thing as before, but with a double this time.
		double num = AsValue<float>();

		switch (value.IsIntegral()) {
		case true: return num == value.AsValue<int32_t>();
		case false: return num == value.AsValue<float>();
		}
	}

	return false;
}

template <typename T>
bool lessThan(T valueA, Value valueB) {
	switch (valueB.Type()) {
	case ValueType::Byte: return valueA < valueB.AsValue<int8_t>();
	case ValueType::Short: return valueA < valueB.AsValue<int16_t>();
	case ValueType::Int: return valueA < valueB.AsValue<int32_t>();
	case ValueType::Long: return valueA < valueB.AsValue<int64_t>();
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
	case ValueType::Byte: return lessThan(AsValue<int8_t>(), value);
	case ValueType::Short: return lessThan(AsValue<int16_t>(), value);
	case ValueType::Int: return lessThan(AsValue<int32_t>(), value);
	case ValueType::Long: return lessThan(AsValue<int64_t>(), value);
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

ValueType smallestTypeNeeded(ValueType a, ValueType b) {
	return a > b ? a : b;
}

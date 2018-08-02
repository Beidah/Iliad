#include "stdafx.h"
#include "Value.h"

#include <sstream>

constexpr size_t getTypeSize(ValueType type) {
	switch (type) {
	case ValueType::Invalid: return 0;
	case ValueType::Char: return sizeof(int8_t);
	case ValueType::Short: return sizeof(int16_t);
	case ValueType::Int: return sizeof(int32_t);
	case ValueType::Long: return sizeof(int64_t);
	case ValueType::Float: return sizeof(float);
	case ValueType::Double: return sizeof(double);
	case ValueType::Bool: return sizeof(bool);
	default: return 0;
	}
}

template <typename T>
ValueType getType(T) {
	static_assert(!std::is_same<T, T>::value, "This is not a supported type.");
	return ValueType::Invalid;
}

template <>
ValueType getType<int8_t>(int8_t) {
	return ValueType::Char;
}

template <>
ValueType getType<int16_t>(int16_t) {
	return ValueType::Short;
}

template <>
ValueType getType<int32_t>(int32_t) {
	return ValueType::Int;
}

template <>
ValueType getType<int64_t>(int64_t) {
	return ValueType::Long;
}

template <>
ValueType getType<float>(float) {
	return ValueType::Float;
}

template <>
ValueType getType<double>(double) {
	return ValueType::Double;
}

template <>
ValueType getType<bool>(bool) {
	return ValueType::Bool;
}

template<>
ValueType getType<Value>(Value value) {
	return value.Type();
}

template <typename T>
ByteArray toBytes(T value) {
	static_assert(!std::is_same<T, T>::value, "This is not a supported type.");
	return ByteArray();
}

template<>
ByteArray toBytes<int8_t>(int8_t value) {
	ByteArray data;
	data.push_back(value);
	return data;
}

template<>
ByteArray toBytes<int16_t>(int16_t value) {
	const static size_t size = sizeof(int16_t);

	ByteArray data;

	for (size_t i = 0; i < size; i++) {
		byte datum = (value >> ((size - i - 1) * 8)) & 0xFF;
		data.push_back(datum);
	}

	return data;
}

template<>
ByteArray toBytes<int32_t>(int32_t value) {
	const static size_t size = sizeof(int32_t);

	ByteArray data;

	for (size_t i = 0; i < size; i++) {
		byte datum = (value >> ((size - i - 1) * 8)) & 0xFF;
		data.push_back(datum);
	}

	return data;
}

template<>
ByteArray toBytes<int64_t>(int64_t value) {
	const static size_t size = sizeof(int64_t);

	ByteArray data;

	for (size_t i = 0; i < size; i++) {
		byte datum = (value >> ((size - i - 1) * 8)) & 0xFF;
		data.push_back(datum);
	}

	return data;
}

template<>
ByteArray toBytes<float>(float value) {
	int32_t intermediate = reinterpret_cast<int32_t&>(value);
	return toBytes(intermediate);
}

template<>
ByteArray toBytes<double>(double value) {
	int64_t intermediate = reinterpret_cast<int64_t&>(value);
	return toBytes(intermediate);
}

template<>
ByteArray toBytes<bool>(bool value) {
	ByteArray data;
	data.push_back(value ? 1 : 0);
	return data;
}

template<>
ByteArray toBytes<Value>(Value value) {
	ByteArray data = value.AsBytes();
	return data;
}


Value::Value(const Value & value) : m_Type(value.Type()), m_Size(value.Size()) {
	m_Data = value.AsBytes();
}

Value::Value(const Value* value) : Value(value->AsBytes(), value->Type()) {}

/*Value::Value(const Value && value) : m_Type(value.type()), m_Size(value.size()) {
	std::copy(value.AsBytes().begin(), value.AsBytes().end(), m_Data.begin());
}*/

Value::Value(ByteArray bytes, ValueType type) : m_Type(type), m_Size(getTypeSize(type)) {
	m_Data = bytes;
}

template<typename T>
T Value::AsValue() const {
	static_assert(std::is_trivially_copyable<T>::value, "Type mismatch.");

	if (std::is_same<T, bool>()) {
		return static_cast<bool>(*this);
	}

	T value = 0;

	for (size_t i = 0; i < m_Size; i++) {
		value |= (m_Data[i] << (8 * (m_Size - i - 1)));
	}

	return value;
}

template <>
float Value::AsValue<float>() const {
	if (!IsNumber()) return 0;

	int32_t intermediate = AsValue<int32_t>();

	return IsDecimal() ? reinterpret_cast<float&>(intermediate) : static_cast<float>(intermediate);
}

template <>
double Value::AsValue<double>() const {
	if (!IsNumber()) return 0;

	int64_t intermediate = AsValue<int64_t>();

	return IsDecimal() ? reinterpret_cast<double&>(intermediate) : static_cast<double>(intermediate);
}


const ByteArray& Value::AsBytes() const {
	return m_Data;
}


const std::string Value::ToString() const {
	std::stringstream valueString;

	switch (m_Type) {
	case ValueType::Char: valueString << static_cast<int>(AsValue<int8_t>()); break;
	case ValueType::Short: valueString << AsValue<int16_t>(); break;
	case ValueType::Int: valueString << AsValue<int32_t>(); break;
	case ValueType::Long: valueString << AsValue<int64_t>(); break;
	case ValueType::Float: valueString << AsValue<float>(); break;
	case ValueType::Double: valueString << AsValue<double>(); break;
	case ValueType::Bool: valueString << static_cast<bool>(this) ? "true" : "false"; break;
	default: return "Unknown value type.";
	}

	return valueString.str();
}

Value Value::operator-() const {
	if (!IsNumber()) return this;

	switch (m_Type) {
	case ValueType::Char: return Value(-AsValue<uint8_t>());
	case ValueType::Short: return Value(-AsValue<int16_t>());
	case ValueType::Int: return Value(-AsValue<int32_t>());
	case ValueType::Long: return Value(-AsValue<int64_t>());
	case ValueType::Float: return Value(-AsValue<float>());
	case ValueType::Double: return Value(-AsValue<double>());
	default: return Value();
	}
}

Value Value::operator+(Value value) const {
	if (!IsNumber() && !value.IsNumber()) return Value();

	ValueType outputType = smallestTypeNeeded(m_Type, value.m_Type);

	switch (outputType) {
	case ValueType::Char: return Value(AsValue<int8_t>() + value.AsValue<int8_t>());
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
	case ValueType::Char: return Value(AsValue<int8_t>() - value.AsValue<int8_t>());
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
	case ValueType::Char: return Value(AsValue<int8_t>() * value.AsValue<int8_t>());
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
	case ValueType::Char: return Value(AsValue<int8_t>() / value.AsValue<int8_t>());
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

Value::operator bool() const {
	for (size_t i = 0; i < m_Size; i++) {
		if (m_Data[i] != 0) return true;
	}

	return false;
}

ValueType Value::smallestTypeNeeded(ValueType a, ValueType b) const {
	return a > b ? a : b;
}

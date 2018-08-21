//! \file ValueType.h
//! \brief The ValueType enum, as well as helper functions to build Value from different types
#pragma once

#include <vector>
#include <type_traits>

//! Transforms a lvalue into an rvalue.
#define FWD(value) std::forward<decltype(value)>(value)

//! An unsigned 8-bits of data.
typedef uint8_t byte;
//! An std::vector of \c byte
typedef std::vector<byte> ByteArray;

//! \enum ValueType
//! The type of data the value is meant to represent.
/*!
Currently supported types of values are:
- Integrals
	- byte (8-bits)
	- shorts (16-bits)
	- int (32-bits)
	- longs (64-bits)
- Decimals
	- float (32-bit single precision)
	- double (64-bit double precision)
- bool

\todo Add other value types.
*/

enum class ValueType {
	Invalid = -1,

	//!@{ 
	//! Integral Numbers
	Byte, Short,
	Int, Long,
	//!@}

	//!@{
	//! Floating Point numbers
	Float, Double,
	//!@}

	//!@{
	//! Char
	// \todo implement char datatype
	Char,
	//!@}

	Bool, //!< Boolean
};

//! Gets the size in bytes of value respresented by ValueType 
constexpr size_t getTypeSize(ValueType type) {
	switch (type) {
	case ValueType::Invalid: return 0;
	case ValueType::Byte: return sizeof(int8_t);
	case ValueType::Short: return sizeof(int16_t);
	case ValueType::Int: return sizeof(int32_t);
	case ValueType::Long: return sizeof(int64_t);
	case ValueType::Float: return sizeof(float);
	case ValueType::Double: return sizeof(double);
	case ValueType::Char: return sizeof(int8_t);
	case ValueType::Bool: return sizeof(bool);
	default: return 0;
	}
}


//!@{ \name Transformer
//! \brief Converts type into appropriate ValueType enum
/*!
Will take a value and convert it's type into the appropriate ValueType. If
passed a Value, it's the same as calling Value.Type().
\tparam T Any supported ValueType, or Value
\return Corresponding ValueType to type T, or type of Value.
*/

template <typename T>
static ValueType getType(T) {
	static_assert(!std::is_same<T, T>::value, "This is not a supported type.");
	return ValueType::Invalid;
}

template <>
static ValueType getType<int8_t>(int8_t) {
	return ValueType::Byte;
}

template <>
static ValueType getType<int16_t>(int16_t) {
	return ValueType::Short;
}

template <>
static ValueType getType<int32_t>(int32_t) {
	return ValueType::Int;
}

template <>
static ValueType getType<int64_t>(int64_t) {
	return ValueType::Long;
}

template <>
static ValueType getType<float>(float) {
	return ValueType::Float;
}

template <>
static ValueType getType<double>(double) {
	return ValueType::Double;
}

template <>
static ValueType getType<bool>(bool) {
	return ValueType::Bool;
}
//!@}

//!@{ \name Serialize
//! \brief Serializes a value into a vector of unsigned chars.
/*!
Takes a value of supported types and converts it to binary. If called on an
instance of Value class, returns it's AsBytes method.
\tparam value Any supported ValueType, or Value
\return A binary representation of value, stored in vector<uint8_t>
*/
template <typename T>
static ByteArray toBytes(T value) {
	static_assert(std::is_arithmetic<T>::value, "This is not a supported type.");
	size_t size = sizeof(T);

	ByteArray data;

	for (size_t i = 0; i < size; i++) {
		byte datum = (value >> ((size - i - 1) * 8)) & 0xFF;
		data.push_back(datum);
	}

	return data;
	return ByteArray();
}

template<>
static ByteArray toBytes<int8_t>(int8_t value) {
	ByteArray data;
	std::copy(&value, &value + 1, data.begin());
	return data;
}

template<>
static ByteArray toBytes<float>(float value) {
	int32_t intermediate = reinterpret_cast<int32_t&>(value);
	return toBytes(intermediate);
}

template<>
static ByteArray toBytes<double>(double value) {
	int64_t intermediate = reinterpret_cast<int64_t&>(value);
	return toBytes(intermediate);
}

template<>
static ByteArray toBytes<bool>(bool value) {
	ByteArray data;
	data.push_back(value ? 1 : 0);
	return data;
}
//!@}
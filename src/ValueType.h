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
- Integrals (8-, 16-, 32-, and 64-bit)
- Decimals
	- float (32-bit single precision)
	- double (64-bit double precision)
- char
- bool

\todo Add other value types.
*/

enum class ValueType {
	Invalid = -1,

	//!@{ 
	//! Integral Numbers
	Int8, Int16,
	Int32, Int64,
	//!@}

	//!@{
	//! Floating Point numbers
	Float, Double,
	//!@}

	
	Char, //!< Char
	String, //!< String

	Bool, //!< Boolean
	Null, //!< Null
};

//! Helper function to find the "smallest" value given.
ValueType smallestTypeNeeded(ValueType a, ValueType b);

//!@{
//! Functions to help determine the type of Value
inline bool IsValid(ValueType type) { return type > ValueType::Invalid; }
inline bool IsNumber(ValueType type) { return type >= ValueType::Int8 && type <= ValueType::Double; }
inline bool IsInt(ValueType type) { return type >= ValueType::Int8 && type <= ValueType::Int64; }
inline bool IsFloat(ValueType type) { return type == ValueType::Float || type == ValueType::Double; }
inline bool IsChar(ValueType type) { return type == ValueType::Char; }
inline bool IsString(ValueType type) { return type == ValueType::String; }
inline bool IsBool(ValueType type) { return type == ValueType::Bool; }
//!@}

//! Get a string of the type name.
std::string ValueTypeToString(ValueType type);

//! Returns the default size of each Value associated with ValueType
size_t ValueTypeSize(ValueType type);


//! A static class to transform different types into the appropriate ValueType enum.
class Transformer {
public:
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
		return ValueType::Int8;
	}

	template <>
	static ValueType getType<int16_t>(int16_t) {
		return ValueType::Int16;
	}

	template <>
	static ValueType getType<int32_t>(int32_t) {
		return ValueType::Int32;
	}

	template <>
	static ValueType getType<int64_t>(int64_t) {
		return ValueType::Int64;
	}

	template <>
	static ValueType getType<float>(float) {
		return ValueType::Float;
	}

	template <>
	static ValueType getType<double>(double) {
		return ValueType::Double;
	}

	template<>
	static ValueType getType<char>(char) {
		return ValueType::Char;
	}

	template<>
	static ValueType getType<std::string>(std::string) {
		return ValueType::String;
	}

	template <>
	static ValueType getType<bool>(bool) {
		return ValueType::Bool;
	}
	//!@}
};


//! \brief A static class to serialize a value into a vector of unsigned chars.
class Serialize {
public:
	//!@{ \name Serialize
	//! Serializes values into byte arrays.
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
	static ByteArray toBytes<std::string>(std::string value) {
		ByteArray bytes(value.begin(), value.end());
		return bytes;
	}

	template<>
	static ByteArray toBytes<bool>(bool value) {
		ByteArray data;
		data.push_back(value ? 1 : 0);
		return data;
	}
	//!@}
};
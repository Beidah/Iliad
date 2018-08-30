//! \file Value.h
//! \brief Details the data representation used by the program.
#pragma once

#include "ValueType.h"



//! Basical value representation
/*!
  Values are stored in an std::vector of unsigned chars of the binary of the value.
  This class stores the bytes, size and an enum of the type of data that is represented.
  Basic arithmetic operators are overloaded to make using values in the compiler code as easy
  as using normal data-types. Values are stored in little endian.
*/
class Value {
private:
	const ValueType m_Type; //!< The type the value represents.
	size_t m_Size; //!< Size of the data in bytes.
	ByteArray m_Data; //!< Bytes of data. Stored in little endian.
	bool m_Initialized; //!< If it has been given a value to begin with or not.


public:

	//!@{ \name Constructors
	//! Initilizes values

	//! Default constructor, returns an "Null" value
	Value() : m_Type(ValueType::Null), m_Size(0), m_Initialized(false) {}


	//! Copy constructor
	Value(const Value& value);

	
private:
	//! Main constructor
	/*!
	  This constructor is the one called by the templated one, and initilizes a valid value.
	  \param bytes A vector<uint8_t> of the binary to be stored, in little endian.
	  \param type ValueType of value.
	*/
	Value(ByteArray bytes, ValueType type);

public:
	//! Template constructor, takes in a value and serializes it.
	/*!
	  Calls main constructor after passing input through both toBytes and getType
	  \param value value of any supported ValueType.
	*/
	template<typename T, typename = std::enable_if_t<!std::is_same_v<T, Value&>&& !std::is_same_v<T, Value>>>
	Value(T&& value) : Value(Serialize::toBytes<T>(value), Transformer::getType(value)) {}

	//! Creates an "uninitilized" value of the given type.
	Value(ValueType type) : m_Type(type), m_Size(ValueTypeSize(type)), m_Initialized(false) { m_Data.reserve(m_Size); }

	

	//!@}

	//!@{ \name Getters
	//! Gets const data from private members.

	//! Converts byte array into type T
	/*! Converts the data into a usable value type. Is able to cast to appropiate types. 
		May result in data lost if used to convert a float to an integral or a larger value
		to a smaller one.
	  \return Value called for.
	*/
	template<typename T>
	T AsValue() const;

	//! Specialized AsValue for bool values. Returns bool operator
	template<>
	bool AsValue<bool>() const { return static_cast<bool>(*this); }

	//! Specialized AsValue for float values.
	template <>
	float AsValue<float>() const {
		// If value is not a number, it's not convertable to a float.
		if (!IsNumber()) return 0;

		if (m_Type == ValueType::Float) {
			int32_t intermediate = 0;

			for (size_t i = 0; i < m_Size; i++) {
				intermediate |= (m_Data[i] << (8 * (m_Size - i - 1)));
			}

			return reinterpret_cast<float&>(intermediate);
		} else {
			switch (m_Type) {
			case ValueType::Int8: return static_cast<float>(AsValue<int8_t>());
			case ValueType::Int16: return static_cast<float>(AsValue<int16_t>());
			case ValueType::Int32: return static_cast<float>(AsValue<int32_t>());
			case ValueType::Int64: return static_cast<float>(AsValue<int64_t>());
			case ValueType::Double: return static_cast<float>(AsValue<double>());	//! \todo compiler warning about loss of percision
			default: return 0; // Unreachable
			}
		}
	}

	//! Specialized AsValue for double Values.
	template <>
	double AsValue<double>() const {
		// If value is not a number, it's not convertable to a float.
		if (!IsNumber()) return 0;

		if (m_Type == ValueType::Double) {
			int64_t intermediate = 0;

			for (size_t i = 0; i < m_Size; i++) {
				intermediate |= (m_Data[i] << (8 * (m_Size - i - 1)));
			}

			return reinterpret_cast<double&>(intermediate);
		} else {
			switch (m_Type) {
			case ValueType::Int8: return static_cast<double>(AsValue<int8_t>());
			case ValueType::Int16: return static_cast<double>(AsValue<int16_t>());
			case ValueType::Int32: return static_cast<double>(AsValue<int32_t>());
			case ValueType::Int64: return static_cast<double>(AsValue<int64_t>());
			case ValueType::Float: return static_cast<double>(AsValue<float>());
			default: return 0; // Unreachable
			}
		}
	}

	//! Specialized AsValue for string values.
	template<>
	std::string AsValue<std::string>() const {
		if (IsChar() || IsString())
			return std::string(m_Data.begin(), m_Data.end());
		else return ToString();
	}

	//! returns a byte array of the value.
	const ByteArray& AsBytes() const;

	//! A string representation of the value for printing.
	std::string ToString() const;

	//! \return Size of Value in bytes.
	size_t Size() const { return m_Size; }
	//! \return ValueType of Value.
	const ValueType& Type() const { return m_Type; }
	//!@}


	//!@{ \name Operators

	//!@{ Unary operator.
	//! \brief Multiplies the value by -1 to get its negative
	/*!
	  \return If Value is a number, a new Value of opposite value, otherwise, returns self.
	*/
	Value operator-() const;
	//!@}


	//!@{ Binary operator, should only be used for number values.
	Value operator+(Value value) const;
	Value operator-(Value value) const;
	Value operator*(Value value) const;
	Value operator/(Value value) const;
	//!@}

	//!@{ Assignment
	Value& operator=(const Value& value);

	template<typename T>
	Value& operator=(T value) { 
		m_Data = Serialize::toBytes(FWD(value));
		m_Initialized = true;
		return *this;
	}

	template<>
	Value& operator=<std::string>(std::string value) {
		m_Size = value.size();
		m_Data.assign(value.begin(), value.end());
		m_Initialized = true;
		return *this;
	}
	//!@}

	//!@{ Comparison
	//! \todo When available, replace with C++20 "spaceship" operator using partial_ordering for unorderable comparisons.
	bool operator==(const Value& value) const;
	bool operator!=(const Value& value) const { return !(*this == value); }
	bool operator<(const Value& value) const;
	bool operator>(const Value& value) const { return value < *this; }
	bool operator<=(const Value& value) const { return (*this < value) || (*this == value); }
	bool operator>=(const Value& value) const { return (*this > value) || (*this == value); }
	//!@}

	//!@{ Boolean value
	explicit operator bool() const;
	//!@}
	//!@}

	//!@{ \name Utilities
	//! Functions to help determine the type of Value
	inline bool IsNumber() const { return m_Type >= ValueType::Int8 && m_Type <= ValueType::Double; }
	inline bool IsIntegral() const { return m_Type >= ValueType::Int8 && m_Type <= ValueType::Int64; }
	inline bool IsDecimal() const { return m_Type >= ValueType::Float && m_Type <= ValueType::Double; }
	inline bool IsBoolean() const { return m_Type == ValueType::Bool; }
	inline bool IsChar() const { return m_Type == ValueType::Char; }
	inline bool IsString() const { return m_Type == ValueType::String; }
	inline bool IsNull() const { return m_Type == ValueType::Null; }
	inline bool IsInitilized() const { return m_Initialized; }
	inline bool IsValid() const { return m_Type != ValueType::Invalid; }
	//!@}
};


template<typename T>
T Value::AsValue() const {
	static_assert(std::is_arithmetic<T>::value, "Type mismatch.");

	// If finding the boolean, use bool operator.
	if (std::is_same<T, bool>()) {
		return static_cast<bool>(*this);
	}

	// If the types matchup, just convert byte array to type.
	if (m_Type == Transformer::getType(T())) {
		T value = 0;
		for (size_t i = 0; i < m_Size; i++) {
			value |= (m_Data[i] << (8 * (m_Size - i - 1)));
		}

		return value;
	}

	if (IsDecimal()) {
		// TODO: Compiler Warning about potential loss of data
		if (m_Type == ValueType::Float) {
			int32_t intermediate = 0;

			for (size_t i = 0; i < m_Size; i++) {
				intermediate |= (m_Data[i] << (8 * (m_Size - i - 1)));
			}

			float floatVal = reinterpret_cast<float&>(intermediate);
			return static_cast<T>(floatVal);
		} else if (m_Type == ValueType::Double) {
			int64_t intermediate = 0;

			for (size_t i = 0; i < m_Size; i++) {
				intermediate |= (m_Data[i] << (8 * (m_Size - i - 1)));
			}

			double doubleVal = reinterpret_cast<double&>(intermediate);
			return static_cast<T>(doubleVal);
		}
	}

	
	// TODO: Compiler Warning about potential loss of data
	//if (sizeof(T) < m_Size) {

	//}

	// If types do not match up, get origianl type and convert.
	switch (m_Type) {
	case ValueType::Int8: return static_cast<T>(AsValue<int8_t>());
	case ValueType::Int16: return static_cast<T>(AsValue<int16_t>());
	case ValueType::Int32: return static_cast<T>(AsValue<int32_t>());
	case ValueType::Int64: return static_cast<T>(AsValue<int64_t>());
	default: static_assert(true, "Type not supported.");
	}

	return T();
}

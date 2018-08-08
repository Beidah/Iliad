//! \file Value.h
//! \brief Details the data representation used by the program.
#pragma once

#include <vector>
#include <type_traits>

#define FWD(value) std::forward<decltype(value)>(value)

typedef uint8_t byte;
typedef std::vector<byte> ByteArray;


//! The type of data the value is meant to represent.
/*!
  Currently supported types of values are:
  - Integrals
	- char (8-bits) note: might rename to byte?
	- shorts (16-bits)
	- int (32-bits)
	- longs (64-bits)
  - Decimals
	- float (32-bit single precision)
	- double (64-bit double precision)
  - bool
  
  TODO:: Add other value types.
*/

enum class ValueType {
	Invalid = -1,

	//!@{ 
	//! Integral Numbers
	Char, Short,
	Int, Long,
	//!@}

	//!@{
	//! Floating Point numbers
	Float, Double,
	//!@}

	Bool, //!< Boolean
};


//! Basical value representation
/*!
  Values are stored in an std::vector of unsigned chars of the binary of the value.
  This class stores the bytes, size and an enum of the type of data that is represented.
  Basic arithmetic operators are overloaded to make using values in the compiler code as easy
  as using normal data-types. Values are stored in little endian.
*/

class Value {
public:
	const ValueType m_Type; //!< The type the value represents.
	const size_t m_Size; //!< Size of the data in bytes.
	ByteArray m_Data; //!< Bytes of data. Stored in little endian.


public:

	//!@{ \name Constructors
	//! Initilizes values

	//! Default constructor, returns an "Invalid" value
	Value() : m_Type(ValueType::Invalid), m_Size(0) {}


	//! Copy constructor
	Value(const Value& value);
	//! Copy constructor
	Value(const Value* value);

	
	//! Main constructor
	/*!
	  This constructor is the one called by the templated one, and initilizes a valid value.
	  \param bytes A vector<uint8_t> of the binary to be stored, in little endian.
	  \param type ValueType of value.
	*/
	Value(ByteArray bytes, ValueType type);

	//! Template constructor, takes in a value and serializes it.
	/*!
	  Calls main constructor after passing input through both toBytes and getType
	  \param value value of any supported ValueType.
	*/
	template<typename T>
	Value(T&& value);

	//!@}

	//!@{ \name Getters
	//! Gets const data from private members.

	//! Converts byte array into type T
	template<typename T>
	T AsValue() const;

	//! returns a byte array of the value.
	const ByteArray& AsBytes() const;

	//! A string representation of the value for printing.
	const std::string ToString() const;

	//! \return Size of Value in bytes.
	const size_t Size() const { return m_Size; }
	//! \return ValueType of Value.
	const ValueType& Type() const { return m_Type; }
	//!@}


	//!@{ \name Operators

	//!@{ \name Unary

	//! \brief Multiplies the value by -1 to get its negative
	Value operator-() const;
	//!@}


	//!@{ \name Binary
	Value operator+(Value value) const;
	Value operator-(Value value) const;
	Value operator*(Value value) const;
	Value operator/(Value value) const;
	//!@}

	//!@{ \name Assignment
	Value &operator = (const Value& value);
	//!@}

	//!@{ \name Logical
	explicit operator bool() const;
	//!@}
	//!@}

	//!@{ \name Utilities
	//! \brief Functions to help determine the type of Value
	inline bool IsNumber() const { return m_Type >= ValueType::Char && m_Type <= ValueType::Double; }
	inline bool IsIntegral() const { return m_Type >= ValueType::Char && m_Type <= ValueType::Long; }
	inline bool IsDecimal() const { return m_Type >= ValueType::Float && m_Type <= ValueType::Double; }
	inline bool IsValid() const { return m_Type != ValueType::Invalid; }
	//!@}

private:
	ValueType smallestTypeNeeded(ValueType a, ValueType b) const;
};



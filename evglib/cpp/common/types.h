#pragma once

#include "cppstd.h"

#define category(name) namespace name {} using namespace name; namespace name

#define stack_only
#define binary_rep(encoding) // Binary, hex, etc for debugger

#define compile_const constexpr
#define view_const const // Cannot be modified by viewer
//#define const // Will not change

#define var auto
#define anyvar auto&&
#define typeof decltype
#define type using

category(evg)
{
	// Class that cannot be used at compile time, indicates when a type is unavailable
	class InvalidType;

	// Platform defined integer aliases
	type UShort = unsigned short;
	type SShort = signed short;
	type Short = short;

	type UInt = unsigned int;
	type SInt = signed int;
	type Int = int;

	type ULong = unsigned long;
	type SLong = signed long;
	type Long = long;

	type ULongLong = unsigned long long;
	type SLongLong = signed long long;
	type LongLong = long long;

	type Float = float;
	type Double = double;

	// Fixed width integer aliases
	type U8 = uint8_t;
	type S8 = int8_t;
	type I8 = int8_t;

	type U16 = uint16_t;
	type S16 = int16_t;
	type I16 = int16_t;

	type U32 = uint32_t;
	type S32 = int32_t;
	type I32 = int32_t;

	type U64 = uint64_t;
	type S64 = int64_t;
	type I64 = int64_t;

	type U128 = InvalidType;
	type S128 = InvalidType;
	type I128 = InvalidType;

	// Float aliases
	type F16 = InvalidType;
	type SF16 = InvalidType;
	type UF16 = InvalidType;

	type F32 = Float;
	type SF32 = Float;
	type UF32 = InvalidType;

	type F64 = Double;
	type SF64 = Double;
	type UF64 = InvalidType;

	type F128 = InvalidType;
	type SF128 = InvalidType;
	type UF128 = InvalidType;

	namespace fast
	{
		type Int = Int;
		type Float = Float;
		type Double = Double;

		type U8 = UInt;
		type S8 = SInt;
		type I8 = Int;

		type U16 = UInt;
		type S16 = SInt;
		type I16 = Int;

		type U32 = UInt;
		type S32 = SInt;
		type I32 = Int;
	}

	namespace least
	{

	}

	// Characters
	type UnicodeChar = fast::U32;

	type UChar = unsigned char;
	type SChar = signed char;
	type Char = UChar;

	enum class Byte : Char {};
	type Size = size_t;
	type PtrNum = uintptr_t;
	type binary_rep(hexadecimal) Hash = Size;

	using VoidFun = void(*)();

	/*

	// Virtual function
	enum
	{
		direct,
		virtual	
	};

	// Encoding
	enum
	{
		hexadecimal,
		ascii,
		utf8,
		ebcdic
	}

	// Automatic threading policy
	enum
	{
		sync,
		async
	};

	// Atomicity
	enum
	{
		nonatomic,
		atomoic
	};

	enum
	{
		aliasing,
		nonaliasing
	};

	enum
	{
		const, // Variable cannot be changed ever

	}

	enum
	{
		owning, // Caller is allowed to modify resource
		viewing // Points to a resource that cannot be modified by *this* function
	};

	anyone can modify - mutable
	i cannot modify - viewing
	you cannot modify - owning
	no one can modify - immutable




	
	*/
}
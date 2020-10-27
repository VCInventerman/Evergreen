#include "simple.h"

category (evg)
{
	// Class that cannot be used at compile time, indicates when a type is unavailable
	class InvalidType;

	// Platform defined integer aliases
	using UShort = unsigned short;
	using SShort = signed short;
	using Short = short;

	using UInt = unsigned int;
	using SInt = signed int;
	using Int = int;

	using ULong = unsigned long;
	using SLong = signed long;
	using Long = long;

	using ULongLong = unsigned long long;
	using SLongLong = signed long long;
	using LongLong = long long;

	// Fixed width integer aliases
	using U8 = uint8_t;
	using S8 = int8_t;
	using I8 = int8_t;

	using U16 = uint16_t;
	using S16 = int16_t;
	using I16 = int16_t;

	using U32 = uint32_t;
	using S32 = int32_t;
	using I32 = int32_t;

	using U64 = uint64_t;
	using S64 = int64_t;
	using I64 = int64_t;

	using U128 = InvalidType;
	using S128 = InvalidType;
	using I128 = InvalidType;

	// Float aliases
	using F16 = InvalidType;
	using SF16 = InvalidType;
	using UF16 = InvalidType;

	using F32 = float;
	using SF32 = float;
	using UF32 = InvalidType;

	using F64 = double;
	using SF64 = double;
	using UF64 = InvalidType;

	using F128 = InvalidType;
	using SF128 = InvalidType;
	using UF128 = InvalidType;

	using FloatFast = float;

	// Characters
	using UnicodeChar = uint_fast32_t;

	using UChar = unsigned char;
	using SChar = signed char;
	using Char = char;

	// Platform description
	using Size = size_t;
	using PtrNum = uintptr_t;
}
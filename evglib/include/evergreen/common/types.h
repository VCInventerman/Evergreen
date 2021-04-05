#pragma once

//is a inline namespace 
//#define category(name) namespace name {} using namespace name; namespace name 

#define stack_only
#define binary_rep(encoding) // Binary, hex, etc for debugger

#define compile_const constexpr
#define view_const const // Cannot be modified by viewer
//#define const // Will not change

#define var auto
#define anyvar auto&&
#define typeof decltype

// Cast operator
#define EVG_CXX_CAST(type) operator type () const { return operator_conv<type>(); }
#define EVG_CXX_CAST_ADAPT(type, adapter) operator type () const { return (type)(operator_conv<adapter>()); }

#define EVG_CXX_CAST_EXPLICIT(type) explicit operator type () const { return operator_conv<type>(); }
#define EVG_CXX_CAST_EXPLICIT_ADAPT(type, adapter) explicit operator type () const { return (type)(operator_conv<adapter>()); }


#define EVG_CXX_CAST_REDIRECT(type) operator type () const { return operator_conv<type>(); }
#define EVG_CXX_CAST_ADAPT_REDIRECT(type, adapter) operator type () const { return (type)(operator_conv<adapter>()); }

#define EVG_CXX_CAST_EXPLICIT_REDIRECT(type) explicit operator type () const { return operator_conv<type>(); }
#define EVG_CXX_CAST_EXPLICIT_ADAPT_REDIRECT(type, adapter) explicit operator type () const { return (type)(operator_conv<adapter>()); }

#define conv(var, type) var.operator type()
#define inheritFR(member, func, ret) ret func() { return member.func() } // Inherit from function from a member that has a return value
#define inheritFV(member, func) void func() { member.func() }





#ifndef EVG_DISABLE_RETERROR
#define EVG_RET_ERRORT Error
#define EVG_RET_ERROR(val) return val;
#else
#define EVG_RET_ERRORT void
#define EVG_RET_ERROR(val) throw 
#endif

inline namespace evg
{
	// Class that cannot be used at compile time, indicates when a type is unavailable
	class InvalidType;

	// Class for use in conditional inheritance
	class Empty {};

	// Platform defined integer aliases
	using UShort = unsigned short;
	using SShort = signed short;
	using Short = short;

	using UInt = unsigned int;
	using SInt = signed int;
	using Int = int;

	using ULong = unsigned long long;
	using SLong = signed long long;
	using Long = long long;

	using ULongLong = unsigned long long;
	using SLongLong = signed long long;
	using LongLong = long long;

	using Float = float;
	using Double = double;

	using Bool = bool;

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

	using F32 = Float;
	using SF32 = Float;
	using UF32 = InvalidType;

	using F64 = Double;
	using SF64 = Double;
	using UF64 = InvalidType;

	using F128 = InvalidType;
	using SF128 = InvalidType;
	using UF128 = InvalidType;

	// Bool aliases
	using Bool8 = Bool;
	using Bool16 = InvalidType;
	using Bool32 = InvalidType;
	using Bool64 = InvalidType;

	namespace fast
	{
		using Int = Int;
		using Float = Float;
		using Double = Double;

		using U8 = UInt;
		using S8 = SInt;
		using I8 = Int;

		using U16 = UInt;
		using S16 = SInt;
		using I16 = Int;

		using U32 = UInt;
		using S32 = SInt;
		using I32 = Int;
	}

	namespace least
	{
		using Int = Int;
		using Float = Float;
		using Double = Double;

		using U8 = UInt;
		using S8 = SInt;
		using I8 = Int;

		using U16 = UInt;
		using S16 = SInt;
		using I16 = Int;

		using U32 = UInt;
		using S32 = SInt;
		using I32 = Int;

		using U64 = ULong;
		using S64 = SLong;
		using I64 = Long;
	}

	// Characters
	using UniChar = fast::U32;

	using UChar = unsigned char;
	using SChar = signed char;
#define Char char
#define CChar const char

	enum class Byte : Char {};
	using Size = uint_least64_t;
	using Offset = int_least64_t;
	using PtrNum = uintptr_t;
	using PtrDiff = ptrdiff_t;
	using binary_rep(hexadecimal) Hash = Size;
	using Pid = U64;

	using VoidFn = void(*)();
	void nop() {} // Global "nop" function

	using EnumVal = int;
	
	template<typename T>
	using Ptr = T*;


	template<typename T>
	using SPtr = std::shared_ptr<T>;

#define makeSPtr std::make_shared


	template<typename T>
	using UPtr = std::unique_ptr<T>;

#define makeUPtr std::make_unique

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
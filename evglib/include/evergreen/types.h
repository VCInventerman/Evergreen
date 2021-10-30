#pragma once

#include <mutex>

#define EVG_UNDEFINED_VIRTUAL_FUNCTION { logError("Undefined virtual function called!"); throw std::runtime_error("Undefined virtual function called!"); }

namespace evg
{
	namespace types
	{
		using namespace std::literals::chrono_literals;

		// Class that cannot be used at compile time, indicates when a type is unavailable
		class InvalidType;

		// Class for use in conditional inheritance
		template <size_t index>
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



		// Fixed width integer aliases
		using UInt8 = uint8_t;
		using SInt8 = int8_t;
		using Int8 = int8_t;

		using UInt16 = uint16_t;
		using SInt16 = int16_t;
		using Int16 = int16_t;

		using UInt32 = uint32_t;
		using SInt32 = int32_t;
		using Int32 = int32_t;

		using UInt64 = uint64_t;
		using SInt64 = int64_t;
		using Int64 = int64_t;

		using UInt128 = InvalidType;
		using SInt128 = InvalidType;
		using Int128 = InvalidType;

		// Float aliases
		using Float16 = InvalidType;
		using SFloat16 = InvalidType;
		using UFloat16 = InvalidType;

		using Float32 = Float;
		using SFloat32 = Float;
		using UFloat32 = InvalidType;

		using Float64 = Double;
		using SFloat64 = Double;
		using UFloat64 = InvalidType;

		using Float128 = InvalidType;
		using SFloat128 = InvalidType;
		using UFloat128 = InvalidType;

		namespace fast
		{
			using Int = Int;
			using Float = Float;
			using Double = Double;

			using UInt8 = UInt;
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

			using UInt8 = UInt;
			using S8 = SInt;
			using I8 = Int;

			using U16 = UInt;
			using S16 = SInt;
			using I16 = Int;

			using U32 = UInt;
			using S32 = SInt;
			using I32 = Int;

			using UInt64 = ULong;
			using S64 = SLong;
			using Int64 = Long;
		}

		// Characters
		using UniChar = fast::U32; // UTF-32 Code Point

		using UChar = unsigned char;
		using SChar = signed char;
//#define char char
#define CChar const char

#if defined(EVG_COMPILER_MSVC)
#define WChar wchar_t
#define CWChar const wchar_t
#else
#define WChar char16_t
#define CWChar const char16_t
#endif

		enum class Byte : char {};
		using Size = uint_least64_t;
		using Offset = int_least64_t;
		using PtrNum = uintptr_t;
		using PtrDiff = ptrdiff_t;
		using Hash = Size;
		using Pid = UInt64;


		using RefCounterNumber = std::atomic<Int>;
		using Mutex = std::mutex;
		using SharedMutex = std::shared_mutex;

		template <typename T>
		using Lock = std::scoped_lock<T>;

		template <typename T>
		using SharedLock = std::shared_lock<T>;


		using VoidFn = void(*)();
		void nop() {} // Corresponds to "nop" x86 instruction

		void nopDeleter(void*) {}

		template <typename T>
		using SPtr = std::shared_ptr<T>;

		template <typename T>
		using UPtr = std::unique_ptr<T>;

		using Time = std::chrono::high_resolution_clock::time_point; // Underlying type on MSVC is Int64
		using Clock = std::chrono::high_resolution_clock;
		using UtcTime = std::chrono::system_clock::time_point; // Unix time (std::chrono::utc_time is not done yet)
		using UtcClock = std::chrono::system_clock;

	};

	using namespace types;
};

using namespace evg::types;


#pragma once

#include <cstdint> //Fixed size numbers
#include <cmath>

#include <bx/math.h> //bx::Vec3

namespace bgl
{
	// Bring standard math functions into this namespace so that they cannot be overriden by vector operators
	using std::floor;
	using std::ceil;
	using std::round;

	// Class that cannot be used at compile time, indicates when a type is unavailable
	class InvalidType;

	// Platform defined integer aliases
	using UChar = unsigned char;
	using SChar = signed char;

	using UShort = unsigned short;
	using SShort = signed short;

	using UInt = unsigned int;
	using SInt = signed int;

	using ULong = unsigned long;
	using SLong = signed long;

	using ULongLong = unsigned long long;
	using SLongLong = signed long long;

	// Fixed width integer aliases
	using U8 = uint8_t;
	using S8 = int8_t;

	using U16 = uint16_t;
	using S16 = int16_t;

	using U32 = uint32_t;
	using S32 = int32_t;
	
	using U64 = uint64_t;
	using S64 = int64_t;

	using F16 = InvalidType;
	using F32 = float;
	using F64 = double;

	using FloatFast = float;

	using unicodeChar = uint_fast32_t;

	template <typename T>
	struct pair3
	{
		T x, y, z;

		inline void operator= (const pair3& a) { x = a.x; y = a.y; z = a.z; }

		inline void operator+= (const pair3& a) { x += a.x; y += a.y; z += a.z; }
		inline void operator-= (const pair3& a) { x -= a.x; y -= a.y; z -= a.z; }
		inline void operator*= (const pair3& a) { x *= a.x; y *= a.y; z *= a.z; }
		inline void operator/= (const pair3& a) { x /= a.x; y /= a.y; z /= a.z; }

		inline void operator+= (const T& a) { x += a; y += a; z += a; }
		inline void operator-= (const T& a) { x -= a; y -= a; z -= a; }
		inline void operator*= (const T& a) { x *= a; y *= a; z *= a; }
		inline void operator/= (const T& a) { x /= a; y /= a; z /= a; }

		inline void zero() { x = 0; y = 0; z = 0; }
		inline void inverse() { x = -x; y = -y; z = -z; }

		inline pair3 operator+(const pair3& a) const
		{
			return pair3{ x + a.x, y + a.y, z + a.z };
		}
		inline pair3 operator-(const pair3& a) const
		{
			return pair3{ x - a.x, y - a.y, z - a.z };
		}
		inline pair3 operator*(const pair3& a) const
		{
			return pair3{ x * a.x, y * a.y, z * a.z };
		}
		inline pair3 operator/(const pair3& a) const
		{
			return pair3{ x / a.x, y / a.y, z / a.z };
		}

		inline pair3 operator+(const T& a) const
		{
			return pair3{ x + a, y + a, z + a };
		}
		inline pair3 operator-(const T& a) const
		{
			return pair3{ x - a, y - a, z - a };
		}
		inline pair3 operator*(const T& a) const
		{
			return pair3{ x * a, y * a, z * a };
		}
		inline pair3 operator/(const T& a) const
		{
			return pair3{ x / a, y / a, z / a };
		}

		inline bool operator==(const pair3& a) const
		{
			return (x == a.x && y == a.y && z == a.z);
		}
		inline bool operator!=(const pair3& a) const
		{
			return (x != a.x || y != a.y || z != a.z);
		}

		inline pair3() = default;
		inline pair3(const bx::Vec3& a) : x(a.x), y(a.y), z(a.z) { }
		inline pair3(const T& _x, const T& _y, const T& _z) : x(_x), y(_y), z(_z) { }
		explicit inline pair3(const T& _val) : x(_val), y(_val), z(_val) { }

		inline operator bx::Vec3() const { return { x, y, z }; }
		inline operator std::string() const { return std::to_string(x) + "x " + std::to_string(y) + "y " + std::to_string(z) + "z "; }
	};

	using ipos = pair3<int>;
	using fpos = pair3<float>;

	ipos floor(const fpos& a)
	{
		
		return { (int)floor(a.x), (int)floor(a.y), (int)floor(a.z) };
	}

	ipos ceil(const fpos& a)
	{
		return { (int)ceilf(a.x), (int)ceilf(a.y), (int)ceilf(a.z) };
	}

	ipos round(const fpos& a)
	{
		return { (int)roundf(a.x), (int)roundf(a.y), (int)roundf(a.z) };
	}

	template <typename T>
	struct pair2
	{
		T x, y;

		inline void operator= (const pair2& a) { x = a.x; y = a.y; }

		inline void operator+= (const pair2& a) { x += a.x; y += a.y; }
		inline void operator-= (const pair2& a) { x -= a.x; y -= a.y; }
		inline void operator*= (const pair2& a) { x *= a.x; y *= a.y; }
		inline void operator/= (const pair2& a) { x /= a.x; y /= a.y; }

		inline void operator+= (const T& a) { x += a; y += a; }
		inline void operator-= (const T& a) { x -= a; y -= a; }
		inline void operator*= (const T& a) { x *= a; y *= a; }
		inline void operator/= (const T& a) { x /= a; y /= a; }

		inline void zero() { x = 0; y = 0; }
		inline void inverse() { x = -x; y = -y; }

		inline pair2 operator+(const pair2& a) const
		{
			return pair2{ x + a.x, y + a.y };
		}
		inline pair2 operator-(const pair2& a) const
		{
			return pair2{ x - a.x, y - a.y };
		}
		inline pair2 operator*(const pair2& a) const
		{
			return pair2{ x * a.x, y * a.y };
		}
		inline pair2 operator/(const pair2& a) const
		{
			return pair2{ x / a.x, y / a.y };
		}

		inline pair2 operator+(const T& a) const
		{
			return pair2{ x + a, y + a };
		}
		inline pair2 operator-(const T& a) const
		{
			return pair2{ x - a, y - a };
		}
		inline pair2 operator*(const T& a) const
		{
			return pair2{ x * a, y * a };
		}
		inline pair2 operator/(const T& a) const
		{
			return pair2{ x / a, y / a };
		}

		inline bool operator==(const pair2& a) const
		{
			return ((x == a.x) && (y == a.y));
		}
		inline bool operator!=(const pair2& a) const
		{
			return ((x != a.x) || (y != a.y));
		}

		inline pair2() = default;
		inline pair2(const T& _x, const T& _y) : x(_x), y(_y) { }
		explicit inline pair2(const T& _val) : x(_val), y(_val) { }

		inline operator std::string() const { return std::to_string(x) + "x " + std::to_string(y) + "y"; }
	};

	using ipos2 = pair2<int>;
	using fpos2 = pair2<float>;

	using angle3 = fpos;
	using angle2 = fpos2;

	ipos2 floor(fpos2 a)
	{
		return { (int)floorf(a.x), (int)floorf(a.y) };
	}

	ipos2 ceil(fpos2 a)
	{
		return { (int)ceilf(a.x), (int)ceilf(a.y) };
	}

	ipos2 round(fpos2 a)
	{
		return { (int)roundf(a.x), (int)roundf(a.y) };
	}

	// Hash that is designed for clusters of points
	// http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
	struct iposClusterHash
	{
		std::size_t operator()(const ipos& pos) const
		{
			return (pos.x * 88339) ^ (pos.z * 91967) ^ (pos.z * 126323);
		}
	};

	// Class that compares one ipos to another by their distance from a center point
	// todo: vectorize
	struct iposCenterCompare
	{
		const ipos c; // Center point

		iposCenterCompare(const ipos& _center) : c(_center) { }

		bool operator()(const ipos& lhs, const ipos& rhs) const
		{
			const double first = sqrt(pow(c.x - lhs.x, 2) + pow(c.y - lhs.y, 2) + pow(c.z - lhs.z, 2));
			const double second = sqrt(pow(c.x - rhs.x, 2) + pow(c.y - rhs.y, 2) + pow(c.z - rhs.z, 2));
			return first < second;
		}
	};

	// Texture Position, all numbers are from 0 to 1
	// todo: convert to use 11 bit integers, figure out what edges these actually correspond to
	struct tatlaspos3
	{
		fpos2 left = { 0.0f, 0.0f }; // Bottom left
		fpos2 right = { 1.0f, 1.0f }; // Top right
		float layer = 0.0f; // W position

		inline bool operator==(const tatlaspos3& a) const
		{
			return ((left == a.left) && (right == a.right) && (layer == a.layer));
		}
		inline bool operator!=(const tatlaspos3& a) const
		{
			return ((left != a.left) || (right != a.right) || (layer != a.layer));
		}
	};

}
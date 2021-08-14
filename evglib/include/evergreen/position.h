#ifndef EVG_POSITION_H
#define EVG_POSITION_H
namespace evg
{

	// POD Type representing a vector of a 3d position
	template<typename T>
	struct Pos3
	{
		using This = Pos3<T>;
		using TemplateType = T;

		T x;
		T y;
		T z;

		constexpr Pos3& operator++ () noexcept // Prefix increment operator
		{
			++x;
			++y;
			++z;

			return *this;
		}
		constexpr Pos3& operator++ (int) noexcept // Postfix increment operator
		{
			Pos3 oldVal = *this;

			++x;
			++y;
			++z;

			return oldVal;
		}
		constexpr Pos3& operator-- () noexcept // Prefix decrement operator
		{
			--x;
			--y;
			--z;

			return *this;
		}
		constexpr Pos3& operator-- (int) noexcept // Postfix decrement operator
		{
			Pos3 oldVal = *this;

			--x;
			--y;
			--z;

			return oldVal;
		}

		explicit operator Tuple<T, T, T>() const noexcept
		{
			return { x, y, z };
		}

#ifdef EVG_LIB_BGFX
		bx::Vec3 bx() const noexcept
		{
			return { (float)x, (float)y, (float)z };
		}
		explicit operator bx::Vec3() const noexcept
		{
			return bx();
		}
#endif // defined(EVG_LIB_BGFX)

		// Multiplication, division, and modulus by a vector of the same width
		constexpr Pos3 operator* (const Pos3& lhs) const noexcept
		{
			return { x * lhs.x, y * lhs.y, z * lhs.z };
		}
		constexpr Pos3 operator/ (const Pos3& lhs) const noexcept
		{
			return { x / lhs.x, y / lhs.y, z / lhs.z };
		}
		constexpr Pos3 operator% (const Pos3& lhs) const noexcept
		{
			return { x / lhs.x, y / lhs.y, z / lhs.z };
		}

		// Multiplication, division, and modulus by a scalar
		constexpr Pos3 operator* (const T& lhs) const noexcept
		{
			return { x * lhs, y * lhs, z * lhs };
		}
		constexpr Pos3 operator/ (const T& lhs) const noexcept
		{
			return { x / lhs, y / lhs, z / lhs };
		}
		constexpr Pos3 operator% (const T& lhs) const noexcept
		{
			return { x % lhs, y % lhs, z % lhs };
		}

		// Addition and subtraction by a vector of the same width
		constexpr Pos3 operator+ (const Pos3& lhs) const noexcept
		{
			return { x + lhs.x, y + lhs.y, z + lhs.z };
		}
		constexpr Pos3 operator- (const Pos3& lhs) const noexcept
		{
			return { x - lhs.x, y - lhs.y, z - lhs.z };
		}

		// Addition and subtraction by a scalar
		constexpr Pos3 operator+ (const T& lhs) const noexcept
		{
			return { x + lhs, y + lhs, z + lhs };
		}
		constexpr Pos3 operator- (const T& lhs) const noexcept
		{
			return { x - lhs, y - lhs, z - lhs };
		}


	};

	using FPos3 = Pos3<Double>;
	using Angle = FPos3;
	using IPos3 = Pos3<Int>;


	void to_json(json& j, const IPos3& pos)
	{
		j = json{ pos.x, pos.y, pos.z };
	}
	void from_json(const json& j, IPos3& pos)
	{
		j.at(0).get_to(pos.x);
		j.at(1).get_to(pos.y);
		j.at(2).get_to(pos.z);
	}



	// POD Type representing a vector of a 2d position
	template<typename T>
	struct Pos2
	{
		using This = Pos2<T>;
		using TemplateType = T;

		T x;
		T y;

		constexpr Pos2& operator++ () noexcept // Prefix increment operator
		{
			++x;
			++y;

			return *this;
		}
		constexpr Pos2& operator++ (int) noexcept // Postfix increment operator
		{
			Pos2 oldVal = *this;

			++x;
			++y;

			return oldVal;
		}
		constexpr Pos2& operator-- () noexcept // Prefix decrement operator
		{
			--x;
			--y;

			return *this;
		}
		constexpr Pos2& operator-- (int) noexcept // Postfix decrement operator
		{
			Pos2 oldVal = *this;

			--x;
			--y;

			return oldVal;
		}

		explicit operator Tuple<T, T>() const noexcept
		{
			return { x, y };
		}

		// Multiplication, division, and modulus by a vector of the same width
		constexpr Pos2 operator* (const Pos2& lhs) const noexcept
		{
			return { x * lhs.x, y * lhs.y };
		}
		constexpr Pos2 operator/ (const Pos2& lhs) const noexcept
		{
			return { x / lhs.x, y / lhs.y };
		}
		constexpr Pos2 operator% (const Pos2& lhs) const noexcept
		{
			return { x / lhs.x, y / lhs.y };
		}

		// Multiplication, division, and modulus by a scalar
		constexpr Pos2 operator* (const T& lhs) const noexcept
		{
			return { x * lhs, y * lhs };
		}
		constexpr Pos2 operator/ (const T& lhs) const noexcept
		{
			return { x / lhs, y / lhs };
		}
		constexpr Pos2 operator% (const T& lhs) const noexcept
		{
			return { x % lhs, y % lhs };
		}

		// Addition and subtraction by a vector of the same width
		constexpr Pos2 operator+ (const Pos2& lhs) const noexcept
		{
			return { x + lhs.x, y + lhs.y };
		}
		constexpr Pos2 operator- (const Pos2& lhs) const noexcept
		{
			return { x - lhs.x, y - lhs.y };
		}

		// Addition and subtraction by a scalar
		constexpr Pos2 operator+ (const T& lhs) const noexcept
		{
			return { x + lhs, y + lhs };
		}
		constexpr Pos2 operator- (const T& lhs) const noexcept
		{
			return { x - lhs, y - lhs };
		}


	};

	using FPos2 = Pos2<Double>;
	using IPos2 = Pos2<Int>;



	void to_json(json& j, const IPos2& pos)
	{
		j = json{ pos.x, pos.y };
	}
	void from_json(const json& j, IPos2& pos)
	{
		j.at(0).get_to(pos.x);
		j.at(1).get_to(pos.y);
	}


	// Normalize number to between -1 and 1
	template <typename ValT, typename RangeT>
	double normalize(const ValT in, const RangeT min, const RangeT max)
	{
		return (2.0 * ((in - double(min)) / (double(max) - double(min)))) - 1;
	}

}; // namespace evg
#endif // #ifndef EVG_POSITION_H
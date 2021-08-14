#pragma once

namespace evg
{

	template<Size bufSize = 256>
	class WStringAdapterBase
	{
	public:
		Vector<wchar_t> u16_raw;
		StringBuilder u8_raw;
		bool valid = false; // Is opposing conversion available
		bool original = 0; // 0: u16_raw
						   // 1: u8_raw

		// Get string buffer for intake
		wchar_t* in_u16()
		{
			valid = false;
			original = 0;
			u16_raw.clear();
			u16_raw.resize(bufSize, 0);
			return u16_raw.data();
		}

		char* in_u8()
		{
			valid = false;
			original = 1;
			u8_raw.clear();
			u8_raw.resize(bufSize, 0);
			return u8_raw.data();
		}

		Vector<wchar_t>& get_u16()
		{
			if (original == 0)
			{
				return u16_raw;
			}

			if (!valid)
			{
				debugBreak();
			}

			return u16_raw;
		}

		StringBuilder& get_u8()
		{
			if (original == 1)
			{
				return u8_raw;
			}

			if (!valid)
			{
				u8_raw.clear();
				utf16ToUtf8(u16_raw, u8_raw);
			}

			return u8_raw;
		}

		// User-supplied input buffer
		template <typename InT>
		StringBuilder& get_u8(const InT& _u16)
		{
			u8_raw.clear();
			utf16ToUtf8(_u16, u8_raw);
			return u8_raw;
		}

		Int size_u16()
		{
			return (Int)bufSize;
		}

		Size size_u8()
		{
			return bufSize;
		}
	};

	using WStringAdapter = WStringAdapterBase<>;

}
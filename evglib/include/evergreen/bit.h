#pragma once

namespace evg
{
	template<typename T, typename PosT>
	void setBit(T& var, PosT pos, PosT val)
	{
		var ^= (-val ^ var) & (1UL << pos);
	}

	template<typename T, typename PosT>
	bool getBit(T& var, PosT pos)
	{
		return (var & (1 << pos)) >> pos;
	}

	template <typename T>
	void swapEndian(T& u)
	{
		union
		{
			T u;
			unsigned char u8[sizeof(T)];
		} source, dest;

		source.u = u;

		for (size_t k = 0; k < sizeof(T); k++)
			dest.u8[k] = source.u8[sizeof(T) - k - 1];

		u = dest.u;
	}


	std::pair<UInt32, UInt> codepointToUtf8(const UniChar cp)
	{
		std::pair<UInt32, UInt> ret = { 0, 1 };

		UInt8* p = (UInt8*)(&ret.first);

		if (cp <= 0x7F)
		{
			ret.second = 1;

			p[0] = (UInt8)cp;

			setBit(p[0], 7, 0);
		}
		else if (cp <= 0x07FF)
		{
			ret.second = 2;

			p[0] = UInt8(cp >> 6);
			p[1] = UInt8(cp);

			setBit(p[0], 7, 1);
			setBit(p[0], 6, 1);
			setBit(p[0], 5, 0);
			setBit(p[1], 7, 1);
			setBit(p[1], 6, 0);
		}
		else if (cp <= 0xFFFF)
		{
			ret.second = 3;

			p[0] = UInt8(cp >> 12);
			p[1] = UInt8(cp >> 6);
			p[2] = UInt8(cp);

			setBit(p[0], 7, 1);
			setBit(p[0], 6, 1);
			setBit(p[0], 5, 1);
			setBit(p[0], 4, 0);
			setBit(p[1], 7, 1);
			setBit(p[1], 6, 0);
			setBit(p[2], 7, 1);
			setBit(p[2], 6, 0);
		}
		else if (cp <= 0x10FFFF)
		{
			ret.second = 4;

			p[0] = UInt8(cp >> 18);
			p[1] = UInt8(cp >> 12);
			p[2] = UInt8(cp >> 6);
			p[3] = UInt8(cp);

			setBit(p[0], 7, 1);
			setBit(p[0], 6, 1);
			setBit(p[0], 5, 1);
			setBit(p[0], 4, 1);
			setBit(p[0], 3, 0);
			setBit(p[1], 7, 1);
			setBit(p[1], 6, 0);
			setBit(p[2], 7, 1);
			setBit(p[2], 6, 0);
			setBit(p[3], 7, 1);
			setBit(p[3], 6, 0);
		}

		return ret;
	}

	std::pair<UInt32, UInt> codepointToUtf16(UniChar cp)
	{
		std::pair<UInt32, UInt> ret = { 0, 1 };

		UInt16* p = (UInt16*)(&ret.first);

		if ((cp <= 0xD7FF) || (cp >= 0xE000 && cp <= 0xFFFF))
		{
			ret.first = (UInt16)cp;
		}
		else
		{
			ret.second = 2;

			cp -= 0x10000;
			p[0] = ((UInt16)cp >> 10) + 0xD800;
			p[1] = ((UInt16)cp & 0b1111111111) + 0xD800;
		}

		return ret;
	}


	Tuple<UniChar, UInt> utf16ToCodepoint(const WChar* const c, const UInt32 maxSize)
	{
		Tuple<UniChar, UInt> ret = { 0x20, 1 };

		if ((*c <= 0xD800 || *c >= 0xDBFF) && maxSize < 2) // Malformed
		{
			return { 0x20, 1 };
		}


		UInt c1 = c[0];
		if (c1 >= 0xd800 && c1 < 0xdc00) {
			UInt c2 = c[1];
			ret.first = ((c1 & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
		}
		ret.first = c1;







		/*UInt32 codepoint = *(UInt32*)(c);
		if (*c <= 0xD800 || *c >= 0xDBFF)
			codepoint &= ~0xFFFF0000; // Zero out top 16 bits

		constexpr UInt32 LEAD_OFFSET = 0xD800 - (0x10000 >> 10);
		constexpr UInt32 SURROGATE_OFFSET = 0x10000UL - (0xD800UL << 10) - 0xDC00UL;

		UInt32 lead = LEAD_OFFSET + (codepoint >> 10);
		UInt32 trail = 0xDC00 + (codepoint & 0x3FF);

		ret.first = (lead << 10) + trail + SURROGATE_OFFSET;
		*/
		if (!(*c <= 0xD800 || *c >= 0xDBFF))
		{
			ret.second = 2;
		}

		return ret;
	}

	UInt32 utf8Len(const char* const c)
	{
		if (!(*c & 0b10000000))
			return 1;
		else if ((*c & 0b11110000) == 0b11110000)
			return 4;
		else if ((*c & 0b11100000) == 0b11100000)
			return 3;
		else if ((*c & 0b11000000) == 0b11000000)
			return 2;
		else
			return 1;
	}

	Tuple<UniChar, UInt> utf8ToCodepoint(const char* const c, const UInt32 maxSize)
	{
		Tuple<UniChar, UInt> ret = { 0, 1 };

		ret.second = utf8Len(c);

		if (ret.second == 1)
		{
			return { *((const unsigned char*)c), 1 };
		}
		else if ((ret.second == 2) && (maxSize >= 2))
		{
			ret.first |= (c[1] & 0b00111111) << 6;
			ret.first |= (c[0] & 0b00011111);
		}
		else if ((ret.second == 3) && (maxSize >= 3))
		{
			ret.first |= (c[0] & 0b00001111) << 12;
			ret.first |= (c[1] & 0b00111111) << 6;
			ret.first |= (c[2] & 0b00111111);
		}
		else if ((ret.second == 4) && (maxSize >= 4))
		{
			ret.first |= ((UInt32)c[3] & 0b00000111) << 18;
			ret.first |= ((UInt32)c[2] & 0b00111111) << 12;
			ret.first |= ((UInt32)c[1] & 0b00111111) << 6;
			ret.first |= ((UInt32)c[0] & 0b00111111);
		}

		return ret;
	}




	template<typename InT, typename OutT>
	void utf16ToUtf8(InT&& in, OutT&& out)
	{
		assert(in.size() < UINT32_MAX);

		// Algorithm not based on UTF-16 bit distribution table, code sourced from https://unicode.org/faq/utf_bom.html

		for (UInt i = 0; i < in.size();)
		{
			auto cp = utf16ToCodepoint(in.data() + i, (UInt32)(in.size() - i + 1));
			auto utf8 = codepointToUtf8(cp.first);

			if ((utf8.second >= 1) && (cp.first != '\0'))
			{
				out.push_back(*((char*)(&utf8.first) + 0));
			}
			if (utf8.second >= 2)
			{
				out.push_back(*((char*)(&utf8.first) + 1));
			}
			if (utf8.second >= 3)
			{
				out.push_back(*((char*)(&utf8.first) + 2));
			}
			if (utf8.second >= 4)
			{
				out.push_back(*((char*)(&utf8.first) + 3));
			}


			i += cp.second;
		}
		*out.end() = '\0';
	}

	template <typename InT, typename OutT>
	void utf8ToUtf16(InT&& in, OutT&& out)
	{
		assert(in.size() < UINT32_MAX);

		for (UInt i = 0; i < in.size();)
		{
			auto cp = utf8ToCodepoint(in.data() + i, (UInt32)(in.size() - i + 1));
			auto utf16 = codepointToUtf16(cp.first);

			if (utf16.second >= 1)
			{
				out.push_back(*((UInt16*)(&utf16.first) + 0));
			}
			if (utf16.second >= 2)
			{
				out.push_back(*((UInt16*)(&utf16.first) + 1));
			}
			/*if (utf16.second >= 3)
			{
				out.push_back(*((char*)(&utf16.first) + 2));
			}
			if (utf16.second >= 4)
			{
				out.push_back(*((char*)(&utf16.first) + 3));
			}*/

			i += cp.second;
		}
		*out.end() = '\0';
	}



}
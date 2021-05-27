#pragma once

template<typename T, typename PosT>
void setBit(T& var, PosT pos, PosT val)
{
	var ^= (-val ^ var) & (1UL << pos);
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


std::pair<UInt32, UInt> CodepointToUtf8(const UniChar cp)
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

std::pair<UniChar, UInt> Utf16ToCodepoint(const wchar_t* const c, const UInt32 maxSize)
{
	std::pair<UniChar, UInt> ret = { 0, 1 };

	if ((*c <= 0xD800 || *c >= 0xDBFF) && maxSize < 2) // Malformed
	{
		return { 0x20, 1 };
	}

	UInt32 codepoint = *(UInt32*)(c);
	if (*c <= 0xD800 || *c >= 0xDBFF)
		codepoint &= ~0xFFFF0000; // Zero out top 16 bits

	constexpr UInt32 LEAD_OFFSET = 0xD800 - (0x10000 >> 10);
	constexpr UInt32 SURROGATE_OFFSET = 0x10000UL - (0xD800UL << 10) - 0xDC00UL;

	UInt32 lead = LEAD_OFFSET + (codepoint >> 10);
	UInt32 trail = 0xDC00 + (codepoint & 0x3FF);

	ret.first = (lead << 10) + trail + SURROGATE_OFFSET;

	return ret;
}




template<typename InT, typename OutT>
void utf16ToUtf8(InT&& in, OutT&& out)
{
	assert(in.size() < UINT32_MAX);

	// Algorithm not based on UTF-16 bit distribution table, code sourced from https://unicode.org/faq/utf_bom.html

	for (Int i = 0; i < in.size();)
	{
		auto cp = Utf16ToCodepoint(in.data().begin_raw + i, (UInt32)(in.size() - i));
		auto utf8 = CodepointToUtf8(cp.first);

		if ((utf8.second >= 1) && (cp.first != '\0'))
		{
			out.push_back(*((Char*)(&utf8.first) + 0));
		}
		if (utf8.second >= 2)
		{
			out.push_back(*((Char*)(&utf8.first) + 1));
		}
		if (utf8.second >= 3)
		{
			out.push_back(*((Char*)(&utf8.first) + 2));
		}
		if (utf8.second >= 4)
		{
			out.push_back(*((Char*)(&utf8.first) + 3));
		}


		i += cp.second;
	}

	out.push_back('\0');
}



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
	LPWSTR in_u16()
	{
		valid = false;
		original = 0;
		u16_raw.resize(bufSize, 0);
		return (wchar_t*)u16_raw.data().begin_raw;
	}

	Char* in_u8()
	{
		valid = false;
		original = 1;
		u8_raw.resize(bufSize, 0);
		return u8_raw.data().begin_raw;
	}

	Vector<wchar_t>& get_u16()
	{
		if (original == 0)
		{
			return u16_raw;
		}

		if (!valid)
		{

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
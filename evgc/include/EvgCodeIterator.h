#pragma once

#include "evglib.h"

using namespace evg;

class EvgCodeIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = UnicodeChar;
	using difference_type = std::ptrdiff_t;
	using pointer = Char*;
	using reference = Char&;

	Char* ptr;

	//operator Char* () { return point; }
	//Char& operator* () { return *ptr; }

	//bool operator== (const EvgCodeIterator& rhs) const { return ptr == rhs.ptr; }
	//bool operator!= (const EvgCodeIterator& rhs) const { return !(*this == rhs); }

	EvgCodeIterator() = default;
	EvgCodeIterator(Char* const _ptr) : ptr(_ptr) {}

	EvgCodeIterator& operator++ () { ptr += size(); return *this; }
	EvgCodeIterator operator++ (int) { EvgCodeIterator ret = *this; ptr += size(); return ret; }

	EvgCodeIterator& operator-- ()
	{
		--ptr;

		if (!(*ptr & 0b10000000)) // If top bit is clear, must be an ASCII char
			return *this;

		do
			--ptr;
		while (*ptr & 0b10000000 && !(*ptr & 0b01000000)); // If top bit is set but second from top is clear, must be an extension to a character

		return *this;
	}
	EvgCodeIterator operator-- (int) { EvgCodeIterator ret = *this; --(*this); return ret; }

	bool operator==(EvgCodeIterator lhs) const { return ptr == lhs.ptr; }
	bool operator!=(EvgCodeIterator lhs) const { return !(*this == lhs); }

	UnicodeChar operator*() { return get(); }

	UInt size()
	{
		if (!(*ptr & 0b10000000)) // If top bit is clear, ascii is used
			return 1;
		else if (*ptr & 0b11110000)
			return 4;
		else if (*ptr & 0b11100000)
			return 3;
		else if (*ptr & 0b11000000)
			return 2;
		else
			return 0;
	}

	UnicodeChar get()
	{
		UnicodeChar ret = '\0';

		Size len = size();

		switch (size())
		{
		case 1: return *ptr;
		case 2: return *(U16*)ptr;
		case 3: std::copy(ptr, ptr + 3, &ret); break;
		case 4: return *(UnicodeChar*)ptr;
		default: return std::numeric_limits<UnicodeChar>::max();
		}

		return ret;
	}

	UnicodeChar next()
	{
		EvgCodeIterator lookahead = this->ptr;
		++lookahead;
		return *lookahead;
	}

	bool nextIs(const Char* const end, std::string_view compare)
	{
		for (Size i = 0; (i < compare.size()) && (i < end - ptr); ++i)
		{
			if (compare[i] != ptr[i])
				return false;
		}

		return true;
	}

	bool itrUntil(const Char* const end, std::string_view compare)
	{
		for (; ptr < end; ++ptr)
		{
			if (nextIs(end, compare))
			{
				ptr += compare.size() - 1;
				return true;
			}
		}

		return false;
	}

	bool nextNonNum(const Char* const end)
	{
		for (; ptr < end; ++ptr)
		{
			if (!isdigit(*ptr))
			{
				return true;
			}
		}

		return false;
	}
};
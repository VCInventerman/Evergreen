#pragma once

#include "evglib.h"

using namespace evg;

enum class LineEnding : UInt
{
	unknown,
	n, // Unix
	rn, // DOS
	r, // Old MacOS
};

class EvgCodeIterator
{
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = UnicodeChar;
	using difference_type = std::ptrdiff_t;
	using pointer = char*;
	using reference = char&;


	ContiguousBufPtrEnd<char> code; // Begin and end of code file
	char* ptr; // Current point in code


	EvgCodeIterator() = default;
	EvgCodeIterator(const ContiguousBufPtrEnd<char> _code, char* const _ptr) : code(_code), ptr(_ptr) {}

	EvgCodeIterator& operator++ ()
	{ 
		UInt csize = size();
		ptr += csize > code.end() - ptr ? 0 : csize; 
		return *this;
	}
	EvgCodeIterator operator++ (int) { EvgCodeIterator ret = *this; ++(*this); return ret; }

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
		EvgCodeIterator lookahead(*this);
		++lookahead;
		return *lookahead;
	}

	void skipNewline()
	{
		UnicodeChar c = **this;
		if (c == '\n')
		{
			++*this;
		}
		else if (c == '\r')
		{
			if (next() == '\n')
			{
				++*this;
				++*this;
			}
			else
			{
				++*this;
			}
		}
	}

	bool atBack()
	{
		return ptr == code.end_raw - 1;
	}

	// Test if a character is present inside this parenthesis level. Skips all further text in between ()
	// Assumes that ptr is currently on a left parenthesis
	bool hasCharInSameLevel(const UnicodeChar target)
	{
		Int depth = 0;

		for (EvgCodeIterator i(*this); i.ptr != code.end_raw; ++i)
		{
			UnicodeChar c = *i;
			
			if (c == '(')
			{
				++depth;
			}
			else if (c == ')')
			{
				--depth;
			}
			if (c == target && depth == 0)
				return true;

		}

		return false;
	}
};
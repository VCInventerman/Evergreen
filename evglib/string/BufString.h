#pragma once

#include "../common/types.h"
#include "../common/alloc.h"
#include "../common/ContiguousBuf.h"

template <typename CharT = Char, typename AllocatorT = CLibAllocator>
class BufString
{
public:
	ContiguousBufPtrEnd<Char> data;

	//stack_only Char smallString[stackAlloc];
};

class StringView
{
public:
	Char* begin;
	Char* end; // Pointer to one past last element
};
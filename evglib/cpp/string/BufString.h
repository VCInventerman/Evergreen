#pragma once

template<Size stackAlloc = 16>
class BufString
{
public:
	Char* begin;
	Char* end;

	stack_only Char smallString[stackAlloc];
};

class StringView
{
public:
	Char* begin;
	Char* end; // Pointer to one past last element
};
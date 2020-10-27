#include "../common/types.h"

template<Size stackAlloc = 16>
class StringBuf
{
public:
	Char* begin;
	Char* end;

	stackOnly Char smallString[stackAlloc];
};

class StringView
{
public:
	Char* begin;
	Char* end; // Pointer to one past last element
};
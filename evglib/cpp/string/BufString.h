template<Size stackAlloc = 16>
class StringBuf
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
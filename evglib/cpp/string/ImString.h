#include <atomic>

// Immutable string

class ImStringElem
{
public:
	Hash hash;

	std::atomic<Size> refs;

	Char* end;

	Char data[];
};

template <typename RetT, typename ErrorT = CString>
class RetPair
{
	RetT val;
	ErrorT err;
};

class Allocator
{

};

class MallocAllocator : Allocator
{
	// Requires placement new
	void* alloc(Size size)
	{
		return malloc(size);
	}

	// Manually destruct before
	void dealloc(void* target)
	{
		free(target);
	}
};

template <typename _CharT = Char, typename _Allocator = std::allocator>
class ImStringManager
{
public:
	static ImStringManager defaultCache;

	std::map<Hash, ImStringElem> strings;

};

template <typename T>
class Iterator
{
public:
	void next();

};

template <>
class Iterator <ImString>
{
public:
	ImString::

};

template <typename CharT = Char>
class ImString
{
	ImStringElem* source;

	class Iterator
	{
		CharT* elem;
		Char* end;
	};

	CharT& operator[] (Size index)
	{
		return source->data[index];
	}

	CharT& front()
	{
		return source->data[0];
	}
	CharT& back()
	{
		return *source->end;
	}

	Char* data()
	{
		return source->data;
	}


	CharT* begin()
	{
		return source->data;
	}
	CharT* end()
	{
		return source->end;
	}
};

class ImStringSlice
{
	ImStringElem* source;
	Char* begin;
	Char* end;
};
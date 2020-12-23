#pragma once

#include "../string/String.h"

class Error
{
public:
	//String msg;
	const char* const msg;

	Error() : msg("") {}
	Error(Int simpleVal) : msg(simpleVal ? "Unspecified error" : "") {}
	Error(const char* const _msg) : msg(_msg) {}
};

template <typename RetT, typename ErrorT = Error>
class RetPair
{
public:
	RetT val;
	ErrorT err;

	RetPair() = default;
	RetPair(RetT&& _val) : val(_val), err() {}
};

class Allocator
{
public:
	void* alloc(Size size);
	Error dealloc(void* target);
};

class CLibAllocator : Allocator
{
public:
	static constexpr void* GenericMallocError = nullptr;

	RetPair<void*> alloc(Size size)
	{
		RetPair<void*> ret(malloc(size));

		if (ret.val == GenericMallocError)
		{
			//ret.err = Error("Generic Malloc Error");
			return ret;
		}
	}

	// Manually destruct before
	Error dealloc(void* target)
	{
		free(target);
		return {};
	}
};
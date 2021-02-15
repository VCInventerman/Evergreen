#pragma once

#include "evergreen/string/StringView.h"

namespace evg
{

	class Error
	{
	public:
		StringViewHash& msg;
		SPtr<Byte[]> detailsStore;

		virtual StringViewHash& details() { return "No further details."; }

		Error() : msg("") {}
		Error(Int simpleVal) : msg(simpleVal ? "Unspecified error" : "") {}
		Error(const char* const _msg) : msg(_msg) {}
	};



	// ErrorStack is a thread_local vector of Error. Each Error is either the base class or a derived class that can interpret the contents of the details stored on the heap.
	class ErrorStack
	{
	public:

	};

	//thread_local ErrorStack

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
			return ret;
		}

		// Manually destruct before
		Error dealloc(void* target)
		{
			free(target);
			return {};
		}
	};

}
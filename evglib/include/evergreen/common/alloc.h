#pragma once

#include "evergreen/string/StringView.h"

namespace evg
{
	// An error consists of an array of bytes that are different with every error and a pointer to a pair of two functions: a message and associated details
	// The first function is general and cannot use the value of detailStore, while the second can get more specific
	//todo: make allocation more efficient
	class Error
	{
	public:
		using InterpretFuncs = Tuple<StringViewHash(*)(), String(*)(UPtr<Byte[]>& detailStore)>;

		InterpretFuncs* funcs;
		UPtr<Byte[]> detailStore;

		StringViewHash msg() { return funcs ? funcs->first() : "No message"; }
		String details() { return funcs ? funcs->second(detailStore) : (String)"No further details"; }

		Error() : funcs(), detailStore(nullptr) {} // Leave detailStore as a nullptr and use no message
		Error(const Error& _err) : funcs(_err.funcs), detailStore(&_err.detailStore[0]) {}
	};



	// ErrorStack is a thread_local vector of Error that allows them to stack
	class ErrorStack
	{
	public:

	};

	//thread_local ErrorStack

	template <typename ValueT, typename ErrorT = Error>
	class Ret
	{
	public:
		ValueT val;
		ErrorT err;

		Ret() = default;
		Ret(ValueT _val) : val(_val), err() {}
		Ret(ValueT _val, ErrorT _err) : val(_val), err(_err) {}
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

		Ret<void*> alloc(Size size)
		{
			Ret<void*> ret(malloc(size));

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
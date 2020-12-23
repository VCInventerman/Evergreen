#pragma once

#include "../common/types.h"
#include "../common/alloc.h"
#include "../common/ContiguousBuf.h"

// Immutable string
// Stores strings in a map of hash values to null-terminated (until bootstrap for c compat) strings syncronized by a shared mutex
// Strings are reference counted and remove themselves from map when atomic reference count reaches zero unless they are put in at compile time and unremovable (change this?)

// buffers in the standard library are stored as a pointer to one past the last element 
// end comes first in storage (not in argument) because of possibility for buffer to exist at end of class
// end is logically less useful and technically not required (just like size) so it comes second

//todo: ImStringElem created in same allocation as string in manner similar to std::shared_pointer::make_shared (using virtual function or flag of owning mem)
//todo: Omit static map if requested
//todo: sync(shared) for map that integrates shared mutex
//todo: pair with named elements
//todo: noerr keyword controlling whether error pointer is contained in return
//todo: if returning class such as error and value, allow editing individual values during function: "ret.err = "Malloc"
//todo: allow use of ImString directly by getting rid of extra <>

template <typename CharT = Char, typename AllocatorT = CLibAllocator>
class ImStringBase
{
public:

	// Values that are kept in map, point to a string
	//todo: custom map implementation that lets Elem contain a hash using offset pointers
	//todo: allow calling constructor manually
	class Elem
	{
	public:
		std::atomic<Size> refs;
		bool canBeRemoved;

		ContiguousBufPtrEnd<Char> data;
		Hash hash;

		Elem(const Hash _hash, const Char* const _data, const Size _size) : hash(_hash)
		{
			refs = 0;
			canBeRemoved = false;

			data = ContiguousBufPtrEnd<Char>(const_cast<Char*>(_data), _size);
		}
		Elem(const Hash _hash, const Char* const _data, const Char* const _end) : hash(_hash)
		{
			refs = 0;
			canBeRemoved = false;
	
			data = ContiguousBufPtrEnd<Char>(_data, _end);
		}

		void addRef()
		{
			++refs;
		}
		void subRef()
		{
			--refs;
		}
	};

	class Manager
	{
	public:
		static Manager defaultManager;

		std::map<Hash, Elem> strings;
		std::shared_mutex m_strings;

		Elem* nullString;

		// Get hash and length of string from a null-terminated CString
		// djb2 by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
		static std::pair<Hash, Size> getHashLenCString(const Char* const str)
		{
			Hash hash = 5381;
			Int c;

			const Char* i = str;
			while (c = *i++)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return { hash, i - str };
		}

		Manager()
		{
			nullString = &strings.emplace(std::piecewise_construct, std::forward_as_tuple(getHashLenCString((const Char*)"").first), std::forward_as_tuple(getHashLenCString((const Char*)"").first, (const Char*)"", 1)).first->second;
		}

		// Requires a pointer that will not be freed during the lifetime of the element
		Elem* emplace(const Char* const _str)
		{
			anyvar info = getHashLenCString(_str);

			m_strings.lock_shared();
			auto&& exists = strings.find(info.first);
			m_strings.unlock_shared();

			if (exists == strings.cend())
			{
				std::lock_guard<std::shared_mutex> lock(m_strings);
				return &strings.emplace(std::piecewise_construct, std::forward_as_tuple(info.first), std::forward_as_tuple(info.first, _str, info.second)).first->second;
			}
			else
			{
				exists->second.addRef();
				return &exists->second;
			}
		}

		// Copies the supplied string
		Elem* insert(const Char* const _str)
		{

		}
	};


	Elem* source;

	static constexpr auto npos = static_cast<Size>(-1);

	ImStringBase() = default;
	ImStringBase(Elem* const _source) : source(_source) {}
	ImStringBase(const Char* const str)
	{
		source = Manager::defaultManager.emplace(str);
	}



	CharT& operator[] (Size index)
	{
		return source->data[index];
	}

	Size size()
	{
		return source->data.size();
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
		return &*source->data.begin();
	}
	CharT* end()
	{
		return &*source->data.end();
	}

	//todo: template variant by reference or integer return value
	CharT* find(const Char letter)
	{
		for (auto&& i : *this)
		{
			if (*i == letter)
			{
				return &*i;
			}
		}

		return nullptr;
	}

	CharT* rfind(const Char letter)
	{
		for (ContiguousBufPtrEnd<Char>::RevIterator i = source->data.rbegin(); i != source->data.rend(); ++i)
		//for (auto&& i : this->source->data.rbegin())
		{
			if (*i == letter)
			{
				return &*i;
			}
		}

		return nullptr;
	}

	operator char* ()
	{
		return (char*)&*source->data.begin();
	}
	operator Char* ()
	{
		return (Char*)&*source->data.begin();
	}

	const bool operator< (const ImStringBase<CharT, AllocatorT>& rhs) const { return source->hash < rhs.source->hash; }
};

ImStringBase<>::Manager ImStringBase<>::Manager::defaultManager;
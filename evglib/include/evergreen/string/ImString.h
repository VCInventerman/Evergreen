#pragma once

#include "evergreen/common/types.h"
//#include "evergreen/common/alloc.h"
#include "evergreen/string/StringView.h"

// Immutable string
// Stores strings in a map of hash values to null-terminated (until bootstrap for c compat) strings syncronized by a shared mutex
// Strings are reference counted and remove themselves from map when atomic reference count reaches zero unless they are put in at compile time and unremovable (change this?)

// buffers in the standard library are stored as a pointer to the first and one past the last element 
// end comes first in storage (not in argument) because of possibility for buffer to exist at end of class
// end is logically less useful and technically not required (just like size) so it comes second

//todo: ImStringElem created in same allocation as string in manner similar to std::shared_pointer::make_shared (using virtual function or flag of owning mem)
//todo: Omit static map if requested
//todo: sync(shared) for map that integrates shared mutex
//todo: pair with named elements
//todo: noerr keyword controlling whether error pointer is contained in return
//todo: if returning class such as error and value, allow editing individual values during function: "ret.err = "Malloc"
//todo: allow use of ImString directly by getting rid of extra <>

namespace evg
{
	template <typename CharT = CChar> //, typename AllocatorT = CLibAllocator>
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

			StringViewHash string;

			Elem(const Hash _hash, CharT* const _data, const Size _size) : string(_data, _hash)
			{
				refs = 0;
				canBeRemoved = false;
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
			static std::pair<Hash, Size> getHashLenCString(CharT* const str)
			{
				Hash hash = 5381;
				Int c;

				CharT* i = str;
				while ((c = *i++))
					hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

				return { hash, i - str };
			}

			Manager()
			{
				nullString = &strings.emplace(std::piecewise_construct, std::forward_as_tuple(getHashLenCString((CChar*)"").first), std::forward_as_tuple(getHashLenCString((CChar*)"").first, (CChar*)"", 1)).first->second;
			}

			// Requires a pointer that will not be freed during the lifetime of the element
			Elem* emplace(CharT* const _str)
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
			Elem* insert(CharT* const _str)
			{

			}
		};


		Elem* source;

		static constexpr auto npos = static_cast<Size>(-1);

		ImStringBase() = default;
		ImStringBase(Elem* const _source) : source(_source) {}
		ImStringBase(CharT* const str)
		{
			source = Manager::defaultManager.emplace(str);
		}



		CharT& operator[] (Size index)
		{
			return source->string.data[index];
		}

		Size size()
		{
			return source->string.data.size();
		}

		CharT& front()
		{
			return source->string.data[0];
		}
		CharT& back()
		{
			return *source->string.end;
		}

		CharT* data()
		{
			return source->string.data;
		}


		CharT* begin()
		{
			return &*source->string.data.begin();
		}
		CharT* end()
		{
			return &*source->string.data.end();
		}

		//todo: template variant by reference or integer return value
		CharT* find(CharT letter)
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

		CharT* rfind(CharT letter)
		{
			for (auto i = source->string.data.rbegin(); i != source->string.data.rend(); ++i)
				//for (auto&& i : this->source->data.rbegin())
			{
				if (*i == letter)
				{
					return &*i;
				}
			}

			return nullptr;
		}

		template<typename T>
		T operator_conv() const
		{
			return source->string.data;
		}

		EVG_CXX_CAST(CharT*)

		const bool operator< (const ImStringBase<CharT>& rhs) const { return source->string.hash < rhs.source->string.hash; }
	};

	template <>
	ImStringBase<>::Manager ImStringBase<>::Manager::defaultManager = {};
}
#pragma once

#include "evergreen/types.h"
//#include "evergreen/common/alloc.h"
#include "evergreen/StringView.h"
#include <evergreen/Vector.h>
#include <evergreen/bit.h>
#include <evergreen/StringBuilder.h>

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
//todo: allow use of ImStringBase directly by getting rid of extra <>

namespace evg
{
	template<typename CharT = Char, typename AllocatorT = std::allocator<CharT>>
	class ImStringBase
	{
	public:
		using This = StringViewBase<CharT>;

		using value_type = CharT;
		using allocator_type = AllocatorT;
		using size_type = Size;
		using difference_type = Offset;
		using reference = CharT&;
		using const_reference = const CharT&;
		using iterator = RandomContigIterator<const CharT>;
		using const_iterator = RandomContigIterator<const CharT>;
		using reverse_iterator = RevRandomContigIterator<const CharT>;
		using const_reverse_iterator = RevRandomContigIterator<const CharT>;


		constexpr static Size npos = std::numeric_limits<Size>::max();

		// Values that are kept in map, point to a string
		//todo: custom map implementation that lets Elem contain a hash using offset pointers
		//todo: allow calling constructor manually
		class Elem
		{
		public:
			StringViewHashBase<CharT> string;
			std::atomic<Size> refs;
			bool owns; // Indicates whether the attached string will be deleted with this element
			StringViewBase<WChar> alternate;

			Elem(const StringViewHashBase<CharT> _string, const bool copy = true) : owns(copy), alternate()
			{
				if (copy)
				{
					CharT* mem = defaultManager.allocator.allocate(_string.size() + 1);
					std::copy(_string.cbegin(), _string.cend(), mem);
					mem[_string.size()] = '\0';
					string = { mem, _string.size(), _string.hash() };
				}
				else
				{
					string = StringViewHashBase<CharT>(_string);
				}
			}

			void addRef()
			{
				++refs;
			}
			void subRef()
			{
				--refs;

				if (refs <= 0)
				{
					destroyStrings();
				}
			}

			const WChar* getAlternate()
			{
				if (alternate.data() == nullptr)
				{
					Vector<WChar> builder;

					utf8ToUtf16(string, builder);
					
					/*for (auto& c : builder)
					{
						if (c == '/')
							c = '\\';
					}*/

					alternate = builder.data();

					builder.invalidate();
				}

				return alternate.data();
			}

			void destroyAlternative()
			{
				delete[] alternate.data();
				alternate = {};
			}

			void destroyStrings()
			{
				if (owns)
				{
					delete[] string.data();
				}

				delete[] alternate.data();
			}
		};

		class Manager
		{
		public:
			std::map<Hash, Elem> strings;
			SharedMutex m_strings;

			Elem* nullString;

			[[no_unique_address]] AllocatorT allocator;
			
			Manager()
			{
				const CharT* nativeNullString;

				if constexpr (std::is_same<Char, CharT>::value)
				{
					nativeNullString = "\0";
				}
				else
				{
					nativeNullString = L"\0";
				}

				nullString = 
					&strings.emplace(std::piecewise_construct, 
						std::forward_as_tuple(hashes::djb2<Hash, CharT>(nativeNullString)), 
						std::forward_as_tuple(StringViewHashBase<CharT>(nativeNullString))).first->second;
			}

			bool has(const Hash hash)
			{
				SharedLock<SharedMutex> lock(m_strings);
				return strings.find(hash) != strings.end();
			}

			auto find (const Hash hash)
			{
				SharedLock<SharedMutex> lock(m_strings);
				return strings.find(hash);
			}

			Elem* insert(const StringViewHashBase<CharT> str, const bool copy = true)
			{
				m_strings.lock_shared();
				auto&& exists = find(str.hash());
				m_strings.unlock_shared();

				if (exists == strings.cend())
				{
					std::lock_guard<std::shared_mutex> lock(m_strings);
					return &strings.emplace(std::piecewise_construct, std::forward_as_tuple(str.hash()), 
						std::forward_as_tuple(str, copy)).first->second;
				}
				else
				{
					exists->second.addRef();
					return &exists->second;
				}

			}

			Elem* insert(const CharT* _begin, const CharT* _end, const bool copy = true)
			{
				return insert(StringViewHashBase<CharT>(_begin, _end), copy);
			}
		};

		static Manager defaultManager;


		Elem* source;

		ImStringBase() { source = defaultManager.nullString; }
		ImStringBase(Elem* const _source) : source(_source) {}
		ImStringBase(const StringViewHashBase<CharT> str, bool copy = true)
		{
			source = defaultManager.insert(str, copy);
		}
		ImStringBase(const CharT* const _begin, CharT* const _end, bool copy = true)
		{
			source = defaultManager.insert(StringViewHashBase<CharT>(_begin, _end), copy);
		}
		ImStringBase(const CharT* const str, const Bool copy = true)
		{
			source = defaultManager.insert(str, copy);
		}
		ImStringBase(const std::string& str)
		{
			source = defaultManager.insert(str.data(), true);
		}
		ImStringBase(const ImStringBase& _str)
		{
			source = _str.source;
			source->addRef();
		}
		ImStringBase(ImStringBase&& _str) // Move constructor - avoid changing counter
		{
			source = _str.source;
			_str.source = nullptr;
		}
		ImStringBase(const StringBuilderBase<CharT>& _str)
		{
			source = defaultManager.insert(_str.data(), _str.data() + _str.size(), true);
			source->addRef();			
		}
		ImStringBase(StringBuilderBase<CharT>&& _str)
		{
			_str.ensureNullTerminated();
			source = defaultManager.insert(_str.data(), _str.data() + _str.size(), false);
			source->addRef();

			_str.data_raw.data_raw = {};
			_str.data_raw.reserved_raw = nullptr;
		}
		ImStringBase(const WChar* const _str)
		{
			StringBuilder builder;
			utf16ToUtf8(ContiguousBufPtrEnd<const WChar>(_str, strlen(_str)), builder);

			builder.ensureNullTerminated();
			source = defaultManager.insert(builder.data(), builder.data() + builder.size(), false);
			source->addRef();

			builder.data_raw.data_raw = {};
			builder.data_raw.reserved_raw = nullptr;
		}

		
		~ImStringBase()
		{
			if (source != nullptr)
				source->subRef();
		}

		ImStringBase& operator=(const ImStringBase& _str)
		{
			source->subRef();

			source = _str.source;
			source->addRef();
			return *this;
		}
		ImStringBase& operator=(ImStringBase&& _str)
		{
			source->subRef();

			source = _str.source;
			_str.source = nullptr;
			return *this;
		}
		ImStringBase& operator=(const char* const _str)
		{
			source = defaultManager.insert(StringViewHashBase<CharT>(_str), true);
		}
		ImStringBase& operator=(const WChar* const _str)
		{
			Vector<Char> builder;
			utf16ToUtf8(ContiguousBufPtrEnd(_str, wcslen(_str)), builder);
			source = defaultManager.insert(StringViewHashBase<CharT>(builder.data()), false);
			builder.data().data_raw = { nullptr, nullptr };
			builder.data().reserved_raw = nullptr;
		}


		CharT& operator[] (Size index)
		{
			return data()[index];
		}

		Size size() const
		{
			return source->string.size();
		}

		bool empty() const
		{
			return source == defaultManager.nullString;
		}

		CharT& front() const
		{
			return data()[0];
		}
		CharT& back() const
		{
			return *(source->string.end() - 1);
		}

		const ContiguousBufPtrEnd<const CharT> range() const { return source->string.range(); }
		const CharT* data() const { return source->string.data(); }
		const_iterator begin() const { return range().cbegin(); }
		const_iterator end() const { return range().cend(); }
		const_iterator cbegin() const { return range().cbegin(); }
		const_iterator cend() const { return range().cend(); }
		const_reverse_iterator rbegin() const { return range().crbegin(); }
		const_reverse_iterator rend() const { return range().crend(); }
		const_reverse_iterator crbegin() const { return range().crbegin(); }
		const_reverse_iterator crend() const { return range().crend(); }

		//todo: template variant by reference or integer return value
		Size find(CharT letter) const
		{
			return source->string.find(letter);
		}

		Size rfind(CharT letter) const
		{
			return source->string.rfind(letter);
		}

		bool has(CharT letter) const
		{
			return source->string.hash(letter);
		}

		ImStringBase& operator+= (const ImStringBase& rhs)
		{
			CharT* mem = new CharT[this->size() + rhs.size()];
			std::copy(this->cbegin(), this->cend(), mem);
			std::copy(rhs.cbegin(), rhs.cend(), mem + this->size());
			mem[this->size() + rhs.size() + 1] = '\0';

			source = new Elem(StringViewHashBase<CharT>(mem, this->size() + rhs.size()), false);

			return *this;
		}

		ImStringBase& replaceAll(const CharT elm, const CharT sub)
		{
			CharT* mem = new CharT[this->size()];
			std::copy(this->cbegin(), this->cend(), mem);

			for (auto& c : ContiguousBufPtrEnd(mem, mem + size()))
			{
				if (c == elm)
					c = sub;
			}

			source = new Elem(StringViewHashBase<CharT>(mem, this->size()), false);

			return *this;
		}

		const WChar* alternate() { return source->getAlternate(); }

		operator ContiguousBufPtrEnd<const Char>() const { return range(); }
		explicit operator std::string() const { return data(); }

		friend std::ostream& operator<< (std::ostream& stream, const ImStringBase& rhs)
		{
			stream << rhs.data();
			return stream;
		}

		const bool operator< (const ImStringBase& rhs) const { return source->string.hash() < rhs.source->string.hash(); }
	};

	template<>
	ImStringBase<>::Manager ImStringBase<>::defaultManager = {};

	template<>
	ImStringBase<WChar>::Manager ImStringBase<WChar>::defaultManager = {};

}
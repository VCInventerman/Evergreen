#pragma once

#include "evergreen/types.h"
//#include "evergreen/common/alloc.h"
#include "evergreen/StringView.h"
#include <evergreen/Vector.h>
#include <evergreen/bit.h>
#include <evergreen/StringBuilder.h>

// Immutable string
// Stores strings in a map of hash values to null-terminated (until bootstrap for c compat) strings syncronized by a shared mutex
// Strings are reference counted and remove themselves from map when atomic reference count reaches zero unless they are put in at compile time and unremovable

namespace evg
{
	template<typename CharT = char, typename AllocatorT = std::allocator<CharT>>
	class ImStringBase
	{
	public:
		using This = ImStringBase<CharT>;

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


		// Reference-keeping element kept inside map
		class Elem
		{
		public:
			using This = Elem;


			StringViewHashBase<CharT> string;
			std::function<void()> deleter;
			
			std::atomic<Size> refs;


			// Creates copy of given string to manage as needed
			Elem(const StringViewHashBase<CharT> _string)
			{
				Size allocSize = _string.size() + 1;
				CharT* mem = defaultManager.allocator.allocate(allocSize);
				std::copy(_string.cbegin(), _string.cend(), mem);
				mem[_string.size()] = '\0';
				string = { mem, _string.size(), _string.hash() };
				deleter = [mem, allocSize]() { defaultManager.allocator.deallocate(mem, allocSize); };

				refs = 0;
			}

			// Take string that has been already been created and remember how to delete it
			Elem(const StringViewHashBase<CharT> _string, std::function<void()> _deleter)
			{
				string = _string;
				deleter = _deleter;

				refs = 0;
			}

			~Elem()
			{
				refs = 0;
				deleter();
			}

			// Manage reference count
			void addRef()
			{
				++refs;
			}
			void subRef()
			{
				if (--refs <= 0)
				{
					// Invalidates this!
					defaultManager.erase(hash());
				}
			}
			Size getRefs()
			{
				return refs;
			}

			Hash hash()
			{
				return string.hash();
			}

			friend ImStringBase<CharT>;
		};

		class Manager
		{
		public:
			using This = Manager;

		protected:
			std::map<Hash, Elem> strings_;
			boost::shared_mutex m_strings_;

			Elem* nullString_;

			[[no_unique_address]] AllocatorT allocator;

		public:
			Manager()
			{
				// Create a string that represents ""
				const CharT* nativeNullString = nullptr;

				if constexpr (std::is_same<CharT, char>::value) { nativeNullString = "\0"; }
				else if constexpr (std::is_same<CharT, wchar_t>::value) { nativeNullString = L"\0"; }
				else if constexpr (std::is_same<CharT, char16_t>::value) { nativeNullString = u"\0"; }
				else 
				{ 
					static_assert(std::is_same_v<CharT, char> || std::is_same_v<CharT, wchar_t> || std::is_same_v<CharT, char16_t>,
						"ImStringBase must be used with either char, wchar_t, or char16_t"); 
				}

				nullString_ = &strings_.emplace(std::piecewise_construct,
						std::forward_as_tuple(hashes::djb2<Hash>(nativeNullString)),
						std::forward_as_tuple(StringViewHashBase<CharT>(nativeNullString), nop)).first->second;
			}

		protected:
			bool has(const Hash hash)
			{
				boost::shared_lock_guard<boost::shared_mutex> lock(m_strings_);
				return strings_.find(hash) != strings_.end();
			}

			Elem* insert(const StringViewHashBase<CharT> str, std::function<void()> deleter)
			{
				boost::upgrade_lock<boost::shared_mutex> readLock(m_strings_);
				auto&& itr = strings_.find(str.hash());
				
				if (itr == strings_.cend())
				{
					boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(readLock);

					return &strings_.emplace(std::piecewise_construct,
						std::forward_as_tuple(str.hash()),
						std::forward_as_tuple(str, deleter)).first->second;
				}
				else
				{
					deleter();
					return &itr->second;
				}
			}

			Elem* insert(const StringViewHashBase<CharT> _str)
			{
				boost::upgrade_lock<boost::shared_mutex> readLock(m_strings_);
				auto&& itr = strings_.find(_str.hash());

				if (itr == strings_.cend())
				{
					boost::upgrade_to_unique_lock<boost::shared_mutex> writeLock(readLock);

					return &strings_.emplace(std::piecewise_construct,
						std::forward_as_tuple(_str.hash()),
						std::forward_as_tuple(_str)).first->second;
				}
				else
				{
					return &itr->second;
				}
			}

			// Removes an element from the string map
			// Returns true on a successful removal and false on failure
			bool erase(const Hash hash)
			{
				if (hash == nullString_->hash())
					return false;

				boost::lock_guard<boost::shared_mutex> lock(m_strings_);
				auto&& itr = strings_.find(hash);

				if (itr != strings_.cend())
				{
					strings_.erase(itr);
					return true;
				}
				else
				{
					return false;
				}
			}

			friend ImStringBase<CharT>;
		};

		static Manager defaultManager;

	protected:
		mutable Elem* source;

	protected:
		// Construct from implementation-defined interned string
		ImStringBase(Elem* const _source)
		{
			source = _source;
			source->addRef();
		}

		void destroy()
		{
			if (source != nullptr && source != defaultManager.nullString_)
			{
				//	std::cout << "Destroying string " << data() << '\n';
				source->subRef();
				source = nullptr;
			}
		}

	public:
		// Default constructor - initialized to a nullString by default
		ImStringBase() { source = defaultManager.nullString_; }

		// Copy constructor 
		ImStringBase(const ImStringBase& str)
		{
			source = str.source;
			source->addRef();
		}
		ImStringBase(ImStringBase&& str) // Move constructor - avoid changing counter
		{
			source = str.source;
			str.source = nullptr;
		}


		ImStringBase(const StringViewHashBase<CharT> str)
		{
			source = defaultManager.insert(str);
			source->addRef();
		}
		ImStringBase(const StringViewHashBase<CharT> str, std::function<void()> _deleter)
		{
			source = defaultManager.insert(str, _deleter);
			source->addRef();
		}
		ImStringBase(const CharT* const str)
		{
			source = defaultManager.insert(str);
			source->addRef();
		}
		ImStringBase(const wchar_t* const str)
		{
			Vector<char> builder;
			utf16ToUtf8(ContiguousBufPtrEnd(str, strlen(str)), builder);
			source = defaultManager.insert(StringViewHashBase<CharT>(builder.data()), nop);
			source->addRef();
			builder.invalidate();
		}

		// Construct from constant std::string reference - copies data
		ImStringBase(const std::string& str)
		{
			

			source = defaultManager.insert(StringViewHashBase<CharT>(str.data(), str.data() + str.size()));
			source->addRef();
		}

		// Construct from constant StringBuilder reference - copies data
		ImStringBase(const StringBuilderBase<CharT>& str)
		{
			source = defaultManager.insert(StringViewHashBase<CharT>(str.data(), str.data() + str.size()));
			source->addRef();
		}

		// Construct from constant StringBuilder rvalue reference - moves data
		ImStringBase(StringBuilderBase<CharT>&& str)
		{
			str.ensureNullTerminated();
			source = defaultManager.insert(StringViewHashBase<CharT>(str.data(), str.data() + str.size()), str.deleter());
			source->addRef();

			str.invalidate();
		}


		ImStringBase& operator=(const ImStringBase& str)
		{
			destroy();
			source = str.source;
			source->addRef();
			return *this;
		}
		ImStringBase& operator=(ImStringBase&& str)
		{
			destroy();
			source = str.source;
			str.source = nullptr;
			return *this;
		}
		ImStringBase& operator=(const StringViewHashBase<CharT> str)
		{
			destroy();
			source = defaultManager.insert(str);
			source->addRef();
		}
		ImStringBase& operator=(const CharT* const str)
		{
			destroy();
			source = defaultManager.insert(StringViewHashBase<CharT>(str));
			source->addRef();
		}
		ImStringBase& operator=(const WChar* const str)
		{
			destroy();
			Vector<char> builder;
			utf16ToUtf8(ContiguousBufPtrEnd(str, strlen(str)), builder);
			char* toDelete = builder.data();
			source = defaultManager.insert(StringViewHashBase<CharT>(builder.data()), [toDelete]() {delete toDelete; });
			source->addRef();
			builder.invalidate();
			return *this;
		}

		// Construct from constant StringBuilder reference - copies data
		ImStringBase& operator=(const StringBuilderBase<CharT>& str)
		{
			destroy();
			source = defaultManager.insert(StringViewHashBase<CharT>(str.data(), str.data() + str.size()), str.deleter());
			source->addRef();
			return *this;
		}

		// Construct from constant StringBuilder rvalue reference - moves data
		ImStringBase& operator=(StringBuilderBase<CharT>&& str)
		{
			destroy();
			str.ensureNullTerminated();
			source = defaultManager.insert(StringViewHashBase<CharT>(str.data(), str.data() + str.size()), str.deleter());
			source->addRef();

			str.invalidate();
			return *this;
		}


		~ImStringBase()
		{
			destroy();
		}

		static ImStringBase& fromU16(const WChar* const _str)
		{
			StringBuilder builder;
			utf16ToUtf8(ContiguousBufPtrEnd<const WChar>(_str, strlen(_str)), builder);

			builder.ensureNullTerminated();
			This newStr(builder);

			builder.invalidate();
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
			return source->string.has(letter);
		}

		ImStringBase& operator+= (const ImStringBase& rhs)
		{
			CharT* mem = new CharT[this->size() + rhs.size()];
			std::copy(this->cbegin(), this->cend(), mem);
			std::copy(rhs.cbegin(), rhs.cend(), mem + this->size());
			mem[this->size() + rhs.size() + 1] = '\0';

			*this = This(StringViewHashBase<CharT>(mem, this->size() + rhs.size()), [mem]() {delete mem; });

			return *this;
		}

		ImStringBase& replaceAll(const CharT elm, const CharT sub)
		{
			CharT* mem = new CharT[this->size() + 1];
			mem[this->size()] = '\0';
			std::copy(this->cbegin(), this->cend(), mem);

			for (auto& c : ContiguousBufPtrEnd(mem, mem + size()))
			{
				if (c == elm)
					c = sub;
			}

			*this = This(StringViewHashBase<CharT>(mem, size()), [mem]() {delete mem; });

			return *this;
		}

		const WChar* alternate() { return source->getAlternate(); }

		explicit operator ContiguousBufPtrEnd<const char>() const { return range(); }
		explicit operator std::string() const
		{
			if (source == nullptr)
			{
				std::cout << "String was nullptr?????\n";
				debugBreak();
			}

			return std::string(data(), size());
		}
		explicit operator const CharT* () const { return data(); }

		friend std::ostream& operator<< (std::ostream& stream, const ImStringBase& rhs)
		{
			stream << std::basic_string_view<CharT>(rhs.data(), rhs.size());
			return stream;
		}

		const bool operator< (const ImStringBase& rhs) const { return source->hash() < rhs.source->hash(); }

		bool valid() const noexcept
		{
			return (source != nullptr) && source->string.valid();
		}
	};

	template<>
	ImStringBase<>::Manager ImStringBase<>::defaultManager = {};

	//template<>
	//ImStringBase<WChar>::Manager ImStringBase<WChar>::defaultManager = {};

}
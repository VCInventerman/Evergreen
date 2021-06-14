#pragma once
#pragma warning(push)
#pragma warning(disable : 4706)

#include "evergreen/ContiguousBuf.h"

namespace evg
{
	namespace hashes
	{
		// Get hash and length of string from a null-terminated CString
		// djb2 by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
		template <typename RetT = std::pair<Hash, Size>, typename CharT = Char>
		typename std::enable_if<std::is_same<RetT, std::pair<Hash, Size>>::value, std::pair<Hash, Size>>::type djb2(const CharT* const str)
		{
			Hash hash = 5381;
			Int c;

			const CharT* i = str;
			while ((c = *i++))
			{
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
			}
				

			return { hash, i - str - 1 };
		}

		// Avoid returning the size
		template <typename RetT = std::pair<Hash, Size>, typename CharT = Char>
		typename std::enable_if<std::is_same<RetT, Hash>::value, Hash>::type djb2(const CharT* const str)
		{
			Hash hash = 5381;
			Int c;

			const CharT* i = str;
			while ((c = *i++))
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}



		// Get hash and length of string from a ContiguousBufPtrEnd (safer)
		// djb2 by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
		template <typename RetT = std::pair<Hash, Size>, typename CharT = Char>
		typename std::enable_if<std::is_same<RetT, std::pair<Hash, Size>>::value, std::pair<Hash, Size>>::type djb2(const ContiguousBufPtrEnd<CharT> str)
		{
			Hash hash = 5381;

			for (Int c : str)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return { hash, str.size() };
		}

		// Avoid returning the size
		template <typename RetT = std::pair<Hash, Size>, typename CharT = Char>
		typename std::enable_if<std::is_same<RetT, Hash>::value, Hash>::type djb2(const ContiguousBufPtrEnd<CharT> str)
		{
			Hash hash = 5381;

			for (Int c : str)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
	}

	template <typename CharT = Char>
	Size strlen(const CharT* const str, const Size maxLen = 0x00FFFFFFFFFFFFFF)
	{
		//todo: vectorization

		const CharT* c = str;
		const CharT* const max = str + maxLen;

		while ((*c != '\0') && (c <= max))
		{
			++c;
		}

		return c - str;
	}


	// Contains a pointer to the begin and end
	// Defaults to djb2 hash
	template<typename CharT = CChar>
	class StringViewBase
	{
	public:
		using This = StringViewBase<CharT>;
		using Iterator = RandomContigIterator<CharT>;
		using CIterator = RandomContigIterator<const CharT>;
		using RevIterator = RevRandomContigIterator<CharT>;
		using CRevIterator = RevRandomContigIterator<const CharT>;

		constexpr static Size npos = std::numeric_limits<Size>::max();

	public: // Access is discouraged
		ContiguousBufPtrEnd<const CharT> data_raw;

	public:
		StringViewBase() : data_raw() {}
		StringViewBase(const CharT* const _data) : data_raw(_data, strlen(_data)) {}
		StringViewBase(const CharT* const _data, const Size _size) : data_raw(_data, _size) {}
		StringViewBase(const CharT* const _begin, const CharT* _end) : data_raw(_begin, _end) {}
		/*constexpr StringViewBase(std::initializer_list<const CharT*> _data)
		{
			data_raw.begin_raw = *_data.begin();
			data_raw.end_raw = *_data.end();
		}*/


		Size size() const { return range().size(); }

		ContiguousBufPtrEnd<const CharT> range() const { return data_raw; }
		const CharT* data() const { return data_raw.begin_raw; }
		CIterator begin() const { return range().begin(); }
		CIterator end() const { return range().end(); }
		CIterator cbegin() const { return range().cbegin(); }
		CIterator cend() const { return range().cend(); }
		CRevIterator rbegin() const { return range().rbegin(); }
		CRevIterator rend() const { return range().rend(); }
		CRevIterator crbegin() const { return range().crbegin(); }
		CRevIterator crend() const { return range().crend(); }

		bool ptrComp(const This& rhs) const // Shallow compare ContiguousBufPtrEnd
		{
			return data() == rhs.data();
		}

		bool dataComp(const This& rhs) const // Deep compare string contents
		{
			if (size() != rhs.size()) return false;
			return strncmp(begin(), rhs.begin(), size()) == 0;
		}

		bool comp (const This& rhs) const { return dataComp(rhs); }

		Hash hash() const { return hashes::djb2<Hash>(begin()); }

		bool valid() const { return data().valid(); }

		Size find(CharT letter) const
		{
			for (CChar& i : *this)
			{
				if (i == letter)
				{
					return &i - &*begin();
				}
			}

			return npos;
		}

		Size rfind(CharT letter) const
		{
			for (auto i = rbegin(); i != rend(); ++i)
			{
				if (*i == letter)
				{
					return &*i;
				}
			}

			return npos;
		}

		StringViewBase slice(Size offset) const
		{
			return StringViewBase(&*begin() + offset, size() - offset);
		}


		inline bool operator==(const This& rhs) const { return comp(rhs); }
		inline bool operator!=(const This& rhs) const { return !comp(rhs); }

		CharT& operator[](const Size i) { return data_raw[i]; }

		operator std::string_view()
		{
			return std::string_view(data_raw, size());
		}

		// Get representation that is compatible for comparisons with std::string
		std::string_view view() const
		{
			return std::string_view(data_raw, size());
		}
	};

	using StringView = StringViewBase<>;

	// Contains a pointer to the begin and end, and a hash
	// Defaults to djb2 hash
	//todo: allow construction with known hash (cannot because it uses the same type as Size)
	template<typename CharT = CChar>
	class StringViewHashBase
	{
	public:
		using This = StringViewHashBase<CharT>;
		using Iterator = RandomContigIterator<CharT>;
		using CIterator = RandomContigIterator<const CharT>;
		using RevIterator = RevRandomContigIterator<CharT>;
		using CRevIterator = RevRandomContigIterator<const CharT>;

		constexpr static Size npos = std::numeric_limits<Size>::max();

	public: // Access is discouraged
		ContiguousBufPtrEnd<const CharT> data_raw;
		Hash hash_raw;

	public:
		// Constructors
		StringViewHashBase() : data_raw(), hash_raw() {}
		StringViewHashBase(const StringViewHashBase& _str)
		{
			data_raw = _str.data_raw;
			hash_raw = _str.hash_raw;
		}
		StringViewHashBase(StringViewHashBase&& _str)
		{
			data_raw = _str.data_raw;
			hash_raw = _str.hash_raw;
		}
		StringViewHashBase(const CharT* const _data)
		{ 
			auto hashlen = hashes::djb2<std::pair<Hash, Size>>(_data);
			data_raw = ContiguousBufPtrEnd<const CharT>(_data, hashlen.second);
			hash_raw = hashlen.first;
		}
		StringViewHashBase(const CharT* const _data, const Size _size)
		{
			data_raw = ContiguousBufPtrEnd<const CharT>(_data, _size);
			hash_raw = hashes::djb2<Hash>(_data);
		}
		StringViewHashBase(const CharT* const _begin, const CharT* _end)
		{
			data_raw = ContiguousBufPtrEnd<const CharT>(_begin, _end);
			hash_raw = hashes::djb2<Hash>(_begin);	
		}
		StringViewHashBase(const CharT* const _data, const Size _size, const Hash _hash) : data_raw(_data, _size), hash_raw(_hash) {}
		StringViewHashBase(const CharT* const _begin, const CharT* _end, const Hash _hash) : data_raw(_begin, _end), hash_raw(_hash) {}
		StringViewHashBase& operator=(const StringViewHashBase& _str) = default;
		StringViewHashBase& operator=(StringViewHashBase&& _str) = default;

		// Member functions
		Size size() const { return range().size(); }

		ContiguousBufPtrEnd<const CharT> range() const { return data_raw; }
		const CharT* data() const { return data_raw.begin_raw; }
		CIterator begin() const { return range().begin(); }
		CIterator end() const { return range().end(); }
		CIterator cbegin() const { return range().cbegin(); }
		CIterator cend() const { return range().cend(); }
		CRevIterator rbegin() const { return range().rbegin(); }
		CRevIterator rend() const { return range().rend(); }
		CRevIterator crbegin() const { return range().crbegin(); }
		CRevIterator crend() const { return range().crend(); }

		bool ptrComp(const ContiguousBufPtrEnd<CharT>& rhs) const // Shallow compare ContiguousBufPtrEnd
		{
			return data == rhs.data;
		}

		bool dataComp(const ContiguousBufPtrEnd<CharT>& rhs) const // Deep compare hash (this is a dataComp because the hash is a representation of the data)
		{
			return hash == rhs.hash;
		}

		bool comp(const This& rhs) const { return dataComp(rhs); }

		Size localSize() const { return sizeof(ContiguousBufPtrEnd<CharT>); } // Size of object
		Size remoteSize() const { return size(); } // Size of other resources owned by this object
		Size totalSize() const { return localSize() + remoteSize(); } // Total size

		Hash hash() const { return hash_raw; }

		Size find(CharT letter) const
		{
			for (CChar& i : *this)
			{
				if (i == letter)
				{
					return &i - &*begin();
				}
			}

			return npos;
		}

		Size rfind(CharT letter) const
		{
			for (auto i = rbegin(); i != rend(); ++i)
			{
				if (*i == letter)
				{
					return &*i - &*begin();
				}
			}

			return npos;
		}

		bool has(CharT letter) const
		{
			for (auto&& i : *this)
			{
				if (i == letter) { return true; }
			}
			return false;
		}

		StringViewHashBase slice(Size offset) const
		{
			return StringViewHashBase(&*begin() + offset, &*end());
		}

		
		// C++ operators
		inline bool operator==(const ContiguousBufPtrEnd<CharT>& rhs) const { return operator_dataComp(rhs); }
		inline bool operator!=(const ContiguousBufPtrEnd<CharT>& rhs) const { return !operator_dataComp(rhs); }

		CharT& operator[](const Size i) const { return data_raw[i]; }
	};

	using StringViewHash = StringViewHashBase<>;
}

#pragma warning(pop)
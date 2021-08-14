#pragma once

#if defined(EVG_COMPILER_MSVC)
#pragma warning(push)
#pragma warning(disable : 4706)
#else
//#pragma GCC diagnostic error "-Wuninitialized"
//#pragma GCC diagnostic push
#endif

#include "evergreen/ContiguousBuf.h"

namespace evg
{
	namespace hashes
	{
		// Get hash and length of string from a null-terminated CString
		// djb2 by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
		template <typename RetT = std::pair<Hash, Size>, typename CharT = char>
		typename std::enable_if<std::is_same<RetT, std::pair<Hash, Size>>::value, std::pair<Hash, Size>>::type djb2(const CharT* const str)
		{
			Hash hash = 5381;
			Size c;

			const CharT* i = str;
			while ((c = *i++))
			{
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
			}


			return { hash, i - str - 1 };
		}

		// Avoid returning the size
		template <typename RetT = std::pair<Hash, Size>, typename CharT = char>
		typename std::enable_if<std::is_same<RetT, Hash>::value, Hash>::type djb2(const CharT* const str)
		{
			Hash hash = 5381;
			Size c;

			const CharT* i = str;
			while ((c = *i++))
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}



		// Get hash and length of string from a ContiguousBufPtrEnd (safer)
		// djb2 by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
		template <typename RetT = std::pair<Hash, Size>, typename CharT = char>
		typename std::enable_if<std::is_same<RetT, std::pair<Hash, Size>>::value, std::pair<Hash, Size>>::type djb2(const ContiguousBufPtrEnd<CharT> str)
		{
			Hash hash = 5381;

			for (Size c : str)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return { hash, str.size() };
		}

		// Avoid returning the size
		template <typename RetT = std::pair<Hash, Size>, typename CharT = char>
		typename std::enable_if<std::is_same<RetT, Hash>::value, Hash>::type djb2(const ContiguousBufPtrEnd<CharT> str)
		{
			Hash hash = 5381;

			for (Size c : str)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
	}

	template <typename CharT = char>
	Size strlen(const CharT* const str, const Size maxLen = 0x00FFFFFFFFFFFFFF)
	{
		if constexpr (std::is_same<CharT, char>::value)
		{
			return ::strlen(str);
		}
		else
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




	}

	template<typename CharT>
	Size stringFind(const CharT* begin, const CharT* end, const CharT val) noexcept
	{
		constexpr Size npos = std::numeric_limits<Size>::max();

		if (begin >= end)
			return npos;

		for (CharT* c = begin; c < end; ++c)
		{
			if (*c == val)
			{
				return c - begin;
			}
		}

		return npos;
	}

	template<typename CharT>
	Size stringRFind(const CharT* begin, const CharT* end, const CharT val) noexcept
	{
		constexpr Size npos = std::numeric_limits<Size>::max();

		if (begin >= end)
			return npos;

		for (CharT* c = begin; c < end; ++c)
		{
			if (*c == val)
			{
				return c - begin;
			}
		}

		return npos;
	}

	template<typename CharT>
	Size stringCount(const CharT* begin, const CharT* end, const CharT val) noexcept
	{
		Size counter = 0;

		if (begin >= end)
			return 0;

		for (const char* c = begin; c < end; ++c)
		{
			if (*c == val)
			{
				++counter;
			}
		}

		return counter;
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
		ContiguousBufPtrEnd<const CharT> data_;

	public:
		StringViewBase() : data_() {}
		StringViewBase(const CharT* const _data) : data_(_data, strlen(_data)) {}
		StringViewBase(const CharT* const _data, const Size _size) : data_(_data, _size) {}
		StringViewBase(const CharT* const _begin, const CharT* _end) : data_(_begin, _end) {}
		StringViewBase(const std::string& _data) : data_(_data.data(), _data.size()) {}

		// Constexpr constructor from string literal
		template <Size size>
		constexpr StringViewBase(const char(&str)[size]) : data_(str, str + size) {}


		Size size() const { return range().size(); }

		ContiguousBufPtrEnd<const CharT> range() const { return data_; }
		const CharT* data() const { return data_.data(); }
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

		bool comp(const This& rhs) const { return dataComp(rhs); }

		Hash hash() const { return hashes::djb2<Hash>(begin()); }

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

		CharT& operator[](const Size i) { return data_[i]; }

		operator std::string_view()
		{
			return std::string_view(data_, size());
		}

		// Get representation that is compatible for comparisons with std::string
		std::string_view view() const
		{
			return std::string_view(data_, size());
		}

		bool valid() const noexcept
		{
			return data_.valid();
		}

		void invalidate() noexcept
		{
			data_.invalidate();
		}

		operator ContiguousBufPtrEnd<const char>() const noexcept
		{
			return data_;
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
		ContiguousBufPtrEnd<const CharT> data_;
		Hash hash_;

	public:
		// Constructors
		StringViewHashBase() : data_(), hash_() {}
		StringViewHashBase(const StringViewHashBase& _str)
		{
			data_ = _str.data_;
			hash_ = _str.hash_;
		}
		StringViewHashBase(StringViewHashBase&& _str)
		{
			data_ = _str.data_;
			hash_ = _str.hash_;
		}
		StringViewHashBase(const CharT* const _data)
		{
			auto hashlen = hashes::djb2<std::pair<Hash, Size>>(_data);
			data_ = ContiguousBufPtrEnd<const CharT>(_data, hashlen.second);
			hash_ = hashlen.first;
		}
		StringViewHashBase(const CharT* const _data, const Size _size)
		{
			data_ = ContiguousBufPtrEnd<const CharT>(_data, _size);
			hash_ = hashes::djb2<Hash>(_data);
		}
		StringViewHashBase(const CharT* const _begin, const CharT* _end)
		{
			data_ = ContiguousBufPtrEnd<const CharT>(_begin, _end);
			hash_ = hashes::djb2<Hash>(_begin);
		}
		StringViewHashBase(const std::string& _data) : data_(_data.data(), _data.size())
		{
			hash_ = hashes::djb2<Hash>(_data.data());
		}

		// Constexpr constructor from string literal
		template <Size size>
		constexpr StringViewHashBase(const char(&str)[size]) : data_(str, str + size)
		{
			hash_ = hashes::djb2<Hash>(str, size);
		}

		StringViewHashBase(const CharT* const _data, const Size _size, const Hash _hash) : data_(_data, _size), hash_(_hash) {}
		StringViewHashBase(const CharT* const _begin, const CharT* _end, const Hash _hash) : data_(_begin, _end), hash_(_hash) {}
		StringViewHashBase& operator=(const StringViewHashBase& _str) = default;
		StringViewHashBase& operator=(StringViewHashBase&& _str) = default;

		// Member functions
		Size size() const { return range().size(); }

		ContiguousBufPtrEnd<const CharT> range() const { return data_; }
		const CharT* data() const { return data_.data(); }
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
			return data() == rhs.data();
		}

		bool dataComp(const ContiguousBufPtrEnd<CharT>& rhs) const // Deep compare hash (this is a dataComp because the hash is a representation of the data)
		{
			return hash() == rhs.hash();
		}

		bool comp(const This& rhs) const { return dataComp(rhs); }

		Size localSize() const { return sizeof(ContiguousBufPtrEnd<CharT>); } // Size of object
		Size remoteSize() const { return size(); } // Size of other resources owned by this object
		Size totalSize() const { return localSize() + remoteSize(); } // Total size

		Hash hash() const { return hash_; }

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

		bool empty() const
		{
			return size() == 0;
		}

		bool valid() const noexcept
		{
			return data_.valid();
		}

		void invalidate() noexcept
		{
			data_.invalidate();
			hash_ = 0;
		}


		// C++ operators
		inline bool operator==(const ContiguousBufPtrEnd<CharT>& rhs) const { return operator_dataComp(rhs); }
		inline bool operator!=(const ContiguousBufPtrEnd<CharT>& rhs) const { return !operator_dataComp(rhs); }

		CharT& operator[](const Size i) const { return data_[i]; }

		operator ContiguousBufPtrEnd<const char>() const noexcept
		{
			return data_;
		}
	};

	using StringViewHash = StringViewHashBase<>;
}

#if defined(EVG_COMPILER_MSVC)
#pragma warning(pop)
#else
//#pragma GCC diagnostic pop
#endif

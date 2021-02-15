#pragma once

#include "evergreen/common/ContiguousBuf.h"

namespace evg
{
	namespace hashes
	{
		// Get hash and length of string from a null-terminated CString
		// djb2 by Dan Bernstein: http://www.cse.yorku.ca/~oz/hash.html
		template <typename RetT, typename CharT = CChar>
		typename std::enable_if<std::is_same<RetT, std::pair<Hash, Size>>::value, std::pair<Hash, Size>>::type djb2(const CharT* const str)
		{
			Hash hash = 5381;
			Int c;

			const CharT* i = str;
			while ((c = *i++))
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return { hash, i - str };
		}

		// Avoid returning the size
		template <typename RetT, typename CharT = CChar>
		typename std::enable_if<std::is_same<RetT, Hash>::value, Hash>::type djb2(const CharT* const str)
		{
			Hash hash = 5381;
			Int c;

			const CharT* i = str;
			while ((c = *i++))
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
	}


	// Contains a pointer to the begin and end
	// Defaults to djb2 hash
	template<typename CharT = CChar>
	class StringViewBase
	{
	public:
		ContiguousBufPtrEnd<CharT> data;


		StringViewBase() = default;
		StringViewBase(const CharT* const _data) : data(_data, strlen(_data)) {}
		StringViewBase(const CharT* const _data, const Size _size) : data(_data, _size) {}
		StringViewBase(const CharT* const _begin, const CharT* _end) : data(_begin, _end) {}


		bool operator_ptrComp(const ContiguousBufPtrEnd<CharT>& rhs) const // Shallow compare ContiguousBufPtrEnd
		{
			return data == rhs.data;
		}

		bool operator_dataComp(const ContiguousBufPtrEnd<CharT>& rhs) const // Deep compare string contents
		{
			if (data.size() != rhs.size()) return false;
			return strncmp(data.begin(), rhs.begin(), data.size()) == 0;
		}

		inline bool operator==(const ContiguousBufPtrEnd<CharT>& rhs) const { return operator_dataComp(rhs); }
		inline bool operator!=(const ContiguousBufPtrEnd<CharT>& rhs) const { return !operator_dataComp(rhs); }

		Hash operator_hash() const { return hashes::djb2<Hash>(data.begin()); }
	};

	using StringView = StringViewBase<CChar>;

	// Contains a pointer to the begin and end, and a hash
	// Defaults to djb2 hash
	//todo: allow construction with known hash (cannot because it uses the same type as Size)
	template<typename CharT = CChar>
	class StringViewHashBase
	{
	public:
		ContiguousBufPtrEnd<CharT> data;
		Hash hash;

		StringViewHashBase() = default;
		StringViewHashBase(const CharT* const _data)
		{ 
			auto hashlen = hashes::djb2<std::pair<Hash, Size>>(_data);
			data = ContiguousBufPtrEnd<CharT>(_data, hashlen.second);
			hash = hashlen.first;
		}
		StringViewHashBase(const CharT* const _data, const Size _size) : data(_data, _size)
		{
			data = ContiguousBufPtrEnd<CharT>(_data, _size);
			hash = hashes::djb2<Hash>(_data);
		}
		StringViewHashBase(const CharT* const _begin, const CharT* _end) : data(_begin, _end)
		{
			data = ContiguousBufPtrEnd<CharT>(_begin, _end);
			hash = hashes::djb2<Hash>(_begin);	
		}
		StringViewHashBase(const CharT* const _data, const Size _size, const Hash _hash) : data(_data, _size), hash(_hash) {}
		StringViewHashBase(const CharT* const _begin, const CharT* _end, const Hash _hash) : data(_begin, _end), hash(_hash) {}


		bool operator_ptrComp(const ContiguousBufPtrEnd<CharT>& rhs) const // Shallow compare ContiguousBufPtrEnd
		{
			return data == rhs.data;
		}

		bool operator_dataComp(const ContiguousBufPtrEnd<CharT>& rhs) const // Deep compare hash (this is a dataComp because the hash is a representation of the data)
		{
			return hash == rhs.hash;
		}

		inline bool operator==(const ContiguousBufPtrEnd<CharT>& rhs) const { return operator_dataComp(rhs); }
		inline bool operator!=(const ContiguousBufPtrEnd<CharT>& rhs) const { return !operator_dataComp(rhs); }

		Hash operator_hash() const { return hash; }
	};

	using StringViewHash = StringViewHashBase<CChar>;
}
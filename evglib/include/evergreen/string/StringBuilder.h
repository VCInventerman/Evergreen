#pragma once

//#include "evergreen/common/alloc.h"
#include "evergreen/common/ContiguousBuf.h"
#include "evergreen/common/types.h"
#include "evergreen/containers/Vector.h"

namespace evg
{

	template <typename CharT = Char>
	class StringBuilderBase
	{
	public:
		using This = StringBuilderBase<CharT>;
		using Iterator = RandomContigIterator<CharT>;
		using CIterator = RandomContigIterator<const CharT>;
		using RevIterator = RevRandomContigIterator<CharT>;
		using CRevIterator = RevRandomContigIterator<const CharT>;

	public: // Access is discouraged
		Vector<CharT> data_raw;

	public:
		StringBuilderBase() : data_raw() {}
		StringBuilderBase(const StringView _string) : data_raw(_string.data()) {}
		StringBuilderBase(const CharT* const _data) : data_raw(ContiguousBufPtrEnd<const CharT>(_data, strlen(_data)), true) {}
		StringBuilderBase(const CharT* const _data, const Size _size) : data_raw(_data, _size) {}
		StringBuilderBase(const CharT* const _begin, const CharT* _end) : data_raw(_begin, _end) {}
		StringBuilderBase(const Size _size) : data_raw(_size) {}
		StringBuilderBase(const Size _size, const CharT _val) : data_raw(_size, _val) {}


		void reserve(const Size _size) { data_raw.reserve(_size); }

		Size size() const { return data().size(); }

		ContiguousBufPtrEnd<CharT> data() const { return data_raw.data_raw; }
		Iterator begin() { return data().begin(); }
		Iterator end() { return data().end(); }
		CIterator cbegin() const { return data().cbegin(); }
		CIterator cend() const { return data().cend(); }
		RevIterator rbegin() { return data().rbegin(); }
		RevIterator rend() { return data().rend(); }
		CRevIterator crbegin() const { return data().crbegin(); }
		CRevIterator crend() const { return data().crend(); }

		operator CChar* () const { return data(); }
		operator String () { return String(data_raw.data_raw, data_raw.size()); }
		Char& operator[] (Size i) { return *(data_raw.data_raw.begin_raw + i); }

		void push_back(const CharT& val)
		{
			data_raw.push_back(val);
		}

		void push_back(CharT&& val)
		{
			data_raw.push_back(val);
		}

		void resize(const Size size, const CharT& val)
		{
			data_raw.resize(size, val);
		}

		void clear()
		{
			data_raw.clear();
		}
	};


	using StringBuilder = StringBuilderBase<>;

}
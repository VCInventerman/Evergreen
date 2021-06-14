#pragma once

//#include "evergreen/common/alloc.h"
#include "evergreen/ContiguousBuf.h"
#include "evergreen/types.h"
#include "evergreen/Vector.h"

namespace evg
{

	template <typename CharT = Char, typename AllocatorT = std::allocator<CharT>>
	class StringBuilderBase
	{
	public:
		using This = StringBuilderBase;

		using value_type = CharT;
		using allocator_type = AllocatorT;
		using size_type = Size;
		using difference_type = Offset;
		using reference = CharT&;
		using const_reference = const CharT&;
		using iterator = RandomContigIterator<CharT>; // todo: unicode iterator
		using const_iterator = RandomContigIterator<const CharT>;
		using reverse_iterator = RevRandomContigIterator<CharT>;
		using const_reverse_iterator = RevRandomContigIterator<const CharT>;

	public: // Access is discouraged
		Vector<CharT> data_raw;

	public:
		StringBuilderBase() : data_raw() {}
		StringBuilderBase(const StringView _string) : data_raw(_string.range(), true) {}
		StringBuilderBase(const CharT* const _data) : data_raw(ContiguousBufPtrEnd<const CharT>(_data, strlen(_data)), true) {}
		StringBuilderBase(const CharT* const _data, const Size _size) : data_raw(_data, _size) {}
		StringBuilderBase(const CharT* const _begin, const CharT* _end) : data_raw(_begin, _end) {}
		StringBuilderBase(const Size _size) : data_raw(_size) {}
		StringBuilderBase(const Size _size, const CharT _val) : data_raw(_size, _val) {}
		StringBuilderBase(const const_iterator _begin, const const_iterator _end) : data_raw(_begin, _end) {}
		StringBuilderBase(const StringBuilderBase& _str) : data_raw(_str.data_raw) {}
		StringBuilderBase(StringBuilderBase&& _str) : data_raw(std::move(_str.data_raw)) {}
		StringBuilderBase& operator= (const StringBuilderBase& _str)
		{
			data_raw = _str.data_raw;
		}
		StringBuilderBase& operator= (StringBuilderBase&& _str)
		{
			data_raw = std::move(_str.data_raw);
		}
		This& operator= (const CharT* const _data)
		{
			data_raw.assign(ContiguousBufPtrEnd<const CharT>(_data, strlen(_data)));
			//data_raw = std::move(Vector<CharT>(ContiguousBufPtrEnd<const CharT>(_data, strlen(_data)), true));
			return *this;
		}
		


		void reserve(const Size _size) { data_raw.reserve(_size); }

		Size size() const { return range().size(); }

		CharT* data() const { return data_raw.data_raw.begin_raw; }
		ContiguousBufPtrEnd<CharT> range() const { return data_raw.data_raw; }
		iterator begin() { return range().begin(); }
		iterator end() { return range().end(); }
		const_iterator cbegin() const { return range().cbegin(); }
		const_iterator cend() const { return range().cend(); }
		reverse_iterator rbegin() { return range().rbegin(); }
		reverse_iterator rend() { return range().rend(); }
		const_reverse_iterator crbegin() const { return range().crbegin(); }
		const_reverse_iterator crend() const { return range().crend(); }

		operator const CharT* () const { return data(); }
		//operator String () { return String(data_raw.data_raw, data_raw.size()); }
		Char& operator[] (Size i) { return *(data_raw.data_raw.begin_raw + i); }

		friend std::ostream& operator<< (std::ostream& stream, const StringBuilderBase& rhs)
		{
			stream << rhs.data();
			return stream;
		}

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

		void ensureNullTerminated()
		{
			*end() = '\0';
		}

		This& replaceAll(const CharT elm, const CharT sub)
		{
			for (auto& c : *this)
			{
				if (c == elm)
					c = sub;
			}

			return *this;
		}

		This& operator+= (const ContiguousBufPtrEnd<const char> rhs)
		{
			data_raw.append(rhs.cbegin(), rhs.cend());
			return *this;
		}
		This& operator+= (const std::string& rhs)
		{
			data_raw.append(rhs.cbegin(), rhs.cend());
			return *this;
		}
		This& operator+= (const CharT* const rhs)
		{
			data_raw.append(rhs, rhs + strlen(rhs));
			return *this;
		}
		This& operator+= (const CharT rhs)
		{
			push_back(rhs);
			return *this;
		}
	};


	using StringBuilder = StringBuilderBase<>;

}
#pragma once

//#include "evergreen/common/alloc.h"
#include "evergreen/ContiguousBuf.h"
#include "evergreen/types.h"
#include "evergreen/Vector.h"

namespace evg
{

	template <typename CharT = char, typename AllocatorT = std::allocator<CharT>>
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

	protected: // Access is discouraged
		Vector<CharT> data_;

	public:
		StringBuilderBase() : data_() {}
		StringBuilderBase(const StringView _string) : data_(_string.range(), true) {}
		StringBuilderBase(const CharT* const _data) : data_(ContiguousBufPtrEnd<const CharT>(_data, strlen(_data)), true) {}
		StringBuilderBase(const CharT* const _data, const Size _size) : data_(_data, _size) {}
		StringBuilderBase(const CharT* const _begin, const CharT* _end) : data_(_begin, _end) {}
		StringBuilderBase(const Size _size) : data_(_size) {}
		StringBuilderBase(const Size _size, const CharT _val) : data_(_size, _val) {}
		StringBuilderBase(const const_iterator _begin, const const_iterator _end) : data_(_begin, _end) {}
		StringBuilderBase(const StringBuilderBase& _str) : data_(_str.data_) {}
		StringBuilderBase(StringBuilderBase&& _str) : data_(std::move(_str.data_)) {}
		StringBuilderBase& operator= (const StringBuilderBase& _str)
		{
			data_ = _str.data_;
		}
		StringBuilderBase& operator= (StringBuilderBase&& _str)
		{
			data_ = std::move(_str.data_);
		}
		This& operator= (const CharT* const _data)
		{
			data_.assign(ContiguousBufPtrEnd<const CharT>(_data, strlen(_data)));
			//data_ = std::move(Vector<CharT>(ContiguousBufPtrEnd<const CharT>(_data, strlen(_data)), true));
			return *this;
		}
		


		void reserve(const Size _size) { data_.reserve(_size); }

		Size size() const { return range().size(); }

		CharT* data() const { return data_.data(); }
		ContiguousBufPtrEnd<CharT> range() const { return data_.range(); }
		iterator begin() { return range().begin(); }
		iterator end() { return range().end(); }
		const_iterator cbegin() const { return range().cbegin(); }
		const_iterator cend() const { return range().cend(); }
		reverse_iterator rbegin() { return range().rbegin(); }
		reverse_iterator rend() { return range().rend(); }
		const_reverse_iterator crbegin() const { return range().crbegin(); }
		const_reverse_iterator crend() const { return range().crend(); }

		operator const CharT* () const { return data(); }
		//operator String () { return String(data_.data_, data_.size()); }
		char& operator[] (Size i) { return *(data_.data_.begin_ + i); }

		friend std::ostream& operator<< (std::ostream& stream, const StringBuilderBase& rhs)
		{
			stream << rhs.data();
			return stream;
		}

		void push_back(const CharT& val)
		{
			data_.push_back(val);
		}

		void push_back(CharT&& val)
		{
			data_.push_back(val);
		}

		void resize(const Size size, const CharT val)
		{
			data_.resize(size, val);
		}

		void resize(const Size size)
		{
			data_.resize(size, 0);
		}

		void clear()
		{
			data_.clear();
		}

		void ensureNullTerminated()
		{
			if (data_.sizeReserved() > data_.size() + 1)
			{
				*end() = '\0';
			}
			else
			{
				data_.reserve(data_.size() + 2);
				*end() = '\0';
			}
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

		template <typename T>
		This& operator+= (const T& rhs)
		{
			data_.append(rhs.cbegin(), rhs.cend());
			return *this;
		}

		This& operator+= (const CharT* rhs)
		{
			data_.append(rhs, rhs + strlen(rhs));
			return *this;
		}



		/*This& operator+= (const ContiguousBufPtrEnd<const char> rhs)
		{
			data_.append(rhs.cbegin(), rhs.cend());
			return *this;
		}
		This& operator+= (const std::string& rhs)
		{
			data_.append(rhs.cbegin(), rhs.cend());
			return *this;
		}
		This& operator+= (const Path& rhs)
		{
			data_.append(rhs.cbegin(), rhs.cend());
			return *this;
		}*/
		/*This& operator+= (const CharT* const rhs)
		{
			data_.append(rhs, rhs + strlen(rhs));
			return *this;
		}*/
		This& operator+= (const CharT rhs)
		{
			push_back(rhs);
			return *this;
		}
		This& operator+= (const unsigned char rhs)
		{
			push_back((CharT)rhs);
			return *this;
		}
		This& operator+= (const signed char rhs)
		{
			push_back((CharT)rhs);
			return *this;
		}
		This& operator+= (const UInt16 rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%u", rhs);
			resize(oldSize + written);
			return *this; 
		}
		This& operator+= (const UInt32 rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%hu", rhs);
			resize(oldSize + written);
			return *this;
		}
		This& operator+= (const UInt64 rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%llu", rhs);
			resize(oldSize + written);
			return *this;
		}
		This& operator+= (const Int16 rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%hd", rhs);
			resize(oldSize + written);
			return *this;
		}
		This& operator+= (const Int32 rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%d", rhs);
			resize(oldSize + written);
			return *this;
		}
		This& operator+= (const Int64 rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%lld", rhs);
			resize(oldSize + written);
			return *this;
		}
		This& operator+= (const Float rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%.6f", (double)rhs);
			resize(oldSize + written);
			return *this;
		}
		This& operator+= (const Double rhs)
		{
			Size oldSize = size();
			resize(size() + 10);
			Size written = snprintf(data() + oldSize, 10, "%.6f", rhs);
			resize(oldSize + written);
			return *this;
		}

		void invalidate() noexcept
		{
			data_.invalidate();
		}

		Size find(const CharT val) const noexcept
		{
			return std::basic_string_view<CharT>(data(), size()).find(val);
		}

		Size rfind(const CharT val) const noexcept
		{
			return std::basic_string_view<CharT>(data(), size()).rfind(val);
		}

		Size count(const CharT val) const noexcept
		{
			return stringCount(data(), data() + size(), val);
		}

		std::function<void()> deleter()
		{
			return data_.deleter();
		}
	};


	using StringBuilder = StringBuilderBase<>;

}
#pragma once

namespace evg
{

	template <typename T>
	class RandomContigIterator
	{
	public:
		using This = RandomContigIterator<T>;

		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = Offset;
		using pointer = T*;
		using reference = T&;

	public: // Access is discouraged
		T* ptr;

	public:
		RandomContigIterator() : ptr() {}
		RandomContigIterator(T* const _ptr) : ptr(_ptr) {}


		This& operator++ () { ++ptr; return *this; }
		This operator++ (int) { This ret = *this; ++(*this); return ret; }

		This& operator-- () { --ptr; return *this; }
		This operator-- (int) { This ret = *this; --(*this); return ret; }

		bool operator==(This lhs) const { return ptr == lhs.ptr; }
		bool operator!=(This lhs) const { return !(*this == lhs); }

		Int64 operator-(const This& rhs) const { return ptr - rhs.ptr; }
		Int64 operator+(const This& rhs) const { return ptr + rhs.ptr; }
		This operator-(const Size rhs) const { return ptr - rhs; }
		This operator+(const Size rhs) const { return ptr + rhs; }

		This& operator+=(const This& rhs) { ptr += rhs.ptr; return *this; }
		This& operator-=(const This& rhs) { ptr -= rhs.ptr; return *this; }
		This& operator+=(const Size rhs) { ptr += rhs; return *this; }
		This& operator-=(const Size rhs) { ptr -= rhs; return *this; }

		bool operator<(const This& rhs) { return ptr < rhs.ptr; }
		bool operator>(const This& rhs) { return ptr > rhs.ptr; }
		bool operator<=(const This& rhs) { return ptr <= rhs.ptr; }
		bool operator>=(const This& rhs) { return ptr >= rhs.ptr; }

		T& operator*() const noexcept { return *ptr; }
	};

	template <typename T>
	class RevRandomContigIterator
	{
	public:
		using This = RevRandomContigIterator<T>;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;

	public: // Access is discouraged
		T* ptr;

	public:
		RevRandomContigIterator() : ptr() {}
		RevRandomContigIterator(T* const _ptr) : ptr(_ptr) {}


		RevRandomContigIterator& operator++ () { --ptr; return *this; }
		RevRandomContigIterator operator++ (int) { This ret = *this; --(*this); return ret; }

		RevRandomContigIterator& operator-- () { ++ptr; return *this; }
		RevRandomContigIterator operator-- (int) { This ret = *this; ++(*this); return ret; }

		bool operator==(This lhs) const { return ptr == lhs.ptr; }
		bool operator!=(This lhs) const { return !(*this == lhs); }

		Int64 operator-(const This& rhs) const { return ptr + rhs.ptr; }
		Int64 operator+(const This& rhs) const { return ptr - rhs.ptr; }
		This& operator-(const Size& rhs) const { ptr += rhs; return *this; }
		This& operator+(const Size& rhs) const { ptr -= rhs; return *this; }

		T& operator*() { return *ptr; }
	};

	// Templated contiguous range - Only requires one int for counter
	template <Int begin_, Int end_, typename CountT = Int>
	class TContiguousRange
	{
	public:
		class Iterator
		{
		public:
			CountT counter;

			Iterator(const Int _counter = begin_) : counter(_counter) {}

			Iterator& operator++ () { ++counter; return *this; }
			Iterator operator++(int) { Iterator ret = *this; ++(*this); return ret; }

			Iterator& operator-- () { --counter; return *this; }
			Iterator operator--(int) { Iterator ret = *this; --(*this); return ret; }

			bool operator==(Iterator lhs) const { return counter == lhs.counter; }
			bool operator!=(Iterator lhs) const { return !(*this == lhs); }

			CountT operator*() { return counter; }
		};

		Iterator begin() { return begin_; }
		Iterator end() { return end_; /*return end_ >= begin_ ? end_ + 1 : end_ - 1;*/ }
	};

	template <typename CountT = Int>
	class ContiguousRange
	{
	public:
		CountT begin_;
		CountT end_;

		class Iterator
		{
		public:
			CountT counter;

			Iterator(const CountT _counter) : counter(_counter) {}

			Iterator& operator++ () { ++counter; return *this; }
			Iterator operator++(int) { Iterator ret = *this; ++(*this); return ret; }

			Iterator& operator-- () { --counter; return *this; }
			Iterator operator--(int) { Iterator ret = *this; --(*this); return ret; }

			bool operator==(Iterator lhs) const { return counter == lhs.counter; }
			bool operator!=(Iterator lhs) const { return !(*this == lhs); }

			CountT operator*() { return counter; }
		};

		ContiguousRange(const CountT _begin_raw, const CountT _end_raw) : begin_(_begin_raw), end_(_end_raw) {}
		ContiguousRange(const CountT _end_raw) : begin_(0), end_(_end_raw) {}

		Iterator begin() { return begin_; }
		Iterator end() { return end_; /*return end_ >= begin_ ? end_ + 1 : end_ - 1;*/ }
	};

	using Range = ContiguousRange<>;




	//todo: c# like interfaces like begin() and begin.raw
	//todo: arguments in multiple orders
	//todo: iterator classes
	//todo: fake private that only works with scope and warnings
	// Interface ContiguousBuf implements begin and end
	/*template <typename T>
	class ContiguousBuf
	{
	public:
		T* getBegin();
		T* getEnd();
	};*/

	template <typename T>
	class ContiguousBufPtrEnd
	{
	public:
		using This = ContiguousBufPtrEnd<T>;

		using value_type = T;
		using size_type = Size;
		using difference_type = Offset;
		using reference = T&;
		using const_reference = const T&;
		using iterator = RandomContigIterator<T>;
		using const_iterator = RandomContigIterator<const T>;
		using reverse_iterator = RevRandomContigIterator<T>;
		using const_reverse_iterator = RevRandomContigIterator<const T>;

	public:
		T* begin_;
		T* end_;

	public:
		constexpr ContiguousBufPtrEnd() noexcept : begin_(nullptr), end_(nullptr) {}
		//ContiguousBufPtrEnd(const ContiguousBufPtrEnd& _data) : begin_(_data.begin_), end_(_data.end_) {}
		//ContiguousBufPtrEnd(ContiguousBufPtrEnd&& _data) : begin_(_data.begin_), end_(_data.end_) {}
		constexpr ContiguousBufPtrEnd(T* const _begin, T* const _end) noexcept : begin_(_begin), end_(_end) {}
		constexpr ContiguousBufPtrEnd(T* const _begin, const Size _size) noexcept : begin_(_begin), end_(_begin + _size) {}
		//ContiguousBufPtrEnd(const Size _size) : begin_(new T[_size]), end_(begin_ + _size) {}
		//ContiguousBufPtrEnd(const ContiguousBufPtrEnd<std::remove_const<T>> rhs) : begin_(rhs.begin_), end_(rhs.end_) {}


		T& at(Size index) const noexcept
		{
			return begin_[index];
		}
		//Ret<T&> sat(Size index)
		//{
		//	return index > end_ - begin_ ? begin_[index] : Error("Invalid contiguous buffer index");
		//}

		Size size() const noexcept
		{
			return end_ - begin_;
		}

		T* data() const noexcept { return begin_; }
		iterator begin() const noexcept { return begin_; }
		iterator end() const noexcept { return end_; }
		const_iterator cbegin() const noexcept { return begin_; }
		const_iterator cend() const noexcept { return end_; }
		reverse_iterator rbegin() const noexcept { return end_; }
		reverse_iterator rend() const noexcept { return begin_; }
		const_reverse_iterator crbegin() const noexcept { return end_; }
		const_reverse_iterator crend() const noexcept { return begin_; }

		bool valid() const noexcept
		{
			return (begin_ != nullptr) && (end_ != nullptr) && (begin_ <= end_);
		}

		operator T* () const noexcept { return begin_; }
		operator ContiguousBufPtrEnd<const T>() const noexcept { return { begin_, end_ }; }

		void invalidate() noexcept
		{
			begin_ = nullptr;
			end_ = nullptr;
		}
	};


	/*template <typename T, typename SizeT = Size>
	class ContiguousBufPtrSize
	{
	public:
		T* begin_;
		SizeT size_raw;

		ContiguousBufPtrSize() = default;
		ContiguousBufPtrSize(T* const _begin, T* const _end) : begin_(_begin), size_raw(_end - _begin) {}
		ContiguousBufPtrSize(T* const _begin, const SizeT _size) : begin_(_begin), size_raw(_size) {}
	};

	template <typename T>
	class ContiguousBufPtrUnbounded
	{
	public:
		T* data_;

		ContiguousBufPtrUnbounded() = default;
		ContiguousBufPtrUnbounded(T* const _data) : data_(_data) {}
	};

	template <typename T>
	class ContiguousBufDataEnd
	{
	public:
		T* end;
		T data[];
	};

	template <typename T, typename SizeT = Size>
	class ContiguousBufDataSize
	{
	public:
		SizeT size;
		T data[];
	};*/


	/*
	template<long FROM, long TO>
	class Range {
	public:
		class iterator {
			long num = FROM;
		public:
			iterator(long _num = 0) : num(_num) {}
			iterator& operator++() { num = TO >= FROM ? num + 1 : num - 1; return *this; }
			iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
			bool operator==(iterator other) const { return num == other.num; }
			bool operator!=(iterator other) const { return !(*this == other); }
			long operator*() { return num; }
			// iterator traits
			using difference_type = long;
			using value_type = long;
			using pointer = const long*;
			using reference = const long&;
			using iterator_category = std::forward_iterator_tag;
		};
		iterator begin() { return FROM; }
		iterator end() { return TO >= FROM ? TO + 1 : TO - 1; }
	};
	*/





	template <typename CharT = char>
	class UnicodeIterator
	{
	public:
		using This = UnicodeIterator;
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = UniChar;
		using difference_type = PtrDiff;

	public: // Access is discouraged
		CharT* ptr;

	public:


	};
}
#pragma once
//#include "evergreen/common/alloc.h"

//todo: c# like interfaces like begin() and begin.raw
//todo: arguments in multiple orders
//todo: iterator classes
//todo: fake private that only works with scope and warnings
// Interface ContiguousBuf implements begin and end
template <typename T>
class ContiguousBuf
{
public:
	T* getBegin();
	T* getEnd();
};

//todo: ownership and auto-delete
template <typename T>
class ContiguousBufPtrEnd
{
public:
	T* begin_raw;
	T* end_raw;

	ContiguousBufPtrEnd() = default;
	ContiguousBufPtrEnd(T* const _begin, T* const _end) : begin_raw(_begin), end_raw(_end) {}
	ContiguousBufPtrEnd(T* const _begin, const Size _size) : begin_raw(_begin), end_raw(_begin + _size) {}
	ContiguousBufPtrEnd(const Size _size) : begin_raw(new Char[_size]), end_raw(begin_raw + _size) {}

	T& at(Size index)
	{
		return begin_raw[index];
	}
	//RetPair<T&> sat(Size index)
	//{
	//	return index > end_raw - begin_raw ? begin_raw[index] : Error("Invalid contiguous buffer index");
	//}

	Size size()
	{
		return end_raw - begin_raw;
	}

	class Iterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

		T* ptr;

		Iterator(T* const _ptr) : ptr(_ptr) {}

		Iterator& operator++ () { ++ptr; return *this; }
		Iterator operator++ (int) { Iterator ret = *this; ++(*this); return ret; }

		Iterator& operator-- () { --ptr; return *this; }
		Iterator operator-- (int) { Iterator ret = *this; --(*this); return ret; }

		bool operator==(Iterator lhs) const { return ptr == lhs.ptr; }
		bool operator!=(Iterator lhs) const { return !(*this == lhs); }

		I64 operator-(const Iterator& rhs) const { return ptr - rhs.ptr; }
		I64 operator+(const Iterator& rhs) const { return ptr + rhs.ptr; }

		T& operator*() { return *ptr; }
	};


	Iterator begin()
	{
		return begin_raw;
	}
	Iterator end()
	{
		return end_raw;
	}


	class RevIterator
	{
	public:
		T* ptr;

		RevIterator(T* const _ptr) : ptr(_ptr) {}

		RevIterator& operator++ () { --ptr; return *this; }
		RevIterator operator++ (int) { RevIterator ret = *this; --(*this); return ret; }

		RevIterator& operator-- () { ++ptr; return *this; }
		RevIterator operator-- (int) { RevIterator ret = *this; ++(*this); return ret; }

		bool operator==(RevIterator lhs) const { return ptr == lhs.ptr; }
		bool operator!=(RevIterator lhs) const { return !(*this == lhs); }

		T& operator*() { return *ptr; }
	};


	RevIterator rbegin()
	{
		return end_raw;
	}
	RevIterator rend()
	{
		return begin_raw;
	}

	operator T* () { return begin_raw; }
};

template <typename T, typename SizeT = Size>
class ContiguousBufPtrSize
{
public:
	T* begin_raw;
	SizeT size_raw;

	ContiguousBufPtrSize() = default;
	ContiguousBufPtrSize(T* const _begin, T* const _end) : begin_raw(_begin), size_raw(_end - _begin) {}
	ContiguousBufPtrSize(T* const _begin, const SizeT _size) : begin_raw(_begin), size_raw(_size) {}
};

template <typename T>
class ContiguousBufPtrUnbounded
{
public:
	T* data_raw;

	ContiguousBufPtrUnbounded() = default;
	ContiguousBufPtrUnbounded(T* const _data) : data_raw(_data) {}
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
};


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

// Templated contiguous range - Only requires one int for counter
template <Int begin_raw, Int end_raw, typename CountT = Int>
class TContiguousRange
{
public:
	class Iterator
	{
	public:
		CountT counter;

		Iterator(const Int _counter = begin_raw) : counter(_counter) {}
		
		Iterator& operator++ () { ++counter; return *this; }
		Iterator operator++(int) { Iterator ret = *this; ++(*this); return ret; }

		Iterator& operator-- () { --counter; return *this; }
		Iterator operator--(int) { Iterator ret = *this; --(*this); return ret; }

		bool operator==(Iterator lhs) const { return counter == lhs.counter; }
		bool operator!=(Iterator lhs) const { return !(*this == lhs); }

		CountT operator*() { return counter; }
	};

	Iterator begin() { return begin_raw; }
	Iterator end() { return end_raw; /*return end_raw >= begin_raw ? end_raw + 1 : end_raw - 1;*/ }
};

template <typename CountT = Int>
class ContiguousRange
{
public:
	CountT begin_raw;
	CountT end_raw;

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

	ContiguousRange(const CountT _begin_raw, const CountT _end_raw) : begin_raw(_begin_raw), end_raw(_end_raw) {}

	Iterator begin() { return begin_raw; }
	Iterator end() { return end_raw; /*return end_raw >= begin_raw ? end_raw + 1 : end_raw - 1;*/ }
};

using Range = ContiguousRange<>;
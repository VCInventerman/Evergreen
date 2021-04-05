#pragma once

#include "evergreen/common/types.h"
#include "evergreen/common/ContiguousBuf.h"

//todo: interfaces: ex. Vector implements Array
//todo: template by whether or not vector owns memory instead of appending View


namespace evg
{


	template <typename T, typename AllocatorT = std::allocator<T>>
	class Vector
	{
	public:
		using This = typename Vector<T>;
		using Iterator = typename RandomContigIterator<T>;
		using CIterator = typename RandomContigIterator<const T>;
		using RevIterator = typename RevRandomContigIterator<T>;
		using CRevIterator = typename RevRandomContigIterator<const T>;

		using value_type = typename T;
		using allocator_type = typename AllocatorT;
		using size_type = Size;
		using difference_type = Offset;
		using reference = typename T&;
		using const_reference = typename const T&;
		using iterator = typename Iterator;
		using const_iterator = typename CIterator;
		using reverse_iterator = typename RevIterator;
		using const_reverse_iterator = typename CRevIterator;

	public: // Access is discouraged
		ContiguousBufPtrEnd<T> data_raw;
		T* reserved_raw; // Pointer to end of reserved memory

		[[no_unique_address]] AllocatorT allocator;

	public:
		Vector() : data_raw(), reserved_raw() {}
		Vector(const ContiguousBufPtrEnd<const T> _data, const bool _copy = true) : data_raw()
		{
			if (_copy)
			{
				Char* mem = new Char[_data.size()];
				std::copy(_data.cbegin(), _data.cend(), mem);
				data_raw = ContiguousBufPtrEnd(mem, _data.size());
			}
			else // Move ownership
			{
				data_raw = { const_cast<T*>(_data.begin_raw), _data.size() };
			}
		}
		Vector(const Size _size) : data_raw(new Char[_size], _size) {}

		~Vector()
		{
			if (data_raw.begin_raw != nullptr)
			{
				delete[] data_raw.begin_raw;
			}
		}

		void reserve(const Size _size)
		{
			if (_size > sizeReserved())
			{
				// Create a new memory region and move all objects to it, then set it as the object's memory and delete old
				T* mem = allocator.allocate(_size);

				if (mem == nullptr)
				{
					//todo: out of memory error
				}

				for (Int i : Range(size()))
				{
					mem[i] = std::move((*this)[i]);
				}

				T* beginOld = data().begin_raw;

				data_raw = ContiguousBufPtrEnd<T>(mem, _size);
				reserved_raw = mem + _size;

				allocator.deallocate(beginOld, 1);
			}
		}

		Size size() const { return data_raw.size(); }
		Size sizeReserved() const { return reserved_raw - data_raw.begin_raw; }

		ContiguousBufPtrEnd<T>& data() { return data_raw; }
		Iterator begin() { return data().begin(); }
		Iterator end() { return data().end(); }
		CIterator cbegin() const { return data_raw.cbegin(); }
		CIterator cend() const { return data_raw.cend(); }
		RevIterator rbegin() { return data().rbegin(); }
		RevIterator rend() { return data().rend(); }
		CRevIterator crbegin() const { return data_raw.crbegin(); }
		CRevIterator crend() const { return data_raw.crend(); }

		void push_back(const T& val)
		{
			reserve(size() + 1);

			data()[size()] = val;

			++(data().end_raw);
		}

		void push_back(T&& val)
		{
			reserve(size() + 1);

			data()[size()] = std::move(val);

			++(data().end_raw);
		}

		T& operator[](const Size i) { return data_raw[i]; }
	};

	template<typename T, typename HashFT = std::hash<T>>
	struct ArrayHash
	{
		Hash operator()(const T& val)
		{
			Hash seed = val.size();
			HashFT hasher;

			for (auto& i : val)
			{
				seed ^= HashFT(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
		}
	};
}
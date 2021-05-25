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
		using This = Vector<T>;

		using value_type = T;
		using allocator_type = AllocatorT;
		using size_type = Size;
		using difference_type = Offset;
		using reference = T&;
		using const_reference = const T&;
		using iterator = RandomContigIterator<T>;
		using const_iterator = RandomContigIterator<const T>;
		using reverse_iterator = RevRandomContigIterator<T>;
		using const_reverse_iterator = RevRandomContigIterator<const T>;

		
		constexpr static Size SMALLEST_RESERVE = 32;

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
				T* mem = new T[_data.size()];
				std::copy(_data.cbegin(), _data.cend(), mem);
				data_raw = ContiguousBufPtrEnd(mem, _data.size());
			}
			else // Move ownership
			{
				data_raw = { const_cast<T*>(_data.begin_raw), _data.size() };
			}
		}
		Vector(const Size _size) : data_raw(new T[_size], _size) {}
		Vector(const Size _size, const T& val) : data_raw(new T[_size], _size)
		{
			std::fill(begin(), end(), val);
		}

		~Vector()
		{
			if (data_raw.begin_raw != nullptr)
			{
				std::cout << "DELETE VECTOR\n";
				delete[] data_raw.begin_raw;
			}
		}

		// The maximum extra size allocated is 1024 bytes
		void reserve(const Size _size)
		{
			if (_size > sizeReserved())
			{
				Size newSize = std::max(((_size < 32) * SMALLEST_RESERVE), _size * 4);

				if (_size > 1024)
				{
					newSize = (_size - (_size % 1024)) + 1024;
				}


				// Create a new memory region and move all objects to it, then set it as the object's memory and delete old
				T* mem = allocator.allocate(newSize);

				if (mem == nullptr)
				{
					//todo: out of memory error
					std::cout << "OUT OF MEMORY ERROR";
				}

				for (Size i : ContiguousRange<Size>(size()))
				{
					mem[i] = std::move((*this)[i]);
				}

				T* beginOld = data().begin_raw;

				data_raw = ContiguousBufPtrEnd<T>(mem, data_raw.size());
				reserved_raw = mem + newSize;

				static Size count = 0;
				++count;
				std::cout << "RESIZE " << count << '\n';

				allocator.deallocate(beginOld, 1);
			}
		}

		Size size() const { return data_raw.size(); }
		Size sizeReserved() const { return reserved_raw - data_raw.begin_raw; }

		ContiguousBufPtrEnd<T>& data() { return data_raw; }
		iterator begin() { return data().begin(); }
		iterator end() { return data().end(); }
		const_iterator cbegin() const { return data_raw.cbegin(); }
		const_iterator cend() const { return data_raw.cend(); }
		reverse_iterator rbegin() { return data().rbegin(); }
		reverse_iterator rend() { return data().rend(); }
		const_reverse_iterator crbegin() const { return data_raw.crbegin(); }
		const_reverse_iterator crend() const { return data_raw.crend(); }

		void push_back(const T& val)
		{
			reserve(size() + 1); //todo: increase size in larger increments

			data()[size()] = val;

			++(data().end_raw);
		}

		void push_back(T&& val)
		{
			reserve(size() + 1);

			data()[size()] = std::move(val);

			++(data().end_raw);
		}

		void resize(const Size _size, const T& val)
		{
			reserve(_size);
			data_raw = ContiguousBufPtrEnd(data().begin_raw, _size);
			std::fill(begin(), end(), val);
		}

		void clear()
		{
			std::destroy(begin(), end());
			data_raw.end_raw = data_raw.begin_raw;
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
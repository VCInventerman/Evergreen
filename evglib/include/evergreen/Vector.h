#pragma once

#include "evergreen/types.h"
#include "evergreen/ContiguousBuf.h"

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

	protected: // Access is discouraged
		ContiguousBufPtrEnd<T> data_;
		T* reserved_; // Pointer to end of reserved memory

		Size orgReserved = 0;

		[[no_unique_address]] AllocatorT allocator;

	public:
		Vector() : data_(), reserved_() {}
		Vector(const ContiguousBufPtrEnd<const T> _data, const bool _copy = true)
		{
			if (_copy)
			{
				T* mem = allocator.allocate(_data.size());
				std::copy(_data.cbegin(), _data.cend(), mem);
				data_ = ContiguousBufPtrEnd(mem, _data.size());
				reserved_ = data_.end_;
			}
			else // Move ownership
			{
				data_ = { const_cast<T*>(_data.begin_), _data.size() };
				reserved_ = data_.end_;
			}
		}
		Vector(const Size _size) : data_(allocator.allocate(_size), _size), reserved_(data_.end_) {}
		Vector(const Size _size, const T& val) : data_(allocator.allocate(_size), _size), reserved_(data_.end_)
		{
			std::fill(begin(), end(), val);
		}
		Vector(const const_iterator _begin, const const_iterator _end) : reserved_()
		{
			append(_begin, _end);
		}
		constexpr Vector(std::initializer_list<T> _data) : data_(), reserved_()
		{
			reserve(_data.size());
			
			for (auto& i : _data)
			{
				push_back(std::move(i));
			}
		}
		Vector(const Vector& rhs) { new (this) Vector(rhs.data_, true); } // Copy constructor
		Vector(Vector&& rhs) 
		{ 
			data_ = rhs.data_;
			reserved_ = rhs.reserved_;

			rhs.data_ = {}; 
			rhs.reserved_ = nullptr; 
		} // Move constructor
		Vector& operator= (const Vector& rhs)  // Copy constructor
		{
			new (this) Vector(ContiguousBufPtrEnd<const T>(rhs.data_.begin_, rhs.data_.end_), true);
			return *this;
		}
		Vector& operator= (Vector&& rhs)  // Move constructor
		{ 
			new (this) Vector(ContiguousBufPtrEnd<const T>(rhs.data_.begin_, rhs.data_.end_), false);
			rhs.data_ = {};
			rhs.reserved_ = nullptr;
			return *this; 
		}
		Vector& operator= (std::initializer_list<T> _data)
		{
			data_ = {};
			reserved_ = nullptr;

			reserve(_data.size());

			for (auto& i : _data)
			{
				push_back(std::move(i));
			}
		}

		~Vector()
		{
			if (data_.begin_ != nullptr)
			{
				//std::cout << "DELETE VECTOR " << size() << ' ' << sizeReserved() << " \n";
				allocator.deallocate(data(), sizeReserved());
				data_.begin_ = nullptr;
				data_.end_ = nullptr;
				reserved_ = nullptr;
			}
		}

		// Allocate additional space, takes number of elements
		// The maximum extra size allocated is 1024 bytes
		void reserve(const Size _size)
		{
			if (_size > 1024000000000)
			{
				std::cout << "evg::Vector reserve() called with too large of memory allocation!\n";
				throw std::bad_alloc();
			}

			if (_size + 2 > sizeReserved())
			{
				Size newSize = std::max(((_size < 32) * SMALLEST_RESERVE), _size * 4) + 1;
				if (newSize <= sizeReserved())
				{
					debugBreak();
				}

				if (_size > 1024)
				{
					newSize = (_size - (_size % 1024)) + 1024;
				}


				// Create a new memory region and move all objects to it, then set it as the object's memory and delete old
				T* mem = allocator.allocate(newSize);

				if (mem == nullptr)
				{
					//todo: out of memory error
					std::cout << "OUT OF MEMORY ERROR\n";
					throw std::bad_alloc();
				}

				for (Size i : ContiguousRange<Size>(size()))
				{
					mem[i] = std::move((*this)[i]);
				}

				ContiguousBufPtrEnd<T> oldMem = { data(), reserved_ };

				data_ = ContiguousBufPtrEnd<T>(mem, data_.size());
				reserved_ = mem + newSize;
				orgReserved = sizeReserved();

				allocator.deallocate(oldMem.data(), oldMem.size());
				
			}
		}

		Size size() const { return data_.size(); }
		Size sizeReserved() const { return reserved_ - data_.begin_; }

		T* data() const noexcept { return data_; }

		// Get object that can be iterated over
		ContiguousBufPtrEnd<T>& range() noexcept { return data_; }
		const ContiguousBufPtrEnd<T>& range() const noexcept { return data_; }


		iterator begin() { return range().begin(); }
		iterator end() { return range().end(); }
		const_iterator cbegin() const { return range().cbegin(); }
		const_iterator cend() const { return range().cend(); }
		reverse_iterator rbegin() { return range().rbegin(); }
		reverse_iterator rend() { return range().rend(); }
		const_reverse_iterator crbegin() const { return range().crbegin(); }
		const_reverse_iterator crend() const { return range().crend(); }
		T& back() { return *(end() - 1); }

		void push_back(const T& val)
		{
			reserve(size() + 1); //todo: increase size in larger increments

			iterator itr = end();
			*itr = val;

			++(range().end_);
		}

		void push_back(T&& val)
		{
			reserve(size() + 1);

			range()[size()] = std::move(val);

			++(range().end_);
		}

		template< class... ArgsT >
		void emplace_back(ArgsT&&... args)
		{
			Size s1 = size();
			resize(size() + 1);
			Size s2 = size();
			std::allocator_traits<AllocatorT>::construct(allocator, &*end() - 1, ::std:: forward<ArgsT>(args)...);
			++(range().end_);
		}



		// Resize and leave any additional elements uninitialized
		void resize(const Size _size)
		{
			reserve(_size);
			data_ = ContiguousBufPtrEnd<T>(data(), _size);
		}

		void resize(const Size _size, const T& val)
		{
			Size oldSize = size();
			reserve(_size);
			data_ = ContiguousBufPtrEnd<T>(data(), _size);
			if (oldSize < size())
			{
				std::fill(begin() + oldSize, end(), val);
			}
		}

		void clear()
		{
			std::destroy(begin(), end());
			data_.end_ = data_.begin_;
		}

		// Make vector invalid
		// Different from the destructor, assumes pointers to data were kept
		void invalidate()
		{
			data_ = { nullptr, nullptr };
			reserved_ = nullptr;
		}

		T& operator[](const Size i) { return data_[i]; }

		void assign(const ContiguousBufPtrEnd<const T> rhs)
		{
			resize(rhs.size());
			std::copy(rhs.cbegin(), rhs.cend(), data_.begin());
		}

		void append(const T& elm)
		{
			push_back(elm);
		}
		void append(T&& elm)
		{
			push_back(elm);
		}

		template<typename IterT>
		void append(IterT _begin, IterT _end)
		{
			// Reserve space ahead of time if size of range can be determined trivially
			if constexpr (std::is_base_of<std::random_access_iterator_tag, typename std::iterator_traits<IterT>::iterator_category>::value)
			{
				reserve(size() + std::distance(_begin, _end));
			}

			for (auto i = _begin; i != _end; ++i)
			{
				push_back(*i);
			}
		}

		std::function<void()> deleter()
		{		
			T* allocData = data();
			Size allocSize = sizeReserved();
			return [allocData, allocSize]() {std::allocator<T> fakeAllocator; fakeAllocator.deallocate(allocData, allocSize); };
		}
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
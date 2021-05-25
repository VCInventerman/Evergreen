#pragma once

#include <string_view>

namespace evg
{
	// View of constant string data terminated with a null character
	class cstring_view
	{
	public:
		const char* data;

		cstring_view() = default;
		cstring_view(const char* const _data) : data(_data) {}

		inline operator const char* () const { return data; }
		explicit operator std::string_view() const { return std::string_view(data, size()); }

		void set(const char* const _data)
		{
			data = _data;
		}

		size_t size(const size_t max = SIZE_MAX) const
		{
			size_t count = 0;
			for (; (count[data] != 0) && (count < max); ++count);
			return count;
		}
	};

	// String that is terminated through a null character
	class cstring
	{
	public:
		char* data;

		cstring() = default;
		cstring(char* const _data) : data(_data) {}

		inline operator char* () const { return data; }
		explicit operator std::string_view() const { return std::string_view(data, size()); }

		void set(char* const _data)
		{
			data = _data;
		}

		size_t size(const size_t max = SIZE_MAX) const
		{
			size_t count = 0;
			for (; (count[data] != 0) && (count < max); ++count);
			return count;
		}
	};

	class string_range_view
	{
	public:
		const char* begin;
		const char* end;

		string_range_view() = default;
		string_range_view(const char* const _begin, const char* const _end) : begin(_begin), end(_end) {}

		explicit operator std::string_view() const { return std::string_view(begin, size()); }

		void set(const char* const _begin, const char* const _end)
		{
			begin = _begin;
			end = _end;
		}

		size_t size() const
		{
			return  end - begin;
		}
	};

	class string_range
	{
	public:
		char* begin;
		char* end;

		string_range() = default;
		string_range(char* const _begin, char* const _end) : begin(_begin), end(_end) {}

		explicit operator std::string_view() const { return std::string_view(begin, size()); }

		void set(char* const _begin, char* const _end)
		{
			begin = _begin;
			end = _end;
		}

		size_t size() const
		{
			return  end - begin;
		}
	};
}
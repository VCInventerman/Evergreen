#include <string_view>
#include <string>

// String that is terminated through a null character
class StringC
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

// View of constant string data terminated with a null character
class cStringC
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
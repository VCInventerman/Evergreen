#pragma once

// String that is terminated through a null character
class CString
{
public:
	char* data;

	CString() = default;
	CString(char* const _data) : data(_data) {}

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
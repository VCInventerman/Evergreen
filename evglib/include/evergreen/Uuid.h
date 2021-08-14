#pragma once

namespace evg
{
	/*
	class Uuid
	{
	public:
		using This = Uuid;


	protected:
#if defined(__SIZEOF_INT128__)
		unsigned __int128 data_;
#else
		UInt64 data_;
#endif


	public:

#if defined(__SIZEOF_INT128__)
		bool operator< (const This& rhs) const
		{
			return data_ < rhs.data_;
		}
#else
		bool operator< (const This& rhs) const
		{
			
			return false;
		}
#endif


	};
	*/
}
















#if !defined(GUID_DEFINED)
#define GUID_DEFINED
struct GUID {
	uint32_t Data1;
	uint16_t Data2;
	uint16_t Data3;
	uint8_t Data4[8];
};
#endif

namespace guid_parse
{
	namespace details
	{
		constexpr const size_t short_guid_form_length = 36;	// XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
		constexpr const size_t long_guid_form_length = 38;	// {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}

															//
		constexpr uint8_t parse_hex_digit(const char c)
		{
			using namespace std::string_literals;
			return
				('0' <= c && c <= '9')
				? c - '0'
				: ('a' <= c && c <= 'f')
				? 10 + c - 'a'
				: ('A' <= c && c <= 'F')
				? 10 + c - 'A'
				:
				throw std::domain_error{ "invalid character in GUID"s };
		}

		constexpr uint8_t parse_hex_uint8_t(const char* ptr)
		{
			return (parse_hex_digit(ptr[0]) << 4) + parse_hex_digit(ptr[1]);
		}

		constexpr uint16_t parse_hex_uint16_t(const char* ptr)
		{
			return (parse_hex_uint8_t(ptr) << 8) + parse_hex_uint8_t(ptr + 2);
		}

		constexpr uint32_t parse_hex_uint32_t(const char* ptr)
		{
			return (parse_hex_uint16_t(ptr) << 16) + parse_hex_uint16_t(ptr + 4);
		}

		constexpr GUID parse_guid(const char* begin)
		{
			return GUID{
				parse_hex_uint32_t(begin),
				parse_hex_uint16_t(begin + 8 + 1),
				parse_hex_uint16_t(begin + 8 + 1 + 4 + 1),
				{
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2 + 2 + 2),
					parse_hex_uint8_t(begin + 8 + 1 + 4 + 1 + 4 + 1 + 2 + 2 + 1 + 2 + 2 + 2 + 2 + 2)
				}

			};
		}

		constexpr GUID make_guid_helper(const char* str, size_t N)
		{
			using namespace std::string_literals;
			using namespace details;

			return (!(N == long_guid_form_length || N == short_guid_form_length))
				? throw std::domain_error{ "String GUID of the form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} or XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX is expected"s }
				: (N == long_guid_form_length && (str[0] != '{' || str[long_guid_form_length - 1] != '}'))
				? throw std::domain_error{ "Missing opening or closing brace"s }

			: parse_guid(str + (N == long_guid_form_length ? 1 : 0));
		}


		template<size_t N>
		constexpr GUID make_guid(const char(&str)[N])
		{
			return make_guid_helper(str, N - 1);
		}
	}
	using details::make_guid;

	namespace literals
	{
		constexpr GUID operator "" _guid(const char* str, size_t N)
		{
			using namespace details;
			return make_guid_helper(str, N);
		}
	}
}

namespace evg
{
	using namespace guid_parse::literals;

	// it is probably a poor idea to use this without ensuring it is inline
	inline const char* uuidToString(const Uuid& uuid)
	{
		char* result = (char*)alloca(37);

		Size i = 0;
		for (Uuid::const_iterator it_data = uuid.begin(); it_data != uuid.end(); ++it_data, ++i) {
			const Size hi = ((*it_data) >> 4) & 0x0F;
			result[i] = boost::uuids::detail::to_char(hi);

			const size_t lo = (*it_data) & 0x0F;
			result[i] = boost::uuids::detail::to_char(lo);

			if (i == 3 || i == 5 || i == 7 || i == 9) {
				result += '-';
			}
		}

		result[36] = '\0';

		return result;

	}
}



namespace guid_parse
{
	namespace details {

		template<typename T>
		constexpr uint8_t get_nth_byte_of(T data, size_t pos) {
			return ((uint8_t)((data >> (8 * pos)) & 0xff));
		}

		constexpr boost::uuids::uuid uuid_creator(GUID guid) {
			return
				boost::uuids::uuid
			{ get_nth_byte_of(guid.Data1, 3), get_nth_byte_of(guid.Data1, 2), get_nth_byte_of(guid.Data1, 1), get_nth_byte_of(guid.Data1, 0)
				, get_nth_byte_of(guid.Data2, 1), get_nth_byte_of(guid.Data2, 0)
				, get_nth_byte_of(guid.Data3, 1), get_nth_byte_of(guid.Data3, 0)
				, guid.Data4[0], guid.Data4[1],guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]
			};
		}

		constexpr boost::uuids::uuid make_uuid_helper(const char* str, size_t N)
		{
			using namespace std::string_literals;
			using namespace details;

			return
				uuid_creator(
					(!(N == long_guid_form_length || N == short_guid_form_length))
					? throw std::domain_error{ "String GUID of the form {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} or XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX is expected"s }
					: (N == long_guid_form_length && (str[0] != '{' || str[long_guid_form_length - 1] != '}'))
					? throw std::domain_error{ "Missing opening or closing brace"s }

					: parse_guid(str + (N == long_guid_form_length ? 1 : 0))
				);
		}

		template<size_t N>
		constexpr boost::uuids::uuid make_uuid(const char(&str)[N])
		{
			return make_uuid_helper(str, N - 1);
		}
	}

	using details::make_uuid;

	namespace literals
	{
		constexpr boost::uuids::uuid operator "" _uuid(const char* str, size_t N)
		{
			using namespace details;
			return make_uuid_helper(str, N);
		}
	}
}
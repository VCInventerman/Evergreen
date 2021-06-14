#pragma once

#include "evergreen/types.h"

namespace evg
{
	// Variable length max 32 bit integer for positive values
	namespace vuint32
	{
		void encode(const uint32_t& num, std::vector<char>& buf)
		{
			if (num < (1 << 7)) // <128
			{
				buf.push_back((uint8_t)num);
			}
			else if (num < (1 << 14)) // <16,384
			{
				uint8_t byte;

				byte = (num >> 7) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = num & ((1 << 7) - 1); // AND with bit mask 0111 1111
				buf.push_back(byte);
			}
			else if (num < (1 << 21)) // <2,097,152
			{
				uint8_t byte;

				byte = (num >> 14) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = (num >> 7) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = num & ((1 << 7) - 1); // AND with bit mask 0111 1111
				buf.push_back(byte);
			}
			else if (num < (1 << 28)) // <268,435,456
			{
				uint8_t byte;

				byte = (num >> 21) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = (num >> 14) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = (num >> 7) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = num & ((1 << 7) - 1); // AND with bit mask 0111 1111
				buf.push_back(byte);
			}
			else // <4,294,967,296
			{
				uint8_t byte;

				byte = (num >> 28) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = (num >> 21) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = (num >> 14) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = (num >> 7) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = num & ((1 << 7) - 1); // AND with bit mask 0111 1111
				buf.push_back(byte);
			}
		}

		uint8_t getSize(const char* num)
		{
			if (!(num[0] & (1 << 7)))
			{
				return 1;
			}
			else if (!(num[1] & (1 << 7)))
			{
				return 2;
			}
			else if (!(num[2] & (1 << 7)))
			{
				return 3;
			}
			else if (!(num[3] & (1 << 7)))
			{
				return 4;
			}
			else if (!(num[4] & (1 << 7)))
			{
				return 5;
			}
			else
			{
				return 1;
			}
		}

		uint32_t decode(const char* num)
		{
			// If the MSB of the furthest left byte is not set
			if (!(num[0] & (1 << 7)))
			{
				return (uint32_t)*num;
			}
			else if (!(num[1] & (1 << 7)))
			{
				uint32_t retNum = num[1] & ((1 << 7) - 1); // Copy 7 lowest bits
				retNum |= (num[0] & ((1 << 7) - 1)) << 7; // Shift next group 1 bit right AND mask 0011 1111 1000 0000

				return retNum;
			}
			else if (!(num[2] & (1 << 7)))
			{
				uint32_t retNum = num[2] & ((1 << 7) - 1); // Copy 7 lowest bits		
				retNum |= (num[1] & ((1 << 7) - 1)) << 7; // Shift next group 1 bit right AND mask 0011 1111 1000 0000
				retNum |= (num[0] & ((1 << 7) - 1)) << 14;

				return retNum;
			}
			else if (!(num[3] & (1 << 7)))
			{
				uint32_t retNum = num[3] & ((1 << 7) - 1); // Copy 7 lowest bits		
				retNum |= (num[2] & ((1 << 7) - 1)) << 7; // Shift next group 1 bit right AND mask 0011 1111 1000 0000
				retNum |= (num[1] & ((1 << 7) - 1)) << 14;
				retNum |= (num[0] & ((1 << 7) - 1)) << 21;

				return retNum;
			}
			else if (!(num[4] & (1 << 7)))
			{
				uint32_t retNum = num[4] & ((1 << 7) - 1); // Copy 7 lowest bits		
				retNum |= (num[3] & ((1 << 7) - 1)) << 7; // Shift next group 1 bit right AND mask 0011 1111 1000 0000
				retNum |= (num[2] & ((1 << 7) - 1)) << 14;
				retNum |= (num[1] & ((1 << 7) - 1)) << 21;
				retNum |= (num[0] & ((1 << 7) - 1)) << 28;

				return retNum;
			}

			return (uint32_t)*num;
		}
	}

	// Variable length max 16 bit integer for positive values
	namespace vuint16
	{
		void encode(const uint16_t& num, std::vector<char>& buf)
		{
			if (num < (1 << 7)) // <128
			{
				buf.push_back((uint8_t)num);
			}
			else if (num < (1 << 14)) // <16,384
			{
				uint8_t byte;

				byte = (num >> 7) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = num & ((1 << 7) - 1); // AND with bit mask 0111 1111
				buf.push_back(byte);
			}
			else // <2,097,152
			{
				uint8_t byte;

				byte = (num >> 14) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = (num >> 7) & ((1 << 7) - 1); // AND number shifted 1 bit with bit mask 0111 1111 0000 0000
				byte |= (1 << 7); // Set MSB
				buf.push_back(byte);

				byte = num & ((1 << 7) - 1); // AND with bit mask 0111 1111
				buf.push_back(byte);
			}
		}

		uint8_t getSize(const char* num)
		{
			if (!(num[0] & (1 << 7)))
			{
				return 1;
			}
			else if (!(num[1] & (1 << 7)))
			{
				return 2;
			}
			else if (!(num[2] & (1 << 7)))
			{
				return 3;
			}
			else
			{
				return 1;
			}
		}

		uint32_t decode(const char* num)
		{
			// If the MSB of the furthest left byte is not set
			if (!(num[0] & (1 << 7)))
			{
				return (uint32_t)*num;
			}
			else if (!(num[1] & (1 << 7)))
			{
				uint32_t retNum = num[1] & ((1 << 7) - 1); // Copy 7 lowest bits
				retNum |= (num[0] & ((1 << 7) - 1)) << 7; // Shift next group 1 bit right AND mask 0011 1111 1000 0000

				return retNum;
			}
			else if (!(num[2] & (1 << 7)))
			{
				uint32_t retNum = num[2] & ((1 << 7) - 1); // Copy 7 lowest bits		
				retNum |= (num[1] & ((1 << 7) - 1)) << 7; // Shift next group 1 bit right AND mask 0011 1111 1000 0000
				retNum |= (num[0] & ((1 << 7) - 1)) << 14;

				return retNum;
			}

			return (uint32_t)*num;
		}
	}
}
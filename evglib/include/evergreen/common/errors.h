#pragma once

#include "evergreen/common/types.h"
#include "evergreen/string/StringView.h"

namespace evg
{
	namespace errors
	{
		class OutOfMemoryError : public Error
		{
			class DetailsMap
			{
				U64 freeMem;
				U64 totalMem;
			};

			ImString details() override
			{
				DetailsMap* detailsMap = &*detailsStore;
				std::string ret = std::to_string(detailsMap.freeMem) + 'B free " + std::to_string(detailsMap.totalMem) + "B total";

			}
		};
	}
}
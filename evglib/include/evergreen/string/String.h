#pragma once

#include "evergreen/common/types.h"

#include "evergreen/string/StringView.h"

#include "evergreen/string/CString.h"
#include "evergreen/string/ImString.h"

namespace evg
{
	using String = ImString;
}


#include "evergreen/string/StringBuilder.h"

namespace evg
{
	// Basic format-string: Assemble a String from all of the provided slices
	template<typename ... Ts>
	StringBuilder f(Ts const& ... args)
	{
		StringBuilder ret;
		ret.reserve((args.size()) ...);
		((ret += args), ...);
		return ret;
	}
}



#include "StringFormatter.h"
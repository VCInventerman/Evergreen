#pragma once

#include "evergreen/common/types.h"

#include "evergreen/string/StringView.h"

#include "evergreen/string/CString.h"
#include "evergreen/string/ImString.h"

namespace evg
{
	using ImString = ImStringBase<>;

	using String = ImString;
}


#include "evergreen/string/StringBuilder.h"

namespace evg
{
	

	// Append each of the arguments after the first using operator_assignAdd +=
	template<typename BufT, typename ... Ts>
	void operator_concat(BufT& buf, Ts const& ... args)
	{
		((buf += args), ...);
	}

	// Append each of the arguments after the first using operator_assignAdd +=, with a spacer in between each one
	template<typename BufT, typename SpacerT, typename ... Ts>
	void operator_concatSpaced(BufT& buf, SpacerT const& spacer, Ts const& ... firstArg, Ts const& ... args)
	{
		buf += firstArg;

		if constexpr (sizeof...(args) > 0)
		{
			buf += spacer;
			operator_concatSpaced(buf, args...);
		}
	}

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
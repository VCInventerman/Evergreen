#pragma once

#include "evergreen/common/types.h"

#include "evergreen/string/StringView.h"
#include "evergreen/string/BufString.h"
#include "evergreen/string/CString.h"
#include "evergreen/string/ImString.h"

#include "StringFormatter.h"

namespace evg
{
	using ImString = ImStringBase<>;

	using String = ImString;
}
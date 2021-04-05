#pragma once

#include <evergreen/common/ContiguousBuf.h>

// Search through a container for an element and return true if it is found.
// It is general because it does not optimize for sorting
template <typename ContainerT, typename ValueT = ContainerT::value_type>
bool contains(ContainerT&& container, const ValueT desired)
{
	for (auto&& i : container)
	{
		if (i == desired)
		{
			return true;
		}
	}
	return false;
}
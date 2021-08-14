#pragma once

#include <evergreen/ContiguousBuf.h>

// Search through a container for an element and return true if it is found.
// It is general because it does not optimize for sorting
template <typename ContainerT, typename ValueT = typename ContainerT::value_type>
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



template <typename ElmT, typename ... Ts>
bool isAnyOf(const ElmT& elm, Ts... args)
{
	return (((elm == args) || ...));

}
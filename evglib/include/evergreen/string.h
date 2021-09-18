#pragma once

#include <evergreen/types.h>

#include <evergreen/StringView.h>

#include <evergreen/ImString.h>

namespace evg
{
	using ImString = ImStringBase<>;
	using String = ImString;
}


#include <evergreen/StringBuilder.h>

namespace evg
{
	template<typename T>
	class has_size
	{
		typedef char one;
		struct two { char x[2]; };

		template <typename C> static one test(decltype(&C::size));
		template <typename C> static two test(...);


	public:
		enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};



	// Basic format-string: Assemble a String from all of the provided slices
	template<typename ... Ts>
	StringBuilder& f(Ts const& ... args)
	{
		const Size count = sizeof...(args);

		thread_local StringBuilder retStore;
		StringBuilder& ret = retStore;
		ret.clear();

		if constexpr (count == 0)
		{
			return ret;
		}

		// This block is only used when f() does not keep a thread_local storage
		/*Size potentialSize = (([&]
			{
				if constexpr (std::is_fundamental<Ts>::value)
				{
					return 0;
				}
				else
				{
					if constexpr (has_size<Ts>::value) //todo: observe classes where size() costs the same as iterating (map)
					{
						return args.size();
					}
					else
					{
						return 0;
					}
				}
			}()) + ...);
		ret.reserve(potentialSize);*/


		((ret += args), ...);
		ret.ensureNullTerminated();
		return ret;
	}
}
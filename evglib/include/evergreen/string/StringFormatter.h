#pragma once

#include "evergreen/common/types.h"

namespace bgl
{
	class StringFormatter
	{
	public:
		std::map<std::string_view, std::function<void(std::string& out)>> keys;

		static StringFormatter standard;

		void setStringKey(const std::string_view key, const std::string val)
		{
			keys[key] = [val](std::string& out) { out += val; };
		}

		void setFuncKey(const std::string_view key, std::function<void(std::string& out)> func)
		{
			keys[key] = func;
		}

		void processParam(const std::string_view param, std::string& out)
		{
			const auto itr = keys.find(param);

			if (itr != keys.cend())
				itr->second(out);
		}

		void format(const std::string_view in, std::string& out)
		{
			std::string key;
			size_t start = std::string::npos;

			for (size_t i = 0; i < in.size(); i++)
			{
				if ((i + 1 < in.size()) && (in[i] == '<'))
				{
					if (in[i + 1] == '<')
					{
						out += '<';
						++i;
					}
					else
						start = i + 1;
				}
				else if (in[i] == '>')
				{
					if ((i + 1 < in.size()) && (in[i + 1] == '<'))
					{
						out += '>';
						++i;
					}
					else if (start != std::string::npos)
					{
						processParam(std::string_view(in.data() + start, i - start), out);
						start = std::string::npos;
					}
					else
						out += '>';
				}
				else
				{
					if (start == std::string::npos)
						out += in[i];
				}
			}
		}

		std::string format(const std::string_view in)
		{
			std::string temp;
			format(in, temp);
			return temp;
		}
	};

	StringFormatter StringFormatter::standard;
}
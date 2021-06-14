#pragma once

#include <evergreen/file.h>

namespace evg
{

	// Cascading hirearchy of configuration files
	// Each file is a collection of either scalars (String, Int), or a map (list is a map indexed by numbers)
	// Supported types: String, Number, Bool, Null, Map, List

	constexpr Char Separator = '.';

	/*class ConfigElement
	{
	public:
		virtual void emit() = 0;

		template
	};



	class ConfigList
	{
	public:

		std::list<

		void emit()
		{

		}



	};
	*/





	class Configuration
	{
	public:
		struct ConfigComp
		{
			/*bool operator() (const Tuple<Int, String, json>& lhs, const Tuple<Int, String, json>& rhs) const
			{
				return lhs.first < rhs.first;
			}*/

			constexpr bool operator() (const Tuple<Int, String, json>& lhs, const Tuple<Int, String, json>& rhs) const
			{
				return lhs.first < rhs.first;
			}
		};

		using Root = std::map<Int, Tuple<String, json>, std::greater<Int>>;
		Root roots; // Set of integer priority, category of root, and its contents as json

		std::vector<StringView> levels; // When parsing a path, stores each period-separated sequence

		// Get value from tree
		// Example: "BlockGame.viewDistance.x"
		template <typename RetT>
		RetT get(const String key)
		{
			if (key.empty())
			{
				Root::iterator i = roots.begin();
				return i->second.second.get<RetT>();
			}

			String::const_iterator i = key.cbegin();

			levels.clear();
			levels.emplace_back(&*key.cbegin(), &*key.cbegin());

			while (i != key.cend())
			{
				if (*i != '.')
				{
					levels.back().data_raw.end_raw++;
				}
				else
				{
					levels.emplace_back(&*levels.back().end() + 1, &*levels.back().end());
				}

				++i;
			}
			levels.back().data_raw.end_raw++;


			
			for (auto& currentRoot : roots) // Iterate from highest priority root to lowest looking for key
			{
				json& currentRootJson = currentRoot.second.second;
				//json::iterator itr; /*=*/
				auto itr = currentRootJson.find(levels.begin()->view());
				bool failed = false;
				

				for (std::vector<StringView>::iterator level = levels.begin() + 1; (level != levels.end()) && (failed != true); ++level) // Descend through each level in the levels list
				{
					json::iterator search = itr->find(level->view());

					if (search == itr->end())
					{
						failed = true;
					}
					else
					{
						itr = search;
					}
				}

				if (failed == false)
				{
					return itr->get<RetT>();
				}
			}

			nop();
		}

		Int getInt(const String key)
		{
			return get<Int>(key);
		}

		template <typename T>
		void insert(const T& elm)
		{

		}

		/*RetT resolve(const String key)
		{
			for (Configuration* i = this; i != nullptr; i = i->parent)
			{

			}
		}*/
	};

	static Configuration pconf; // Program configuration








}

void evgInitConfig(evg::String programName)
{
	using namespace evg;

	Path adminConfig = f(getAdminDataFolder(), "/Evergreen/", programName, "/config.json");

	if (adminConfig.isFile())
	{
		auto file = simpleFileRead(adminConfig);

		json readConf = json::parse(file.begin(), file.end());

		pconf.roots.insert({ 20, {"admin", std::move(readConf)} });
	}



	Path userConfig = f(getUserDataFolder(), "/Evergreen/", programName, "/config.json");

	if (userConfig.isFile())
	{
		auto file = simpleFileRead(userConfig);

		json readConf = json::parse(file.begin(), file.end());

		pconf.roots.insert({ 40, {"user", std::move(readConf) } });
	}

	Path localConfig = f(thisProgram.parentPath, "/Evergreen/", programName, "/config.json");

	if (localConfig.isFile())
	{
		auto file = simpleFileRead(localConfig);

		json readConf = json::parse(file.begin(), file.end());

		pconf.roots.insert({ 60, {"local", std::move(readConf)} });
	}

	//todo: command line params
}
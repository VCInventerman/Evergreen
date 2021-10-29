#pragma once

#include <evergreen/file.h>

namespace evg
{

	// Cascading hirearchy of configuration files
	// Each file is a collection of either scalars (String, Int), or a map (list is a map indexed by numbers)
	// Supported types: String, Number, bool, Null, Map, List
	// Smaller priorities are read first

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

		StringEvalEngine* stringEval;


		template<typename RetT>
		struct ConfigReturn
		{
			using type = std::optional<RetT>;
		};

		template<>
		struct ConfigReturn<json&>
		{
			using type = json*;
		};

		template<>
		struct ConfigReturn<const json&>
		{
			using type = const json*;
		};


		// Get a json element from the tree
		// Example: "BlockGame.viewDistance.x"
		template <typename RetT>
		typename ConfigReturn<RetT>::type get(const StringViewHash key)
		{
			if constexpr (std::is_same<RetT, json*>::value)
			{
				//InvalidType i;
				return {};
			}

			if (roots.empty())
			{
				std::cout << "Tried to use configuration before it was initialized!\n";
				return {};
			}

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
					levels.back().data_.end_++;
				}
				else
				{
					levels.emplace_back(&*levels.back().end() + 1, &*levels.back().end() + 1);
				}

				++i;
			}

			for (auto& currentRoot : roots) // Iterate from highest priority root to lowest looking for key
			{
				json& currentRootJson = currentRoot.second.second;
				//json::iterator itr; /*=*/
				auto itr = currentRootJson.find(levels.begin()->view());
				bool failed = false;


				for (std::vector<StringView>::iterator level = levels.begin() + 1; (level != levels.end()) && (failed != true); ++level) // Descend through each level in the levels list
				{
					if (itr == currentRootJson.cend())
					{
						return {};
					}

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
					if constexpr (std::is_arithmetic<RetT>::value)
					{
						if (itr->is_number())
						{
							return itr->get<RetT>();
						}
						else if (itr->is_string())
						{
							try
							{
								if (*itr->get_ptr<json::string_t*>() == "")
									return {}; // Don't parse code that is just ""

								return stringEval->eval<RetT>(*itr->get_ptr<json::string_t*>());
							}
							catch (const std::exception& error)
							{
								logError("Failed to evalulate config string: ", error.what());
								return {};
							}
						}
					}

					return itr->get<RetT>();
				}
			}

			return {};
		}

		// Use the reference type!
		template <>
		ConfigReturn<json>::type get<json>(const StringViewHash key) = delete;

		// Use the reference type!
		template <>
		ConfigReturn<const json>::type get<const json>(const StringViewHash key) = delete;

		// Use the reference type!
		template <>
		ConfigReturn<json&>::type get<json&>(const StringViewHash key) = delete;

		// Use the reference type!
		template <>
		ConfigReturn<const json&>::type get<const json&>(const StringViewHash key) = delete;

		// Modifying roots is unsupported! Use a const reference.
		template <>
		ConfigReturn<json*>::type get<json*>(const StringViewHash key) = delete;

		// Get a json element from the tree
		template <>
		ConfigReturn<const json*>::type get<const json*>(const StringViewHash key)
		{
			if (roots.empty())
			{
				std::cout << "Tried to use configuration before it was initialized!\n";
				return {};
			}

			if (key.empty())
			{
				Root::iterator i = roots.begin();
				return &i->second.second;
			}

			String::const_iterator i = key.cbegin();

			levels.clear();
			levels.emplace_back(&*key.cbegin(), &*key.cbegin());

			while (i != key.cend())
			{
				if (*i != '.')
				{
					levels.back().data_.end_++;
				}
				else
				{
					levels.emplace_back(&*levels.back().end() + 1, &*levels.back().end());
				}

				++i;
			}
			levels.back().data_.end_++;



			for (auto& currentRoot : roots) // Iterate from highest priority root to lowest looking for key
			{
				json& currentRootJson = currentRoot.second.second;
				//json::iterator itr; /*=*/
				auto itr = currentRootJson.find(levels.begin()->view());
				bool failed = false;


				for (std::vector<StringView>::iterator level = levels.begin() + 1; (level != levels.end()) && (failed != true); ++level) // Descend through each level in the levels list
				{
					if (itr == currentRootJson.cend())
					{
						return {};
					}

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
					return &*itr;
				}
			}

			return {};
		}

		template <typename RetT, typename AtT>
		std::optional<RetT> readTo(const json& base, const AtT& index)
		{
			if (base.is_object())
			{
				const auto confItr = base.find(index);
				
				if (confItr == base.cend())
					return {};

				const json& conf = *confItr;

				if constexpr (std::is_arithmetic<RetT>::value)
				{
					if (conf.is_number())
					{
						return conf.get<RetT>();
					}
					else if (conf.is_string())
					{
						try
						{
							return stringEval->eval<RetT>(*conf.get_ptr<const json::string_t*>());
						}
						catch (const std::exception& error)
						{
							std::cout << error.what() << '\n';
							debugBreak();
						}
					}
				}
				else
				{
					return conf.get<RetT>();
				}
			}
			
			return {};
		}



		std::optional<Int> getInt(const StringViewHash key)
		{
			return get<Int>(key);
		}

		const std::string* getString(const StringViewHash key)
		{
			auto res = get<const std::string*>(key);

			if (res)
				return *res;
			else
				return nullptr;
		}

		const json* getJson(const StringViewHash key)
		{
			auto res = get<const json*>(key);

			if (res)
				return *res;
			else
				return nullptr;
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

	// Program configuration
	static Configuration pconf; // Program configuration





}

void evgInitConfig(evg::String programName, evg::SemVer version)
{
	using namespace evg;

#if defined(EVG_DEFAULT_CONFIG)
	pconf.roots.insert({ 80, {"default", EVG_DEFAULT_CONFIG} });
#endif

	pconf.stringEval = StringEvalEngine::defaultEval;

	Path adminConfig = f(getAdminDataFolder(), "/Evergreen/", programName, "/config.json");

	if (adminConfig.isFile())
	{
		auto file = simpleFileRead(adminConfig);

		if (file.size() > 0)
		{
			json readConf = json::parse(file.begin(), file.end());

			pconf.roots.insert({ 20, {"admin", std::move(readConf)} });
		}
	}



	Path userConfig = f(getUserDataFolder(), "/Evergreen/", programName, "/config.json");

	if (userConfig.isFile())
	{
		auto file = simpleFileRead(userConfig);

		if (file.size() > 0)
		{
			json readConf = json::parse(file.begin(), file.end());

			pconf.roots.insert({ 40, {"user", std::move(readConf) } });
		}
	}

	Path localConfig = f(getExecParentFolder(), "/Evergreen/", programName, "/config.json");

	if (localConfig.isFile())
	{
		auto file = simpleFileRead(localConfig);

		if (file.size() > 0)
		{
			json readConf = json::parse(file.begin(), file.end());

			pconf.roots.insert({ 60, {"local", std::move(readConf)} });
		}
	}

	//todo: command line params
}
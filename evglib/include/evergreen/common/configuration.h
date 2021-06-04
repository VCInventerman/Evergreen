#pragma once

#include <evergreen/string/String.h>

// Cascading hirearchy of configuration files
// Each file is a collection of either scalars (String, Int), or a map (list is a map indexed by numbers)

class ConfigElement
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



class Configuration
{
public:
	Configuration* parent = nullptr;

	std::map<String, ConfigElement*

	// Get value from tree
	// Example: "BlockGame.viewDistance.x"
	template <typename RetT>
	RetT& get(const String key)
	{
		if (key == "")
		{
			return resolve("");
		}

	}

	Int getInt(const String key)
	{
		return get<Int>(key);
	}

	template <typename T>
	void insert(const T& elm)
	{

	}

	RetT resolve(const String key)
	{
		for (Configuration* i = this; i != nullptr; i = i->parent)
		{

		}
	}
};
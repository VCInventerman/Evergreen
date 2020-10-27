#pragma once

#include <yaml-cpp/node/node.h>

#include "coreLogging.h"

namespace bgl
{

	bool nodeIsScalar(const YAML::Node& n)
	{
		if (!n.IsDefined())
		{
			return false;
		}
		if (n.IsScalar())
		{
			return true;
		}
		return false;
	}

	template<typename T>
	void loadNodeVar(const YAML::Node& n, T& store, std::string details)
	{
		if (!nodeIsScalar(n))
		{
			errorLog << "YAML key read failed! Value will be left at default: " << details;
			return;
		}

		store = n.as<T>();
	}

	template<typename T>
	void loadNodeVarSilent(const YAML::Node& n, T& store)
	{
		if (!nodeIsScalar(n))
		{
			return;
		}

		store = n.as<T>();
	}

	template<typename T>
	void loadNodeVarInt(const YAML::Node& n, T& store, std::string details)
	{
		if (!nodeIsScalar(n))
		{
			errorLog << "YAML key read failed! Value will be left at default: " << details;
			return;
		}

		store = (T)n.as<int>();
	}

	template<typename T>
	void loadNodeVarSilentInt(const YAML::Node& n, T& store)
	{
		if (!nodeIsScalar(n))
		{
			return;
		}

		store = (T)n.as<int>();
	}

	bool getNodeBool(const YAML::Node& n, bool error_condition = false)
	{
		if (!nodeIsScalar(n))
		{
			errorLog << "YAML key read failed! Value will be left at default.";
			return error_condition;
		}

		return n.as<bool>();
	}

	bool getNodeBoolSilent(const YAML::Node& n, bool error_condition = false)
	{
		if (!nodeIsScalar(n))
		{
			return error_condition;
		}

		return n.as<bool>();
	}

}
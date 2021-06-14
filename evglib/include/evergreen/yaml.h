#pragma once

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#pragma warning( push )
#pragma warning(disable:4251)
#pragma warning(disable:4996)
#pragma warning(disable:4275)
#include <yaml-cpp/yaml.h>
#pragma warning( pop )
//#undef _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING


#include "evergreen/alloc.h"
#include "evergreen/errors.h"

namespace evg
{
	Ret<YAML::Node> loadYaml(const Path path)
	{
		YAML::Node node;
		try
		{
			node = YAML::LoadFile(std::string(path));
		}
		catch (const YAML::Exception& e)
		{
			return Ret<YAML::Node>(node, YamlCppError::create(e));
		}
		return Ret<YAML::Node>(node);
	}

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
	EVG_RET_ERRORT loadNodeVar(const YAML::Node& n, T& store, std::string details)
	{
		if (!nodeIsScalar(n))
		{
			//errorLog << "YAML key read failed! Value will be left at default: " << details;
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
			//errorLog << "YAML key read failed! Value will be left at default: " << details;
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
			//errorLog << "YAML key read failed! Value will be left at default.";
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
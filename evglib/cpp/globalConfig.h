#pragma once

#include <cstdint>
#include <vector>

#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>

//#include <boost/filesystem/path.hpp>
#include <boost/dll/runtime_symbol_info.hpp> //Program_location

#include "path.h"

namespace bgl
{

	Path globalConfigPath = defaultGlobalConfigPath;
	Path clientConfigPath = defaultClientConfigPath;
	Path serverConfigPath = defaultServerConfigPath;

	YAML::Node globalConfigNode;
	YAML::Node clientConfigNode;
	YAML::Node serverConfigNode;

	void fatalSimpleExit(std::string message) //Can be called before log is created
	{
		std::cerr << message;
		std::ofstream errorLog;
		errorLog.open("BG-ERROR.log");
		errorLog << message;
		errorLog.close();
		exit(1);
	}

	void readParams()
	{
		if (std::filesystem::exists(globalConfigPath))
		{
			try
			{
				globalConfigNode = YAML::LoadFile(globalConfigPath.string());
			}
			catch (...)
			{
				fatalSimpleExit("Params at " + globalConfigPath.string() + " could not be read! Exiting...");
			}
		}
		else
		{
			fatalSimpleExit("Params at " + globalConfigPath.string() + " could not be found! Exiting...");
		}
	}

}
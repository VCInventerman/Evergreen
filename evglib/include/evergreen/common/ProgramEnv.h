#pragma once

#include "evergreen/common/types.h"
#include "evergreen/containers/Vector.h"
#include "evergreen/string/String.h"

using ExitCode = Int;

enum
{
	ExitSuccess = 0,
	ExitFailure = -1
};

class ProgramEnv
{
public:


	Vector<String> args;
	String executableName;

	std::chrono::steady_clock::time_point beginTime;

	std::vector<void(*)(void)> atExitList;

	void setArgs(int argc, char** argv)
	{
		executableName = String((Char*)argv[0]);
		/*if (argc > 1)
		{
			args = Vector<String>((const char*)argv + 1, (const char*)argc);
		}*/
	}

	void quit(const ExitCode code = ExitSuccess)
	{
		exit(code);
	}

	void callAtExit()
	{
		for (auto&& i : atExitList)
		{
			i();
		}
	}
};

ProgramEnv thisProgram;


ExitCode EvgMain();

#ifdef EVG_INSERT_MAIN
int main(int argc, char** argv) // Wrapper
{
	thisProgram.setArgs(argc, argv);
	ExitCode code = EvgMain();
	thisProgram.callAtExit();
	return code;
}
#endif
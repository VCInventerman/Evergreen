#pragma once

#include "types.h"
#include "../containers/Vector.h"

class ExitCode
{
public:
	Int val;

	enum
	{
		Success = 0,
		Failure = -1
	};

	operator int() { return val; }

	ExitCode() : val(Success) {}
	ExitCode(const Int _val) : val(_val) {}
};

class ProgramEnv
{
public:
	type ExitCode = int;

	VectorView<StringC> args;

	std::chrono::steady_clock::time_point beginTime;

	std::vector<void(*)(void)> atExitList;

	void setArgs(const Int argc, const Char** argv)
	{
		args = { (StringC*)argv, argc };
	}

	void quit(const ExitCode code = ExitCode::Success)
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
#pragma once

#include "types.h"
#include "../containers/Vector.h"
#include "../string/String.h"

//todo: enum of "nontrivial" types like ExitCode

class ProgramEnv
{
public:
	class ExitCode
	{
	public:
		Int val;

		static ExitCode Success() { return { 0 }; }
		static ExitCode Failure() { return { -1 }; }

		operator int() { return val; }

		ExitCode() : val(Success().val) {}
		ExitCode(const Int _val) : val(_val) {}
	};

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

	void quit(const ExitCode code = ExitCode::Success())
	{
		exit(code.val);
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


ProgramEnv::ExitCode EvgMain();

#ifndef HAS_MAIN
int main(int argc, char** argv) // Wrapper
{
	thisProgram.setArgs(argc, argv);
	ProgramEnv::ExitCode code = EvgMain();
	thisProgram.callAtExit();
	return code.val;
}
#endif
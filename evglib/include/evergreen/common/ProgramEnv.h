#pragma once

#include "evergreen/common/types.h"
#include "evergreen/containers/Vector.h"
#include "evergreen/string/String.h"
#include "evergreen/common/alloc.h"

namespace evg
{

	using ExitCode = Int;

	enum
	{
		ExitSuccess = 0,
		ExitFailure = -1
	};


	class ProgramEnv
	{
	public:
		// Pointer or PID representing process for use with
		UInt64 nativeHandle;
		Pid pid;

		Vector<String> args;
		String executableName;
		Path root; // Where executable file is stored

		std::chrono::steady_clock::time_point beginTime;

		std::vector<void(*)(void)> atExitList;

		boost::log::sources::severity_logger<boost::log::trivial::severity_level> log;
		
		



		void setArgs(int argc, char** argv)
		{
			executableName = (Path(argv[0])).filename();
			root = argv[0];
			if (argc > 1)
			{
				//args = Vector<String>((const char*)argv + 1, (const char*)argv + argc);
			}
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


#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
		Error init()
		{
			nativeHandle = (UInt64)GetCurrentProcess(); // Always -1
			pid = GetCurrentProcessId();

			return {};
		}
#elif defined(EVG_COMPILE_AOT)
		Error init()
		{
			
			pid = getpid();
		}
#else
		Error init();
#endif

		
		

#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
		// Get amount of physical memory present of system (excluding pagefile)
		Size memTotal()
		{
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);

			return status.ullTotalPhys;
		}
#else
		// Get amount of physical memory present of system (excluding pagefile)
		Size memTotal();
#endif

		

#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
		// Get amount of memory used by this process
		Size memUsed()
		{
			PROCESS_MEMORY_COUNTERS_EX counters;
			counters.cb = sizeof(counters);
			GetProcessMemoryInfo((HANDLE)nativeHandle, (PPROCESS_MEMORY_COUNTERS)&counters, sizeof(counters));
			return counters.PrivateUsage;
		}
#else
		// Get amount of memory used by this process
		Size memUsed();
#endif

		

#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
		// Get amount of memory that can be used without resorting to a pagefile
		Size memRemain()
		{
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);

			return status.ullAvailPhys;
		}
#else
		// Get amount of memory that can be used without resorting to a pagefile
		Size memRemain();
#endif

		

#if defined(EVG_PLATFORM_WIN) && defined(EVG_COMPILE_AOT)
		// Get amount of memory that this process could allocate, including the pagefile. Anything above memRemain() will be slow.
		Size memRemainAll()
		{
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);

			return status.ullAvailPageFile;
		}
#else
		// Get amount of memory that this process could allocate, including the pagefile. Anything above memRemain() will be slow.
		Size memRemainAll();
#endif
		
	};

	ProgramEnv thisProgram;


	enum LogSeverity
	{
		trace = 100, // Extremely detailed information over most small actions
		debug = 200, // Detailed information
		info = 300, // Useful information
		warn = 400, // Something that may be wrong, but will preserve desired behavior
		error = 500, // Something that is incorrect and may result in wrong behavior
		fatal = 600, // Something that is incorrect and causes a fatal exit
	};

	// Call the global log
	template<typename ... Ts>
	void log(LogSeverity sev, Ts const& ... args)
	{
		StringBuilder buf;
		((buf += args), ...);
		BOOST_LOG_SEV(thisProgram.log, sev) << buf;
	}

	template<typename ... Ts>
	void logTrace(Ts const& ... args) { log(boost::log::trivial::trace, args...); }

	template<typename ... Ts>
	void logDebug(Ts const& ... args) { log(boost::log::trivial::debug, args...); }

	template<typename ... Ts>
	void logInfo(Ts const& ... args) { log(boost::log::trivial::info, args...); }

	template<typename ... Ts>
	void logWarn(Ts const& ... args) { log(boost::log::trivial::warning, args...); }

	template<typename ... Ts>
	void logError(Ts const& ... args) { log(boost::log::trivial::error, args...); }

	template<typename ... Ts>
	void logFatal(Ts const& ... args) { log(boost::log::trivial::fatal, args...); }



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
}

using evg::EvgMain;
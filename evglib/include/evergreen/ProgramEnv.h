#pragma once

#include "evergreen/types.h"
#include "evergreen/Vector.h"
#include "evergreen/string.h"

namespace evg
{
	const json::string_t* getStringJson(const json& source, const char* key)
	{
		const json::const_iterator itr = source.find(key);

		if (itr == source.cend() || !itr->is_string())
		{
			return nullptr;
		}

		return itr->get_ptr<const json::string_t*>();
	}

	std::optional<int> getIntJson(const json& source, const char* key)
	{
		const json::const_iterator itr = source.find(key);

		if (itr == source.cend() ||
			// Ensure that the source is either an integer or an unsigned integer
			(!itr->is_number_integer() && !itr->is_number_unsigned()))
		{
			return {};
		}
		else
		{
			return itr->get<int>();
		}
	}

	template <typename T>
	bool loadJsonTo(const json& source, const char* key, T& out)
	{
		const json::const_iterator itr = source.find(key);

		if constexpr (std::is_integral_v<T>)
		{
			if (itr == source.cend() ||
				// Ensure that the source is either an integer or an unsigned integer
				(!itr->is_number_integer() && !itr->is_number_unsigned()))
			{
				return false;
			}
			else
			{
				itr->get_to<T>(out);
				return true;
			}
		}
		else if constexpr (std::is_same_v<T, const json::string_t*>)
		{
			if (itr == source.cend() || !itr->is_string())
			{
				return false;
			}
			else
			{
				T ret = itr->get_ptr<T>();
				if (ret)
					out = ret;
				return out != nullptr;
			}

		}
		else
		{
			static_assert(std::is_integral_v<T> || std::is_same_v<T, const json::string_t*>);
		}
	}


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
		String programName;
		Path executablePath;
		Path parentPath; // Where executable file is stored

		std::chrono::steady_clock::time_point beginTime;

		std::vector<void(*)(void)> atExitList;

		boost::log::sources::severity_logger<LogSeverity> log;
		//boost::mutex logM;
		//boost::log::attributes::mutable_constant<LogSeverity> logSevAttr;


		ProgramEnv() {} // : logSevAttr(LogSeverity::info) {}


		void setArgs(const int argc, const char* const argv[])
		{
			executablePath = argv[0];
			executableName = executablePath.filename();
			parentPath = executablePath.parent();
			if (argc > 1)
			{
				ContiguousBufPtrEnd(argv + 1, argv + argc);
				//args = Vector<String>(ContiguousBufPtrEnd<const char*>(argv + 1, argv + argc));
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
		void init()
		{
			nativeHandle = (UInt64)GetCurrentProcess(); // Always -1
			pid = GetCurrentProcessId();
			CoInitializeEx(NULL, COINIT_MULTITHREADED);
		}
#elif defined(EVG_COMPILE_AOT)
		void init()
		{

			pid = getpid();
		}
#else
		void init();
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

	Path getExecParentFolder()
	{
		return thisProgram.parentPath;
	}









	// Call the global log
	template<typename ... Ts>
	void log(const LogSeverity sev, Ts const& ... args)
	{
		//boost::log::keywords::severity = sev;
		//BOOST_LOG_SEV(thisProgram.log, (boost::log::trivial::severity_level)sev) << f(args...);
		//BOOST_LOG_STREAM_WITH_PARAMS((thisProgram.log), (boost::log::keywords::severity = (sev)));// ((<< args) ...);

#ifdef EVG_LIB_LOG
		BOOST_LOG_SEV(thisProgram.log, sev) << f(args...);
#else
		std::cout << f(args...);
#endif

		//if (thisProgram.logM.try_lock_for(boost::chrono::seconds(5)))
		//{
			//thisProgram.logSevAttr.set(sev);

		/*auto rec = thisProgram.log.open_record(boost::log::keywords::severity = sev);
		if (rec)
		{
			//auto attrs = boost::log::core::get()->get_global_attributes();
			auto attrs = rec.attribute_values();

			for (auto i : attrs)
			{
				std::cout << i.first.string() << '\n';
			}

			auto gotSev = attrs.find("Severity")->second.extract_or_throw<LogSeverity>();


			boost::log::record_ostream stream(rec);
			//stream << boost::log::add_value("Severity", sev);
			((stream << args), ...);
			stream.flush();
			thisProgram.log.push_record(std::move(rec));
		}*/

		//	thisProgram.logM.unlock();
		//}
	}

	template<typename T>
	void logRaw(const LogSeverity sev, const T& message)
	{
		log(sev, message);
	}

	template<typename ... Ts>
	void logTrace(Ts const& ... args)
	{
		log(LogSeverity::trace, args...);
	}

	template<typename ... Ts>
	void logDebug(Ts const& ... args)
	{
		log(LogSeverity::debug, args...);
	}

	template<typename ... Ts>
	void logInfo(Ts const& ... args)
	{
		log(LogSeverity::info, args...);
	}

	template<typename ... Ts>
	void logWarn(Ts const& ... args)
	{
		log(LogSeverity::warn, args...);
	}

	template<typename ... Ts>
	void logError(Ts const& ... args)
	{
		log(LogSeverity::error, args...);
	}


	template<typename ... Ts>
	void logFatal(Ts const& ... args)
	{
		log(LogSeverity::fatal, args...);
	}

#ifdef EVG_ANDROID_LOGCAT
    class androidStreamToLogcat : public std::streambuf {
    public:
        enum { bufsize = 128 }; // ... or some other suitable buffer size
        androidStreamToLogcat() { this->setp(buffer, buffer + bufsize - 1); }

    private:
        int overflow(int c)
        {
            if (c == traits_type::eof()) {
                *this->pptr() = traits_type::to_char_type(c);
                this->sbumpc();
            }
            return this->sync()? traits_type::eof(): traits_type::not_eof(c);
        }

        int sync()
        {
            int rc = 0;
            if (this->pbase() != this->pptr()) {
                char* writebuf = (char*)alloca(bufsize + 1);
                memcpy(writebuf, this->pbase(), this->pptr() - this->pbase());
                writebuf[this->pptr() - this->pbase()] = '\0';

                rc = __android_log_write(ANDROID_LOG_INFO, "librobotsee", writebuf) > 0;
                this->setp(buffer, buffer + bufsize - 1);
            }
            return rc;
        }

        char buffer[bufsize];
    };

	struct androidLogSinkBackend : public boost::log::sinks::basic_sink_backend<boost::log::sinks::concurrent_feeding>
	{
		void consume(const boost::log::record_view& rec)
		{
			LogSeverity sev = rec["Severity"].extract_or_default(LogSeverity::info);

			int androidSev = ANDROID_LOG_UNKNOWN;

			if (sev == LogSeverity::trace)
				androidSev = ANDROID_LOG_VERBOSE;
			else if (sev == LogSeverity::debug)
				androidSev = ANDROID_LOG_DEBUG;
			else if (sev == LogSeverity::info)
				androidSev = ANDROID_LOG_INFO;
			else if (sev == LogSeverity::warn)
				androidSev = ANDROID_LOG_WARN;
			else if (sev == LogSeverity::error)
				androidSev = ANDROID_LOG_ERROR;
			else if (sev == LogSeverity::fatal)
				androidSev = ANDROID_LOG_FATAL;

			const char* log_msg = rec[boost::log::expressions::smessage].get().c_str();

			const char* log_module = "librobotsee";

			__android_log_write(androidSev, log_module, log_msg);
		}
	};
#endif


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

	class Configuration;
	extern Configuration pconf;
}



using evg::EvgMain;

void evgInitConfig(evg::String programName, const evg::SemVer version);

namespace evg
{
	// Initialize environment
	// argc and argv are the standard parameters provided by the OS
	// programName is a user defined string used for choosing configuration branches
	void evgProgramBegin(const char* const programName, const char* const version)
	{
		using namespace evg;

		// Imbue the Boost.Filesystem library with the system default locale
		// MUST be called before any usage of Boost.Filesystem or the program will not abort correctly
		boost::filesystem::path::imbue(std::locale(""));

#ifdef EVG_ANDROID_LOGCAT
        std::cout.rdbuf(new androidStreamToLogcat);
#endif

#ifdef EVG_LIB_SDL2
		SDL_SetMainReady();
#endif

#ifdef EVG_LIB_BOOST
		evg::threads.init();
#endif

#ifdef EVG_LIB_V8
		evg::StringEvalEngine::initV8();
#endif

		evgInitConfig(programName, evg::SemVer(version));



#ifdef EVG_LIB_BOOST_LOG
		// Get a pointer to the global logging core
		boost::shared_ptr<boost::log::core> core = boost::log::core::get();

		boost::log::register_simple_formatter_factory<LogSeverity, char>("Severity");


		boost::log::add_common_attributes();

		//auto attrTest = core->add_global_attribute("Severity", thisProgram.logSevAttr);


#ifdef EVG_PLATFORM_WIN
		// Enable the windows debugger log if the config file asks for it or if
		// the config file does not define a preference and the debugger is active.
		// This choice could also be a filter executed for every log entry
		auto res1 = evg::pconf.get<bool>("log.winDbg");
		if ((res1 && *res1) || (!res1 && boost::log::expressions::is_debugger_present()()))
		{
			auto sink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::debug_output_backend>>();

			constexpr const char* formatterDefault = "[%Severity%]: %Message%\n"; // This sink backend does not automatically output a newline
			const std::string* formatter = evg::pconf.getString("log.winDbgFormat");
			sink->set_formatter(boost::log::parse_formatter(formatter ? formatter->data() : formatterDefault));

			core->add_sink(sink);
		}
#endif

#ifdef EVG_ANDROID_LOGCAT
		boost::shared_ptr<boost::log::sinks::asynchronous_sink<androidLogSinkBackend>> sink = boost::make_shared<boost::log::sinks::asynchronous_sink<androidLogSinkBackend>>();
		core->add_sink(sink);
#endif

		/*
		// Attach log to console depending on config
		auto resString = evg::pconf.getString("log.console");
		if (resString && *resString == "cout")
		{
			boost::log::add_console_log(std::cout);
		}
		else if (resString && *resString == "clog")
		{
			boost::log::add_console_log(std::clog);
		}
		else if (resString && *resString == "cerr")
		{
			boost::log::add_console_log(std::cerr);
		}
		else if (resString && *resString == "none")
		{
			// Do not log to the console (only if requested)
		}
		else
		{
			// Log to the console if there is no user preference
			boost::log::add_console_log(std::cout);
		}

		// Set the severity level that will be used for files and the console based on config options
		// If "level" is defined, all sinks without an override will use it
		// "consoleLevel" and "fileLevel" can be used to override their respective sinks
		resString = evg::pconf.getString("log.console");
		*/

		constexpr auto logSinkError = [](const json::const_iterator configNode, const json::const_iterator begin)
		{
			std::cout << "Invalid log configuration for sink " << std::distance(configNode, begin) << '\n';
		};

		const evg::json* res3 = evg::pconf.getJson("log.sinks");
		if (res3)
		{
			for (evg::json::const_iterator i = res3->cbegin(); i < res3->cend(); ++i)
			{
				auto type = getStringJson(*i, "type");
				if (!type)
				{
					// Give up loading this sink if it has an invalid type
					logSinkError(i, res3->cbegin());
					continue;
				}

				if (*type == "console")
				{
					std::basic_ostream<char>* stream = &std::cout;

					auto streamName = getStringJson(*i, "stream");

					if (!streamName)
					{
						// Continue forward with using cout if no stream is defined
						logSinkError(i, res3->cbegin());
					}

					if (*streamName == "cout")
					{
						stream = &std::cout;
					}
					else if (*streamName == "cerr")
					{
						stream = &std::cerr;
					}
					else if (*streamName == "cerr")
					{
						stream = &std::clog;
					}

					LogSeverity minSev = LogSeverity::trace;
					LogSeverity maxSev = LogSeverity::fatal;

					const std::string* str = getStringJson(*i, "minSev");
					std::optional<LogSeverity> sevOpt;
					if (str && (sevOpt = LogSeverityConv::fromString(*str)))
						minSev = *sevOpt;

					str = getStringJson(*i, "maxSev");
					if (str && (sevOpt = LogSeverityConv::fromString(*str)))
						maxSev = *sevOpt;

					auto sink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>();

					sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(stream, nopDeleter));

					sink->set_filter(logSevKeyword >= minSev && logSevKeyword <= maxSev);

					/*sink->set_filter([minSev, maxSev](const boost::log::attribute_value_set& attrs)
						{
							auto sev = logSevKeyword;

							return (sev >= minSev) && (sev <= maxSev);
						});*/



					constexpr const char* formatterDefault = "[%Severity%]: %Message%";
					const std::string* formatter = nullptr;
					loadJsonTo(*i, "formatter", formatter);
					sink->set_formatter(boost::log::parse_formatter(formatter ? formatter->data() : formatterDefault));
					//sink->set_formatter(boost::log::expressions::stream << boost::log::expressions::attr<LogSeverity, severity_tag>("Severity") << ' ' << boost::log::expressions::smessage);

					core->add_sink(sink);
				}
				else if (*type == "file")
				{

				}
			}
		}
		else
		{

		}
#endif // defined(EVG_LIB_BOOST_LOG)

		evg::logInfo(programName, ' ', version);
	}

	void evgProgramBegin(const int argc, const char* const argv[], const char* const programName, const char* const version)
	{
		evg::thisProgram.setArgs(argc, argv);

		evgProgramBegin(programName, version);
	}

	void evgProgramEnd()
	{
#ifdef EVG_LIB_V8
		evg::StringEvalEngine::endV8();
#endif

		// Disable the Boost.Logging library
		// Attempts to prevent use-after-deinitialization of Boost.Filesystem
		boost::log::core::get()->remove_all_sinks();
	}
}

using evg::evgProgramBegin;
using evg::evgProgramEnd;
#pragma once

#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <regex>

//#include <boost/filesystem/path.hpp>
//#include <boost/filesystem/fstream.hpp>
//#include <boost/filesystem/operations.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>

#include "coreLogging.h"
#include "path.h"
#include "core/common/globalConfig.h"

/*
Severity levels:

trace: Detailed information
info: Useful information
warning: Something that is incorrect, but will preserve desired behavior
error: Something that is incorrect and may result in wrong behavior
fatal: Something that is incorrect and causes a fatal exit
*/

namespace bgl
{

	Path gLogPath;

	using ReadHandler = void(*)(const std::error_code&, const size_t);
	using WriteHandler = void(*)(const std::error_code&, const size_t);
	using ResolveHandler = void(*)(const std::error_code&, const size_t);

	uint64_t getTime()
	{
		return std::chrono::steady_clock::now().time_since_epoch().count() / 1000000;
	}

	/*
Severity levels:

trace: Detailed information
info: Useful information
warning: Something that is incorrect, but will preserve desired behavior
error: Something that is incorrect and may result in wrong behavior
fatal: Something that is incorrect and causes a fatal exit

*/

	enum class Severity
	{
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Fatal
	};

	class TimeSevLogger
	{
	public:
		using severity_logger = boost::log::sources::severity_logger<boost::log::trivial::severity_level>;

		std::string buf;
		severity_logger out;
		Severity sev = Severity::Info;


		TimeSevLogger& flush()
		{
			//BOOST_LOG_SEV(out, sev) << buf;
			buf.clear();
			sev = Severity::Info;

			return *this;
		}

		TimeSevLogger& operator<<(const bool& msg)
		{
			if (msg)
				buf += "true";
			else
				buf += "false";

			return *this;
		}
		TimeSevLogger& operator<<(const char& msg)
		{
			buf += msg;

			return *this;
		}
		TimeSevLogger& operator<<(const uint16_t& msg)
		{
			buf += std::to_string(msg);

			return *this;
		}
		TimeSevLogger& operator<<(const int16_t& msg)
		{
			buf += std::to_string(msg);

			return *this;
		}
		TimeSevLogger& operator<<(const uint32_t& msg)
		{
			buf += std::to_string(msg);

			return *this;
		}
		TimeSevLogger& operator<<(const int32_t& msg)
		{
			buf += std::to_string(msg);

			return *this;
		}
		TimeSevLogger& operator<<(const uint64_t& msg)
		{
			buf += std::to_string(msg);

			return *this;
		}
		TimeSevLogger& operator<<(const int64_t& msg)
		{
			buf += std::to_string(msg);

			return *this;
		}
		TimeSevLogger& operator<<(const std::string& msg)
		{
			buf += msg;

			return *this;
		}
	};

	class ProgramDetails
	{
	public:
		std::chrono::steady_clock::time_point beginTime;
		vector_view<cstring> args;
		cstring executableName;
		Path root;
		TimeSevLogger logger;

		ProgramDetails() :
			beginTime(std::chrono::steady_clock::now()),
			root(defaultAppRoot)
		{}

		void init(int _argc, char* _argv[])
		{

		}

		void setArgs(const int _argc, char** _argv)
		{
			args = std::vector<std::string_view>(_argv, _argv + _argc);
		}

		void seedRand()
		{
			srand(uint(time(NULL)));
		}

		void seedRand(const uint seed)
		{
			srand(seed);
		}
	};

	ProgramDetails this_program;
	TimeSevLogger& gLog = this_program.logger;

#define traceLog BOOST_LOG_SEV(gLog.out, boost::log::trivial::trace)

#define debugLog BOOST_LOG_SEV(gLog.out, boost::log::trivial::debug)

#define infoLog BOOST_LOG_SEV(gLog.out, boost::log::trivial::info)

#define warningLog BOOST_LOG_SEV(gLog.out, boost::log::trivial::warning)

#define errorLog BOOST_LOG_SEV(gLog.out, boost::log::trivial::error)

#define fatalLog BOOST_LOG_SEV(gLog.out, boost::log::trivial::fatal)

	Path platformPath(const Path& base)
	{
		std::string sbase = base.string();
#ifdef _WIN32
		boost::replace_all(sbase, "/", "\\");
#else
		boost::replace_all(sbase, "\\", "/");
#endif
		return Path(sbase);
	}

	using time_point = std::chrono::steady_clock::time_point;

	inline std::chrono::steady_clock::time_point time_now() noexcept
	{
		return std::chrono::steady_clock::now();
	}

	std::string getLogTime()
	{
		std::time_t now = std::time(0);
		char time[100];
		std::strftime(time, sizeof(time), "[%H:%M:%S]", std::localtime(&now));
		return std::string(time);
	}

	class log_time_impl :
		public boost::log::attribute::impl
	{
	public:
		boost::log::attribute_value get_value()
		{
			std::time_t now = std::time(0);
			char time[100];
			std::strftime(time, sizeof(time), "[%H:%M:%S]", std::localtime(&now)); // Will be in format "/2020-03-29-15:56:06.log
			return boost::log::attributes::make_attribute_value(std::string(time));
		}
	};

	class log_time :
		public boost::log::attribute
	{
	public:
		log_time() : boost::log::attribute(new log_time_impl())
		{
		}
		explicit log_time(boost::log::attributes::cast_source const& source) : boost::log::attribute(source.as< log_time_impl >())
		{
		}
	};

	void initLog()
	{
		if (globalConfigNode["logFolder"].IsDefined())
		{
			gLogPath = this_program.root.string() + "/" + globalConfigNode["logFolder"].as<std::string>();
			if (!std::filesystem::exists(gLogPath))
			{
				fatalSimpleExit("Log Folder at " + gLogPath.string() + " did not exist! Exiting...");
			}
		}
		else
		{
			fatalSimpleExit("Global Config at " + globalConfigPath.string() + " did not contain a log folder definition! Exiting...");
		}


		Path logFile = gLogPath;
		std::time_t now = std::time(0);
		char time[100];
		std::strftime(time, sizeof(time), "/%Y_%m_%d-%H-%M-%S.log", std::localtime(&now)); // Will be in format "/2020-03-29-15:56:06.log
		logFile.append(time);

		boost::log::core::get()->add_global_attribute("DayTime", log_time());

		typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > text_sink;
		boost::shared_ptr< text_sink > t_sink = boost::make_shared< text_sink >();

		logFile = platformPath(logFile);
		t_sink->locked_backend()->add_stream(boost::make_shared< std::ofstream >(logFile));
		t_sink.get()->set_formatter(
			boost::log::expressions::stream
			<< boost::log::expressions::attr<std::string>("DayTime")
			<< " ["
			<< boost::log::trivial::severity
			<< "]: "
			<< boost::log::expressions::message);
		boost::log::core::get()->add_sink(t_sink);

		boost::log::add_console_log(std::cout, boost::log::keywords::format = (
			boost::log::expressions::stream
			<< boost::log::expressions::attr<std::string>("DayTime")
			<< " ["
			<< boost::log::trivial::severity
			<< "]: "
			<< boost::log::expressions::message));

		//#if defined _DEBUG && defined _WIN32 //Windows only debug log visible in IDE
#if defined _WIN32
		typedef boost::log::sinks::synchronous_sink< boost::log::sinks::debug_output_backend > debug_sink;
		boost::shared_ptr<debug_sink> d_sink = boost::make_shared<debug_sink>();
		d_sink.get()->set_formatter(
			boost::log::expressions::stream
			<< boost::log::expressions::attr<std::string>("DayTime")
			<< " ["
			<< boost::log::trivial::severity
			<< "]: "
			<< boost::log::expressions::message
			<< "\n"); //Debug sink does not automatically newline
		d_sink.get()->set_filter(boost::log::expressions::is_debugger_present());
		boost::log::core::get()->add_sink(d_sink);
#endif

		boost::log::core::get()->set_filter
		(
			boost::log::trivial::severity >= boost::log::trivial::debug
		);
	}

}
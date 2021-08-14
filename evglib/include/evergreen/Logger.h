#ifndef EVG_LOGGER_H
#define EVG_LOGGER_H
namespace evg
{
	//using LogSeverity = boost::log::trivial::severity_level;

	enum class LogSeverity
	{
		trace = 0, // Extremely detailed information over most small actions
		debug, // Detailed information
		info, // Useful information
		warn, // Something that may be wrong, but will preserve desired behavior
		error, // Something that is incorrect and may result in wrong behavior
		fatal, // Something that is incorrect and causes a fatal exit
		count
	};

	namespace LogSeverityConv
	{


		static constexpr std::string_view sevStrings[] =
		{
			"trace",
			"debug",
			"info",
			"warn",
			"error",
			"fatal"
		};

		constexpr static LogSeverity defaultVal = LogSeverity::info;

		bool isValid(const LogSeverity val)
		{
			if (val < LogSeverity::trace || val > LogSeverity::fatal)
				return false;
			else
				return true;
		}

		std::optional<LogSeverity> fromString(const std::string_view string)
		{
			if (string == "trace")
				return LogSeverity::trace;
			else if (string == "debug")
				return LogSeverity::debug;
			else if (string == "info")
				return LogSeverity::info;
			else if (string == "warn")
				return LogSeverity::warn;
			else if (string == "error")
				return LogSeverity::error;
			else if (string == "fatal")
				return LogSeverity::fatal;
			else
				return {};
		}

		std::string_view toString(const LogSeverity val)
		{
			if ((Size)val > sizeof(sevStrings) / sizeof(std::string_view) || (Size)val < 0)
			{
				return "";
			}
			else
			{
				return sevStrings[(Size)val];
			}
		}
	};



	//BOOST_LOG_ATTRIBUTE_KEYWORD(severityK, "Severity", ::evg::LogSeverity)


	template<typename CharT, typename TraitsT>
	std::basic_ostream<CharT, TraitsT>& operator<< (std::basic_ostream<CharT, TraitsT>& stream, evg::LogSeverity& sev)
	{
		static const char* sevStrings[] =
		{
			"trace",
			"debug",
			"info",
			"warn",
			"error",
			"fatal"
		};

		if ((Size)sev < sizeof(sevStrings) / sizeof(char*))
		{
			stream << sevStrings[(Size)sev];
		}
		else
		{
			stream << (int)sev;
		}

		return stream;
	}

	struct severity_tag;

	boost::log::formatting_ostream& operator<<(boost::log::formatting_ostream& stream, boost::log::to_log_manip<evg::LogSeverity, severity_tag> const& manip)
	{
		static const char* sevStrings[] =
		{
			"trace",
			"debug",
			"info",
			"warn",
			"error",
			"fatal"
		};

		evg::LogSeverity sev = manip.get();

		if ((Size)sev < sizeof(sevStrings) / sizeof(char*))
		{
			stream << sevStrings[(Size)sev];
		}
		else
		{
			stream << (int)sev;
		}

		return stream;
	}

};

BOOST_LOG_ATTRIBUTE_KEYWORD(logSevKeyword, "Severity", evg::LogSeverity)



namespace evg
{




	template<typename T>
	void logRaw(const LogSeverity sev, const T& message);

	// Call the global log
	template<typename ... Ts>
	void log(const LogSeverity sev, Ts const& ... args);

	template<typename ... Ts>
	void logTrace(Ts const& ... args);

	template<typename ... Ts>
	void logDebug(Ts const& ... args);

	template<typename ... Ts>
	void logInfo(Ts const& ... args);

	template<typename ... Ts>
	void logWarn(Ts const& ... args);

	template<typename ... Ts>
	void logError(Ts const& ... args);

	template<typename ... Ts>
	void logFatal(Ts const& ... args);



#ifdef EVG_LIB_V8
	template <LogSeverity sev>
	void logJsCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		StringBuilder ret;

		auto context = info.GetIsolate()->GetCurrentContext();

		for (auto i : Range(info.Length()))
		{
			auto val = info[i];

			if (val->IsNumber())
			{
				double out = 0.0;
				bool status = val->NumberValue(context).To(&out);
				if (!status)
				{
					logError("Failed to convert value to number!");
					return;
				}
				ret += out;
			}
			else
			{
				auto stringMaybe = val->ToString(context);
				v8::Local<v8::String> string;
				if (!stringMaybe.ToLocal(&string))
				{
					logError("Failed to convert value to string!");
					return;
				}

				Size oldSize = ret.size();
				ret.resize(ret.size() + string->Utf8Length(info.GetIsolate()));
				char* buf = ret.data() + oldSize;
				string->WriteUtf8(info.GetIsolate(), buf, ret.size() - oldSize);
			}
		}

		ret.ensureNullTerminated();
		//logRaw(sev, ret);
	}

	void createJsFunc(v8::Isolate* isolate, const std::string_view name, v8::FunctionCallback funcPtr)
	{
		auto context = isolate->GetCurrentContext();

		v8::Local<v8::FunctionTemplate> templateFunc = v8::FunctionTemplate::New(isolate, funcPtr);

		auto funcMaybe = templateFunc->GetFunction(context);
		v8::Local<v8::Function> func;
		if (!funcMaybe.ToLocal(&func))
		{
			logError("Failed function creation in js");
			return;
		}


		// Create the name of the function
		auto key = v8::String::NewFromUtf8(isolate, name.data(), v8::NewStringType::kInternalized, name.size());
		v8::Local<v8::String> keyVal;
		if (!key.ToLocal(&keyVal))
		{
			logError("Failed string creation in js");
			return;
		}

		auto res = context->Global()->Set(context, keyVal.As<v8::Value>(), func);
		if (res.IsNothing())
			logError("Failed setting global variable in js");
	}



#endif // defined(EVG_LIB_V8)

};
#endif // EVG_LOGGER_H
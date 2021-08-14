#ifndef EVG_STRINGEVALENGINE_H
#define EVG_STRINGEVALENGINE_H

namespace evg
{

#if defined(EVG_LIB_V8)
	class JsCompileError : public EvgError
	{
	public:
		using This = EvgError;

		JsCompileError() = default;
		JsCompileError(const std::string& reason_) : EvgError(reason_) {}
		JsCompileError(const char* const reason_) : EvgError(reason_) {}
		JsCompileError(std::string&& reason_) : EvgError(std::move(reason_)) {}
	};



	// Object that can evaluate strings as JS code
	class StringEvalEngine
	{
	public:
		using This = StringEvalEngine;


		inline static This* defaultEval = nullptr;
		inline static v8::Platform* defaultPlatform = nullptr;
		inline static v8::Isolate* defaultIsolate = nullptr;
		inline static v8::ArrayBuffer::Allocator* defaultAllocator = nullptr;


		v8::Persistent<v8::Context> context;


		StringEvalEngine(v8::Isolate* isolate) : context(defaultIsolate, v8::Context::New(defaultIsolate)) {}

		template <typename RetT>
		RetT eval(std::string_view string)
		{
			v8::Isolate::Scope isolateScope(defaultIsolate);
			v8::HandleScope handleScope(defaultIsolate);
			v8::Local<v8::Context> localContext = v8::Local<v8::Context>::New(defaultIsolate, context);
			v8::Context::Scope contextScope(localContext);

			v8::ScriptCompiler::Source source(v8::String::NewFromUtf8(defaultIsolate, string.data(), v8::NewStringType::kNormal, string.size()).ToLocalChecked());

			v8::Local<v8::Script> script;

			v8::TryCatch catcher(defaultIsolate);

			bool res1 = v8::ScriptCompiler::Compile(localContext, &source,
				v8::ScriptCompiler::CompileOptions::kEagerCompile, v8::ScriptCompiler::kNoCacheBecauseScriptTooSmall).ToLocal(&script);

			// Test if script was compiled successfully
			if (!res1 || script.IsEmpty())
			{
				if (catcher.HasCaught())
				{
					throw JsCompileError(*v8::String::Utf8Value(defaultIsolate, catcher.Message()->Get()));
				}
				else
				{
					throw JsCompileError();
				}
			}

			auto resMaybe = script->Run(localContext);
			v8::Local<v8::Value> res;
			if (!resMaybe.ToLocal(&res))
			{
				StringBuilder ret = "Failed to run script! ";
				v8::Local<v8::String> string = catcher.Message()->Get();

				Size oldSize = ret.size();
				ret.resize(ret.size() + string->Utf8Length(defaultIsolate));
				char* buf = ret.data() + oldSize;
				string->WriteUtf8(defaultIsolate, buf, ret.size() - oldSize);
				logError(ret);
			}

			if constexpr (std::is_same_v<RetT, void>)
			{
				return;
			}
			else
			{
				if (res->IsNumber())
				{
					if constexpr (std::is_integral_v<RetT>)
					{
						return res->Int32Value(localContext).FromJust();
					}
					else if constexpr (std::is_floating_point_v<RetT>)
					{
						return res->NumberValue(localContext).FromJust();
					}

				}

				return {};
			}
		}

		static void initV8()
		{
			v8::V8::InitializeICU("C:\\Program Files\\Evergreen\\Assets\\icudtl.dat");
			v8::V8::InitializeExternalStartupData("C:\\Program Files\\Evergreen\\Assets\\icudtl.dat");

			defaultPlatform = v8::platform::NewDefaultPlatform().release();
			v8::V8::InitializePlatform(defaultPlatform);

			v8::V8::Initialize();

			v8::Isolate::CreateParams defaultEvalCreateParams;
			defaultAllocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
			defaultEvalCreateParams.array_buffer_allocator = defaultAllocator;
			defaultIsolate = v8::Isolate::New(defaultEvalCreateParams);


			{
				v8::Isolate::Scope isolateScope(defaultIsolate);
				v8::HandleScope handleScope(defaultIsolate);

				defaultEval = new This(defaultIsolate);



			}
		}

		static void endV8()
		{
			defaultIsolate->Dispose();
			v8::V8::Dispose();
			v8::V8::ShutdownPlatform();
			delete defaultAllocator;

		}
	};
#else defined(EVG_LIB_V8)
	// Object that can evaluate strings as JS code
	class StringEvalEngine
	{
	public:
		using This = StringEvalEngine;


		inline static This* defaultEval = nullptr;

		template <typename RetT>
		RetT eval(std::string_view string)
		{
			if constexpr (std::is_integral_v<RetT>)
			{
				return (RetT)std::strtoll(string.data(), nullptr, 10);
			}
			else if (std::is_floating_point_v<RetT>)
			{
				return (RetT)std::strtold(string.data(), nullptr);
			}

			return {};
		}

		static void initV8()
		{

		}

		static void endV8()
		{

		}
	};
#endif

}; // namespace evg
#endif // #ifndef EVG_STRINGEVALENGINE_H
#pragma once

#include <evergreen/common/alloc.h>

namespace evg
{
	inline namespace errors
	{
		class OutOfMemoryError
		{
		public:
			static Error create(const UInt64 _freeMem, const UInt64 _totalMem)
			{
				Error ret;
				ret.detailStore = std::make_unique<Byte[]>(DetailSize);
				DetailMap* detailMap = (DetailMap*)(&ret.detailStore[0]);
				*detailMap = { _freeMem, _totalMem };
				ret.funcs = &funcs;
				return ret;
			}

			struct DetailMap
			{
				UInt64 freeMem;
				UInt64 totalMem;
			};

			static constexpr Size DetailSize = sizeof(DetailMap);

			static StringViewHash msg() { return "Out of memory"; }
			static String details(UniquePtr<Byte[]>& detailStore)
			{
				DetailMap* detailMap = (DetailMap*)&detailStore[0];
				return std::to_string(detailMap->freeMem) + "B free " + std::to_string(detailMap->totalMem) + "B total";
			}

			static inline Error::InterpretFuncs funcs = { msg, details };
		};

		//todo: error that reports the contents of a string stored in details


		class YamlCppError
		{
		public:
			static Error create(const YAML::Exception& e)
			{
				Error ret;
				ret.detailStore = std::make_unique<Byte[]>(DetailSize);
				new (&ret.detailStore[0]) DetailMap(e.mark, e.msg);
				ret.funcs = &funcs;
				return ret;
			}

			struct DetailMap
			{
				YAML::Mark mark;
				String message;

				DetailMap() = default;
				DetailMap(const YAML::Mark& _mark, const String _message) : mark(_mark), message(_message) {}
			};

			static constexpr Size DetailSize = sizeof(DetailMap);

			static StringViewHash msg() { return "yaml-cpp error"; }
			static String details(UniquePtr<Byte[]>& detailStore)
			{
				DetailMap* detailMap = (DetailMap*)&detailStore[0];
				return "Mark: " + std::to_string(detailMap->mark.column) + std::to_string(detailMap->mark.line) + std::to_string(detailMap->mark.pos) + " Message: " + detailMap->message.operator_conv<CChar*>() + "B total";
			}

			static inline Error::InterpretFuncs funcs = { msg, details };
		};
	}
}
#pragma once

#define _LIBCPP_ENABLE_CXX20_REMOVED_TYPE_TRAITS // Boost currently requires it

#ifdef EVG_LIB_ZLIB
#include <zlib.h>
#endif

#ifdef EVG_LIB_BOOST

#if defined(EVG_COMPILER_GCC) || defined(EVG_COMPILER_CLANG)
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic push
#endif


//#include <boost/asio/dispatch.hpp>

/*#include <boost/asio/detail/config.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/detail/type_traits.hpp>
#include <boost/asio/execution_context.hpp>
#include <boost/asio/execution/executor.hpp>
#include <boost/asio/is_executor.hpp>
#include <boost/asio/detail/push_options.hpp>
*/

//#include <boost/asio.hpp>

#if defined(EVG_PLATFORM_WIN)
#include <boost/asio/detail/config.hpp>
#define private public // look, i really needed access to the HANDLE in io_context
#include <boost/asio/io_context.hpp>
#undef private
#endif

#include <boost/asio/execution.hpp>
#include <boost/asio.hpp>
#include <boost/circular_buffer.hpp>

#ifdef EVG_LIB_BEAST
#include <boost/beast.hpp>
#endif

#if __has_include(<openssl/conf.h>)
#include <boost/asio/ssl.hpp>

#ifdef EVG_LIB_BEAST
#include <boost/beast/ssl.hpp>
#endif
#endif


#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>

#if defined(EVG_COMPILER_GCC) || defined(EVG_COMPILER_CLANG)
#pragma GCC diagnostic pop
#endif

#endif


// Required
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/locks.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/debug_output_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#include <boost/asio/high_resolution_timer.hpp>

namespace evg { using Uuid = boost::uuids::uuid; };




#ifdef EVG_PLATFORM_WIN
//#define NOMINMAX

#include <windows.h>
#include <psapi.h>

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <Audiopolicy.h>
#include <Devicetopology.h>
#include <Endpointvolume.h>
#include <initguid.h>
#include <windows.h>
#include <math.h>
#include <appmodel.h>
#include <cguid.h>
//#define _ATL_ALLOW_UNSIGNED_CHAR
//#include <atlbase.h>
#include <stdio.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

#include <io.h>
#include <fcntl.h>

#include <windowsx.h>
#include <shlobj.h>
#include <shobjidl_core.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <propkey.h>

#undef min
#undef max
#endif

#ifdef EVG_PLATFORM_POSIX
//#include <aio.h>


#ifdef EVG_PLATFORM_LINUX
#include <linux/aio_abi.h>
#include <sys/mman.h>
#include <linux/mman.h>
#endif


#endif


#ifdef EVG_LIB_LLVM
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/CodeGen/LinkAllAsmWriterComponents.h"
#include "llvm/CodeGen/LinkAllCodegenComponents.h"
#include "llvm/CodeGen/MIRParser/MIRParser.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/AutoUpgrade.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LLVMRemarkStreamer.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/MC/SubtargetFeature.h"
#include "llvm/Pass.h"
#include "llvm/Remarks/HotnessThresholdParser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Transforms/HelloNew/HelloWorld.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCWinCOFFStreamer.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/CodeGen/Passes.h"

#include <lld/Common/Driver.h>
#include <llvm/Demangle/Demangle.h>
#endif

#ifdef EVG_LIB_DISCORD
#include <discord-game-sdk/discord_game_sdk.h>
#endif

#ifdef EVG_LIB_JSON_NLOHMANN
#include <evergreen/json.h>
namespace evg { using ::nlohmann::json; };
#endif

#ifdef EVG_LIB_SDL2
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#endif

#ifdef EVG_LIB_BGFX
#include <bgfx/platform.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <inttypes.h>
#include <bx/debug.h>
#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <bx/pixelformat.h>
#endif

#ifdef EVG_LIB_V8

//#define V8_COMPRESS_POINTERS true
//#define USING_V8_SHARED true

#include <include/v8.h>

#include <include/libplatform/libplatform.h>


#endif

#include <parallel_hashmap/phmap.h>
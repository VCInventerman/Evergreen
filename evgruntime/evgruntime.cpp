#pragma warning (disable : 4996)
#pragma warning (disable : 4146)

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

#include "evglib.h"

using Endian = llvm::support::endianness;

class ElfDecoder
{
public:
	struct ELFHeader32
	{
		U32 magic; // e_ident[EI_MAG0] through e_ident[EI_MAG3]
		U8 format; // e_ident[EI_CLASS]
		U8 endian; // 	e_ident[EI_DATA]
		U8 version1; // e_ident[EI_DATA]
		U8 abi1; // e_ident[EI_DATA]
		U8 abi2; // e_ident[EI_ABIVERSION]
		U8 pad[7]; // e_ident[EI_PAD]
		U16 type; // e_type
		U16 isa; // e_machine
		U32 version2; // e_version
		U32 entry; // e_entry
		U32 programHeader; // e_phoff
		U32 sectionHeader; // e_shoff
		U32 flags; // e_flags
		U16 headerSize; // e_ehsize
		U16 programHeaderEntrySize; // e_phentsize
		U16 programHeaderSize; // e_phnum
		U16 sectionHeaderEntrySize; // e_shentsize
		U16 sectionHeaderSize; // e_shnum
		U16 sectionNameEntry; // e_shstrndx
	};
	struct ELFHeader64
	{
		U32 magic; // e_ident[EI_MAG0] through e_ident[EI_MAG3]
		U8 format; // e_ident[EI_CLASS]
		U8 endian; // 	e_ident[EI_DATA]
		U8 version1; // e_ident[EI_DATA]
		U8 abi1; // e_ident[EI_DATA]
		U8 abi2; // e_ident[EI_ABIVERSION]
		U8 pad[7]; // e_ident[EI_PAD]
		U16 type; // e_type
		U16 isa; // e_machine
		U32 version2; // e_version
		U64 entry; // e_entry
		U64 programHeader; // e_phoff
		U64 sectionHeader; // e_shoff
		U32 flags; // e_flags
		U16 headerSize; // e_ehsize
		U16 programHeaderEntrySize; // e_phentsize
		U16 programHeaderSize; // e_phnum
		U16 sectionHeaderEntrySize; // e_shentsize
		U16 sectionHeaderSize; // e_shnum
		U16 sectionNameEntry; // e_shstrndx
	};

	struct ElfSectionHeader32
	{
		U32 name; // sh_name
		U32 type; // sh_type
		U32 flags; // sh_flags
		U32 addr; // sh_addr
		U32 offset; // sh_offset
		U32 size; // sh_size
		U32 link; // sh_link
		U32 info; // sh_info
		U32 align; // sh_addralign
		U32 entrySize; // sh_entsize
	};
	struct ElfSectionHeader64
	{
		U32 name; // sh_name
		U32 type; // sh_type
		U64 flags; // sh_flags
		U64 addr; // sh_addr
		U64 offset; // sh_offset
		U64 size; // sh_size
		U32 link; // sh_link
		U32 info; // sh_info
		U64 align; // sh_addralign
		U64 entrySize; // sh_entsize
	};

	struct ElfSymbol32
	{
		U32 name; // st_name
		U32 value; // st_value
		U32 size; // st_size
		U8 info; // st_info
		U8 other; // st_other
		U16 sectionHeaderIndex; // st_shndx
	};
	struct ElfSymbol64
	{
		U32 name; // st_name
		U8 info; // st_info
		U8 other; // st_other
		U16 sectionHeaderIndex; // st_shndx
		U64 value; // st_value
		U64 size; // st_size
	};


	class SectionHeader
	{
	public:
		char* begin;

		char* name;
		EnumVal type; // Similar to llvm::ELF::SHT_NULL
		U64 flags;
		U64 offset;
		U64 size;
		U32 link;
		U32 info;
		U64 align;
		U64 entrySize;

		SectionHeader() = default;
		SectionHeader(char* const _begin, const UInt _bitwidth, char* const _sectionNames) : begin(_begin)
		{
			if (_bitwidth == 64)
			{
				ElfSectionHeader64* header = (ElfSectionHeader64*)begin;
				
				name = _sectionNames + header->name;
				type = header->type;
				flags = header->flags;
				offset = header->offset;
				size = header->size;
				link = header->link;
				info = header->info;
				align = header->align;
				entrySize = header->entrySize;
			}
			else
			{
				ElfSectionHeader32* header = (ElfSectionHeader32*)begin;

				name = _sectionNames + header->name;
				type = header->type;
				flags = header->flags;
				offset = header->offset;
				size = header->size;
				link = header->link;
				info = header->info;
				align = header->align;
				entrySize = header->entrySize;
			}
		
		}

		// Partially fill out section before name table is available
		void findNameTable(char* const _begin, const UInt _bitwidth, char* const _fileBegin)
		{
			begin = _begin;

			if (_bitwidth == 64)
			{
				ElfSectionHeader64* header = (ElfSectionHeader64*)begin;

				type = header->type;
				flags = header->flags;
				offset = header->offset;
				size = header->size;
				link = header->link;
				info = header->info;
				align = header->align;
				entrySize = header->entrySize;

				name = _fileBegin + offset + header->name;
			}
			else
			{
				ElfSectionHeader32* header = (ElfSectionHeader32*)begin;

				type = header->type;
				flags = header->flags;
				offset = header->offset;
				size = header->size;
				link = header->link;
				info = header->info;
				align = header->align;
				entrySize = header->entrySize;

				name = _fileBegin + offset + header->name;
			}

		}
	};

	class Symbol
	{
	public:
		char* begin;

		char* name;
		char* value;
		U64 size;
		EnumVal info;
		EnumVal visibility;
		SectionHeader* def;

		Symbol(char* const _begin, const UInt _bitwidth, char* const _sectionNames, std::vector<SectionHeader>& _sectionHeaderTable, char* const _text) : begin(_begin)
		{
			if (_bitwidth == 64)
			{
				ElfSymbol64* sym = (ElfSymbol64*)begin;

				name = _sectionNames + sym->name;
				value = _text + sym->value;
				size = sym->size;
				info = sym->info;
				visibility = sym->other;
				def = sym->sectionHeaderIndex == 0xFFF1 ? _sectionHeaderTable.data() : _sectionHeaderTable.data() + sym->sectionHeaderIndex;
			}
			else
			{
				ElfSymbol32* sym = (ElfSymbol32*)begin;

				name = _sectionNames + sym->name;
				value = _text + sym->value;
				size = sym->size;
				info = sym->info;
				visibility = sym->other;
				def = sym->sectionHeaderIndex == 0xFFF1 ? _sectionHeaderTable.data() : _sectionHeaderTable.data() + sym->sectionHeaderIndex;
			}
		}
	};

	const static std::map<U16, EnumVal> isaLookupTable; // = { {0x00, llvm::Triple::UnknownArch }, {0x03, llvm::Triple::x86 } }


	char* begin; // Object file
	char* end;

	U32 bitwidth;
	Endian endian;
	EnumVal type;
	EnumVal isa;
	char* entry; // Does not appear in libraries used by Evergreen
	char* programHeader; // Does not appear in libraries used by Evergreen
	char* sectionHeaderTable; // Pointer to an array of raw section headers
	U32 flags;
	U16 programHeaderEntrySize;
	U16 programHeaderSize; // Number of entries in program header table
	U16 sectionHeaderEntrySize;
	U16 sectionHeaderSize; // Number of entries in section header table	
	
	SectionHeader sectionNameTableHeader;
	SectionHeader* stringTableHeader; // .strtab
	SectionHeader* symbolTableHeader; // .symtab
	SectionHeader* textHeader; // .text

	char* sectionNameTable; // Pointer to array of null-terminated strings end-to-end
	char* stringTable; // .strtab
	Symbol* symbolTable; // .symtab
	char* text; // .text

	std::vector<SectionHeader> sections;
	std::vector<Symbol> symbols;



	ElfDecoder(char* const _data, const Size _size) : begin(_data), end(_data + _size) {}

	void parse()
	{
		ELFHeader64* header = (ELFHeader64*)begin;

		assert(begin[0] == 0x7F); // Verify magic number
		assert(begin[1] == 'E'); 
		assert(begin[2] == 'L'); 
		assert(begin[3] == 'F');
		bitwidth = header->format * 32;
		endian = header->endian == 1 ? Endian::little : Endian::big;
		assert(header->version1 == 1); // Verify version 1
		// Ignore ABI defininition (0x07-0x08)
		assert(*((U32*)begin + 0x09) == 0 && *((U16*)begin + 0x09 + 4) == 0 && *(begin + 0x09 + 4 + 2) == 0); // Verify 7-byte pad
		type = header->type; // ELF begin type (Should be llvm::ELF::ET_x)
		isa = isaLookupTable.at(header->isa);
		assert(header->version2 == 1); // Verify version 1
		
		if (bitwidth == 64)
		{
			entry = begin + header->entry != 0 ? begin + header->entry : nullptr;
			programHeader = header->programHeader != 0 ? begin + header->programHeader : nullptr;
			sectionHeaderTable = header->sectionHeader != 0 ? begin + header->sectionHeader : nullptr;
			flags = header->flags;
			assert(header->headerSize == 64);
			programHeaderEntrySize = header->programHeaderEntrySize;
			programHeaderSize = header->programHeaderSize;
			sectionHeaderEntrySize = header->sectionHeaderEntrySize;
			sectionHeaderSize = header->sectionHeaderSize;
			sectionNameTableHeader.findNameTable(sectionHeaderTable + (header->sectionNameEntry * sectionHeaderEntrySize), bitwidth, begin);
		}
		else // 32 bit
		{
			ELFHeader32* header32 = (ELFHeader32*)begin;

			entry = begin + header32->entry != 0 ? begin + header32->entry : nullptr;
			programHeader = header32->programHeader != 0 ? begin + header32->programHeader : nullptr;
			sectionHeaderTable = header32->sectionHeader != 0 ? begin + header32->sectionHeader : nullptr;
			flags = header32->flags;
			assert(header32->headerSize == 52);
			programHeaderEntrySize = header32->programHeaderEntrySize;
			programHeaderSize = header32->programHeaderSize;
			sectionHeaderEntrySize = header32->sectionHeaderEntrySize;
			sectionHeaderSize = header32->sectionHeaderSize;
			sectionNameTableHeader.findNameTable(sectionHeaderTable + (header32->sectionNameEntry * sectionHeaderEntrySize), bitwidth, begin);
		}

		sectionNameTable = begin + sectionNameTableHeader.offset;

		// Parse section headers (program headers will never appear)
		for (auto i : ContiguousRange(0, (int)sectionHeaderSize))
		{
			sections.push_back(SectionHeader(sectionHeaderTable + (i * sectionHeaderEntrySize), bitwidth, sectionNameTable));
			std::cout << "Section: " << i << " Name: " << sections[i].name << '\n';
		}

		symbolTableHeader = &*std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) { return lhs.type == llvm::ELF::SHT_SYMTAB; });
		stringTableHeader = &*std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) { return lhs.type == llvm::ELF::SHT_STRTAB; });
		textHeader = &*std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) 
			{ return (lhs.type == llvm::ELF::SHT_PROGBITS) && (lhs.flags & (llvm::ELF::SHF_EXECINSTR | llvm::ELF::SHF_ALLOC)); });

		symbolTable = (Symbol*)(begin + symbolTableHeader->offset);
		stringTable = begin + stringTableHeader->offset;
		text = begin + textHeader->offset;

		// Parse symbols
		for (auto i : ContiguousRange(0, (int)(symbolTableHeader->size / (bitwidth == 64 ? sizeof(ElfSymbol64) : sizeof(ElfSymbol32)))))
		{
			symbols.push_back(Symbol((char*)(symbolTable) + (i * (bitwidth == 64 ? sizeof(ElfSymbol64) : sizeof(ElfSymbol32))), bitwidth, sectionNameTable, sections, text));
			std::cout << "Symbol: " << i << " Name: " << symbols[i].name << " Associated section: " << symbols[i].def->name << '\n';
		}

		
	}
};
const std::map<U16, EnumVal> ElfDecoder::isaLookupTable = { {0x00, llvm::Triple::UnknownArch}, {0x03, llvm::Triple::x86}, {0x3E, llvm::Triple::x86_64} };

int main(int argc, char** argv)
{
	std::cout << "Evergreen Runtime v10000\n";

	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmParsers();
	llvm::InitializeAllAsmPrinters();

	llvm::InitializeNativeTarget();
	llvm::InitializeNativeTargetAsmPrinter();
	llvm::InitializeNativeTargetAsmParser();

	llvm::InitializeAllDisassemblers();

	llvm::LLVMContext context;

	MemFile inputIR("C:/Users/nickk/source/repos/Testing1/EvergreenTest/EvergreenTest.o");

	llvm::SMDiagnostic error;
	UPtr<llvm::Module> mod = llvm::parseIR(llvm::MemoryBufferRef(llvm::StringRef(inputIR.data, inputIR.size), "EvergreenTest"), error, context);

	auto targetTriple = "x86_64-pc-windows-msvc-elf"; //llvm::sys::getProcessTriple();
	std::string serror;
	auto target = llvm::TargetRegistry::lookupTarget(targetTriple, serror);

	auto cpu = llvm::sys::getHostCPUName(); //"x86-64";
	auto features = ""; //llvm::sys::getHostCPUFeatures()

	llvm::TargetOptions opt;
	auto relocmodel = llvm::Optional<llvm::Reloc::Model>(llvm::Reloc::PIC_);
	auto codemodel = llvm::Optional<llvm::CodeModel::Model>(llvm::CodeModel::Large);
	auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, relocmodel, codemodel, llvm::CodeGenOpt::None, /*jit*/ false);

	mod->setDataLayout(targetMachine->createDataLayout());
	mod->setTargetTriple(targetTriple);
	mod->setCodeModel(llvm::CodeModel::Large);

	/*
	//todo: optimize
	llvm::PassManager<llvm::Function> pm;
	llvm::AnalysisManager<llvm::Function> am;
	llvm::HelloWorldPass p;
	auto amp = llvm::PassInstrumentationAnalysis();
	am.registerPass([&]() {return amp; });

	auto fn = mod->getFunction("EvgMain");

	for (auto& f : mod->functions())
	{
		pm.run(f, am);
	}*/


	llvm::legacy::PassManager lpm;

	llvm::SmallVector<char> outbuf;
	llvm::raw_svector_ostream outstream(outbuf);
	std::error_code ec;
	//llvm::raw_fd_ostream outstreamF("C:/Users/nickk/source/repos/Testing1/EvergreenTest/EvergreenTestMC.o", ec, llvm::sys::fs::FileAccess::FA_Write);
	//llvm::raw_pwrite_stream* outstream = (llvm::raw_pwrite_stream*)&outstreamF;

	bool supported = targetMachine->addPassesToEmitFile(lpm, outstream, nullptr, llvm::CGFT_ObjectFile);
	lpm.run(*mod);


	using MathFn = int(*)(int, int);

	ElfDecoder decoder(outbuf.data(), outbuf.size());
	decoder.parse();

	ElfDecoder::Symbol* call = &*std::find_if(decoder.symbols.begin(), decoder.symbols.end(), [](const ElfDecoder::Symbol& lhs) {
		std::cout << ' ' << lhs.info; return strcmp(lhs.name, "math") == 0; });
	char* region = call->value;


	//UInt oldVal;
	//VirtualProtect(call, 100, PAGE_EXECUTE_READWRITE, (PDWORD)&oldVal);

	//std::cout << "Val: " << call(5, 6);



	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	auto const page_size = system_info.dwPageSize;

	void* buffer = call; //= VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);

	std::memcpy(buffer, region, call->size);

	DWORD dummy;
	VirtualProtect(buffer, call->size + 50, PAGE_EXECUTE_READWRITE, &dummy);

	MathFn toCall = (MathFn)buffer;

	//VirtualFree(buffer, 0, MEM_RELEASE);

	U64* b = (U64*)buffer;

	std::cout << toCall(5, 6) << "\n";


	
	//using intFn = int(*)();
	/*

	auto itr = std::find(outstring.begin(), outstring.end(), 0b11001011);

	mathFn call = (mathFn)(outstring.data() + 489);

	UInt oldVal;
	VirtualProtect(call, 100, PAGE_EXECUTE_READWRITE, (PDWORD)&oldVal);
	//std::cout << call(2, 4);*/

	/*
	llvm::LLVMTargetMachine* ltargetMachine = static_cast<llvm::LLVMTargetMachine*>(targetMachine);

	// Add common CodeGen passes.
	llvm::MachineModuleInfoWrapperPass* MMIWP = new llvm::MachineModuleInfoWrapperPass(ltargetMachine);
	llvm::TargetPassConfig* PassConfig = ltargetMachine->createPassConfig(lpm);
	PassConfig->setDisableVerify(false);
	lpm.add(PassConfig);
	lpm.add(MMIWP);

	assert(!PassConfig->addISelPasses());
	PassConfig->addMachinePasses();
	PassConfig->setInitialized();


	//    llvm::LLVMTargetMachine::addPassesToGenerateCode(targetMachine, lpm, false, *MMIWP);
	//if (!PassConfig)
	//    return true;
	//assert(llvm::TargetPassConfig::willCompleteCodeGenPipeline() &&
	//    "Cannot emit MC with limited codegen pipeline");

	llvm::MCContext* Ctx = &MMIWP->getMMI().getContext();
	targetMachine->getObjFileLowering()->Initialize(*Ctx, *targetMachine);
	//llvm::MCContext* Ctx = &targetMachine->getObjFileLowering()->getContext();
	if (targetMachine->Options.MCOptions.MCSaveTempLabels)
		Ctx->setAllowTemporaryLabels(false);

	// Create the code emitter for the target if it exists.  If not, .o file
	// emission fails.
	const llvm::MCSubtargetInfo& STI = *targetMachine->getMCSubtargetInfo();
	const llvm::MCRegisterInfo& MRI = *targetMachine->getMCRegisterInfo();
	llvm::MCCodeEmitter* MCE = target->createMCCodeEmitter(*targetMachine->getMCInstrInfo(), MRI, *Ctx);
	llvm::MCAsmBackend* MAB = target->createMCAsmBackend(STI, MRI, targetMachine->Options.MCOptions);

	const llvm::Triple& triple = targetMachine->getTargetTriple();
	
	//std::unique_ptr<llvm::MCStreamer> AsmStreamer(getTarget().createMCObjectStreamer(
	//    triple, *Ctx, std::unique_ptr<MCAsmBackend>(MAB), MAB->createObjectWriter(Out),
	//    std::unique_ptr<MCCodeEmitter>(MCE), STI, Options.MCOptions.MCRelaxAll,
	//    Options.MCOptions.MCIncrementalLinkerCompatible,
	//     true));

	// Create the AsmPrinter, which takes ownership of AsmStreamer if successful.
	//llvm::FunctionPass* Printer =
	//    getTarget().createAsmPrinter(*this, std::move(AsmStreamer));
	//if (!Printer)
	//    return true;

	//PM.add(Printer);
	//PM.add(createFreeMachineFunctionPass());

	//auto streamer = target->createAsmStreamer(Ctx, outstream, true, false, );
	std::unique_ptr<llvm::MCStreamer> streamer(target->createMCObjectStreamer(triple, *Ctx, std::unique_ptr<llvm::MCAsmBackend>(MAB), MAB->createObjectWriter(*outstream),
		std::unique_ptr<llvm::MCCodeEmitter>(MCE), STI, targetMachine->Options.MCOptions.MCRelaxAll, targetMachine->Options.MCOptions.MCIncrementalLinkerCompatible, true));
	llvm::AsmPrinter* printer = target->createAsmPrinter(*targetMachine, std::move(streamer));
	lpm.add((llvm::FunctionPass*)printer);
	lpm.add(llvm::createFreeMachineFunctionPass());

	bool modified = lpm.run(*mod);
	*/



	//lpm.add(llvm::createFreeMachineFunctionPass());

	//auto mmi = llvm::MachineModuleInfo(ltargetMachine);
	//unsigned int counter = 1;
	//for (auto& mfunBase : mod->functions())
	//{
	//llvm::Function* mfunBase = mod->getFunction("math");
	//auto st = targetMachine->getSubtargetImpl(*mfunBase);
	//llvm::MachineFunction mfun = llvm::MachineFunction(*mfunBase, *ltargetMachine, *st, counter, mmi);

	//std::cout << mfun.getName().data() << '\n';
	//mfun.dump();
	//printer->SetupMachineFunction(mfun);
	//std::cout << "Functions: " << mod->getFunctionList().size() << '\n';
	
	//std::cout << "Functions: " << mod->getFunctionList().size() << '\n';
	//printer->runOnMachineFunction(mfun);
	//(llvm::FunctionPass*)(printer)->runOnFunction(mfunBase);

	//lpm.add(llvm::createMachineFunctionPrinterPass(llvm::outs()));

	//mfun.dump();
	//assert(!modified);

	//counter++;
	//}

	

	return EXIT_SUCCESS;
}
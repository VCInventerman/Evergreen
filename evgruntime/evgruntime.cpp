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

#include <llvm/Demangle/Demangle.h>

#include "evglib.h"

using Endian = llvm::support::endianness;

//todo: proper ARM, X86_32, ARM64 support
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

	struct ElfRel32
	{
		U32 offset; // r_offset
		U32 info; // r_info

		U32 sym() { return info >> 8; }
		U8 type() { return U8(info); }
		U32 cinfo() { return (sym() << 8) + type(); }
	};
	struct ElfRel64
	{
		U64 offset; // r_offset
		U64 info; // r_info

		U64 sym() { return info >> 32; }
		U32 type() { return U32(info); }
		U32 cinfo() { return (U64(sym()) << 32) + type(); }
		U64 typeData() { return (info << 32) >> 40; }
		U64 typeId() { return (info << 56) >> 56; }
		U64 typeInfo() { return (typeData() << 8) + type(); }
	};

	struct ElfRela32
	{
		U32 offset; // r_offset
		U32 info; // r_info
		I32 addend; // r_addend

		U32 sym() { return info >> 8; }
		U8 type() { return U8(info & 0xff); }
		U32 cinfo() { return (sym() << 8) + type(); }
	};
	struct ElfRela64
	{
		U64 offset; // r_offset
		U64 info; // r_info
		I64 addend; // r_addend

		U64 sym() { return info >> 32; }
		U32 type() { return U32(info & 0xffffffff); }
		U32 cinfo() { return (U64(sym()) << 32) + type(); }
		U64 typeData() { return (info << 32) >> 40; }
		U64 typeId() { return (info << 56) >> 56; }
		U64 typeInfo() { return (typeData() << 8) + type(); }
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

		char* data;

		SectionHeader() = default;
		SectionHeader(char* const _begin, const UInt _bitwidth, char* const _sectionNames, char* const _fileBegin) : begin(_begin)
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

				data = _fileBegin + offset;
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

				data = _fileBegin + offset;
			}

		}

		// Fill out section before name table is available
		static SectionHeader findStringTable(char* const _begin, const UInt _bitwidth, char* const _fileBegin)
		{
			SectionHeader ret;

			ret.begin = _begin;

			if (_bitwidth == 64)
			{
				ElfSectionHeader64* header = (ElfSectionHeader64*)_begin;

				ret.type = header->type;
				ret.flags = header->flags;
				ret.offset = header->offset;
				ret.size = header->size;
				ret.link = header->link;
				ret.info = header->info;
				ret.align = header->align;
				ret.entrySize = header->entrySize;
				ret.data = _fileBegin + ret.offset;

				ret.name = ret.data + header->name;
			}
			else
			{
				ElfSectionHeader32* header = (ElfSectionHeader32*)_begin;

				ret.type = header->type;
				ret.flags = header->flags;
				ret.offset = header->offset;
				ret.size = header->size;
				ret.link = header->link;
				ret.info = header->info;
				ret.align = header->align;
				ret.entrySize = header->entrySize;
				ret.data = _begin + ret.offset;

				ret.name = _fileBegin + ret.offset + header->name;
			}
			
			return ret;
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

		Symbol(char* const _begin, const UInt _bitwidth, char* const _sectionNames, std::vector<SectionHeader*>& _sectionHeaderTable, char* const _text) : begin(_begin)
		{
			if (_bitwidth == 64)
			{
				ElfSymbol64* sym = (ElfSymbol64*)begin;

				name = _sectionNames + sym->name;
				value = _text + sym->value;
				size = sym->size;
				info = sym->info;
				visibility = sym->other;
				def = sym->sectionHeaderIndex == 0xFFF1 ? nullptr : *(_sectionHeaderTable.data() + sym->sectionHeaderIndex);
			}
			else
			{
				ElfSymbol32* sym = (ElfSymbol32*)begin;

				name = _sectionNames + sym->name;
				value = _text + sym->value;
				size = sym->size;
				info = sym->info;
				visibility = sym->other;
				def = sym->sectionHeaderIndex == 0xFFF1 ? nullptr : *(_sectionHeaderTable.data() + sym->sectionHeaderIndex);
			}
		}
	};

	class Relocation
	{
	public:
		char* begin;

		char* location;
		U64 info;
		U64 addend;

		U64 symbolIndex;
		Symbol* symbol;

		Relocation(char* const _begin, const UInt _bitwidth, char* const symbolTable, char* const section) : begin(_begin)
		{
			if (_bitwidth == 64)
			{
				ElfRela64* rela = (ElfRela64*)begin;
				
				location = section + rela->offset;
				info = rela->info;
				addend = rela->addend;

				symbolIndex = rela->sym();

				U64 test = rela->typeId();
				llvm::ELF::R_X86_64_PLT32;
				switch (rela->type())
				{
				case llvm::ELF::R_X86_64_GOTPC64: // 64 bit offset to GOT
				{
					U64 a1 = rela->addend;
					U64 a2 = rela->offset;
					nop();
				}
				case llvm::ELF::R_X86_64_GOTOFF64: // 64-bit PC relative offset to GOT
				{
					nop();
				}
				};
			}
			else
			{
				ElfRela32* rela = (ElfRela32*)begin;

				location = symbolTable + rela->offset;
				info = rela->info;
				addend = rela->addend;
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
	char* sectionHeaderTable; // Pointer to an array of raw section headers
	U32 flags;
	U16 programHeaderEntrySize;
	U16 programHeaderSize; // Number of entries in program header table
	U16 sectionHeaderEntrySize;
	U16 sectionHeaderSize; // Number of entries in section header table	

	//SectionHeader sectionNameTableHeader;
	/*SectionHeader* stringTableHeader; // .strtab
	SectionHeader* symbolTableHeader; // .symtab
	SectionHeader* textHeader; // .text
	SectionHeader* relaTextTableHeader; // .rela.text
	SectionHeader* staticDataHeader; // .rodata*/

	//char* sectionNameTable; // Pointer to array of null-terminated strings end-to-end
	/*char* stringTable; // .strtab
	char* symbolTable; // .symtab
	char* text; // .text
	char* relaTextTable;  // .rela.text
	char* staticData; // .rodata*/

	std::map<std::string, SectionHeader> sections;
	std::vector<SectionHeader*> sectionsList; // todo: boost multi-index
	std::vector<Symbol> symbols;
	std::vector<Relocation> relocations;



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
			sectionHeaderTable = header->sectionHeader != 0 ? begin + header->sectionHeader : nullptr;
			flags = header->flags;
			assert(header->headerSize == 64);
			programHeaderEntrySize = header->programHeaderEntrySize;
			programHeaderSize = header->programHeaderSize;
			sectionHeaderEntrySize = header->sectionHeaderEntrySize;
			sectionHeaderSize = header->sectionHeaderSize;
			auto inserted = sections.insert({ ".strtab", SectionHeader::findStringTable(sectionHeaderTable + (header->sectionNameEntry * sectionHeaderEntrySize), bitwidth, begin) });
			sectionsList.push_back(&inserted.first->second);
		}
		else // 32 bit
		{
			ELFHeader32* header32 = (ELFHeader32*)begin;

			sectionHeaderTable = header32->sectionHeader != 0 ? begin + header32->sectionHeader : nullptr;
			flags = header32->flags;
			assert(header32->headerSize == 52);
			programHeaderEntrySize = header32->programHeaderEntrySize;
			programHeaderSize = header32->programHeaderSize;
			sectionHeaderEntrySize = header32->sectionHeaderEntrySize;
			sectionHeaderSize = header32->sectionHeaderSize;
			auto inserted = sections.insert({ ".strtab", SectionHeader::findStringTable(sectionHeaderTable + (header->sectionNameEntry * sectionHeaderEntrySize), bitwidth, begin) });
			sectionsList.push_back(&inserted.first->second);
		}

		// Parse section headers (program headers will never appear)
		for (auto i : ContiguousRange(0, (int)sectionHeaderSize))
		{
			SectionHeader newSection(sectionHeaderTable + (i * sectionHeaderEntrySize), bitwidth, sections[".strtab"].data, begin);
			auto inserted = sections.insert({ newSection.name, newSection });
			sectionsList.push_back(&inserted.first->second);
			std::cout << "Section: " << i << " Name: " << newSection.name << '\n';
		}

		/*symbolTableHeader = &*std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) { return strcmp(lhs.name, ".symtab") == 0; });
		stringTableHeader = &*std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) { return strcmp(lhs.name, ".strtab") == 0; });
		textHeader = &*std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) { return strcmp(lhs.name, ".text") == 0; });
		relaTextTableHeader = &*std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) { return strcmp(lhs.name, ".rela.text") == 0; });
		staticDataHeader = std::find_if(sections.begin(), sections.end(), [](const SectionHeader& lhs) { return strcmp(lhs.name, ".rodata") == 0; })._Ptr;

		symbolTable = begin + symbolTableHeader->offset;
		stringTable = begin + stringTableHeader->offset;
		text = begin + textHeader->offset;
		relaTextTable = begin + relaTextTableHeader->offset;
		staticData = begin + staticDataHeader->offset;*/

		// Parse relocations
		for (auto i : ContiguousRange(0, (int)(sections[".rela.text"].size / (bitwidth == 64 ? sizeof(ElfRela64) : sizeof(ElfRela32)))))
		{
			relocations.push_back(Relocation((char*)(sections[".rela.text"].data)+(i * (bitwidth == 64 ? sizeof(ElfRela64) : sizeof(ElfRela32))), bitwidth, sections[".symtab"].data, sections[".text"].data));
			 
		}

		// Parse symbols
		for (auto i : ContiguousRange(0, (int)(sections[".symtab"].size / (bitwidth == 64 ? sizeof(ElfSymbol64) : sizeof(ElfSymbol32)))))
		{
			symbols.push_back(Symbol(sections[".symtab"].data+(i * (bitwidth == 64 ? sizeof(ElfSymbol64) : sizeof(ElfSymbol32))), bitwidth, sections[".strtab"].data, sectionsList, sections[".text"].data));
			std::cout << "Symbol: " << i << " Name: " << symbols[i].name << " Associated section: " << (symbols[i].def == nullptr ? "" : symbols[i].def->name) << '\n';
		}



		// Give relocations links to completed symbols for debugging
		for (auto i : ContiguousRange(0, (int)(sections[".rela.text"].size / (bitwidth == 64 ? sizeof(ElfRela64) : sizeof(ElfRela32)))))
		{
			relocations[i].symbol = &symbols[relocations[i].symbolIndex];
			std::cout << "Relocation: " << i << " Info: " << relocations[i].info << " Name: " << relocations[i].symbol->name << '\n';
		}

		char* str = std::find_if(symbols.begin(), symbols.end(), [](const ElfDecoder::Symbol& lhs) { std::cout << ' ' << lhs.info; return strcmp(lhs.name, "?EvgMain@@YAHXZ") == 0; })->value;

		nop();
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

	mod->eraseNamedMetadata(mod->getNamedMetadata("llvm.linker.options"));
	/*auto meta = mod->getNamedMetadata("llvm.linker.options");
	meta->dump();

	for (const auto* i : meta->operands())
	{
		std::cout << '\n';
		llvm::cast<llvm::MDNode>(i)->dump();
	}*/


	llvm::legacy::PassManager lpm;

	llvm::SmallVector<char> outbuf;
	llvm::raw_svector_ostream outstream(outbuf);
	std::error_code ec;
	//llvm::raw_fd_ostream outstreamF("C:/Users/nickk/source/repos/Testing1/EvergreenTest/EvergreenTestMC.o", ec, llvm::sys::fs::FileAccess::FA_Write);
	//llvm::raw_pwrite_stream& outstream = *(llvm::raw_pwrite_stream*)&outstreamF;

	bool supported = targetMachine->addPassesToEmitFile(lpm, outstream, nullptr, llvm::CGFT_ObjectFile);
	lpm.run(*mod);


	std::ofstream ofs("C:/Users/nickk/source/repos/Testing1/EvergreenTest/EvergreenTestMC.o", std::ofstream::trunc | std::ofstream::binary);
	ofs.write(outbuf.data(), outbuf.size());
	ofs.close();

	using MathFn = int(*)(int, int);
	using EvgMainPtr = int(*)();

	ElfDecoder decoder(outbuf.data(), outbuf.size());
	decoder.parse();

	ElfDecoder::Symbol* call = &*std::find_if(decoder.symbols.begin(), decoder.symbols.end(), [](const ElfDecoder::Symbol& lhs) {
		std::cout << ' ' << lhs.info; return strcmp(lhs.name, "?EvgMain@@YAHXZ") == 0; });
	char* region = call->value;



	SYSTEM_INFO system_info;
	GetSystemInfo(&system_info);
	auto const page_size = system_info.dwPageSize;

	void* buffer = call; //= VirtualAlloc(nullptr, page_size, MEM_COMMIT, PAGE_READWRITE);

	std::memcpy(buffer, region, call->size);

	DWORD dummy;
	VirtualProtect(buffer, call->size + 50, PAGE_EXECUTE_READWRITE, &dummy);

	EvgMainPtr toCall = (EvgMainPtr)buffer;

	//VirtualFree(buffer, 0, MEM_RELEASE);

	U64* b = (U64*)buffer;

	std::cout << '\n' << toCall() << "\n";



	return EXIT_SUCCESS;
}
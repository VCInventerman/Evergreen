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
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCObjectWriter.h"

#include "llvm/CodeGen/Passes.h"

#include "evglib.h"

int main(int argc, char** argv)
{
    std::cout << "Evergreen Runtime v10000\n";

    llvm::LLVMContext context;

    MemFile file("C:/Users/nickk/source/repos/Testing1/EvergreenTest/EvergreenTest.o");

    llvm::SMDiagnostic error;
    UPtr<llvm::Module> mod = llvm::parseIR(llvm::MemoryBufferRef(llvm::StringRef(file.data, file.size), "EvergreenTest"), error, context);

    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    std::string serror;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, serror);

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto rm = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, rm);

    mod->setDataLayout(targetMachine->createDataLayout());
    mod->setTargetTriple(targetTriple);
    
    //todo: optimize
    llvm::PassManager<llvm::Function> pm;
    llvm::AnalysisManager<llvm::Function> am;

    //auto p = llvm::TargetLibraryInfoWrapperPass(llvm::Triple(targetTriple));
    
    //auto n = llvm::TargetLibraryInfoWrapperPass::name();
    llvm::HelloWorldPass p;
    
    //llvm::TargetRegistry::targets;
    
    /*llvm::MCObjectFileInfo finfo;
    llvm::MCContext mccontext(targetMachine->getMCAsmInfo(), targetMachine->getMCRegisterInfo(), &finfo);
    UPtr<llvm::MCAsmBackend> backend = std::make_unique<llvm::MCAsmBackend>();
    UPtr<llvm::MCCodeEmitter> cemitter = std::make_unique<llvm::MCCodeEmitter>();
    UPtr<llvm::MCObjectWriter> owriter = std::make_unique<llvm::MCObjectWriter>();

    std::unique_ptr<llvm::MCStreamer> streamer(new llvm::MCWinCOFFStreamer(mccontext, std::move(backend), std::move(cemitter), std::move(owriter)));
    auto printer = target->createAsmPrinter(*targetMachine, std::move(streamer));
    */
    //llvm::X86
    
    //std::ofstream out("C:/Users/nickk/source/repos/Testing1/EvergreenTest/EvergreenTestMC.o");
    //std::string outstring;
    //llvm::raw_pwrite_stream* outstream = (llvm::raw_pwrite_stream * )new llvm::raw_string_ostream(outstring);
    std::error_code ec;
    llvm::raw_pwrite_stream* outstream = (llvm::raw_pwrite_stream*)
        new llvm::raw_fd_ostream("C:/Users/nickk/source/repos/Testing1/EvergreenTest/EvergreenTestMC.o", ec, llvm::sys::fs::FileAccess::FA_Write);


    llvm::MCObjectFileInfo finfo;
    llvm::MCContext mccontext(targetMachine->getMCAsmInfo(), targetMachine->getMCRegisterInfo(), &finfo);
    
    const llvm::MCSubtargetInfo& sti = *targetMachine->getMCSubtargetInfo();
    const llvm::MCRegisterInfo& mri = *targetMachine->getMCRegisterInfo();
    llvm::MCCodeEmitter* mce = target->createMCCodeEmitter(*targetMachine->getMCInstrInfo(), mri, mccontext);
    llvm::MCAsmBackend* mab = target->createMCAsmBackend(sti, mri, targetMachine->Options.MCOptions);
    if (!mce || !mab)
        return true;

    llvm::Triple triple = targetMachine->getTargetTriple();
    UPtr<llvm::MCStreamer> asmStreamer(target->createMCObjectStreamer(
        triple, mccontext, UPtr<llvm::MCAsmBackend>(mab), mab->createObjectWriter(*outstream),
        UPtr<llvm::MCCodeEmitter>(mce), sti, targetMachine->Options.MCOptions.MCRelaxAll,
        targetMachine->Options.MCOptions.MCIncrementalLinkerCompatible,
        /*DWARFMustBeAtTheEnd*/ true));

    llvm::AsmPrinter* asmprinter = target->createAsmPrinter(*targetMachine, std::move(asmStreamer));



    

    // Create the AsmPrinter, which takes ownership of AsmStreamer if successful.
    /*FunctionPass* Printer =
        getTarget().createAsmPrinter(*this, std::move(AsmStreamer));
    if (!Printer)
        return true;*/








    //pm.addPass(printer.createPrinterPass());
    //am.registerPass(p);

    auto amp = llvm::PassInstrumentationAnalysis();
    am.registerPass([&]() {return amp; });

    auto fn = mod->getFunction("EvgMain");
    //pm.run(*mod, am);
    //pm.run(*fn, am);

    for (auto& f : mod->functions())
    {
        //asmprinter->runOnFunction(f)
    }


    llvm::legacy::PassManager lpm;

    //lpm.add(asmprinter->createPass(0));

    llvm::MCContext* mccontextp = &mccontext;
    bool supported = targetMachine->addPassesToEmitMC(lpm, mccontextp, *outstream);

    lpm.run(*mod);
   

    return EXIT_SUCCESS;
}
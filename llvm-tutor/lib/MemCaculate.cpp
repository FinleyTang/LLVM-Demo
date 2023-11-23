//
// Created by ftang on 2023/11/23.
//

#include "MemCaculate.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

// Pretty-prints the result of this analysis
static void printMemCaculateResult(llvm::raw_ostream &,
                                     const ResultMemCaculate &OC);

//-----------------------------------------------------------------------------
// MemCaculate implementation
//-----------------------------------------------------------------------------
llvm::AnalysisKey MemCaculate::Key;

MemCaculate::Result MemCaculate::generateMemMap(llvm::Function &Func) {
    MemCaculate::Result memMap;

    for (auto &BB : Func) {
        for (auto &Inst : BB) {
            StringRef Name = Inst.getOpcodeName();

            if (memMap.find(Name) == memMap.end()) {
                memMap[Inst.getOpcodeName()] = 1;
            } else {
                memMap[Inst.getOpcodeName()]++;
            }
        }
    }

    return memMap;
}

MemCaculate::Result MemCaculate::run(llvm::Function &Func,
                                         llvm::FunctionAnalysisManager &) {
    return generateMemMap(Func);
}

PreservedAnalyses MemCaculatePrinter::run(Function &Func,
                                            FunctionAnalysisManager &FAM) {
    auto &MemMap = FAM.getResult<MemCaculate>(Func);

    // In the legacy PM, the following string is printed automatically by the
    // pass manager. For the sake of consistency, we're adding this here so that
    // it's also printed when using the new PM.
    OS << "Printing analysis 'MemCaculate Pass' for function '"
       << Func.getName() << "':\n";

    printMemCaculateResult(OS, MemMap);
    return PreservedAnalyses::all();
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getMemCaculatePluginInfo() {
    return {
            LLVM_PLUGIN_API_VERSION, "MemCaculate", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                // #1 REGISTRATION FOR "opt -passes=print<opcode-counter>"
                // Register MemCaculatePrinter so that it can be used when
                // specifying pass pipelines with `-passes=`.
                PB.registerPipelineParsingCallback(
                        [&](StringRef Name, FunctionPassManager &FPM,
                            ArrayRef<PassBuilder::PipelineElement>) {
                            if (Name == "mem-caculate") {
                                FPM.addPass(MemCaculatePrinter(llvm::errs()));
                                return true;
                            }
                            return false;
                        });
                // #2 REGISTRATION FOR "-O{1|2|3|s}"
                // Register MemCaculatePrinter as a step of an existing pipeline.
                // The insertion point is specified by using the
                // 'registerVectorizerStartEPCallback' callback. To be more precise,
                // using this callback means that MemCaculatePrinter will be called
                // whenever the vectoriser is used (i.e. when using '-O{1|2|3|s}'.
                PB.registerVectorizerStartEPCallback(
                        [](llvm::FunctionPassManager &PM,
                           llvm::OptimizationLevel Level) {
                            PM.addPass(MemCaculatePrinter(llvm::errs()));
                        });
                // #3 REGISTRATION FOR "FAM.getResult<MemCaculate>(Func)"
                // Register MemCaculate as an analysis pass. This is required so that
                // MemCaculatePrinter (or any other pass) can request the results
                // of MemCaculate.
                PB.registerAnalysisRegistrationCallback(
                        [](FunctionAnalysisManager &FAM) {
                            FAM.registerPass([&] { return MemCaculate(); });
                        });
            }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getMemCaculatePluginInfo();
}

//------------------------------------------------------------------------------
// Helper functions - implementation
//------------------------------------------------------------------------------
static void printMemCaculateResult(raw_ostream &OutS,
                                     const ResultMemCaculate &memMap) {
    OutS << "================================================="
         << "\n";
    OutS << "LLVM-TUTOR: MemCaculate results\n";
    OutS << "=================================================\n";
    const char *str1 = "OPCODE";
    const char *str2 = "#TIMES USED";
    OutS << format("%-20s %-10s\n", str1, str2);
    OutS << "-------------------------------------------------"
         << "\n";
    for (auto &Inst : memMap) {
        OutS << format("%-20s %-10lu\n", Inst.first().str().c_str(),
                       Inst.second);
    }
    OutS << "-------------------------------------------------"
         << "\n\n";
}

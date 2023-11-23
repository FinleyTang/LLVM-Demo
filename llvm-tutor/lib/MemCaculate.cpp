//
// Created by ftang on 2023/11/23.
//

#include "MemCaculate.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <iostream>
#include <string>
using namespace llvm;

// Pretty-prints the result of this analysis
//static void printMemCaculateResult(llvm::raw_ostream &,
//                                     const ResultMemCaculate &OC);

//-----------------------------------------------------------------------------
// MemCaculate implementation
//-----------------------------------------------------------------------------
llvm::AnalysisKey MemCaculate::Key;

MemCaculate::Result MemCaculate::generateMemMap(llvm::Function &Func) {
    MemCaculate::Result memMap;
    int malloc_count =0;
    int calloc_count = 0;
    int realloc_count = 0;

    std::cout <<"===================="<< Func.getName().str()<<"====================="<<std::endl;
    for (auto &BB : Func) {
        for (auto &Inst : BB) {
            if (CallInst* CI = dyn_cast<CallInst>(&Inst)) {
                Function* calledFunction = CI->getCalledFunction();
                std::cout << calledFunction->getName().str()<<"\n";
                if (calledFunction && calledFunction->getName() == "malloc") {
                    malloc_count++;
                }
                if (calledFunction && calledFunction->getName() == "calloc") {
                    calloc_count++;
                }
                if (calledFunction && calledFunction->getName() == "realloc") {
                    realloc_count++;
                }
            }
        }
    }
    std::cout <<  "malloc_count: ";
    std::cout << malloc_count<<std::endl;
    std::cout << "calloc_count: ";
    std::cout << calloc_count<<std::endl;
    std::cout << "realloc_count: ";
    std::cout << realloc_count<<std::endl;

    return memMap;
}

MemCaculate::Result MemCaculate::run(llvm::Function &Func,
                                         llvm::FunctionAnalysisManager &) {
    return generateMemMap(Func);
}

PreservedAnalyses MemCaculatePrinter::run(Function &Func,
                                            FunctionAnalysisManager &FAM) {
    auto &MemMap = FAM.getResult<MemCaculate>(Func);
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

                PB.registerVectorizerStartEPCallback(
                        [](llvm::FunctionPassManager &PM,
                           llvm::OptimizationLevel Level) {
                            PM.addPass(MemCaculatePrinter(llvm::errs()));
                        });

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


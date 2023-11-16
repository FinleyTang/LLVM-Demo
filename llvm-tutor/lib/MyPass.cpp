//
// Created by ftang on 2023/11/16.
//

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>

using  namespace llvm;

namespace {
    struct MyPass : public PassInfoMixin<MyPass> {
        PreservedAnalyses run(Function &F, FunctionAnalysisManager  &FAM) {
            std::cout << "my pass in function" << F.getName().str() << std::endl;
            return PreservedAnalyses::all();
        }
    };
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
    return {
            LLVM_PLUGIN_API_VERSION, "MyPass", "v0.1",
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                        [](StringRef Name, FunctionPassManager &FPM,
                           ArrayRef<PassBuilder::PipelineElement>) {
                            if(Name == "my-pass"){
                                FPM.addPass(MyPass());
                                return true;
                            }
                            return false;
                        }
                );
            }
    };
}

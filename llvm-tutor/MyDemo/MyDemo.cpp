//
// Created by ftang on 2023/11/15.
//

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"



using  namespace llvm;
namespace {
    void  visitor(Function &F){
        errs() << "My demo get function" << F.getName() << "\n";
    }



struct MyDemo : PassInfoMixin<MyDemo> {

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        visitor(F);
        return PreservedAnalyses::all();
    }


    static bool isRequired() { return true; }
};
// Legacy PM implementation
struct LegacyMyDemo : public FunctionPass {
    static char ID;
    LegacyMyDemo() : FunctionPass(ID) {}
    // Main entry point - the name conveys what unit of IR this is to be run on.
    bool runOnFunction(Function &F) override {
        visitor(F);
        // Doesn't modify the input unit of IR, hence 'false'
        return false;
    }
};

}

// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getMyDemoPluginInfo() {
    return {LLVM_PLUGIN_API_VERSION, "MyDemo", LLVM_VERSION_STRING,
            [](PassBuilder &PB) {
                PB.registerPipelineParsingCallback(
                        [](StringRef Name, FunctionPassManager &FPM,
                           ArrayRef<PassBuilder::PipelineElement>) {
                            if (Name == "my-demo") {
                                FPM.addPass(MyDemo());
                                return true;
                            }
                            return false;
                        });
            }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return getMyDemoPluginInfo();
}


char LegacyMyDemo::ID = 0;

// This is the core interface for pass plugins. It guarantees that 'opt' will
// recognize LegacyHelloWorld when added to the pass pipeline on the command
// line, i.e.  via '--legacy-hello-world'
static RegisterPass<LegacyMyDemo>
        X("legacy-my-demo", "MY Demo Pass",
          true, // This pass doesn't modify the CFG => true
          false // This pass is not a pure analysis pass => false
);
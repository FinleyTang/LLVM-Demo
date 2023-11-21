//
// Created by ftang on 2023/11/21.
//

#ifndef DEMO01_MYOPCODECOUNT_H
#define DEMO01_MYOPCODECOUNT_H

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"


using ResultMyOpcodeCount = llvm::StringMap<unsigned>;

struct MyOpcodeCount : public llvm::AnalysisInfoMixin<MyOpcodeCount> {
    using Result = ResultMyOpcodeCount;
    Result run(llvm::Function &F,
               llvm::FunctionAnalysisManager &);

    MyOpcodeCount::Result generateMyOpcodeMap(llvm::Function &F);
    // Part of the official API:
    //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
    static bool isRequired() { return true; }

private:
    // A special type used by analysis passes to provide an address that
    // identifies that particular analysis pass type.
    static llvm::AnalysisKey Key;
    friend struct llvm::AnalysisInfoMixin<MyOpcodeCount>;
};


class MyOpcodeCountPrinter : public llvm::PassInfoMixin<MyOpcodeCountPrinter> {
public:
    explicit MyOpcodeCountPrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
    llvm::PreservedAnalyses run(llvm::Function &Func,
                                llvm::FunctionAnalysisManager &FAM);
    static bool isRequired() { return true; }

private:
    llvm::raw_ostream &OS;
};

#endif //DEMO01_MYOPCODECOUNT_H

//
// Created by ftang on 2023/11/23.
//

#ifndef DEMO01_MEMCACULATE_H
#define DEMO01_MEMCACULATE_H


#include "llvm/ADT/StringMap.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

//------------------------------------------------------------------------------
// New PM interface
//------------------------------------------------------------------------------
using ResultMemCaculate = llvm::StringMap<unsigned>;

struct MemCaculate : public llvm::AnalysisInfoMixin<MemCaculate> {
    using Result = ResultMemCaculate;
    Result run(llvm::Function &F,
               llvm::FunctionAnalysisManager &);

    MemCaculate::Result generateMemMap(llvm::Function &F);
    // Part of the official API:
    //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
    static bool isRequired() { return true; }

private:
    // A special type used by analysis passes to provide an address that
    // identifies that particular analysis pass type.
    static llvm::AnalysisKey Key;
    friend struct llvm::AnalysisInfoMixin<MemCaculate>;
};

//------------------------------------------------------------------------------
// New PM interface for the printer pass
//------------------------------------------------------------------------------
class MemCaculatePrinter : public llvm::PassInfoMixin<MemCaculatePrinter> {
public:
    explicit MemCaculatePrinter(llvm::raw_ostream &OutS) : OS(OutS) {}
    llvm::PreservedAnalyses run(llvm::Function &Func,
                                llvm::FunctionAnalysisManager &FAM);
    // Part of the official API:
    //  https://llvm.org/docs/WritingAnLLVMNewPMPass.html#required-passes
    static bool isRequired() { return true; }

private:
    llvm::raw_ostream &OS;
};
#endif
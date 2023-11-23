//========================================================================
// FILE:
//    MyInjectFuncCall.cpp
//
// DESCRIPTION:
//    For each function defined in the input IR module, MyInjectFuncCall inserts
//    a call to printf (from the C standard I/O library). The injected IR code
//    corresponds to the following function call in ANSI C:
//    ```C
//      printf("(llvm-tutor) Hello from: %s\n(llvm-tutor)   number of arguments: %d\n",
//             FuncName, FuncNumArgs);
//    ```
//    This code is inserted at the beginning of each function, i.e. before any
//    other instruction is executed.
//
//    To illustrate, for `void foo(int a, int b, int c)`, the code added by MyInjectFuncCall
//    will generated the following output at runtime:
//    ```
//    (llvm-tutor) Hello World from: foo
//    (llvm-tutor)   number of arguments: 3
//    ```
//
// USAGE:
//      $ opt -load-pass-plugin <BUILD_DIR>/lib/libInjectFunctCall.so `\`
//        -passes=-"inject-func-call" <bitcode-file>
//
// License: MIT
//========================================================================
#include "MyInjectFuncCall.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

#define DEBUG_TYPE "inject-func-call"

//-----------------------------------------------------------------------------
// MyInjectFuncCall implementation
//-----------------------------------------------------------------------------
bool MyInjectFuncCall::runOnModule(Module &M) {
  auto &ctx = M.getContext();
    llvm::Constant *strConstant = llvm::ConstantDataArray::getString(ctx, "Hello, world\n");
    llvm::GlobalVariable *strVar = new llvm::GlobalVariable(M, strConstant->getType(), true, llvm::GlobalValue::InternalLinkage, strConstant);
    strVar->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

    // 获取 printf 函数的声明
    llvm::FunctionType *printfType = llvm::FunctionType::get(llvm::IntegerType::getInt32Ty(ctx), llvm::PointerType::get(llvm::Type::getInt8Ty(ctx), 0), true);
    llvm::FunctionCallee printfFunc = M.getOrInsertFunction("printf", printfType);

    // 在每个函数的开头插入 printf 调用指令
    for (llvm::Module::iterator funcIter = M.begin(), funcEnd = M.end(); funcIter != funcEnd; ++funcIter) {
        llvm::Function &F = *funcIter;
        if (F.isDeclaration())
            continue;

        llvm::BasicBlock &entryBlock = F.getEntryBlock();
        llvm::Instruction &firstInstruction = *(entryBlock.getFirstInsertionPt());
        llvm::IRBuilder<> builder(&firstInstruction);

//        llvm::Constant *func_Constant = llvm::ConstantDataArray::getString(ctx, fname);
//        llvm::AllocaInst* func_name = builder.CreateAlloca(localVarType, nullptr, fname);
        StringRef fname = F.getName().str()+"\n";
        llvm::Type* localType = llvm::ArrayType::get(llvm::IntegerType::get(ctx, 8), fname.size() + 1);
        llvm::AllocaInst* local_Var = builder.CreateAlloca(localType, nullptr, "funcName");

        llvm::Constant* localConstant = llvm::ConstantDataArray::getString(ctx, fname, true);
        builder.CreateStore(localConstant, local_Var);

//        llvm::Value *args[] = {builder.CreateBitCast(strVar, llvm::Type::getInt8PtrTy(ctx)),
//                               builder.CreateBitCast(local_Var, llvm::Type::getInt8PtrTy(ctx))
//        };
        llvm::Value *args[] = {builder.CreateBitCast(local_Var, llvm::Type::getInt8PtrTy(ctx))
        };
        builder.CreateCall(printfFunc, args);
//        builder.CreateCall(printfFunc, args);
    }


  return true;
}

PreservedAnalyses MyInjectFuncCall::run(llvm::Module &M,
                                       llvm::ModuleAnalysisManager &) {
  bool Changed =  runOnModule(M);

  return (Changed ? llvm::PreservedAnalyses::none()
                  : llvm::PreservedAnalyses::all());
}


//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getMyInjectFuncCallPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "inject-func-call", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "my-inject-func-call") {
                    MPM.addPass(MyInjectFuncCall());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMyInjectFuncCallPluginInfo();
}

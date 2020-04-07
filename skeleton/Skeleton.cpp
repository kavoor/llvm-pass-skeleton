#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include <stdio.h>
#include <string>

using namespace llvm;

namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    // Create a list of instructions. Place in list is index
    std::vector<std::string> insns;
    std::vector<Value*> args;

    Function* get_table(LLVMContext &context, Module* module) {
      std::vector<Type*> arg_types{};
      auto return_type = Type::getVoidTy(context);
      FunctionType *FT = FunctionType::get(return_type, arg_types, false);
      Function* print_results_func = Function::Create(FT, Function::ExternalLinkage, "print_results", module);
      return print_results_func;
    }

    void print_keys(LLVMContext &context, Module* module, IRBuilder<> builder){
      std::vector<Type*> arg_types = {Type::getInt32Ty(context)};
      auto return_type = Type::getVoidTy(context);
      FunctionType *FT = FunctionType::get(return_type, arg_types, false);
      Function* print_key_func = Function::Create(FT, Function::ExternalLinkage, "print_string", module);
      auto intType = IntegerType::get(context, 32);

      for(auto it = insns.begin(); it != insns.end(); it++){
        std::string str = *it;

        for (int i =0; i<it->length(); i++){
          int int_char = (int)str[i];
          auto c = ConstantInt::get(intType, int_char);
          std::vector<Value*> args = {c};
          builder.CreateCall(print_key_func, args);
        }
        int int_char = (int)',';
        auto c = ConstantInt::get(intType, int_char);
        std::vector<Value*> args = {c};
        builder.CreateCall(print_key_func, args);
      }
      int int_char = (int)'\n';
      auto c = ConstantInt::get(intType, int_char);
      std::vector<Value*> args = {c};
      builder.CreateCall(print_key_func, args);
    }

    int get_index(const char* name){
      // const char* name= i.getOpcodeName();
      std::vector<std::string>::iterator itr = std::find(insns.begin(), insns.end(), name);
      if (itr != insns.cend()) {
        return std::distance(insns.begin(),itr);
      }else{
        insns.push_back(name);
        return insns.size()-1;
      }
    }

    virtual bool runOnFunction(Function &F) {
      unsigned int addCount = 0;
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> int_arg_types = {Type::getInt32Ty(Ctx)};
      Type *retType = Type::getVoidTy(Ctx);
      Module* module = F.getParent();

      for (BasicBlock& bb : F) {
        for (Instruction& i : bb) {
          if (i.getOpcode()){
            FunctionType *logAddFuncType = FunctionType::get(retType, int_arg_types, false);
            // errs() << i.getOpcodeName();
            int index = get_index(i.getOpcodeName());
            auto intType = IntegerType::get	(Ctx, 32);
            auto constant_index = ConstantInt::get	(intType, index);
            std::vector<Value*> args = {constant_index};
            FunctionCallee logFunc = module->getOrInsertFunction("log_insn", logAddFuncType);
            IRBuilder<> builder(&i);
            builder.CreateCall(logFunc, args);
          }
        }
      }

      std::vector<Type*> arg_types{};
      if (F.getName() == "run") {
        for (auto &B : F) {
          Instruction* t = B.getTerminator();
          if (auto *op = dyn_cast<ReturnInst>(t)) {
            IRBuilder<> builder(op);
            print_keys(Ctx,module,builder);
            FunctionCallee tabulate = get_table(Ctx, module);
            builder.CreateCall(tabulate, args);
          }
        }
      }
      
      // for(auto it = insns.begin(); it != insns.end(); it++){
      //   errs() << *it << ',';
      // }
      return true;
    }
  };
}

char SkeletonPass::ID = 0;

// Register the pass so `opt -skeleton` runs it.
static RegisterPass<SkeletonPass> X("skeleton", "count number of adds");

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

    Function* store_int(LLVMContext &context, Module* module) {
      auto i32t = Type::getInt32Ty(context);
      std::vector<Type*> arg_types = {i32t};
      auto return_type = Type::getVoidTy(context);
      FunctionType *FT = FunctionType::get(return_type, arg_types, false);
      Function* store_keys_func = Function::Create(FT, Function::ExternalLinkage, "store_int", module);
      return store_keys_func;
    }

    Function* init_insn_ptr(LLVMContext &context, Module* module) {
      auto i32t = Type::getInt32Ty(context);
      std::vector<Type*> arg_types = {PointerType::get(i32t, 0)};
      auto return_type = Type::getVoidTy(context);
      FunctionType *FT = FunctionType::get(return_type, arg_types, false);
      Function* init_insn = Function::Create(FT, Function::ExternalLinkage, "init_insn_ptr", module);
      return init_insn;
    }

    Function* copy_counts(LLVMContext &context, Module* module) {
      auto i32t = Type::getInt32Ty(context);
      std::vector<Type*> arg_types = {PointerType::get(i32t, 0)};
      auto return_type = Type::getVoidTy(context);
      FunctionType *FT = FunctionType::get(return_type, arg_types, false);
      Function* copy_counts_func = Function::Create(FT, Function::ExternalLinkage, "copy_counts", module);
      return copy_counts_func;
    }

    void store_keys(LLVMContext &context, Module* module, IRBuilder<> builder){
      FunctionCallee store_keys_func = store_int(context,module);
      auto intType = IntegerType::get(context, 32);

      for(auto it = insns.begin(); it != insns.end(); it++){
        std::string str = *it;

        for (int i =0; i<it->length(); i++){
          int int_char = (int)str[i];
          auto c = ConstantInt::get(intType, int_char);
          std::vector<Value*> args = {c};
          builder.CreateCall(store_keys_func, args);
        }
        int int_char = (int)',';
        auto c = ConstantInt::get(intType, int_char);
        std::vector<Value*> args = {c};
        builder.CreateCall(store_keys_func, args);
      }
      int int_char = (int)'\n';
      auto c = ConstantInt::get(intType, int_char);
      std::vector<Value*> args = {c};
      builder.CreateCall(store_keys_func, args);
    }

    int get_index(const char* name){
      std::vector<std::string>::iterator itr = std::find(insns.begin(), insns.end(), name);
      if (itr != insns.end()) {
        return std::distance(insns.begin(),itr);
      }else{
        insns.push_back(name);
        return insns.size()-1;
      }
    }

    virtual bool runOnFunction(Function &F) {
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> int_arg_types = {Type::getInt32Ty(Ctx)};
      Type *retType = Type::getVoidTy(Ctx);
      Module* module = F.getParent();
      
      std::vector<Value*> input_args;
      for( Function::arg_iterator arg = F.arg_begin(); arg != F.arg_end(); arg++){
        errs() << arg->getName();
        errs() << *(arg->getType()) << ',';
        input_args.push_back(arg);
      }

      auto I = input_args[1];
      auto C = input_args[2];


      for (BasicBlock& bb : F) {
        for (Instruction& i : bb) {
          if (i.getOpcode()){
            FunctionType *logAddFuncType = FunctionType::get(retType, int_arg_types, false);
            int index = get_index(i.getOpcodeName());
            auto intType = IntegerType::get    (Ctx, 32);
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
            std::vector<Value*> ptr_args = {I};
            FunctionCallee init_insn = init_insn_ptr(Ctx, module);
            builder.CreateCall(init_insn, ptr_args);

            store_keys(Ctx, module, builder);

            std::vector<Value*> copy_args = {C};
            FunctionCallee copy = copy_counts(Ctx, module);
            builder.CreateCall(copy, copy_args);
          }
        }
      }

      // Copy instructions over.
      return true;
    };
  };
}

char SkeletonPass::ID = 0;

// Register the pass so `opt -skeleton` runs it.
static RegisterPass<SkeletonPass> X("skeleton", "count number of adds");
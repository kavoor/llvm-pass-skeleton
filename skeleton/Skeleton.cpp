#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"
#include "llvm-c/Core.h"

using namespace llvm;

namespace {

  /* Representation of an Edge */
  struct Edge {
    const BasicBlock *SrcBB;
    const BasicBlock *DestBB;
    bool InMST = false;

    Edge(const BasicBlock *Src, const BasicBlock *Dest)
      : SrcBB(Src), DestBB(Dest) {}
  };
  
  struct CFGMST {
    std::vector<Edge> MST;
  };

  bool static hasCycleHelper(std::vector<Edge> mst, Edge current, Edge target) {
    if(&current == &target) return true;
    for(Edge element : mst){
      if(element.SrcBB == current.DestBB)
        hasCycleHelper(mst, element, target);
    }
    return false;
  }

  bool static hasCycle(std::vector<Edge> mst, Edge target){
    for(Edge el : mst){
      if(&el.SrcBB == &target.DestBB &&
        hasCycleHelper(mst, el, target))
        return true;
    }
    return false;
  }

  void static addEdges(std::vector<Edge>* edges, BasicBlock* B ){
    for (auto it = succ_begin(B), et = succ_end(B); it != et; ++it) {
      BasicBlock* successor = *it;
      edges->push_back(Edge(B, successor));
    }
  }

  std::vector<Edge> static createMST(BasicBlock* B, std::vector<Edge> allEdges){
    std::vector<Edge> mst;
    for (auto it = succ_begin(B), et = succ_end(B); it != et; ++it) {
      BasicBlock* successor = *it;
      if (!hasCycle(mst, Edge(B, successor))) {
         mst.push_back(Edge(B, successor));
      }
    }
    return mst;
  }
  bool static inMST(std::vector<Edge> mst, BasicBlock * bb){
    for (Edge e : mst){
      if(e.DestBB  == bb){
        return true;
      }
    }
    return false;
  }

  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> paramTypes = {Type::getInt32Ty(Ctx)};
      Type *retType = Type::getVoidTy(Ctx);
      FunctionType *logFuncType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee logFunc = F.getParent()->getOrInsertFunction("logop", logFuncType);
      
      int freshNum = 0;
    
      std::vector<Edge> allEdges;

      for (auto &B : F) {
        addEdges(&allEdges, &B);
      }

      // BasicBlock * firstBB = LLVMGetFirstBasicBlock(&F);
      BasicBlock* firstBB = &(F.getEntryBlock());

      auto mst = createMST(firstBB, allEdges);
      
      for (auto &B : F) {
            if (inMST(mst, &B)) continue;
            // Insert *after* `op`.
            errs() << B << "\n";
            IRBuilder<> builder(&B);
            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
            
            auto intType = IntegerType::get	(Ctx, 32);
            auto constantInt = ConstantInt::get	(intType, freshNum);
            freshNum++;
            printf("FRESHNUM %i\n", freshNum);

            // Insert a call to our function.
            Value* args[] = {constantInt};
            builder.CreateCall(logFunc, args);
      }
      for (Edge e : mst){
        errs() << LLVMGetBasicBlockName((e.SrcBB) << " -> " << LLVMGetBasicBlockName(e.DestBB) << "\n" << "@@@@@@@@@@@@@@@@@@\n";
      }
      return true;
    }
  };
};

char SkeletonPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
// static void registerSkeletonPass(const PassManagerBuilder &,
//                          legacy::PassManagerBase &PM) {
//   PM.add(new SkeletonPass());
// }
// static RegisterStandardPasses
//   RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
//                  registerSkeletonPass);

static RegisterPass<SkeletonPass> X("skeleton", "a useless pass");
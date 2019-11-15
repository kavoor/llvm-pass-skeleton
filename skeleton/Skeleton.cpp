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
#include <string>

using namespace llvm;

namespace {

  /* Representation of an Edge */
  struct Edge {
    BasicBlock *SrcBB;
    BasicBlock *DestBB;
    bool InMST = false;

    Edge( BasicBlock *Src,  BasicBlock *Dest)
      : SrcBB(Src), DestBB(Dest) {}
  };
  
  struct CFGMST {
    std::vector<Edge> MST;
  };

  bool static edgeEquals(Edge a, Edge b){
    return a.SrcBB == b.SrcBB && a.DestBB == b.DestBB;
  }

  bool static hasCycleHelper(std::vector<Edge> mst, Edge target) {
    for(Edge element : mst){
      if(element.SrcBB == target.DestBB || element.DestBB == target.DestBB) {
        return true;
      }
    }
    return false;
  }

  bool static hasCycle(std::vector<Edge> mst, Edge target){
    return hasCycleHelper(mst, target);
  }

  void static addEdges(std::vector<Edge>* edges, BasicBlock* B ){
    for (auto it = succ_begin(B), et = succ_end(B); it != et; ++it) {
      BasicBlock* successor = *it;
      edges->push_back(Edge(B, successor));
    }
  }

  void static createMST(BasicBlock* B, std::vector<Edge> allEdges, std::vector<Edge>* mst){
    for (auto it = succ_begin(B), et = succ_end(B); it != et; ++it) {
      BasicBlock* successor = *it;
      if (!hasCycle(*mst, Edge(B, successor))) {
        for(Edge i : *mst){
          if (edgeEquals(Edge(B, successor), i)){
            return;
          }
        }
        mst->push_back(Edge(B, successor));
        createMST(successor, allEdges, mst);
      }
    }
    return;
  }
  bool static inMST(std::vector<Edge> mst, BasicBlock * bb){
    for (Edge e : mst){
      if(e.DestBB  == bb){
        return true;
      }
    }
    return false;
  }

  int freshNum = 0;
  std::vector<BasicBlock*> instrumentedSrcs;
  std::vector<BasicBlock*> instrumentedDests;

  void instrument(BasicBlock * B, LLVMContext& Ctx, FunctionCallee someFunc){
    // Insert *after* `op`.
    // errs() << B << "\n";
    IRBuilder<> builder(B);
    builder.SetInsertPoint(B, B->getFirstInsertionPt());
    
    auto intType = IntegerType::get	(Ctx, 32);
    auto constantInt = ConstantInt::get	(intType, freshNum++);
    // printf("FRESHNUM %i\n", freshNum);

    // Insert a call to our function.
    // args could be name of this bb
    Value* args[] = {constantInt};
    builder.CreateCall(someFunc, args);
  }

  Function* print_results_func = NULL;
  Function* get_print_results_func(LLVMContext &context, Module* module) {
    if (print_results_func == NULL) {
      std::vector<Type*> arg_types{};
      auto return_type = Type::getVoidTy(context);
      FunctionType *FT = FunctionType::get(return_type, arg_types, false);
      print_results_func = Function::Create(FT, Function::ExternalLinkage, "print_results", module);
    }
    return print_results_func;
  }


  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      // Get the function to call from our runtime library.
      LLVMContext &Ctx = F.getContext();
      std::vector<Type*> paramTypes = {Type::getInt32Ty(Ctx)};
      Type *retType = Type::getVoidTy(Ctx);
      FunctionType *logSrcFuncType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee logSrcFunc = F.getParent()->getOrInsertFunction("logsrc", logSrcFuncType);
      FunctionType *logDestFuncType = FunctionType::get(retType, paramTypes, false);
      FunctionCallee logDestFunc = F.getParent()->getOrInsertFunction("logdest", logDestFuncType);
    
      std::vector<Edge> allEdges;

      for (auto &B : F) {
        addEdges(&allEdges, &B);
      }

      // BasicBlock * firstBB = LLVMGetFirstBasicBlock(&F);
      BasicBlock* firstBB = &(F.getEntryBlock());

      std::vector<Edge> mst;
      createMST(firstBB, allEdges, &mst);
      errs() << "SIZE " << mst.size()<<"\n\n\n";


      
      for(Edge edge : allEdges){
        bool doContinue = false;
        for(Edge mstEdge : mst) if (edgeEquals(edge, mstEdge)) doContinue = true;
        if (doContinue) continue;

        errs() << "INSTRUMENTING: " << *edge.SrcBB << "-->" << *edge.DestBB << '\n';
        bool instrumentSource = true;
        bool instrumentDest = true;
        for(BasicBlock * instrumentedB : instrumentedSrcs){
          if(instrumentedB == edge.SrcBB){
            instrumentSource = false;
            break;
          }
        }
        for(BasicBlock * instrumentedB : instrumentedDests){
          if(instrumentedB == edge.DestBB){
            instrumentDest = false;
            break;
          }
        }
        if(instrumentSource) {
          printf("Instrumenting %i as a source\n", freshNum);
          instrument(edge.SrcBB, Ctx, logSrcFunc);
          edge.SrcBB->setName(std::to_string(freshNum));
          instrumentedSrcs.push_back(edge.SrcBB);
        }
        if(instrumentDest){
          printf("Instrumenting %i as a dest\n", freshNum);
          instrument(edge.DestBB, Ctx, logDestFunc);
          edge.DestBB->setName(std::to_string(freshNum));
          instrumentedDests.push_back(edge.DestBB);
        } 
      }
      
      // for (auto &B : F) {
      //       if (inMST(mst, &B)) continue;
      //       // Insert *after* `op`.
      //       errs() << B << "\n";
      //       IRBuilder<> builder(&B);
      //       builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
            
      //       auto intType = IntegerType::get	(Ctx, 32);
      //       auto constantInt = ConstantInt::get	(intType, freshNum);
      //       freshNum++;
      //       printf("FRESHNUM %i\n", freshNum);

      //       // Insert a call to our function.
      //       Value* args[] = {constantInt};
      //       builder.CreateCall(logFunc, args);
      // }
      // for (Edge e : mst){
      //   errs() << *e.SrcBB << " -> " << *e.DestBB << "\n" << "@@@@@@@@@@@@@@@@@@\n";
      // }
      // print the results right before returning from main
      if (F.getName() == "main") {
        Module* module = F.getParent();
        auto &context = F.getContext();
        for (auto &B : F) {
          Instruction* t = B.getTerminator();
          if (auto *op = dyn_cast<ReturnInst>(t)) {
            IRBuilder<> builder(op);
            auto print_results = get_print_results_func(context, module);
            std::vector<Value*> args;
            builder.CreateCall(print_results, args);
          }
        }
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
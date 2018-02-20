#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;
namespace{
  struct BranchBias : public FunctionPass{
    static char ID;
    BranchBias() : FunctionPass(ID){};
    bool runOnFunction(Function& F){
      std::map<BasicBlock*, bool> cond;
      // search and record conditional BB
      for(BasicBlock &BB : F){
        TerminatorInst* terminator = BB.getTerminator();
        unsigned succ_num = terminator->getNumSuccessors();
        // errs()<< "BasicBlock " << BB.getName() << "has :" << succ_num <<'\n';
        if(succ_num > 1){
          for(unsigned i = 0; i < succ_num ; i++){
            cond.insert({terminator->getSuccessor(i),true});
          }
        }
      }
      // insert function into BB
      LLVMContext &context = F.getContext();
      for(BasicBlock &BB : F){
        // insert update function in each BasicBlock
        IRBuilder<> Builder(&*(BB.getFirstInsertionPt())); // init IRBuilder
        Function *updateFunc = cast<Function>(F.getParent()->getOrInsertFunction(
          "updateBranchInfo",                 // name of function
          Type::getVoidTy(context),           // return type
          Type::getInt1Ty(context)            // first parameter type
        ));
        Value* taken = ConstantInt::get(IntegerType::get(context,1), 0);
        if(cond.find(&BB) != cond.end()){
           taken = ConstantInt::get(IntegerType::get(context,1), 1);// test if false
        }
        std::vector<Value*> args;
        args.push_back(taken);
        Builder.CreateCall(updateFunc,args);
        // insert print function in ret BasicBlock
        unsigned succ_num = (BB.getTerminator())->getNumSuccessors();
        if(succ_num == 0){
          Function *printFunc = cast<Function>(F.getParent()->getOrInsertFunction(
            "printOutBranchInfo",                  // name of function
            Type::getVoidTy(context)           // return type
          ));
          Builder.CreateCall(printFunc);
        }
      }
      return false;
    }
  };
}

char BranchBias::ID = 0;
static RegisterPass<BranchBias> X("cse231-bb",
                                  "cse231-bb",
                                  false,
                                  false
                                  );

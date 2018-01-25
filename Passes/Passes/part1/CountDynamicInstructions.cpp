#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "/lib231/lib231.cpp"
#include "llvm/Support/IRBuilder.h"

using namespace llvm;
namespace{
  struct CountDynamicInstructions : public FunctionPass{
    static char ID;
    CountDynamicInstructions() : FunctionPass(ID){}
    bool runOnFunction(Function &F) override{
    std::map<BasicBlock*, int*> BB_Op_map;
    // prepare basic block instruction set
        for(BasicBlock &BB : F){
          int* instr_set = new int[65];
          for(Instruction &I : BB){
            unsigned name = I.getOpcode();
            instr_set[name]++;
          }
          // for(int i = 0; i < 65; i++){
          //   errs() << mapCodeToName(i) << " : " << instr_set[i] << '\n';
          // }
          BB_Op_map.insert({&BB,instr_set});
        }
        // insert block scanner into IR
        for(BasicBlock &BB : F){

        }

    }
  };
}

char CountDynamicInstructions::ID = 0;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi",
                                               "cse231-cdi",
                                               false,
                                               false);

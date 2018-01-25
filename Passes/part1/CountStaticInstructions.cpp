#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"

using namespace llvm;
namespace{
    struct CountStaticInstructions : public FunctionPass{
      static char ID;
      CountStaticInstructions() : FunctionPass(ID){}
      bool runOnFunction(Function &F) override{
        std::map<StringRef, int> count_map;
        Function &Func = F;

        for(BasicBlock &BB : Func){
          for(Instruction &I : BB){
            // errs() << "find instruction: " << I.getOpcodeName() << '\n';
            std::map<StringRef,int>::iterator it;
            StringRef name = I.getOpcodeName();
            it = count_map.find(name);
            if(it != count_map.end()){
              count_map[name]++;
            }else{
              count_map.insert(std::pair<StringRef,int>(name,1));
            }
          }
        }
        std::map<StringRef,int>::iterator it;
        for(it = count_map.begin(); it != count_map.end(); it++){
            errs() << it->first << '\t' << it->second << '\n';
        }

        return false;
      }
    };
}

char CountStaticInstructions::ID = 0;
static RegisterPass<CountStaticInstructions> X("cse231-csi",
                                              "cse231-csi",
                                              false,
                                              false);

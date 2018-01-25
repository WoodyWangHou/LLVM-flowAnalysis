#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/raw_ostream.h"
#include "/lib231/lib231.cpp"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;
namespace{
  struct CountDynamicInstructions : public FunctionPass{
    static char ID;
    CountDynamicInstructions() : FunctionPass(ID){}
    bool runOnFunction(Function &F) override{
    std::map<BasicBlock*, unsigned*> BB_Op_map;
    // prepare basic block instruction set
        for(BasicBlock &BB : F){
          unsigned* instr_set = new unsigned[65];
          for(Instruction &I : BB){
            unsigned name = I.getOpcode();
            instr_set[name]++;
          }
          // for(int i = 0; i < 65; i++){
          //   errs()<< mapCodeToName(i) << ":" << instr_set[i] << '\n';
          // }
          // errs() << "========================"<<'\n';
          BB_Op_map.insert({&BB,instr_set});
        }
        // insert block scanner into IR
        unsigned* instr_code = new unsigned[65];
        for(int i = 0; i < 65; i++){
          instr_code[i] = i;
        }

        LLVMContext &context = F.getContext();
        ArrayType* arrayTy = ArrayType::get(IntegerType::get(context, 32), 65);
        GlobalVariable* instrCode = new GlobalVariable(*(F.getParent()),arrayTy,true,
        GlobalValue::InternalLinkage,ConstantDataArray::get(context, *(new ArrayRef<uint32_t>(instr_code, 65))),
        "instruction_codes");

        for(BasicBlock &BB : F){
          unsigned* data = BB_Op_map.at(&BB);
          GlobalVariable* counts = new GlobalVariable(*(F.getParent()),arrayTy,true,
          GlobalValue::InternalLinkage,ConstantDataArray::get(context, *(new ArrayRef<uint32_t>(data, 65))),
          "instruction_count");

          Function *updateFunc = cast<Function>(F.getParent()->getOrInsertFunction(
            "updateInstrInfo",                           // name of function
            Type::getVoidTy(context),        // return type
            Type::getInt32Ty(context),
            Type::getInt32PtrTy(context),       // first parameter type
            Type::getInt32PtrTy(context)       // second parameter type
          ));

          IRBuilder<> Builder(&*(BB.getFirstInsertionPt())); // init IRBuilder
          Value* sizePtr = ConstantInt::get(IntegerType::get(context,32), 65);
          Value* instrCodePtr = Builder.CreatePointerCast(instrCode, Type::getInt32PtrTy(context));
          Value* countsPtr = Builder.CreatePointerCast(counts, Type::getInt32PtrTy(context));
          std::vector<Value*> args;
          args.push_back(sizePtr);
          args.push_back(instrCodePtr);
          args.push_back(countsPtr);
          Builder.CreateCall(updateFunc, args);
          // if the block contains return
          std::map<BasicBlock*, unsigned*>::iterator it = BB_Op_map.find(&BB);
          if(it->second[1] != 0){
            // if there is a return in the block, print the result
            Function *printFunc = cast<Function>(F.getParent()->getOrInsertFunction(
              "printOutInstrInfo",                           // name of function
              Type::getVoidTy(context)        // return type
            ));
            Builder.CreateCall(printFunc);
          }
        }
    }
  };
}

char CountDynamicInstructions::ID = 0;
static RegisterPass<CountDynamicInstructions> X("cse231-cdi",
                                               "cse231-cdi",
                                               false,
                                               false);

//===- ReachingDefinitionAnalysis.cpp - Implementation of Info for Reaching Definition ----------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides the interface for the passes of CSE 231 projects
//
//===----------------------------------------------------------------------===//
//===- ReachingDefinitionAnalysis.cpp ----------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides the interface for the passes of CSE 231 projects
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "231DFA.h"
#include <set>

using namespace llvm;
namespace{
    // Implementing Info class
    class ReachingInfo : public Info{
    private:
      // the set storing the info at particular edge
      std::set<unsigned> _info;
    public:
      ~ReachingInfo() {
        _info.clear();
      }

      void insert(unsigned idx){
        _info.insert(idx);
      }

      void print(){
        for(unsigned index : this->_info){
          errs() << std::to_string(index) + "|";
        }
        errs() << '\n';
        return;
      }

      static bool equals(Info* info1, Info* info2){
        ReachingInfo* if1 = cast<ReachingInfo>(info1);
        ReachingInfo* if2 = cast<ReachingInfo>(info2);
        for(unsigned idx : if1->_info){
          if(if2->_info.find(idx) == if2->_info.end()){
            return false;
          }
        }
        return true;
      }

      static void join(Info* info1, Info* info2, Info* result){
        assert(!result && "result is not initialized");
        ReachingInfo* if1 = cast<ReachingInfo>(info1);
        ReachingInfo* if2 = cast<ReachingInfo>(info2);
        ReachingInfo* res = cast<ReachingInfo>(result);

        for(unsigned idx : if1->_info){
          res->_info.insert(idx);
        }
        for(unsigned idx : if2->_info){
          res->_info.insert(idx);
        }
        return;
      }
    };

    // Implement DFA template
    class ReachingDefinitionAnalysis : public DataFlowAnalysis<ReachingInfo, true>{
    private:
      std::set<StringRef> code;
    public:
      //constructor
      ReachingDefinitionAnalysis(ReachingInfo &bottom, ReachingInfo &initialState):
      DataFlowAnalysis<ReachingInfo, true>(bottom, initialState){
        code.insert("br");
        code.insert("switch");
        code.insert("alloca");
        code.insert("load");
        code.insert("store");
        code.insert("getelementptr");
        code.insert("icmp");
        code.insert("fcmp");
        code.insert("phi");
        code.insert("select");
      }
      // implement flow function
      void flowfunction(Instruction * I,
      													std::vector<unsigned> & IncomingEdges,
  															std::vector<unsigned> & OutgoingEdges,
  															std::vector<ReachingInfo *> & Infos){
        // if instruction is the program terminal Function
        // flow function is not applicable, return
        if(OutgoingEdges.size() == 0){
          return;
        }
        // union of all edges; flow function 2
        ReachingInfo* computed = new ReachingInfo();
        for(unsigned incoming : IncomingEdges){
          std::pair<unsigned, unsigned> cur(incoming, instrToIndex(I));
          ReachingInfo* if2 = getInfoByEdge(cur);
          ReachingInfo::join(computed, if2, computed);
        }

        if(isa<PHINode>(I)){
          // flow function 3
          BasicBlock* BB = I->getParent();
          for(auto be = BB->phis().begin(), en = BB->phis().end(); be != en; be++){
            computed->insert(instrToIndex(&*be));
          }
        }else{
          if(!I->getType()->isVoidTy()){
            // flow function 1
            if(I->isBinaryOp() || I->isBitwiseLogicOp() || code.find(I->getOpcodeName())!=code.end()){
              computed->insert(instrToIndex(I));
            }
          }
        }

        // append result to Infos for every outgoing OutgoingEdges
        for(unsigned i = 0; i < OutgoingEdges.size(); i++){
          Infos.push_back(computed);
        }

        return;
      }

    };

    struct ReachingDefinitionAnalysisPass : public FunctionPass{
        static char ID;
        ReachingDefinitionAnalysisPass() : FunctionPass(ID){}
        bool runOnFunction(Function &F) override{
          ReachingInfo Bottom;
          ReachingInfo InitialState;
          ReachingDefinitionAnalysis rda(Bottom, InitialState);
          rda.runWorklistAlgorithm(&F);
          rda.print();
          return false;
        }
    };
}

char ReachingDefinitionAnalysisPass::ID = 0;
static RegisterPass<ReachingDefinitionAnalysisPass> X("cse231-reaching",
                                                      "cse231-reaching",
                                                      false,
                                                      false
                                                      );

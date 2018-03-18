//===- LivenessAnalysis.cpp - Implementation of Info for Reaching Definition ----------------------------*- C++ -*-===//
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
//===- LivenessAnalysis.cpp ----------------------------*- C++ -*-===//
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
    class LivenessInfo : public Info {
      private:
        std::set<unsigned> _info;
      public:
        ~LivenessInfo() {
          _info.clear();
        }

        void remove(unsigned idx){
          //remove the instruction index
          _info.erase(idx);
        }

        void insert(unsigned idx){
          _info.insert(idx);
        }

        void print() {
          for(unsigned index : this->_info){
            errs() << std::to_string(index) + "|";
          }
          errs() << '\n';
          return;
        }

        static bool equals(Info * info1, Info * info2){
            LivenessInfo * if1 = cast<LivenessInfo>(info1);
            LivenessInfo * if2 = cast<LivenessInfo>(info2);
            for(unsigned idx : if1->_info){
              if(if2->_info.find(idx) == if2->_info.end()){
                return false;
              }
            }
            return true;
        }

        static void join(Info * info1, Info * info2, Info * result){
          assert(!result && "result is not initialized");
          LivenessInfo* if1 = cast<LivenessInfo>(info1);
          LivenessInfo* if2 = cast<LivenessInfo>(info2);
          LivenessInfo* res = cast<LivenessInfo>(result);

          for(unsigned idx : if1->_info){
            res->_info.insert(idx);
          }
          for(unsigned idx : if2->_info){
            res->_info.insert(idx);
          }
          return;
        }
    };

    class LivenessAnalysis : public DataFlowAnalysis<LivenessInfo, false>{
    private:
      std::set<StringRef> code;
    public:
      ~LivenessAnalysis() {
        this->code.clear();
      }

      LivenessAnalysis(LivenessInfo &bottom, LivenessInfo &initialState):
      DataFlowAnalysis<LivenessInfo, false>(bottom, initialState){
        // init map with instruction that create a variable
        code.insert("alloca");
        code.insert("load");
        code.insert("getelementptr");
        code.insert("icmp");
        code.insert("fcmp");
        code.insert("select");
        // phi node is treated specially
      }
      void flowfunction(Instruction * I,
      													std::vector<unsigned> & IncomingEdges,
  															std::vector<unsigned> & OutgoingEdges,
  															std::vector<LivenessInfo *> & Infos) {
        // if no outgoing edges, do nothing
        if(OutgoingEdges.size() == 0){
          return;
        }
        // unoin all incoming IncomingEdges
        LivenessInfo *computed = new LivenessInfo();
        for(unsigned idx : IncomingEdges){
            std::pair<unsigned, unsigned> edge(idx, instrToIndex(I));
            LivenessInfo * if1 = getInfoByEdge(edge);
            LivenessInfo::join(if1, computed, computed);
        }
        // implement 3 flowfuncitons
        if(isa<PHINode>(I)){
          // flow function 3
          BasicBlock* BB = I->getParent();
          for(auto be = BB->phis().begin(), en = BB->phis().end(); be != en; be++){
            computed->remove(instrToIndex(&*be));
          }
          // trace back each outgoing outgoing edges
          for(unsigned k = 0; k < OutgoingEdges.size(); k++){
            //out[k]
            LivenessInfo * out_info = new LivenessInfo();
            LivenessInfo::join(computed, out_info, out_info);

            BasicBlock* bb = indexToInstr(OutgoingEdges[k])->getParent();
            //process phi block
            for(auto be = BB->phis().begin(), en = BB->phis().end(); be != en; be++){
              PHINode * phi = cast<PHINode>(&*be);
              int index = phi->getBasicBlockIndex(bb);
              if(index != -1){
                // this phi instruction has operand to the out
                Instruction * incoming = dyn_cast<Instruction>(phi->getIncomingValue(index));
                if(incoming){
                  // get instruction, insert idx
                  out_info->insert(instrToIndex(incoming));
                }
              }
            }

            // push out[k] to res
            Infos.push_back(out_info);
          }

        }else{
          for(auto bi = I->op_begin(), be = I->op_end(); bi != be; ++bi){
            // join operands with rest of incoming edges
            Instruction * instr = dyn_cast<Instruction>(bi);
            if(instr){
              // if operand is an instruction
              computed->insert(instrToIndex(instr));
            }
          }

          if(I->isBinaryOp() || I->isBitwiseLogicOp() || code.find(I->getOpcodeName()) != code.end()) {
            // flow function 1
            // if flow function 1, remove the current instruciton index from set
            unsigned idx = instrToIndex(I);
            computed->remove(idx);
          } // Otherwise low function 2

          // push the result to all outgoing edges
          for(unsigned i = 0; i < OutgoingEdges.size(); i++){
            Infos.push_back(computed);
          }
        }
        return;
      }
    };

    struct LivenessAnalysisPass : public FunctionPass{
        static char ID;
        LivenessAnalysisPass() : FunctionPass(ID){}
        bool runOnFunction(Function &F) override{
          LivenessInfo Bottom;
          LivenessInfo InitialState;
          LivenessAnalysis lva(Bottom, InitialState);
          lva.runWorklistAlgorithm(&F);
          lva.print();
          return false;
        }
    };
}

char LivenessAnalysisPass::ID = 0;
static RegisterPass<LivenessAnalysisPass> X("cse231-liveness",
                                            "cse231-liveness",
                                            false,
                                            false
                                           );

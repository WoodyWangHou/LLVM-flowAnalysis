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
    class MayPointToInfo : public Info {
      private:
        std::map<unsigned, std::set<unsigned>*> _info;
      public:
        ~MayPointToInfo() {
          // _info.clear();
        }

        std::set<unsigned>* get(unsigned key){
          if(_info.find(key) == _info.end()){
            return nullptr;
          }
          return _info[key];
        }

        void insert(unsigned ptr, std::set<unsigned>* set){
          if(!set || set->size() == 0){
            return;
          }

          if(_info.find(ptr) != _info.end()){
            this->_info[ptr]->insert(set->begin(), set->end());
          }else{
            std::set<unsigned>* temp = new std::set<unsigned>();
            *temp = *set;
            this->_info[ptr] = temp;
          }
        }

        void print() {
          for(auto bi = _info.begin(), be = _info.end(); bi != be; ++bi){
            unsigned ptr = bi->first;
            errs() << "R" + std::to_string(ptr) + "->(";
            std::set<unsigned> ptee = *(bi->second);
            for(unsigned pt : ptee){
              errs() << "M" + std::to_string(pt) + "/";
            }
            errs() << ")|";
          }
          errs() << '\n';
          return;
        }

        static bool equals(Info * info1, Info * info2){
            MayPointToInfo * if1 = cast<MayPointToInfo>(info1);
            MayPointToInfo * if2 = cast<MayPointToInfo>(info2);

            if(!if1 || !if2){
              return false;
            }

            if(if1->_info.size() != if2->_info.size()){
              return false;
            }
            // if map 1 and map 2 are valid and have same size
            for(auto bi = if1->_info.begin(), be = if1->_info.end(); bi != be; ++bi){
              unsigned ptr = bi->first;
              // if find any pointer missing
              if(if2->_info.find(ptr) == if2->_info.end()){
                return false;
              }
              // if find any pointee num mismatch
              std::set<unsigned> * set2 = if2->_info[ptr];
              std::set<unsigned> * set1 = bi->second;
              if(set2->size() != set1->size()){
                return false;
              }

              for(unsigned ptee : *set1){
                if(set2->find(ptee) == set2->end()){
                  return false;
                }
              }
            }
            return true;
        }

        static void join(Info * info1, Info * info2, Info * result){
          // result is assumed to be initialized before
          MayPointToInfo* if1 = cast<MayPointToInfo>(info1);
          MayPointToInfo* if2 = cast<MayPointToInfo>(info2);
          MayPointToInfo* res = cast<MayPointToInfo>(result);

          // merge if1
          for(auto bi = if1->_info.begin(), be = if1->_info.end(); bi != be; ++bi){
            unsigned ptr = bi->first;
            std::set<unsigned>* temp = bi->second; // copy over elements
            if(res->_info.find(ptr) == res->_info.end()){
              std::pair<unsigned, std::set<unsigned>*> new_entry(ptr, temp);
              res->_info.insert(new_entry);
            }else{
              res->_info[ptr]->insert(temp->begin(), temp->end());
            }
          }

          // merge if2
          for(auto bi = if2->_info.begin(), be =if2->_info.end(); bi != be; ++bi){
            unsigned ptr = bi->first;
            std::set<unsigned>* temp = bi->second; // copy over elements
            if(res->_info.find(ptr) == res->_info.end()){
              std::pair<unsigned, std::set<unsigned>*> new_entry(ptr, temp);
              res->_info.insert(new_entry);
            }else{
              res->_info[ptr]->insert(temp->begin(), temp->end());
            }
          }
        }
    };

    class MayPointToAnalysis : public DataFlowAnalysis<MayPointToInfo, true>{
    public:
      MayPointToAnalysis(MayPointToInfo &bottom, MayPointToInfo &initialState):
      DataFlowAnalysis<MayPointToInfo, true>(bottom, initialState){
        // do nothing
      }

      void flowfunction(Instruction * I,
      													std::vector<unsigned> & IncomingEdges,
  															std::vector<unsigned> & OutgoingEdges,
  															std::vector<MayPointToInfo *> & Infos) {
        // if no outgoing edges, do nothing
        if(OutgoingEdges.size() == 0){
          return;
        }
        // unoin all incoming IncomingEdges
        MayPointToInfo *computed = new MayPointToInfo();
        for(unsigned idx : IncomingEdges){
            std::pair<unsigned, unsigned> edge(idx, instrToIndex(I));
            MayPointToInfo * if1 = getInfoByEdge(edge);
            MayPointToInfo::join(if1, computed, computed);
        }
        // errs() << "computed is:" <<'\n';
        // computed->print();
        StringRef code = I->getOpcodeName();
        unsigned Ri = instrToIndex(I);
        // handle all flow function cases
        if(code.equals("alloca")){
            unsigned ptee = instrToIndex(I);
            // errs() << "Memory object is " + ptee <<'\n';
            std::set<unsigned>* set = new std::set<unsigned>();
            set->insert(ptee);
            computed->insert(Ri,set);
        } else if (code.equals("bitcast") || code.equals("getelementptr")){
            for(auto bi = I->op_begin(), be = I->op_end(); bi != be; ++bi){
              Instruction * instr = dyn_cast<Instruction>(bi);
              if(instr){
                // perform flow function once get operand that is value
                unsigned Rv = instrToIndex(instr);
                std::set<unsigned>* X = computed->get(Rv);
                computed->insert(Ri,X);
              }
            }
          } else if (code.equals("load")){
            for(auto bi = I->op_begin(), be = I->op_end(); bi != be; ++bi){
              Instruction * instr = dyn_cast<Instruction>(bi);
              if(instr){
                // perform flow function once get operand that is value
                unsigned Rp = instrToIndex(instr);
                std::set<unsigned>* X = computed->get(Rp);
                if(X){
                  //check if X exists
                  for(unsigned x : *X){
                    // get all X is pointing to
                    if(x != Rp){
                      // pointer cannot point to itself
                      std::set<unsigned>* Y = computed->get(x);
                      // remove pointer to self
                      std::set<unsigned>* removed_x_Y = new std::set<unsigned>();
                      *removed_x_Y = *Y;
                      removed_x_Y->erase(x);
                      computed->insert(Ri, removed_x_Y);
                    }
                  }
                }

              }
            }
          } else if (code.equals("store")){
            std::set<unsigned>* X = new std::set<unsigned>();
            std::set<unsigned>* Y = new std::set<unsigned>();
            for(auto bi = I->op_begin(), be = I->op_end(); bi != be; ++bi){
              Instruction * instr = dyn_cast<Instruction>(bi);
              if(instr){
                // if operand is instruction, either value or pointer
                if(isa<PointerType>(instr->getType())){
                  // operand is a pointer
                  unsigned Rp = instrToIndex(instr);
                  Y = computed->get(Rp);
                }else{
                  // operand is a value
                  unsigned Rv = instrToIndex(instr);
                  X = computed->get(Rv);
                }
              }
            }

            if(Y && X && Y->size() > 0 && X->size() > 0){
              // X and Y are all available
              for(unsigned Ry : *Y){
                computed->insert(Ry, X); // insert X to every Y
              }
            }
          } else if (code.equals("select")){
            for(auto bi = I->op_begin(), be = I->op_end(); bi != be; ++bi){
              Instruction * instr = dyn_cast<Instruction>(bi);
              if(instr){
                // if the operand is a value/instruction
                unsigned Rm =instrToIndex(instr);
                std::set<unsigned>* X = computed->get(Rm);
                computed->insert(Ri, X);
              }
            }
          }else if (code.equals("phi")){
          // iterate through phi block
            BasicBlock * BB = I->getParent();
            for(auto be = BB->phis().begin(), en = BB->phis().end(); be != en; ++be){
              PHINode* cur = dyn_cast<PHINode>(&*be);
                for(auto bi = cur->op_begin(), ben = cur->op_end(); bi != ben; ++bi){
                  // iterate through a phi instruction to get operands
                  Instruction * instr = dyn_cast<Instruction>(bi);
                  if(instr){
                    // if operand is an instruction
                    unsigned Rk = instrToIndex(instr);
                    std::set<unsigned>* X = computed->get(Rk);
                    computed->insert(Ri, X);
                  }
                }
            }
        }// else do nothing

        // append result to Infos for every outgoing OutgoingEdges
        for(unsigned i = 0; i < OutgoingEdges.size(); i++){
          Infos.push_back(computed);
        }

        return;
      }
    };

    struct MayPointToAnalysisPass : public FunctionPass{
        static char ID;
        MayPointToAnalysisPass() : FunctionPass(ID){}
        bool runOnFunction(Function &F) override{
          MayPointToInfo Bottom;
          MayPointToInfo InitialState;
          MayPointToAnalysis mpta(Bottom, InitialState);
          mpta.runWorklistAlgorithm(&F);
          mpta.print();
          return false;
        }
    };
}

char MayPointToAnalysisPass::ID = 0;
static RegisterPass<MayPointToAnalysisPass> X("cse231-maypointto",
                                            "cse231-maypointto",
                                            false,
                                            false
                                           );

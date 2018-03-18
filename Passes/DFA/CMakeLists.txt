add_llvm_loadable_module(CSE231-DFA
  ReachingDefinitionAnalysis.cpp
  LivenessAnalysis.cpp
  MayPointToAnalysis.cpp
  231DFA.h

  PLUGIN_TOOL
  opt
  )

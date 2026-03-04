add_llvm_component_library(LLVMGraphInfo
  GraphTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support
  TargetParser

  ADD_TO_COMPONENT
  Graph
)

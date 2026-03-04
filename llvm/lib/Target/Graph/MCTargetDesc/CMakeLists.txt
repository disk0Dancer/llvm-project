add_llvm_component_library(LLVMGraphDesc
  GraphMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support
  TargetParser

  ADD_TO_COMPONENT
  Graph
)

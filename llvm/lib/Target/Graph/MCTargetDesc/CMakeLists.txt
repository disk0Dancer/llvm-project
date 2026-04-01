add_llvm_component_library(LLVMGraphDesc
  GraphMCTargetDesc.cpp
  GraphMCAsmInfo.cpp
  GraphInstPrinter.cpp
  GraphMCCodeEmitter.cpp
  GraphAsmBackend.cpp
  GraphELFObjectWriter.cpp

  LINK_COMPONENTS
  MC
  Support
  GraphInfo

  ADD_TO_COMPONENT
  Graph
  )

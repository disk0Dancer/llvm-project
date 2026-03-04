add_llvm_component_group(Graph)

set(LLVM_TARGET_DEFINITIONS Graph.td)

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)

add_llvm_target(GraphCodeGen
  GraphTargetMachine.cpp

  LINK_COMPONENTS
  Analysis
  CodeGen
  Core
  MC
  GraphDesc
  GraphInfo
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils

  ADD_TO_COMPONENT
  Graph
)

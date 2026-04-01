add_llvm_component_group(Graph)

set(LLVM_TARGET_DEFINITIONS Graph.td)

tablegen(LLVM GraphGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM GraphGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM GraphGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM GraphGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM GraphGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM GraphGenCallingConv.inc -gen-callingconv)
tablegen(LLVM GraphGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM GraphGenMCCodeEmitter.inc -gen-emitter)

add_public_tablegen_target(GraphCommonTableGen)

add_llvm_target(GraphCodeGen
  GraphTargetMachine.cpp
  GraphRegisterInfo.cpp
  GraphInstrInfo.cpp
  GraphSubtarget.cpp
  GraphAsmPrinter.cpp
  GraphISelDagToDag.cpp
  GraphISelLowering.cpp
  GraphMCInstLower.cpp
  GraphMachineFunctionInfo.cpp

  LINK_COMPONENTS
  CodeGen
  Core
  Support
  Target
  GraphInfo
  AsmPrinter
  SelectionDAG

  ADD_TO_COMPONENT
  Graph
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)

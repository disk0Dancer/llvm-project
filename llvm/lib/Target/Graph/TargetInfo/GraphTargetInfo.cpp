#include "GraphTargetInfo.h"
#include "../Graph.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheGraphTarget() {
  GRAPH_DUMP_YELLOW
  static Target TheGraphTarget;
  return TheGraphTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeGraphTargetInfo() {
  GRAPH_DUMP_YELLOW
  RegisterTarget<Triple::graph> X(getTheGraphTarget(), "graph",
                                  "Graph target for LLVM course", "GRAPH");
}

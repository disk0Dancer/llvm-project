#include "GraphSubtarget.h"
#include "Graph.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "graph-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "GraphGenSubtargetInfo.inc"

GraphSubtarget::GraphSubtarget(const Triple &TT, const std::string &CPU,
                                const std::string &FS,
                                const TargetMachine &TM)
    : GraphGenSubtargetInfo(TT, CPU, /*TuneCPU=*/CPU, FS), TLInfo(TM, *this),
      FrameLowering(*this), InstrInfo() {
  GRAPH_DUMP_CYAN
}

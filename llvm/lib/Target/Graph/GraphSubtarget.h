#ifndef LLVM_LIB_TARGET_GRAPH_GRAPHSUBTARGET_H
#define LLVM_LIB_TARGET_GRAPH_GRAPHSUBTARGET_H

#include "Graph.h"
#include "GraphFrameLowering.h"
#include "GraphISelLowering.h"
#include "GraphInstrInfo.h"
#include "GraphRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "GraphGenSubtargetInfo.inc"

namespace llvm {

class GraphSubtarget : public GraphGenSubtargetInfo {
  GraphTargetLowering TLInfo;
  GraphFrameLowering FrameLowering;
  GraphRegisterInfo RegInfo;
  GraphInstrInfo InstrInfo;
  SelectionDAGTargetInfo TSInfo;

public:
  GraphSubtarget(const Triple &TT, const std::string &CPU,
                  const std::string &FS, const TargetMachine &TM);

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const GraphTargetLowering *getTargetLowering() const override {
    GRAPH_DUMP_CYAN
    return &TLInfo;
  }
  const GraphFrameLowering *getFrameLowering() const override {
    GRAPH_DUMP_CYAN
    return &FrameLowering;
  }
  const GraphRegisterInfo *getRegisterInfo() const override {
    GRAPH_DUMP_CYAN
    return &RegInfo;
  }
  const GraphInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    GRAPH_DUMP_CYAN
    return &TSInfo;
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPHSUBTARGET_H

#ifndef LLVM_LIB_TARGET_GRAPH_GRAPHFRAMELOWERING_H
#define LLVM_LIB_TARGET_GRAPH_GRAPHFRAMELOWERING_H

#include "Graph.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class GraphSubtarget;

class GraphFrameLowering : public TargetFrameLowering {
public:
  GraphFrameLowering(const GraphSubtarget &STI)
      : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, Align(4), 0),
        STI(STI) {
    GRAPH_DUMP_GREEN
  }

  void emitPrologue(MachineFunction &MF,
                    MachineBasicBlock &MBB) const override {}
  void emitEpilogue(MachineFunction &MF,
                    MachineBasicBlock &MBB) const override {}

  bool hasFPImpl(const MachineFunction &MF) const override { return false; }

private:
  const GraphSubtarget &STI;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPHFRAMELOWERING_H

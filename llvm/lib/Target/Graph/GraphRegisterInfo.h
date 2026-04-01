#ifndef LLVM_LIB_TARGET_GRAPH_GRAPHREGISTERINFO_H
#define LLVM_LIB_TARGET_GRAPH_GRAPHREGISTERINFO_H

#define GET_REGINFO_HEADER
#include "GraphGenRegisterInfo.inc"

namespace llvm {

class TargetInstrInfo;
class GraphSubtarget;

struct GraphRegisterInfo : public GraphGenRegisterInfo {
public:
  GraphRegisterInfo();

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID CC) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool requiresRegisterScavenging(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPHREGISTERINFO_H

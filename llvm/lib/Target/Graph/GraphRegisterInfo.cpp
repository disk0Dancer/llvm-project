#include "GraphRegisterInfo.h"
#include "Graph.h"
#include "GraphFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "GraphGenRegisterInfo.inc"

GraphRegisterInfo::GraphRegisterInfo() : GraphGenRegisterInfo(Graph::R0) {
  GRAPH_DUMP_GREEN
}

const MCPhysReg *
GraphRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  GRAPH_DUMP_GREEN
  return CSR_Graph_SaveList;
}

BitVector GraphRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  GRAPH_DUMP_GREEN
  GraphFrameLowering const *TFI = getFrameLowering(MF);

  BitVector Reserved(getNumRegs());
  Reserved.set(Graph::R0);
  Reserved.set(Graph::R1);

  if (TFI->hasFP(MF)) {
    Reserved.set(Graph::R2);
  }
  return Reserved;
}

bool GraphRegisterInfo::requiresRegisterScavenging(
    const MachineFunction &MF) const {
  return false;
}

bool GraphRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                             int SPAdj, unsigned FIOperandNum,
                                             RegScavenger *RS) const {
  GRAPH_DUMP_GREEN
  assert(SPAdj == 0 && "Unexpected non-zero SPAdj value");

  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  DebugLoc DL = MI.getDebugLoc();

  int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
  Register FrameReg;
  int Offset = getFrameLowering(MF)
      ->getFrameIndexReference(MF, FrameIndex, FrameReg)
      .getFixed();
  Offset += MI.getOperand(FIOperandNum + 1).getImm();

  if (!isInt<16>(Offset)) {
    llvm_unreachable("");
  }

  MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, false, false, false);
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
  return false;
}

Register GraphRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  GRAPH_DUMP_GREEN
  const TargetFrameLowering *TFI = getFrameLowering(MF);
  return TFI->hasFP(MF) ? Graph::R2 : Graph::R1;
}

const uint32_t *
GraphRegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                         CallingConv::ID CC) const {
  GRAPH_DUMP_GREEN
  return CSR_Graph_RegMask;
}

#ifndef LLVM_LIB_TARGET_GRAPH_GRAPHINSTRINFO_H
#define LLVM_LIB_TARGET_GRAPH_GRAPHINSTRINFO_H

#include "MCTargetDesc/GraphInfo.h"
#include "GraphRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "GraphGenInstrInfo.inc"

namespace llvm {

class GraphSubtarget;

class GraphInstrInfo : public GraphGenInstrInfo {
public:
  GraphInstrInfo();
  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator I, Register SrcReg,
                           bool IsKill, int FI, const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI, Register VReg,
                           MachineInstr::MIFlag Flags) const override {};
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPHINSTRINFO_H

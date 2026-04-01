#include "MCTargetDesc/GraphInfo.h"
#include "../Graph.h"
#include "GraphInstPrinter.h"
#include "GraphMCAsmInfo.h"
#include "../TargetInfo/GraphTargetInfo.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_REGINFO_MC_DESC
#include "GraphGenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#include "GraphGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "GraphGenSubtargetInfo.inc"

static MCRegisterInfo *createGraphMCRegisterInfo(const Triple &TT) {
  GRAPH_DUMP_MAGENTA
  MCRegisterInfo *X = new MCRegisterInfo();
  InitGraphMCRegisterInfo(X, Graph::R0);
  return X;
}

static MCInstrInfo *createGraphMCInstrInfo() {
  GRAPH_DUMP_MAGENTA
  MCInstrInfo *X = new MCInstrInfo();
  InitGraphMCInstrInfo(X);
  return X;
}

static MCSubtargetInfo *createGraphMCSubtargetInfo(const Triple &TT,
                                                    StringRef CPU,
                                                    StringRef FS) {
  GRAPH_DUMP_MAGENTA
  return createGraphMCSubtargetInfoImpl(TT, CPU, /*TuneCPU*/ CPU, FS);
}

static MCAsmInfo *createGraphMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  GRAPH_DUMP_MAGENTA
  MCAsmInfo *MAI = new GraphELFMCAsmInfo(TT);
  unsigned SP = MRI.getDwarfRegNum(Graph::R1, true);
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, SP, 0);
  MAI->addInitialFrameState(Inst);
  return MAI;
}

static MCInstPrinter *createGraphMCInstPrinter(const Triple &T,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  GRAPH_DUMP_MAGENTA
  return new GraphInstPrinter(MAI, MII, MRI);
}

// We need to define this function for linking succeed
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeGraphTargetMC() {
  GRAPH_DUMP_MAGENTA
  Target &TheGraphTarget = getTheGraphTarget();
  RegisterMCAsmInfoFn X(TheGraphTarget, createGraphMCAsmInfo);
  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheGraphTarget, createGraphMCRegisterInfo);
  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheGraphTarget, createGraphMCInstrInfo);
  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheGraphTarget,
                                           createGraphMCSubtargetInfo);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(TheGraphTarget,
                                         createGraphMCInstPrinter);
  // Register the MC Code Emitter.
  TargetRegistry::RegisterMCCodeEmitter(TheGraphTarget,
                                         createGraphMCCodeEmitter);
  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(TheGraphTarget, createGraphAsmBackend);
}

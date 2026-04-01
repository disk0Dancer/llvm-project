#include "MCTargetDesc/GraphInstPrinter.h"
#include "Graph.h"
#include "GraphSubtarget.h"
#include "GraphTargetMachine.h"
#include "TargetInfo/GraphTargetInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {

class GraphAsmPrinter : public AsmPrinter {
  const MCSubtargetInfo *STI;

public:
  explicit GraphAsmPrinter(TargetMachine &TM,
                            std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)), STI(TM.getMCSubtargetInfo()) {
    GRAPH_DUMP_GREEN
  }

  void emitInstruction(const MachineInstr *MI) override;

  StringRef getPassName() const override { return "Graph Assembly Printer"; }

  bool lowerPseudoInstExpansion(const MachineInstr *MI, MCInst &Inst);

  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp) const {
    return LowerGraphMachineOperandToMCOperand(MO, MCOp, *this);
  }
};

} // end anonymous namespace

#include "GraphGenMCPseudoLowering.inc"

void GraphAsmPrinter::emitInstruction(const MachineInstr *MI) {
  GRAPH_DUMP_GREEN
  if (MCInst OutInst; lowerPseudoInstExpansion(MI, OutInst)) {
    EmitToStreamer(*OutStreamer, OutInst);
    return;
  }

  MCInst TmpInst;
  if (!lowerGraphMachineInstrToMCInst(MI, TmpInst, *this))
    EmitToStreamer(*OutStreamer, TmpInst);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeGraphAsmPrinter() {
  RegisterAsmPrinter<GraphAsmPrinter> X(getTheGraphTarget());
}

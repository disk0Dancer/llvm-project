#ifndef LLVM_LIB_TARGET_GRAPH_GRAPH_H
#define LLVM_LIB_TARGET_GRAPH_GRAPH_H

#include "MCTargetDesc/GraphMCTargetDesc.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"

/*#define GRAPH_DUMP(Color)                                                      \
  {                                                                            \
    llvm::errs().changeColor(Color)                                            \
        << __func__ << "\n\t\t" << __FILE__ << ":" << __LINE__ << "\n";        \
    llvm::errs().changeColor(llvm::raw_ostream::WHITE);                        \
  }*/
#define GRAPH_DUMP(Color) {}

#define GRAPH_DUMP_RED     GRAPH_DUMP(llvm::raw_ostream::RED)
#define GRAPH_DUMP_GREEN   GRAPH_DUMP(llvm::raw_ostream::GREEN)
#define GRAPH_DUMP_YELLOW  GRAPH_DUMP(llvm::raw_ostream::YELLOW)
#define GRAPH_DUMP_CYAN    GRAPH_DUMP(llvm::raw_ostream::CYAN)
#define GRAPH_DUMP_MAGENTA GRAPH_DUMP(llvm::raw_ostream::MAGENTA)
#define GRAPH_DUMP_WHITE   GRAPH_DUMP(llvm::raw_ostream::WHITE)

namespace llvm {

class GraphTargetMachine;
class FunctionPass;
class GraphSubtarget;
class AsmPrinter;
class InstructionSelector;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;

bool lowerGraphMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                     AsmPrinter &AP);
bool LowerGraphMachineOperandToMCOperand(const MachineOperand &MO,
                                          MCOperand &MCOp,
                                          const AsmPrinter &AP);
FunctionPass *createGraphISelDag(GraphTargetMachine &TM,
                                  CodeGenOptLevel OptLevel);

} // namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPH_H

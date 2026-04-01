#ifndef LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHINFO_H
#define LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHINFO_H

#include "llvm/MC/MCInstrDesc.h"

namespace llvm {

namespace GraphOp {
enum OperandType : unsigned {
  OPERAND_SIMM16 = MCOI::OPERAND_FIRST_TARGET,
};
} // namespace GraphOp

} // end namespace llvm

#endif

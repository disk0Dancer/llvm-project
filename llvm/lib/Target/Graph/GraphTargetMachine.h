#ifndef LLVM_LIB_TARGET_GRAPH_GRAPHTARGETMACHINE_H
#define LLVM_LIB_TARGET_GRAPH_GRAPHTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {

class GraphTargetMachine : public CodeGenTargetMachineImpl {
public:
  GraphTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPHTARGETMACHINE_H

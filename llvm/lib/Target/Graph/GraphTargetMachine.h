#ifndef LLVM_LIB_TARGET_GRAPH_GRAPHTARGETMACHINE_H
#define LLVM_LIB_TARGET_GRAPH_GRAPHTARGETMACHINE_H

#include "GraphSubtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"
#include <optional>

namespace llvm {
extern Target TheGraphTarget;

class GraphTargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  GraphSubtarget Subtarget;

public:
  GraphTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     std::optional<Reloc::Model> RM,
                     std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                     bool JIT);

  const GraphSubtarget *getSubtargetImpl(const Function &) const override {
    GRAPH_DUMP_CYAN
    return &Subtarget;
  }
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;
  TargetLoweringObjectFile *getObjFileLowering() const override;
};
} // end namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPHTARGETMACHINE_H

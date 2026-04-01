#include "GraphTargetMachine.h"
#include "Graph.h"
#include "TargetInfo/GraphTargetInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include <optional>

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeGraphTarget() {
  GRAPH_DUMP_CYAN
  RegisterTargetMachine<GraphTargetMachine> A(getTheGraphTarget());
}

GraphTargetMachine::GraphTargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       std::optional<Reloc::Model> RM,
                                       std::optional<CodeModel::Model> CM,
                                       CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(
          T, "e-m:e-p:32:32-i8:8:32-i16:16:32-i64:64-n32", TT, CPU, FS,
          Options, Reloc::Static,
          getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  GRAPH_DUMP_CYAN
  initAsmInfo();
}

namespace {

class GraphPassConfig : public TargetPassConfig {
public:
  GraphPassConfig(GraphTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  GraphTargetMachine &getGraphTargetMachine() const {
    return getTM<GraphTargetMachine>();
  }

  bool addInstSelector() override {
    GRAPH_DUMP_CYAN
    addPass(createGraphISelDag(getGraphTargetMachine(), getOptLevel()));
    return false;
  }
};

} // end anonymous namespace

TargetPassConfig *GraphTargetMachine::createPassConfig(PassManagerBase &PM) {
  GRAPH_DUMP_CYAN
  return new GraphPassConfig(*this, PM);
}

TargetLoweringObjectFile *GraphTargetMachine::getObjFileLowering() const {
  GRAPH_DUMP_CYAN
  return TLOF.get();
}

#ifndef LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHMCASMINFO_H
#define LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class GraphELFMCAsmInfo : public MCAsmInfoELF {
public:
  explicit GraphELFMCAsmInfo(const Triple &TheTriple);
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHMCASMINFO_H

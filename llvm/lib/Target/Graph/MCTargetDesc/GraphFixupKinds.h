#ifndef LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHFIXUPKINDS_H
#define LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Graph {
enum Fixups {
  fixup_Graph_PC16 = FirstTargetFixupKind,
  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
} // namespace Graph
} // namespace llvm

#endif

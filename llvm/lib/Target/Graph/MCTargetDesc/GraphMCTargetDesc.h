#ifndef LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHMCTARGETDESC_H
#define LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHMCTARGETDESC_H

#include <memory>

namespace llvm {

class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCAsmBackend;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

MCCodeEmitter *createGraphMCCodeEmitter(const MCInstrInfo &MCII,
                                        MCContext &Ctx);

MCAsmBackend *createGraphAsmBackend(const Target &T,
                                    const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createGraphELFObjectWriter(bool Is64Bit,
                                                                  uint8_t OSABI);

} // namespace llvm

// Defines symbolic names for Graph registers.
#define GET_REGINFO_ENUM
#include "GraphGenRegisterInfo.inc"

// Defines symbolic names for the Graph instructions.
#define GET_INSTRINFO_ENUM
#include "GraphGenInstrInfo.inc"

#endif // LLVM_LIB_TARGET_GRAPH_MCTARGETDESC_GRAPHMCTARGETDESC_H

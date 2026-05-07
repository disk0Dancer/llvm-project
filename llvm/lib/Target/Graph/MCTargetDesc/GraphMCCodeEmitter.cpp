#include "MCTargetDesc/GraphFixupKinds.h"
#include "MCTargetDesc/GraphInfo.h"
#include "MCTargetDesc/GraphMCTargetDesc.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/EndianStream.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "mccodeemitter"

STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

namespace {
class GraphMCCodeEmitter : public MCCodeEmitter {
  GraphMCCodeEmitter(const GraphMCCodeEmitter &) = delete;
  void operator=(const GraphMCCodeEmitter &) = delete;
  MCContext &Ctx;
  MCInstrInfo const &MCII;

public:
  GraphMCCodeEmitter(const MCInstrInfo &mcii, MCContext &ctx)
      : Ctx(ctx), MCII(mcii) {}

  ~GraphMCCodeEmitter() override = default;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// Return binary encoding of operand.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned getSImm16OpValue(const MCInst &MI, unsigned OpNo,
                            SmallVectorImpl<MCFixup> &Fixups,
                            const MCSubtargetInfo &STI) const;

  unsigned getBranchTarget16OpValue(const MCInst &MI, unsigned OpNo,
                                    SmallVectorImpl<MCFixup> &Fixups,
                                    const MCSubtargetInfo &STI) const;
};
} // end anonymous namespace

#include "GraphGenMCCodeEmitter.inc"

MCCodeEmitter *llvm::createGraphMCCodeEmitter(const MCInstrInfo &MCII,
                                              MCContext &Ctx) {
  return new GraphMCCodeEmitter(MCII, Ctx);
}

void GraphMCCodeEmitter::encodeInstruction(const MCInst &MI,
                                           SmallVectorImpl<char> &CB,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  uint32_t Bits = getBinaryCodeForInstr(MI, Fixups, STI);
  support::endian::write<uint32_t>(CB, Bits, llvm::endianness::little);
  ++MCNumEmitted;
}

unsigned
GraphMCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                      SmallVectorImpl<MCFixup> &Fixups,
                                      const MCSubtargetInfo &STI) const {
  if (MO.isReg())
    return Ctx.getRegisterInfo()->getEncodingValue(MO.getReg());
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  assert(MO.isExpr());
  const MCExpr *Expr = MO.getExpr();
  int64_t Res;
  if (Expr->evaluateAsAbsolute(Res))
    return Res;
  Fixups.push_back(MCFixup::create(
      0, Expr, static_cast<MCFixupKind>(Graph::fixup_Graph_16)));
  return 0;
}

unsigned
GraphMCCodeEmitter::getSImm16OpValue(const MCInst &MI, unsigned OpNo,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isImm())
    return static_cast<unsigned>(MO.getImm());

  assert(MO.isExpr());
  const MCExpr *Expr = MO.getExpr();
  int64_t Res;
  if (Expr->evaluateAsAbsolute(Res))
    return Res;
  Fixups.push_back(MCFixup::create(
      0, Expr, static_cast<MCFixupKind>(Graph::fixup_Graph_16)));
  return 0;
}

unsigned
GraphMCCodeEmitter::getBranchTarget16OpValue(const MCInst &MI, unsigned OpNo,
                                             SmallVectorImpl<MCFixup> &Fixups,
                                             const MCSubtargetInfo &STI) const {
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isImm())
    return MO.getImm() / 4;

  assert(MO.isExpr());
  const MCExpr *Expr = MO.getExpr();
  Fixups.push_back(MCFixup::create(
      0, Expr, static_cast<MCFixupKind>(Graph::fixup_Graph_PC16)));
  return 0;
}

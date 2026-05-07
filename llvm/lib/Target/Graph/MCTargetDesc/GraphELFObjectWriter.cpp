#include "../Graph.h"
#include "MCTargetDesc/GraphFixupKinds.h"
#include "MCTargetDesc/GraphMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class GraphELFObjectWriter : public MCELFObjectTargetWriter {
public:
  GraphELFObjectWriter(bool Is64Bit, uint8_t OSABI)
      : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_GRAPH,
                                /*HasRelocationAddend*/ true) {}

  ~GraphELFObjectWriter() override = default;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;

  bool needsRelocateWithSymbol(const MCValue &Val, const MCSymbol &Sym,
                               unsigned Type) const override;
};
} // namespace

unsigned GraphELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
  MCFixupKind Kind = Fixup.getKind();
  if (Kind >= FirstLiteralRelocationKind)
    return Kind - FirstLiteralRelocationKind;

  switch (Kind) {
  case Graph::fixup_Graph_PC16:
    return ELF::R_GRAPH_PC16;
  case Graph::fixup_Graph_16:
    return ELF::R_GRAPH_16;
  default:
    llvm_unreachable("Unimplemented fixup -> relocation");
  }
}

bool GraphELFObjectWriter::needsRelocateWithSymbol(const MCValue &,
                                                   const MCSymbol &,
                                                   unsigned Type) const {
  return false;
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createGraphELFObjectWriter(bool Is64Bit, uint8_t OSABI) {
  return std::make_unique<GraphELFObjectWriter>(Is64Bit, OSABI);
}

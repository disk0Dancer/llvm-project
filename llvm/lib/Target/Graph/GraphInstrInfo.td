//===----------------------------------------------------------------------===//
// Graph profiles and nodes
//===----------------------------------------------------------------------===//

def SDT_GraphBrCC : SDTypeProfile<0, 2, [ SDTCisVT<0, i32>,
  SDTCisVT<1, OtherVT>]>;

def graph_brcc : SDNode<"GraphISD::BR_CC", SDT_GraphBrCC,
  [SDNPHasChain]>;

def graph_ret : SDNode<"GraphISD::RET", SDTNone,
  [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

//===----------------------------------------------------------------------===//
// Instruction format superclass
//===----------------------------------------------------------------------===//

include "GraphInstrFormats.td"

//===----------------------------------------------------------------------===//
// Graph Operand, Complex Patterns and Transformations Definitions.
//===----------------------------------------------------------------------===//

class SImmAsmOperand<int width> : AsmOperandClass {
  let Name = "SImm" # width;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", Name);
}

def simm16 : Operand<i32>, ImmLeaf<i32, [{return isUInt<16>(Imm);}]> {
  let ParserMatchClass = SImmAsmOperand<16>;
  let EncoderMethod = "getSImm16OpValue";
  let DecoderMethod = "decodeSImmOperand<16>";
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<16>(Imm);
    return MCOp.isBareSymbolRef();
  }];
  let OperandType = "OPERAND_SIMM16";
  let OperandNamespace = "GraphOp";
}

def btarget16 : Operand<OtherVT> {
  let ParserMatchClass = SImmAsmOperand<16>;
  let PrintMethod = "printBranchOperand";
  let EncoderMethod = "getBranchTarget16OpValue";
  let DecoderMethod = "decodeSImmOperand<16>";
  let MCOperandPredicate = [{
    int64_t Imm;
    if (MCOp.evaluateAsConstantImm(Imm))
      return isInt<16>(Imm);
    return MCOp.isBareSymbolRef();
  }];
  let OperandType = "OPERAND_PCREL";
}

def LI16 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((uint64_t)N->getZExtValue() & 0xffff,
                                   SDLoc(N), MVT::i32);
}]>;

def HI16 : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((uint64_t)N->getZExtValue() >> 16, SDLoc(N),
                                   MVT::i32);
}]>;
def addr : ComplexPattern<i32, 2, "SelectAddr", [], []>;

//===----------------------------------------------------------------------===//
// Instructions specific format
//===----------------------------------------------------------------------===//

let hasSideEffects = 0, mayLoad = 0, mayStore = 0,
    Constraints = "$r2 = $r2_" in
class IncCCi<bits<8> op>
  : GraphInst<op, (outs GPR:$r1, GPR:$r2_), (ins GPR:$r2, simm16:$r3_imm),
              NAME#" $r1 $r2 $r3_imm", []>;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0 in
multiclass Arithm<bits<8> opcode, SDPatternOperator op> {
  def NAME : GraphInst<opcode, (outs GPR:$r1), (ins GPR:$r2, GPR:$r3_imm),
                       NAME#" $r1 $r2 $r3_imm",
                       [(set GPR:$r1, (op GPR:$r2, GPR:$r3_imm))]>;
  def NAME#I : GraphInst<!add(opcode,0x10), (outs GPR:$r1), (ins GPR:$r2, simm16:$r3_imm),
                         NAME#"i $r1 $r2 $r3_imm",
                         [(set GPR:$r1, (op GPR:$r2, simm16:$r3_imm))]>;
}

//===----------------------------------------------------------------------===//
// Graph Instructions
//===----------------------------------------------------------------------===//


let hasSideEffects = 0, mayLoad = 0, mayStore = 0, isBarrier = 1, isBranch = 1, isTerminator = 1, r1 = 0, r2 = 0 in
def B : GraphInst<0xB0, (outs), (ins btarget16:$r3_imm), "B $r3_imm",
  [(br bb:$r3_imm)]>;

let hasSideEffects = 0, mayLoad = 0, mayStore = 0, isCall = 1, isBranch = 1, isTerminator = 1, r2 = 0, r3_imm = 0 in
def BR : GraphInst<0xBB, (outs), (ins GPR:$r1), "BR $r1",
  [(brind GPR:$r1)]>;

let isBarrier = 1, isReturn = 1, isTerminator = 1 in
def PseudoRET : GraphPseudoInst<(outs), (ins), "# PseudoRET",
  [(graph_ret)]>,
  PseudoInstExpansion<(BR R0)>;

def SDT_GraphCallSeqStart : SDCallSeqStart<[ SDTCisVT<0, i32>,
                                             SDTCisVT<1, i32> ]>;
def SDT_GraphCallSeqEnd   : SDCallSeqEnd  <[ SDTCisVT<0, i32>,
                                             SDTCisVT<1, i32> ]>;
def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_GraphCallSeqStart,
                           [SDNPHasChain, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END",   SDT_GraphCallSeqEnd,
                           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
let Defs = [R1], Uses = [R1] in {
  def ADJCALLSTACKDOWN : GraphPseudoInst<
      (outs), (ins i32imm:$amt1, i32imm:$amt2),
      "# ADJCALLSTACKDOWN $amt1 $amt2",
      [(callseq_start timm:$amt1, timm:$amt2)]>;
  def ADJCALLSTACKUP : GraphPseudoInst<
      (outs), (ins i32imm:$amt1, i32imm:$amt2),
      "# ADJCALLSTACKUP $amt1 $amt2",
      [(callseq_end timm:$amt1, timm:$amt2)]>;
}

let isBranch = 1, isTerminator = 1, r2 = 0 in
def BR_COND : GraphInst<0xBC, (outs), (ins GPR:$r1, btarget16:$r3_imm),
  "BR_COND $r1 $r3_imm", [(graph_brcc GPR:$r1, bb:$r3_imm)]>;

// movli
let isAsCheapAsAMove = 1, isCodeGenOnly = 1, isReMaterializable = 1, r2 = 0 in
def MOVLI : GraphInst<0xAA, (outs GPR:$r1), (ins simm16:$r3_imm),
  "MOVli $r1 $r3_imm",
  [(set GPR:$r1, simm16:$r3_imm)]>;
// movhi
let isAsCheapAsAMove = 1, isCodeGenOnly = 1, isReMaterializable = 1, r2 = 0 in
def MOVHI : GraphInst<0xAF, (outs GPR:$r1), (ins simm16:$r3_imm),
  "MOVhi $r1 $r3_imm", []>;

def INC_EQi : IncCCi<0x90>;
def INC_NEi : IncCCi<0x91>;
def INC_LEi : IncCCi<0x92>;
def INC_LTi : IncCCi<0x93>;
def INC_GEi : IncCCi<0x94>;
def INC_GTi : IncCCi<0x95>;

defm ADD : Arithm<0x30, add>;
defm SUB : Arithm<0x31, sub>;
defm MUL : Arithm<0x32, mul>;
defm DIV : Arithm<0x33, sdiv>;
defm OR  : Arithm<0x34, or>;
defm AND : Arithm<0x35, and>;
defm XOR : Arithm<0x36, xor>;
defm SRA : Arithm<0x37, sra>;
defm SRL : Arithm<0x38, srl>;
defm SHL : Arithm<0x39, shl>;
defm UREM: Arithm<0x3a, urem>;

// Memory ops live in a separate opcode block to avoid colliding with the
// immediate-form arithmetic (0x40..0x4A produced by the Arithm multiclass).
let hasSideEffects = 0, mayLoad = 1, mayStore = 0 in
def LD : GraphInst<0x50, (outs GPR:$r1), (ins GPR:$r2, simm16:$r3_imm),
  "LD $r1 $r2 $r3_imm", []>;

let hasSideEffects = 0, mayLoad = 0, mayStore = 1 in
def ST : GraphInst<0x51, (outs), (ins GPR:$r1, GPR:$r2, simm16:$r3_imm),
  "ST $r1 $r2 $r3_imm", []>;

// graphics intrinsics
let hasSideEffects = 1, mayLoad = 0, mayStore = 0 in {
def GFX_INIT : GraphInst<0xC0, (outs), (ins GPR:$r1, GPR:$r2),
  "GFX_INIT $r1 $r2", [(int_graph_init GPR:$r1, GPR:$r2)]> { let r3_imm = 0; }
def GFX_SET_COLOR : GraphInst<0xC1, (outs), (ins GPR:$r1),
  "GFX_SET_COLOR $r1", [(int_graph_set_color GPR:$r1)]> { let r2 = 0; let r3_imm = 0; }
def GFX_DRAW_PIXEL : GraphInst<0xC2, (outs), (ins GPR:$r1, GPR:$r2),
  "GFX_DRAW_PIXEL $r1 $r2", [(int_graph_draw_pixel GPR:$r1, GPR:$r2)]> { let r3_imm = 0; }
def GFX_FLUSH : GraphInst<0xC3, (outs), (ins),
  "GFX_FLUSH", [(int_graph_flush)]> { let r1 = 0; let r2 = 0; let r3_imm = 0; }
}

// abs
let r3_imm = 0 in
def ABS : GraphInst<0xAB, (outs GPR:$r1), (ins GPR:$r2), "ABS $r1 $r2", [(set GPR:$r1, (abs GPR:$r2))]>;

//===----------------------------------------------------------------------===//
// Graph Other patterns
//===----------------------------------------------------------------------===//

def i32hi16 : Operand<i32>, PatLeaf<(i32 imm), [{
  // i32hi16 predicate - true if the 32-bit immediate has only leftmost 16
  // bits set.
  return ((N->getZExtValue() & 0xFFFF0000UL) == N->getZExtValue());}], HI16> {
}

// i32 imm
def : Pat<(i32 imm:$r3_imm), (ORI (MOVHI (HI16 imm:$r3_imm)), (LI16 imm:$r3_imm))>;
def : Pat<(i32 i32hi16:$r3_imm), (MOVHI i32hi16:$r3_imm)>;

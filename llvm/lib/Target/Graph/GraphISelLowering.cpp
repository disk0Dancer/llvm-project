#include "GraphISelLowering.h"
#include "Graph.h"
#include "GraphMachineFunctionInfo.h"
#include "GraphRegisterInfo.h"
#include "GraphSubtarget.h"
#include "GraphTargetMachine.h"
#include "MCTargetDesc/GraphInfo.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/ISDOpcodes.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include <algorithm>

#define DEBUG_TYPE "Graph-lower"

using namespace llvm;

static const MCPhysReg ArgGPRs[] = {Graph::R9, Graph::R10, Graph::R11,
                                    Graph::R12};

void GraphTargetLowering::ReplaceNodeResults(SDNode *N,
                                             SmallVectorImpl<SDValue> &Results,
                                             SelectionDAG &DAG) const {
  llvm_unreachable("");
}

GraphTargetLowering::GraphTargetLowering(const TargetMachine &TM,
                                         const GraphSubtarget &STI)
    : TargetLowering(TM), STI(STI) {
  GRAPH_DUMP_RED
  addRegisterClass(MVT::i32, &Graph::GPRRegClass);
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(Graph::R1);

  for (unsigned Opc = 0; Opc < ISD::BUILTIN_OP_END; ++Opc)
    setOperationAction(Opc, MVT::i32, Expand);

  setOperationAction(ISD::ADD, MVT::i32, Legal);
  setOperationAction(ISD::SUB, MVT::i32, Legal);
  setOperationAction(ISD::MUL, MVT::i32, Legal);
  setOperationAction(ISD::SDIV, MVT::i32, Legal);
  setOperationAction(ISD::UREM, MVT::i32, Legal);
  setOperationAction(ISD::OR, MVT::i32, Legal);
  setOperationAction(ISD::AND, MVT::i32, Legal);
  setOperationAction(ISD::XOR, MVT::i32, Legal);
  setOperationAction(ISD::ABS, MVT::i32, Legal);
  setOperationAction(ISD::SRA, MVT::i32, Legal);
  setOperationAction(ISD::SRL, MVT::i32, Legal);
  setOperationAction(ISD::SHL, MVT::i32, Legal);

  setOperationAction(ISD::LOAD, MVT::i32, Legal);
  setOperationAction(ISD::STORE, MVT::i32, Legal);

  setOperationAction(ISD::Constant, MVT::i32, Legal);
  setOperationAction(ISD::UNDEF, MVT::i32, Legal);

  setOperationAction(ISD::BR_CC, MVT::i32, Custom);

  setOperationAction(ISD::FRAMEADDR, MVT::i32, Legal);
}

const char *GraphTargetLowering::getTargetNodeName(unsigned Opcode) const {
  GRAPH_DUMP_RED
  switch (Opcode) {
  case GraphISD::CALL:
    return "GraphISD::CALL";
  case GraphISD::RET:
    return "GraphISD::RET";
  case GraphISD::BR_CC:
    return "GraphISD::BR_CC";
  case GraphISD::INC_EQi:
    return "GraphISD::INC_EQi";
  case GraphISD::INC_NEi:
    return "GraphISD::INC_NEi";
  case GraphISD::INC_LEi:
    return "GraphISD::INC_LEi";
  case GraphISD::INC_LTi:
    return "GraphISD::INC_LTi";
  case GraphISD::INC_GEi:
    return "GraphISD::INC_GEi";
  case GraphISD::INC_GTi:
    return "GraphISD::INC_GTi";
  }
  return nullptr;
}

//===----------------------------------------------------------------------===//
// Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

#include "GraphGenCallingConv.inc"

//===----------------------------------------------------------------------===//
// Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

static Align getPrefTypeAlign(EVT VT, SelectionDAG &DAG) {
  return DAG.getDataLayout().getPrefTypeAlign(
      VT.getTypeForEVT(*DAG.getContext()));
}

SDValue GraphTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                                       SmallVectorImpl<SDValue> &InVals) const {
  GRAPH_DUMP_RED
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  assert(!CLI.IsTailCall);
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;
  EVT PtrVT = getPointerTy(DAG.getDataLayout());

  MachineFunction &MF = DAG.getMachineFunction();

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_Graph);

  unsigned NumBytes = CCInfo.getStackSize();

  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[i];
    unsigned Size = Flags.getByValSize();
    Align Alignment = Flags.getNonZeroByValAlign();

    int FI =
        MF.getFrameInfo().CreateStackObject(Size, Alignment, /*isSS=*/false);
    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue SizeNode = DAG.getConstant(Size, DL, MVT::i32);

    Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, Alignment,
                          /*IsVolatile=*/false,
                          /*AlwaysInline=*/false,
                          /*CI=*/nullptr, std::nullopt, MachinePointerInfo(),
                          MachinePointerInfo());
    ByValArgs.push_back(FIPtr);
  }

  Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, CLI.DL);

  SmallVector<std::pair<Register, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;
  for (unsigned i = 0, j = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue = OutVals[i];
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (VA.getLocInfo() == CCValAssign::Indirect) {
      Align StackAlign =
          std::max(getPrefTypeAlign(Outs[i].ArgVT, DAG),
                   getPrefTypeAlign(ArgValue.getValueType(), DAG));
      TypeSize StoredSize = ArgValue.getValueType().getStoreSize();
      unsigned ArgIndex = Outs[i].OrigArgIndex;
      unsigned ArgPartOffset = Outs[i].PartOffset;
      assert(ArgPartOffset == 0);
      SmallVector<std::pair<SDValue, SDValue>> Parts;
      while (i + 1 != e && Outs[i + 1].OrigArgIndex == ArgIndex) {
        SDValue PartValue = OutVals[i + 1];
        unsigned PartOffset = Outs[i + 1].PartOffset - ArgPartOffset;
        SDValue Offset = DAG.getIntPtrConstant(PartOffset, DL);
        EVT PartVT = PartValue.getValueType();
        StoredSize += PartVT.getStoreSize();
        StackAlign = std::max(StackAlign, getPrefTypeAlign(PartVT, DAG));
        Parts.push_back(std::make_pair(PartValue, Offset));
        ++i;
      }
      SDValue SpillSlot = DAG.CreateStackTemporary(StoredSize, StackAlign);
      int FI = cast<FrameIndexSDNode>(SpillSlot)->getIndex();
      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, SpillSlot,
                       MachinePointerInfo::getFixedStack(MF, FI)));
      for (const auto &Part : Parts) {
        SDValue PartValue = Part.first;
        SDValue PartOffset = Part.second;
        SDValue Address =
            DAG.getNode(ISD::ADD, DL, PtrVT, SpillSlot, PartOffset);
        MemOpChains.push_back(
            DAG.getStore(Chain, DL, PartValue, Address,
                         MachinePointerInfo::getFixedStack(MF, FI)));
      }
      ArgValue = SpillSlot;
    } else {
      assert(VA.getLocInfo() == CCValAssign::Full);
    }

    if (Flags.isByVal())
      ArgValue = ByValArgs[j++];

    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");

      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, DL, Graph::R1, PtrVT);
      SDValue Address =
          DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr,
                      DAG.getIntPtrConstant(VA.getLocMemOffset(), DL));

      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, Address, MachinePointerInfo()));
    }
  }

  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  SDValue Glue;

  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, Glue);
    Glue = Chain.getValue(1);
  }

  if (GlobalAddressSDNode *S = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = S->getGlobal();
    assert(getTargetMachine().shouldAssumeDSOLocal(GV));
    Callee = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0, 0);
  }

  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  for (auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));

  const TargetRegisterInfo *TRI = STI.getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConv);
  Ops.push_back(DAG.getRegisterMask(Mask));

  if (Glue.getNode())
    Ops.push_back(Glue);

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  Chain = DAG.getNode(GraphISD::CALL, DL, NodeTys, Ops);
  DAG.addNoMergeSiteInfo(Chain.getNode(), CLI.NoMerge);
  Glue = Chain.getValue(1);

  Chain = DAG.getCALLSEQ_END(Chain, DAG.getConstant(NumBytes, DL, PtrVT, true),
                             DAG.getConstant(0, DL, PtrVT, true), Glue, DL);
  Glue = Chain.getValue(1);

  SmallVector<CCValAssign, 16> RVLocs;
  CCState RetCCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
  RetCCInfo.AnalyzeCallResult(Ins, RetCC_Graph);

  for (auto &VA : RVLocs) {
    SDValue RetValue =
        DAG.getCopyFromReg(Chain, DL, VA.getLocReg(), VA.getLocVT(), Glue);
    Chain = RetValue.getValue(1);
    Glue = RetValue.getValue(2);

    assert(VA.getLocInfo() == CCValAssign::Full);
    InVals.push_back(RetValue);
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
// Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

namespace {

struct ArgDataPair {
  SDValue SDV;
  ISD::ArgFlagsTy Flags;
};

} // end anonymous namespace

static SDValue convertValVTToLocVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL,
                                   const GraphSubtarget &Subtarget) {
  EVT LocVT = VA.getLocVT();

  if (VA.getValVT() == MVT::f32)
    llvm_unreachable("");

  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
    break;
  case CCValAssign::BCvt:
    llvm_unreachable("");
    Val = DAG.getNode(ISD::BITCAST, DL, LocVT, Val);
    break;
  }
  return Val;
}

static SDValue convertLocVTToValVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL,
                                   const GraphSubtarget &Subtarget) {
  if (VA.getValVT() == MVT::f32)
    llvm_unreachable("");

  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
    break;
  case CCValAssign::BCvt:
    llvm_unreachable("");
    Val = DAG.getNode(ISD::BITCAST, DL, VA.getValVT(), Val);
  }
  return Val;
}

static SDValue unpackFromRegLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL,
                                const GraphTargetLowering &TLI) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  EVT LocVT = VA.getLocVT();
  SDValue Val;
  const TargetRegisterClass *RC = TLI.getRegClassFor(LocVT.getSimpleVT());
  Register VReg = RegInfo.createVirtualRegister(RC);
  RegInfo.addLiveIn(VA.getLocReg(), VReg);
  Val = DAG.getCopyFromReg(Chain, DL, VReg, LocVT);

  if (VA.getLocInfo() == CCValAssign::Indirect)
    return Val;

  return convertLocVTToValVT(DAG, Val, VA, DL, TLI.getSubtarget());
}

static SDValue unpackFromMemLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  EVT LocVT = VA.getLocVT();
  EVT ValVT = VA.getValVT();
  EVT PtrVT = MVT::getIntegerVT(DAG.getDataLayout().getPointerSizeInBits(0));
  int FI = MFI.CreateFixedObject(ValVT.getStoreSize(), VA.getLocMemOffset(),
                                 /*IsImmutable=*/true);
  SDValue FIN = DAG.getFrameIndex(FI, PtrVT);
  SDValue Val;

  ISD::LoadExtType ExtType;
  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
  case CCValAssign::Indirect:
  case CCValAssign::BCvt:
    ExtType = ISD::NON_EXTLOAD;
    break;
  }
  Val = DAG.getExtLoad(
      ExtType, DL, LocVT, Chain, FIN,
      MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI), ValVT);
  return Val;
}

SDValue GraphTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  GRAPH_DUMP_RED
  switch (CallConv) {
  default:
    report_fatal_error("Unsupported calling convention");
  case CallingConv::C:
  case CallingConv::Fast:
    break;
  }

  MachineFunction &MF = DAG.getMachineFunction();
  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  unsigned StackSlotSize = MVT(MVT::i32).getSizeInBits() / 8;
  std::vector<SDValue> OutChains;

  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_Graph);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue;
    if (VA.isRegLoc())
      ArgValue = unpackFromRegLoc(DAG, Chain, VA, DL, *this);
    else
      ArgValue = unpackFromMemLoc(DAG, Chain, VA, DL);

    if (VA.getLocInfo() == CCValAssign::Indirect) {
      InVals.push_back(DAG.getLoad(VA.getValVT(), DL, Chain, ArgValue,
                                   MachinePointerInfo()));
      unsigned ArgIndex = Ins[i].OrigArgIndex;
      unsigned ArgPartOffset = Ins[i].PartOffset;
      assert(ArgPartOffset == 0);
      while (i + 1 != e && Ins[i + 1].OrigArgIndex == ArgIndex) {
        CCValAssign &PartVA = ArgLocs[i + 1];
        unsigned PartOffset = Ins[i + 1].PartOffset - ArgPartOffset;
        SDValue Offset = DAG.getIntPtrConstant(PartOffset, DL);
        SDValue Address = DAG.getNode(ISD::ADD, DL, PtrVT, ArgValue, Offset);
        InVals.push_back(DAG.getLoad(PartVA.getValVT(), DL, Chain, Address,
                                     MachinePointerInfo()));
        ++i;
      }
      continue;
    }
    InVals.push_back(ArgValue);
  }

  if (IsVarArg) {
    ArrayRef<MCPhysReg> ArgRegs = ArrayRef(ArgGPRs);
    unsigned Idx = CCInfo.getFirstUnallocated(ArgRegs);
    const TargetRegisterClass *RC = &Graph::GPRRegClass;
    MachineFrameInfo &MFI = MF.getFrameInfo();
    MachineRegisterInfo &RegInfo = MF.getRegInfo();
    GraphFunctionInfo *UFI = MF.getInfo<GraphFunctionInfo>();

    int VaArgOffset, VarArgsSaveSize;

    if (ArgRegs.size() == Idx) {
      VaArgOffset = CCInfo.getStackSize();
      VarArgsSaveSize = 0;
    } else {
      VarArgsSaveSize = StackSlotSize * (ArgRegs.size() - Idx);
      VaArgOffset = -VarArgsSaveSize;
    }

    int FI = MFI.CreateFixedObject(StackSlotSize, VaArgOffset, true);
    UFI->setVarArgsFrameIndex(FI);

    if (Idx % 2) {
      MFI.CreateFixedObject(StackSlotSize, VaArgOffset - (int)StackSlotSize,
                            true);
      VarArgsSaveSize += StackSlotSize;
    }

    for (unsigned I = Idx; I < ArgRegs.size();
         ++I, VaArgOffset += StackSlotSize) {
      const Register Reg = RegInfo.createVirtualRegister(RC);
      RegInfo.addLiveIn(ArgRegs[I], Reg);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, MVT::i32);
      FI = MFI.CreateFixedObject(StackSlotSize, VaArgOffset, true);
      SDValue PtrOff = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      SDValue Store = DAG.getStore(Chain, DL, ArgValue, PtrOff,
                                   MachinePointerInfo::getFixedStack(MF, FI));
      cast<StoreSDNode>(Store.getNode())
          ->getMemOperand()
          ->setValue((Value *)nullptr);
      OutChains.push_back(Store);
    }
    UFI->setVarArgsSaveSize(VarArgsSaveSize);
  }

  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
  }

  return Chain;
}

//===----------------------------------------------------------------------===//
// Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool GraphTargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context,
    const Type *RetTy) const {
  GRAPH_DUMP_RED
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, Context);
  if (!CCInfo.CheckReturn(Outs, RetCC_Graph))
    return false;
  if (CCInfo.getStackSize() != 0 && IsVarArg)
    llvm_unreachable("");
  return true;
}

SDValue
GraphTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                 bool IsVarArg,
                                 const SmallVectorImpl<ISD::OutputArg> &Outs,
                                 const SmallVectorImpl<SDValue> &OutVals,
                                 const SDLoc &DL, SelectionDAG &DAG) const {
  GRAPH_DUMP_RED
  const MachineFunction &MF = DAG.getMachineFunction();
  const GraphSubtarget &STI = MF.getSubtarget<GraphSubtarget>();

  SmallVector<CCValAssign, 16> RVLocs;

  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeReturn(Outs, RetCC_Graph);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    Val = convertValVTToLocVT(DAG, Val, VA, DL, STI);
    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Glue);

    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }
  return DAG.getNode(GraphISD::RET, DL, MVT::Other, RetOps);
}

//===----------------------------------------------------------------------===//
// Target Optimization Hooks
//===----------------------------------------------------------------------===//

SDValue GraphTargetLowering::PerformDAGCombine(SDNode *N,
                                               DAGCombinerInfo &DCI) const {
  return {};
}

//===----------------------------------------------------------------------===//
// Addressing mode description hooks
//===----------------------------------------------------------------------===//

bool GraphTargetLowering::isLegalAddressingMode(const DataLayout &DL,
                                                const AddrMode &AM, Type *Ty,
                                                unsigned AS,
                                                Instruction *I) const {
  GRAPH_DUMP_RED
  if (AM.BaseGV)
    return false;

  if (!isInt<16>(AM.BaseOffs))
    return false;

  switch (AM.Scale) {
  case 0:
    break;
  case 1:
    if (!AM.HasBaseReg)
      break;
    return false;
  default:
    return false;
  }

  return true;
}

unsigned GraphTargetLowering::getIsdOpIncCmp(ISD::CondCode CCVal) const {
  switch (CCVal) {
  default:
    llvm_unreachable("CCVal for INC not implemented");
  case ISD::CondCode::SETEQ:
    return GraphISD::INC_EQi;
  case ISD::CondCode::SETNE:
    return GraphISD::INC_NEi;
  case ISD::CondCode::SETLE:
    return GraphISD::INC_LEi;
  case ISD::CondCode::SETLT:
    return GraphISD::INC_LTi;
  case ISD::CondCode::SETGE:
    return GraphISD::INC_GEi;
  case ISD::CondCode::SETGT:
    return GraphISD::INC_GTi;
  }
}

SDValue GraphTargetLowering::lowerBR_CC(SDValue Op, SelectionDAG &DAG) const {
  SDValue CC = Op.getOperand(1);
  SDValue ADD = Op.getOperand(2);
  ISD::CondCode CCVal = cast<CondCodeSDNode>(CC)->get();
  if (ADD->getOpcode() == ISD::ADD) {
    SDValue INC = ADD->getOperand(1);
    if (INC->getOpcode() == ISD::Constant &&
        cast<ConstantSDNode>(INC)->getZExtValue() == 1) {
      SDValue CMP = Op.getOperand(3);
      SDValue INCCMP = DAG.getNode(getIsdOpIncCmp(CCVal), ADD,
                                   DAG.getVTList({MVT::i32, MVT::i32}),
                                   ADD->getOperand(0), CMP);
      DAG.ReplaceAllUsesWith(ADD, INCCMP.getValue(1));
      DAG.RemoveDeadNode(ADD.getNode());
      SDValue Block = Op->getOperand(4);
      return DAG.getNode(GraphISD::BR_CC, Op, Op.getValueType(),
                         Op.getOperand(0), INCCMP.getValue(0), Block);
    }
  }

  SDValue RHS = Op.getOperand(3);
  if (RHS->getOpcode() == ISD::Constant &&
      isInt<16>(cast<ConstantSDNode>(RHS)->getSExtValue())) {
    SDValue INCCMP = DAG.getNode(getIsdOpIncCmp(CCVal), Op,
                                 DAG.getVTList({MVT::i32, MVT::i32}), ADD, RHS);
    SDValue Block = Op->getOperand(4);
    return DAG.getNode(GraphISD::BR_CC, Op, Op.getValueType(), Op.getOperand(0),
                       INCCMP.getValue(0), Block);
  }
  return Op;
}

SDValue GraphTargetLowering::LowerOperation(SDValue Op,
                                            SelectionDAG &DAG) const {
  switch (Op->getOpcode()) {
  case ISD::BR_CC:
    return lowerBR_CC(Op, DAG);
  default:
    llvm_unreachable("");
  }
}

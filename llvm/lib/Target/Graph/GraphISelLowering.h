#ifndef LLVM_LIB_TARGET_GRAPH_GRAPHISELLOWERING_H
#define LLVM_LIB_TARGET_GRAPH_GRAPHISELLOWERING_H

#include "Graph.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class GraphSubtarget;
class GraphTargetMachine;

namespace GraphISD {

enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET,
  CALL,
  BR_CC,
  INC_EQi,
  INC_NEi,
  INC_LEi,
  INC_LTi,
  INC_GEi,
  INC_GTi,
};

} // namespace GraphISD

class GraphTargetLowering : public TargetLowering {
public:
  explicit GraphTargetLowering(const TargetMachine &TM,
                                const GraphSubtarget &STI);

  const char *getTargetNodeName(unsigned Opcode) const override;

  bool isLegalAddressingMode(const DataLayout &DL, const AddrMode &AM,
                              Type *Ty, unsigned AS,
                              Instruction *I = nullptr) const override;

  GraphSubtarget const &getSubtarget() const { return STI; }

private:
  const GraphSubtarget &STI;

  void ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results,
                          SelectionDAG &DAG) const override;

  SDValue PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI) const override;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &ArgsFlags,
                      LLVMContext &Context, const Type *RetTy) const override;

  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  SDValue lowerBR_CC(SDValue Op, SelectionDAG &DAG) const;
  SDValue lowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;
  unsigned getIsdOpIncCmp(ISD::CondCode CCVal) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_GRAPH_GRAPHISELLOWERING_H

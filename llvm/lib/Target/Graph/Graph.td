include "llvm/Target/Target.td"
include "GraphRegisterInfo.td"
include "GraphInstrInfo.td"
include "GraphCallingConv.td"

class Proc<string Name, list<SubtargetFeature> Features>
  : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

def GraphInstrInfo : InstrInfo;
def Graph : Target {
  let InstructionSet = GraphInstrInfo;
}

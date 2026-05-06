//===--- Graph.cpp - Implement Graph target feature support ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Graph.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

using namespace clang;
using namespace clang::targets;

static constexpr Builtin::Info BuiltinInfo[] = {
#define BUILTIN(ID, TYPE, ATTRS) {#ID, TYPE, ATTRS, nullptr, HeaderDesc::NO_HEADER, ALL_LANGUAGES},
#include "clang/Basic/BuiltinsGraph.inc"
};

void GraphTargetInfo::getTargetDefines(const LangOptions &Opts,
                                       MacroBuilder &Builder) const {
  Builder.defineMacro("__graph__");
  Builder.defineMacro("__GRAPH__");
}

ArrayRef<Builtin::Info> GraphTargetInfo::getTargetBuiltins() const {
  return llvm::ArrayRef(BuiltinInfo,
                        clang::Graph::LastTSBuiltin - Builtin::FirstTSBuiltin);
}

ArrayRef<const char *> GraphTargetInfo::getGCCRegNames() const {
  static const char *const GCCRegNames[] = {
      "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
      "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
  return llvm::ArrayRef(GCCRegNames);
}

bool GraphTargetInfo::validateAsmConstraint(
    const char *&Name, TargetInfo::ConstraintInfo &Info) const {
  switch (*Name) {
  case 'r':
    Info.setAllowsRegister();
    return true;
  default:
    return false;
  }
}

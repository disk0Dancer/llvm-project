//===--- BuiltinsGraph.td - Graph builtin function database -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "clang/Basic/BuiltinsBase.td"

class GraphBuiltin<string Name, string Prototype> : TargetBuiltin {
  let Spellings = [Name];
  let Prototype = Prototype;
}

def GraphInit : GraphBuiltin<"__builtin_graph_init", "void(int, int)">;
def GraphSetColor : GraphBuiltin<"__builtin_graph_set_color", "void(int)">;
def GraphDrawPixel : GraphBuiltin<"__builtin_graph_draw_pixel", "void(int, int)">;
def GraphFlush : GraphBuiltin<"__builtin_graph_flush", "void()">;

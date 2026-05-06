//===- IntrinsicsGraph.td - Defines Graph intrinsics -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "graph" in {
def int_graph_init : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem, IntrHasSideEffects]>;
def int_graph_set_color : DefaultAttrsIntrinsic<[], [llvm_i32_ty], [IntrNoMem, IntrHasSideEffects]>;
def int_graph_draw_pixel : DefaultAttrsIntrinsic<[], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem, IntrHasSideEffects]>;
def int_graph_flush : DefaultAttrsIntrinsic<[], [], [IntrNoMem, IntrHasSideEffects]>;
}

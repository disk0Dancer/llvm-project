; RUN: llc -mtriple=graph < %s | FileCheck %s

define void @graphics() {
entry:
  call void @llvm.graph.init(i32 640, i32 480)
  call void @llvm.graph.set.color(i32 16711680)
  call void @llvm.graph.draw.pixel(i32 10, i32 20)
  call void @llvm.graph.flush()
  ret void
}

; CHECK-LABEL: graphics:
; CHECK: GFX_INIT
; CHECK: GFX_SET_COLOR
; CHECK: GFX_DRAW_PIXEL
; CHECK: GFX_FLUSH

declare void @llvm.graph.init(i32, i32)
declare void @llvm.graph.set.color(i32)
declare void @llvm.graph.draw.pixel(i32, i32)
declare void @llvm.graph.flush()

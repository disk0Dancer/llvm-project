; RUN: llc -mtriple=graph %s -o - | FileCheck %s
;
; A compact Game of Life-style graphics workload.  This intentionally uses
; persistent board buffers plus nested draw/update control flow so HW5 verifies
; the Graph backend instead of simplifying the app down to stateless pixels.

; CHECK-LABEL: main:
; CHECK: GFX_INIT
; CHECK: ST {{.*}} cells
; CHECK: ORi {{.*}} cells
; CHECK: LD {{.*}} 0
; CHECK: GFX_SET_COLOR
; CHECK: GFX_DRAW_PIXEL
; CHECK: BR_COND
; CHECK: ST {{.*}} next+4
; CHECK: GFX_FLUSH

target triple = "graph"

@cells = internal global [4 x i32] zeroinitializer
@next = internal global [4 x i32] zeroinitializer

declare void @llvm.graph.init(i32, i32)
declare void @llvm.graph.set.color(i32)
declare void @llvm.graph.draw.pixel(i32, i32)
declare void @llvm.graph.flush()

define i32 @main() {
entry:
  call void @llvm.graph.init(i32 2, i32 2)
  store i32 1, ptr @cells
  br label %draw

draw:
  %i = phi i32 [ 0, %entry ], [ %i.next, %draw.inc ]
  %cell.ptr = getelementptr [4 x i32], ptr @cells, i32 0, i32 %i
  %alive = load i32, ptr %cell.ptr
  %dead = icmp eq i32 %alive, 0
  br i1 %dead, label %black, label %green

black:
  call void @llvm.graph.set.color(i32 0)
  br label %plot

green:
  call void @llvm.graph.set.color(i32 65280)
  br label %plot

plot:
  %x = urem i32 %i, 2
  %y = sdiv i32 %i, 2
  call void @llvm.graph.draw.pixel(i32 %x, i32 %y)
  br label %draw.inc

draw.inc:
  %i.next = add i32 %i, 1
  %keep.drawing = icmp slt i32 %i.next, 4
  br i1 %keep.drawing, label %draw, label %step

step:
  %old = load i32, ptr @cells
  %next.ptr = getelementptr [4 x i32], ptr @next, i32 0, i32 1
  store i32 %old, ptr %next.ptr
  call void @llvm.graph.flush()
  ret i32 0
}

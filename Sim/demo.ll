; Standalone graphics demo for the Graph simulator.
; Avoids loop patterns that currently trigger backend regalloc issues:
; draws a small picture purely with straight-line code.
target triple = "graph"

declare void @llvm.graph.init(i32, i32)
declare void @llvm.graph.set.color(i32)
declare void @llvm.graph.draw.pixel(i32, i32)
declare void @llvm.graph.flush()

define i32 @main() {
entry:
  call void @llvm.graph.init(i32 16, i32 16)

  ; Red border-ish: a few pixels along the top row.
  call void @llvm.graph.set.color(i32 16711680)        ; 0xFF0000
  call void @llvm.graph.draw.pixel(i32 0,  i32 0)
  call void @llvm.graph.draw.pixel(i32 1,  i32 0)
  call void @llvm.graph.draw.pixel(i32 2,  i32 0)
  call void @llvm.graph.draw.pixel(i32 3,  i32 0)
  call void @llvm.graph.draw.pixel(i32 15, i32 0)

  ; Green diagonal: scatter dots straight-line.
  call void @llvm.graph.set.color(i32 65280)           ; 0x00FF00
  call void @llvm.graph.draw.pixel(i32 1,  i32 1)
  call void @llvm.graph.draw.pixel(i32 3,  i32 3)
  call void @llvm.graph.draw.pixel(i32 5,  i32 5)
  call void @llvm.graph.draw.pixel(i32 7,  i32 7)
  call void @llvm.graph.draw.pixel(i32 9,  i32 9)
  call void @llvm.graph.draw.pixel(i32 11, i32 11)
  call void @llvm.graph.draw.pixel(i32 13, i32 13)

  ; Blue dot.
  call void @llvm.graph.set.color(i32 255)             ; 0x0000FF
  call void @llvm.graph.draw.pixel(i32 8, i32 4)

  call void @llvm.graph.flush()
  ret i32 0
}

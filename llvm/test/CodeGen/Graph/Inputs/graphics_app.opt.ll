; Optimized LLVM IR for graphics_app.c, generated with:
; clang -target graph -O2 -S -emit-llvm graphics_app.c -o graphics_app.opt.ll

target triple = "graph"

declare void @llvm.graph.init(i32, i32)
declare void @llvm.graph.set.color(i32)
declare void @llvm.graph.draw.pixel(i32, i32)
declare void @llvm.graph.flush()

define i32 @main() {
entry:
  call void @llvm.graph.init(i32 64, i32 64)
  call void @llvm.graph.set.color(i32 16711680)
  br label %red_line

red_line:
  %x = phi i32 [ 4, %entry ], [ %x.next, %red_line ]
  call void @llvm.graph.draw.pixel(i32 %x, i32 8)
  %x.next = add nuw nsw i32 %x, 1
  %x.keep = icmp slt i32 %x.next, 20
  br i1 %x.keep, label %red_line, label %diag_entry

diag_entry:
  call void @llvm.graph.set.color(i32 65280)
  br label %diag

diag:
  %i = phi i32 [ 0, %diag_entry ], [ %i.next, %diag ]
  call void @llvm.graph.draw.pixel(i32 %i, i32 %i)
  %i.next = add nuw nsw i32 %i, 1
  %i.keep = icmp slt i32 %i.next, 16
  br i1 %i.keep, label %diag, label %exit

exit:
  call void @llvm.graph.flush()
  ret i32 0
}

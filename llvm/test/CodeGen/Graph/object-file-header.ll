; RUN: llc -mtriple=graph-unknown-unknown -filetype=obj %s -o %t.o
; RUN: llvm-readobj --file-headers %t.o | FileCheck %s

define i32 @f() {
entry:
  ret i32 0
}

; CHECK: Format: elf32-graph
; CHECK: Arch: graph
; CHECK: Machine: EM_GRAPH

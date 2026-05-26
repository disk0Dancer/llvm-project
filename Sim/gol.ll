; Game of Life on a 16x16 toroidal grid for the Graph backend.
;
; Two persistent grids @cells / @next as flat [256 x i32].
; Each generation:
;   * draw loop: paint all 256 cells (green = alive, black = dead)
;   * flush (presents one frame in the simulator window)
;   * compute loop: count 8 neighbours per cell, write next state
;   * swap loop: cells <- next
; Repeats for @GENS generations.

target triple = "graph"

@cells = internal global [256 x i32] zeroinitializer
@next  = internal global [256 x i32] zeroinitializer

declare void @llvm.graph.init(i32, i32)
declare void @llvm.graph.set.color(i32)
declare void @llvm.graph.draw.pixel(i32, i32)
declare void @llvm.graph.flush()

; index helper: idx = (y mod 16) * 16 + (x mod 16), all wrap-around.
define internal i32 @cell_index(i32 %x, i32 %y) {
  %xw = urem i32 %x, 16
  %yw = urem i32 %y, 16
  %row = mul i32 %yw, 16
  %idx = add i32 %row, %xw
  ret i32 %idx
}

define internal void @set_cell(i32 %x, i32 %y) {
  %i = call i32 @cell_index(i32 %x, i32 %y)
  %p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %i
  store i32 1, ptr %p
  ret void
}

define i32 @main() {
entry:
  call void @llvm.graph.init(i32 16, i32 16)

  ; Seed a classic "glider" at the top-left.
  call void @set_cell(i32 1, i32 0)
  call void @set_cell(i32 2, i32 1)
  call void @set_cell(i32 0, i32 2)
  call void @set_cell(i32 1, i32 2)
  call void @set_cell(i32 2, i32 2)

  ; Also seed a small blinker further away.
  call void @set_cell(i32 8, i32 8)
  call void @set_cell(i32 9, i32 8)
  call void @set_cell(i32 10, i32 8)

  br label %gen_loop

gen_loop:
  %gen = phi i32 [ 0, %entry ], [ %gen.next, %gen_tail ]

  ; ---- DRAW LOOP --------------------------------------------------------
  br label %draw

draw:
  %di = phi i32 [ 0, %gen_loop ], [ %di.next, %draw_inc ]
  %dy = udiv i32 %di, 16
  %dx = urem i32 %di, 16
  %dp = getelementptr [256 x i32], ptr @cells, i32 0, i32 %di
  %dv = load i32, ptr %dp
  %dead = icmp eq i32 %dv, 0
  br i1 %dead, label %draw_dead, label %draw_alive

draw_dead:
  call void @llvm.graph.set.color(i32 0)
  call void @llvm.graph.draw.pixel(i32 %dx, i32 %dy)
  br label %draw_inc

draw_alive:
  call void @llvm.graph.set.color(i32 65280)
  call void @llvm.graph.draw.pixel(i32 %dx, i32 %dy)
  br label %draw_inc

draw_inc:
  %di.next = add i32 %di, 1
  %dcond = icmp slt i32 %di.next, 256
  br i1 %dcond, label %draw, label %present

present:
  call void @llvm.graph.flush()

  ; ---- COMPUTE NEXT STATE ----------------------------------------------
  br label %compute

compute:
  %ci = phi i32 [ 0, %present ], [ %ci.next, %compute_inc ]
  %cy = udiv i32 %ci, 16
  %cx = urem i32 %ci, 16

  ; Sum 8 neighbours; each load is gated by computing the wrapped index.
  ; We inline cell_index manually to keep the IR call-light.
  %cym1 = sub i32 %cy, 1
  %cyp1 = add i32 %cy, 1
  %cxm1 = sub i32 %cx, 1
  %cxp1 = add i32 %cx, 1

  %n0i = call i32 @cell_index(i32 %cxm1, i32 %cym1)
  %n0p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n0i
  %n0  = load i32, ptr %n0p
  %n1i = call i32 @cell_index(i32 %cx,   i32 %cym1)
  %n1p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n1i
  %n1  = load i32, ptr %n1p
  %n2i = call i32 @cell_index(i32 %cxp1, i32 %cym1)
  %n2p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n2i
  %n2  = load i32, ptr %n2p
  %n3i = call i32 @cell_index(i32 %cxm1, i32 %cy)
  %n3p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n3i
  %n3  = load i32, ptr %n3p
  %n4i = call i32 @cell_index(i32 %cxp1, i32 %cy)
  %n4p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n4i
  %n4  = load i32, ptr %n4p
  %n5i = call i32 @cell_index(i32 %cxm1, i32 %cyp1)
  %n5p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n5i
  %n5  = load i32, ptr %n5p
  %n6i = call i32 @cell_index(i32 %cx,   i32 %cyp1)
  %n6p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n6i
  %n6  = load i32, ptr %n6p
  %n7i = call i32 @cell_index(i32 %cxp1, i32 %cyp1)
  %n7p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %n7i
  %n7  = load i32, ptr %n7p

  %s01 = add i32 %n0, %n1
  %s23 = add i32 %n2, %n3
  %s45 = add i32 %n4, %n5
  %s67 = add i32 %n6, %n7
  %s0123 = add i32 %s01, %s23
  %s4567 = add i32 %s45, %s67
  %sum   = add i32 %s0123, %s4567

  %cp = getelementptr [256 x i32], ptr @cells, i32 0, i32 %ci
  %cv = load i32, ptr %cp
  %np = getelementptr [256 x i32], ptr @next, i32 0, i32 %ci
  ; Rule, expressed only with branches (the Graph backend has no SELECT_CC):
  ;   sum == 3                          → alive
  ;   sum == 2 and current cell alive   → alive
  ;   else                              → dead
  %is3 = icmp eq i32 %sum, 3
  br i1 %is3, label %set_alive, label %check2

check2:
  %is2 = icmp eq i32 %sum, 2
  br i1 %is2, label %check_alive, label %set_dead

check_alive:
  %alive_nz = icmp ne i32 %cv, 0
  br i1 %alive_nz, label %set_alive, label %set_dead

set_alive:
  store i32 1, ptr %np
  br label %compute_inc

set_dead:
  store i32 0, ptr %np
  br label %compute_inc

compute_inc:
  %ci.next = add i32 %ci, 1
  %ccond = icmp slt i32 %ci.next, 256
  br i1 %ccond, label %compute, label %swap

swap:
  br label %swap_loop

swap_loop:
  %si = phi i32 [ 0, %swap ], [ %si.next, %swap_inc ]
  %nsp = getelementptr [256 x i32], ptr @next,  i32 0, i32 %si
  %csp = getelementptr [256 x i32], ptr @cells, i32 0, i32 %si
  %sv  = load i32, ptr %nsp
  store i32 %sv, ptr %csp
  br label %swap_inc

swap_inc:
  %si.next = add i32 %si, 1
  %scond = icmp slt i32 %si.next, 256
  br i1 %scond, label %swap_loop, label %gen_tail

gen_tail:
  %gen.next = add i32 %gen, 1
  %gcond = icmp slt i32 %gen.next, 10000
  br i1 %gcond, label %gen_loop, label %done

done:
  ret i32 0
}

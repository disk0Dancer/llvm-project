; ModuleID = '/Users/g.churakov/MySoftware/github.com/disk0dancer/edu/llvm-project/Sim/gol.ll'
source_filename = "/Users/g.churakov/MySoftware/github.com/disk0dancer/edu/llvm-project/Sim/gol.ll"
target datalayout = "e-m:e-p:32:32-i8:8:32-i16:16:32-i64:64-n32"
target triple = "graph"

@cells = internal unnamed_addr global [256 x i32] zeroinitializer
@next = internal unnamed_addr global [256 x i32] zeroinitializer

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.graph.init(i32, i32) #0

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.graph.set.color(i32) #0

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.graph.draw.pixel(i32, i32) #0

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.graph.flush() #0

; Function Attrs: nofree norecurse nosync nounwind
define noundef i32 @main() local_unnamed_addr #1 {
entry:
  tail call void @llvm.graph.init(i32 16, i32 16)
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 4), align 4
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 72), align 8
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 128), align 16
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 132), align 4
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 136), align 8
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 544), align 16
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 548), align 4
  store i32 1, ptr getelementptr inbounds nuw (i8, ptr @cells, i32 552), align 8
  br label %gen_loop

gen_loop:                                         ; preds = %swap_loop.preheader, %entry
  %gen = phi i32 [ 0, %entry ], [ %gen.next, %swap_loop.preheader ]
  br label %draw

draw:                                             ; preds = %draw_inc, %gen_loop
  %di = phi i32 [ 0, %gen_loop ], [ %di.next, %draw_inc ]
  %dy1 = lshr i32 %di, 4
  %dx = and i32 %di, 15
  %dp = getelementptr [256 x i32], ptr @cells, i32 0, i32 %di
  %dv = load i32, ptr %dp, align 4
  %dead = icmp eq i32 %dv, 0
  br i1 %dead, label %draw_dead, label %draw_alive

draw_dead:                                        ; preds = %draw
  tail call void @llvm.graph.set.color(i32 0)
  br label %draw_inc

draw_alive:                                       ; preds = %draw
  tail call void @llvm.graph.set.color(i32 65280)
  br label %draw_inc

draw_inc:                                         ; preds = %draw_alive, %draw_dead
  tail call void @llvm.graph.draw.pixel(i32 %dx, i32 %dy1)
  %di.next = add nuw nsw i32 %di, 1
  %exitcond.not = icmp eq i32 %di.next, 256
  br i1 %exitcond.not, label %present, label %draw

present:                                          ; preds = %draw_inc
  tail call void @llvm.graph.flush()
  br label %compute

compute:                                          ; preds = %compute_inc, %present
  %ci = phi i32 [ 0, %present ], [ %ci.next, %compute_inc ]
  %cx = and i32 %ci, 15
  %cxm1 = add nuw nsw i32 %ci, 15
  %cxp1 = add nuw nsw i32 %ci, 1
  %xw.i = and i32 %cxm1, 15
  %yw.i = add nuw nsw i32 %ci, 240
  %row.i = and i32 %yw.i, 240
  %idx.i = or disjoint i32 %row.i, %xw.i
  %n0p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i
  %n0 = load i32, ptr %n0p, align 4
  %idx.i6 = or disjoint i32 %row.i, %cx
  %n1p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i6
  %n1 = load i32, ptr %n1p, align 4
  %xw.i7 = and i32 %cxp1, 15
  %idx.i10 = or disjoint i32 %row.i, %xw.i7
  %n2p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i10
  %n2 = load i32, ptr %n2p, align 4
  %row.i13 = and i32 %ci, 240
  %idx.i14 = or disjoint i32 %xw.i, %row.i13
  %n3p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i14
  %n3 = load i32, ptr %n3p, align 4
  %idx.i18 = or disjoint i32 %xw.i7, %row.i13
  %n4p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i18
  %n4 = load i32, ptr %n4p, align 4
  %yw.i20 = add nuw nsw i32 %ci, 16
  %row.i21 = and i32 %yw.i20, 240
  %idx.i22 = or disjoint i32 %row.i21, %xw.i
  %n5p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i22
  %n5 = load i32, ptr %n5p, align 4
  %idx.i26 = or disjoint i32 %row.i21, %cx
  %n6p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i26
  %n6 = load i32, ptr %n6p, align 4
  %idx.i30 = or disjoint i32 %row.i21, %xw.i7
  %n7p = getelementptr [256 x i32], ptr @cells, i32 0, i32 %idx.i30
  %n7 = load i32, ptr %n7p, align 4
  %s67 = add i32 %n1, %n0
  %s45 = add i32 %s67, %n2
  %s4567 = add i32 %s45, %n3
  %s23 = add i32 %s4567, %n4
  %s01 = add i32 %s23, %n5
  %s0123 = add i32 %s01, %n6
  %sum = add i32 %s0123, %n7
  %np = getelementptr [256 x i32], ptr @next, i32 0, i32 %ci
  switch i32 %sum, label %set_dead [
    i32 3, label %compute_inc
    i32 2, label %check_alive
  ]

check_alive:                                      ; preds = %compute
  %cp = getelementptr [256 x i32], ptr @cells, i32 0, i32 %ci
  %cv = load i32, ptr %cp, align 4
  %alive_nz.not = icmp eq i32 %cv, 0
  br i1 %alive_nz.not, label %set_dead, label %compute_inc

set_dead:                                         ; preds = %compute, %check_alive
  br label %compute_inc

compute_inc:                                      ; preds = %check_alive, %compute, %set_dead
  %storemerge = phi i32 [ 0, %set_dead ], [ 1, %compute ], [ 1, %check_alive ]
  store i32 %storemerge, ptr %np, align 4
  %ci.next = add nuw nsw i32 %ci, 1
  %exitcond31.not = icmp eq i32 %ci.next, 256
  br i1 %exitcond31.not, label %swap_loop.preheader, label %compute

swap_loop.preheader:                              ; preds = %compute_inc
  tail call void @llvm.memcpy.p0.p0.i32(ptr noundef nonnull align 16 dereferenceable(1024) @cells, ptr noundef nonnull align 16 dereferenceable(1024) @next, i32 1024, i1 false)
  %gen.next = add nuw nsw i32 %gen, 1
  %exitcond32.not = icmp eq i32 %gen.next, 80
  br i1 %exitcond32.not, label %done, label %gen_loop

done:                                             ; preds = %swap_loop.preheader
  ret i32 0
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i32(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i32, i1 immarg) #2

attributes #0 = { mustprogress nocallback nofree nosync nounwind willreturn }
attributes #1 = { nofree norecurse nosync nounwind }
attributes #2 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }

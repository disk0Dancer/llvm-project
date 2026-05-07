; RUN: llc -mtriple=graph %S/Inputs/graphics_app.opt.ll -o - | FileCheck %s

; CHECK-LABEL: main:
; CHECK: GFX_INIT
; CHECK: GFX_SET_COLOR
; CHECK: GFX_DRAW_PIXEL
; CHECK: BR_COND
; CHECK: GFX_SET_COLOR
; CHECK: GFX_DRAW_PIXEL
; CHECK: BR_COND
; CHECK: GFX_FLUSH

	.file	"graphics_app.opt.ll"
	.text
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:                                ; %entry
	MOVli r2 64
	GFX_INIT r2 r2
	MOVhi r2 255
	GFX_SET_COLOR r2
	MOVli r2 4
	MOVli r4 8
.LBB0_1:                                ; %red_line
                                        ; =>This Inner Loop Header: Depth=1
	GFX_DRAW_PIXEL r2 r4
	INC_LTi r9 r2 20
	BR_COND r9 .LBB0_1
	B .LBB0_2
.LBB0_2:                                ; %diag_entry
	MOVli r2 65280
	GFX_SET_COLOR r2
	MOVli r2 0
.LBB0_3:                                ; %diag
                                        ; =>This Inner Loop Header: Depth=1
	INC_LTi r9 r2 16
	GFX_DRAW_PIXEL r2 r2
	ORi r2 r4 0
	BR_COND r9 .LBB0_3
	B .LBB0_4
.LBB0_4:                                ; %exit
	GFX_FLUSH
	MOVli r9 0
	BR r0
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        ; -- End function
	.section	".note.GNU-stack","",@progbits

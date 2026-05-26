	.file	"gol.ll"
	.text
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:                                ; %entry
	ST r3 r1 20
	ST r5 r1 16
	ST r6 r1 12
	ST r7 r1 8
	ST r8 r1 4
	MOVli r2 16
	GFX_INIT r2 r2
	MOVli r2 1
	ST r2 r0 cells+552
	ST r2 r0 cells+548
	ST r2 r0 cells+544
	ST r2 r0 cells+136
	ST r2 r0 cells+132
	ST r2 r0 cells+128
	ST r2 r0 cells+72
	ST r2 r0 cells+4
	MOVli r2 0
	ORi r4 r0 cells
	ORi r9 r0 next
	ST r9 r1 0
	MOVhi r10 65535
	ORi r10 r10 65535
	MOVli r11 65280
	MOVli r12 0
.LBB0_1:                                ; %gen_loop
                                        ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_2 Depth 2
                                        ;     Child Loop BB0_7 Depth 2
	ORi r13 r4 0
	MOVli r14 0
.LBB0_2:                                ; %draw
                                        ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	LD r15 r13 0
	INC_NEi r3 r15 0
	BR_COND r3 .LBB0_4
	B .LBB0_3
.LBB0_4:                                ; %draw_alive
                                        ;   in Loop: Header=BB0_2 Depth=2
	GFX_SET_COLOR r11
.LBB0_5:                                ; %draw_inc
                                        ;   in Loop: Header=BB0_2 Depth=2
	ANDi r15 r14 15
	SRLi r3 r14 4
	GFX_DRAW_PIXEL r15 r3
	INC_NEi r15 r14 255
	ADDi r13 r13 4
	BR_COND r15 .LBB0_2
	B .LBB0_6
.LBB0_3:                                ; %draw_dead
                                        ;   in Loop: Header=BB0_2 Depth=2
	GFX_SET_COLOR r2
	B .LBB0_5
.LBB0_6:                                ; %present
                                        ;   in Loop: Header=BB0_1 Depth=1
	MOVli r13 0
	GFX_FLUSH
	LD r14 r1 0
	ORi r15 r4 0
.LBB0_7:                                ; %compute
                                        ;   Parent Loop BB0_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	ADDi r3 r13 240
	ANDi r3 r3 240
	ANDi r5 r13 15
	OR r6 r3 r5
	SHLi r6 r6 2
	ADD r6 r6 r4
	LD r6 r6 0
	ADD r7 r13 r10
	ANDi r7 r7 15
	OR r8 r3 r7
	SHLi r8 r8 2
	ADD r8 r8 r4
	LD r8 r8 0
	ADD r6 r6 r8
	ADDi r8 r13 1
	ANDi r8 r8 15
	OR r3 r3 r8
	SHLi r3 r3 2
	ADD r3 r3 r4
	LD r3 r3 0
	ADD r3 r6 r3
	ANDi r6 r13 240
	ADDi r9 r13 16
	ANDi r9 r9 240
	OR r5 r9 r5
	OR r2 r9 r8
	OR r8 r8 r6
	OR r9 r9 r7
	OR r6 r7 r6
	SHLi r6 r6 2
	ADD r6 r6 r4
	LD r6 r6 0
	ADD r3 r3 r6
	SHLi r6 r8 2
	ADD r6 r6 r4
	LD r6 r6 0
	ADD r3 r3 r6
	SHLi r5 r5 2
	SHLi r2 r2 2
	ADD r2 r2 r4
	ADD r5 r5 r4
	SHLi r9 r9 2
	ADD r9 r9 r4
	LD r9 r9 0
	ADD r9 r3 r9
	LD r3 r5 0
	ADD r9 r9 r3
	LD r2 r2 0
	ADD r5 r9 r2
	MOVli r3 1
	ORi r2 r5 0
	INC_EQi r9 r2 3
	BR_COND r9 .LBB0_11
	B .LBB0_8
.LBB0_8:                                ; %compute
                                        ;   in Loop: Header=BB0_7 Depth=2
	INC_NEi r2 r5 2
	BR_COND r2 .LBB0_10
	B .LBB0_9
.LBB0_9:                                ; %check_alive
                                        ;   in Loop: Header=BB0_7 Depth=2
	LD r2 r15 0
	INC_NEi r9 r2 0
	BR_COND r9 .LBB0_11
	B .LBB0_10
.LBB0_10:                               ; %set_dead
                                        ;   in Loop: Header=BB0_7 Depth=2
	MOVli r3 0
.LBB0_11:                               ; %compute_inc
                                        ;   in Loop: Header=BB0_7 Depth=2
	INC_NEi r2 r13 255
	ST r3 r14 0
	ADDi r14 r14 4
	ADDi r15 r15 4
	BR_COND r2 .LBB0_7
	B .LBB0_12
.LBB0_12:                               ; %swap_loop.preheader
                                        ;   in Loop: Header=BB0_1 Depth=1
	INC_NEi r13 r12 79
	LD r2 r0 next+1020
	ST r2 r0 cells+1020
	LD r2 r0 next+1016
	ST r2 r0 cells+1016
	LD r2 r0 next+1012
	ST r2 r0 cells+1012
	LD r2 r0 next+1008
	ST r2 r0 cells+1008
	LD r2 r0 next+1004
	ST r2 r0 cells+1004
	LD r2 r0 next+1000
	ST r2 r0 cells+1000
	LD r2 r0 next+996
	ST r2 r0 cells+996
	LD r2 r0 next+992
	ST r2 r0 cells+992
	LD r2 r0 next+988
	ST r2 r0 cells+988
	LD r2 r0 next+984
	ST r2 r0 cells+984
	LD r2 r0 next+980
	ST r2 r0 cells+980
	LD r2 r0 next+976
	ST r2 r0 cells+976
	LD r2 r0 next+972
	ST r2 r0 cells+972
	LD r2 r0 next+968
	ST r2 r0 cells+968
	LD r2 r0 next+964
	ST r2 r0 cells+964
	LD r2 r0 next+960
	ST r2 r0 cells+960
	LD r2 r0 next+956
	ST r2 r0 cells+956
	LD r2 r0 next+952
	ST r2 r0 cells+952
	LD r2 r0 next+948
	ST r2 r0 cells+948
	LD r2 r0 next+944
	ST r2 r0 cells+944
	LD r2 r0 next+940
	ST r2 r0 cells+940
	LD r2 r0 next+936
	ST r2 r0 cells+936
	LD r2 r0 next+932
	ST r2 r0 cells+932
	LD r2 r0 next+928
	ST r2 r0 cells+928
	LD r2 r0 next+924
	ST r2 r0 cells+924
	LD r2 r0 next+920
	ST r2 r0 cells+920
	LD r2 r0 next+916
	ST r2 r0 cells+916
	LD r2 r0 next+912
	ST r2 r0 cells+912
	LD r2 r0 next+908
	ST r2 r0 cells+908
	LD r2 r0 next+904
	ST r2 r0 cells+904
	LD r2 r0 next+900
	ST r2 r0 cells+900
	LD r2 r0 next+896
	ST r2 r0 cells+896
	LD r2 r0 next+892
	ST r2 r0 cells+892
	LD r2 r0 next+888
	ST r2 r0 cells+888
	LD r2 r0 next+884
	ST r2 r0 cells+884
	LD r2 r0 next+880
	ST r2 r0 cells+880
	LD r2 r0 next+876
	ST r2 r0 cells+876
	LD r2 r0 next+872
	ST r2 r0 cells+872
	LD r2 r0 next+868
	ST r2 r0 cells+868
	LD r2 r0 next+864
	ST r2 r0 cells+864
	LD r2 r0 next+860
	ST r2 r0 cells+860
	LD r2 r0 next+856
	ST r2 r0 cells+856
	LD r2 r0 next+852
	ST r2 r0 cells+852
	LD r2 r0 next+848
	ST r2 r0 cells+848
	LD r2 r0 next+844
	ST r2 r0 cells+844
	LD r2 r0 next+840
	ST r2 r0 cells+840
	LD r2 r0 next+836
	ST r2 r0 cells+836
	LD r2 r0 next+832
	ST r2 r0 cells+832
	LD r2 r0 next+828
	ST r2 r0 cells+828
	LD r2 r0 next+824
	ST r2 r0 cells+824
	LD r2 r0 next+820
	ST r2 r0 cells+820
	LD r2 r0 next+816
	ST r2 r0 cells+816
	LD r2 r0 next+812
	ST r2 r0 cells+812
	LD r2 r0 next+808
	ST r2 r0 cells+808
	LD r2 r0 next+804
	ST r2 r0 cells+804
	LD r2 r0 next+800
	ST r2 r0 cells+800
	LD r2 r0 next+796
	ST r2 r0 cells+796
	LD r2 r0 next+792
	ST r2 r0 cells+792
	LD r2 r0 next+788
	ST r2 r0 cells+788
	LD r2 r0 next+784
	ST r2 r0 cells+784
	LD r2 r0 next+780
	ST r2 r0 cells+780
	LD r2 r0 next+776
	ST r2 r0 cells+776
	LD r2 r0 next+772
	ST r2 r0 cells+772
	LD r2 r0 next+768
	ST r2 r0 cells+768
	LD r2 r0 next+764
	ST r2 r0 cells+764
	LD r2 r0 next+760
	ST r2 r0 cells+760
	LD r2 r0 next+756
	ST r2 r0 cells+756
	LD r2 r0 next+752
	ST r2 r0 cells+752
	LD r2 r0 next+748
	ST r2 r0 cells+748
	LD r2 r0 next+744
	ST r2 r0 cells+744
	LD r2 r0 next+740
	ST r2 r0 cells+740
	LD r2 r0 next+736
	ST r2 r0 cells+736
	LD r2 r0 next+732
	ST r2 r0 cells+732
	LD r2 r0 next+728
	ST r2 r0 cells+728
	LD r2 r0 next+724
	ST r2 r0 cells+724
	LD r2 r0 next+720
	ST r2 r0 cells+720
	LD r2 r0 next+716
	ST r2 r0 cells+716
	LD r2 r0 next+712
	ST r2 r0 cells+712
	LD r2 r0 next+708
	ST r2 r0 cells+708
	LD r2 r0 next+704
	ST r2 r0 cells+704
	LD r2 r0 next+700
	ST r2 r0 cells+700
	LD r2 r0 next+696
	ST r2 r0 cells+696
	LD r2 r0 next+692
	ST r2 r0 cells+692
	LD r2 r0 next+688
	ST r2 r0 cells+688
	LD r2 r0 next+684
	ST r2 r0 cells+684
	LD r2 r0 next+680
	ST r2 r0 cells+680
	LD r2 r0 next+676
	ST r2 r0 cells+676
	LD r2 r0 next+672
	ST r2 r0 cells+672
	LD r2 r0 next+668
	ST r2 r0 cells+668
	LD r2 r0 next+664
	ST r2 r0 cells+664
	LD r2 r0 next+660
	ST r2 r0 cells+660
	LD r2 r0 next+656
	ST r2 r0 cells+656
	LD r2 r0 next+652
	ST r2 r0 cells+652
	LD r2 r0 next+648
	ST r2 r0 cells+648
	LD r2 r0 next+644
	ST r2 r0 cells+644
	LD r2 r0 next+640
	ST r2 r0 cells+640
	LD r2 r0 next+636
	ST r2 r0 cells+636
	LD r2 r0 next+632
	ST r2 r0 cells+632
	LD r2 r0 next+628
	ST r2 r0 cells+628
	LD r2 r0 next+624
	ST r2 r0 cells+624
	LD r2 r0 next+620
	ST r2 r0 cells+620
	LD r2 r0 next+616
	ST r2 r0 cells+616
	LD r2 r0 next+612
	ST r2 r0 cells+612
	LD r2 r0 next+608
	ST r2 r0 cells+608
	LD r2 r0 next+604
	ST r2 r0 cells+604
	LD r2 r0 next+600
	ST r2 r0 cells+600
	LD r2 r0 next+596
	ST r2 r0 cells+596
	LD r2 r0 next+592
	ST r2 r0 cells+592
	LD r2 r0 next+588
	ST r2 r0 cells+588
	LD r2 r0 next+584
	ST r2 r0 cells+584
	LD r2 r0 next+580
	ST r2 r0 cells+580
	LD r2 r0 next+576
	ST r2 r0 cells+576
	LD r2 r0 next+572
	ST r2 r0 cells+572
	LD r2 r0 next+568
	ST r2 r0 cells+568
	LD r2 r0 next+564
	ST r2 r0 cells+564
	LD r2 r0 next+560
	ST r2 r0 cells+560
	LD r2 r0 next+556
	ST r2 r0 cells+556
	LD r2 r0 next+552
	ST r2 r0 cells+552
	LD r2 r0 next+548
	ST r2 r0 cells+548
	LD r2 r0 next+544
	ST r2 r0 cells+544
	LD r2 r0 next+540
	ST r2 r0 cells+540
	LD r2 r0 next+536
	ST r2 r0 cells+536
	LD r2 r0 next+532
	ST r2 r0 cells+532
	LD r2 r0 next+528
	ST r2 r0 cells+528
	LD r2 r0 next+524
	ST r2 r0 cells+524
	LD r2 r0 next+520
	ST r2 r0 cells+520
	LD r2 r0 next+516
	ST r2 r0 cells+516
	LD r2 r0 next+512
	ST r2 r0 cells+512
	LD r2 r0 next+508
	ST r2 r0 cells+508
	LD r2 r0 next+504
	ST r2 r0 cells+504
	LD r2 r0 next+500
	ST r2 r0 cells+500
	LD r2 r0 next+496
	ST r2 r0 cells+496
	LD r2 r0 next+492
	ST r2 r0 cells+492
	LD r2 r0 next+488
	ST r2 r0 cells+488
	LD r2 r0 next+484
	ST r2 r0 cells+484
	LD r2 r0 next+480
	ST r2 r0 cells+480
	LD r2 r0 next+476
	ST r2 r0 cells+476
	LD r2 r0 next+472
	ST r2 r0 cells+472
	LD r2 r0 next+468
	ST r2 r0 cells+468
	LD r2 r0 next+464
	ST r2 r0 cells+464
	LD r2 r0 next+460
	ST r2 r0 cells+460
	LD r2 r0 next+456
	ST r2 r0 cells+456
	LD r2 r0 next+452
	ST r2 r0 cells+452
	LD r2 r0 next+448
	ST r2 r0 cells+448
	LD r2 r0 next+444
	ST r2 r0 cells+444
	LD r2 r0 next+440
	ST r2 r0 cells+440
	LD r2 r0 next+436
	ST r2 r0 cells+436
	LD r2 r0 next+432
	ST r2 r0 cells+432
	LD r2 r0 next+428
	ST r2 r0 cells+428
	LD r2 r0 next+424
	ST r2 r0 cells+424
	LD r2 r0 next+420
	ST r2 r0 cells+420
	LD r2 r0 next+416
	ST r2 r0 cells+416
	LD r2 r0 next+412
	ST r2 r0 cells+412
	LD r2 r0 next+408
	ST r2 r0 cells+408
	LD r2 r0 next+404
	ST r2 r0 cells+404
	LD r2 r0 next+400
	ST r2 r0 cells+400
	LD r2 r0 next+396
	ST r2 r0 cells+396
	LD r2 r0 next+392
	ST r2 r0 cells+392
	LD r2 r0 next+388
	ST r2 r0 cells+388
	LD r2 r0 next+384
	ST r2 r0 cells+384
	LD r2 r0 next+380
	ST r2 r0 cells+380
	LD r2 r0 next+376
	ST r2 r0 cells+376
	LD r2 r0 next+372
	ST r2 r0 cells+372
	LD r2 r0 next+368
	ST r2 r0 cells+368
	LD r2 r0 next+364
	ST r2 r0 cells+364
	LD r2 r0 next+360
	ST r2 r0 cells+360
	LD r2 r0 next+356
	ST r2 r0 cells+356
	LD r2 r0 next+352
	ST r2 r0 cells+352
	LD r2 r0 next+348
	ST r2 r0 cells+348
	LD r2 r0 next+344
	ST r2 r0 cells+344
	LD r2 r0 next+340
	ST r2 r0 cells+340
	LD r2 r0 next+336
	ST r2 r0 cells+336
	LD r2 r0 next+332
	ST r2 r0 cells+332
	LD r2 r0 next+328
	ST r2 r0 cells+328
	LD r2 r0 next+324
	ST r2 r0 cells+324
	LD r2 r0 next+320
	ST r2 r0 cells+320
	LD r2 r0 next+316
	ST r2 r0 cells+316
	LD r2 r0 next+312
	ST r2 r0 cells+312
	LD r2 r0 next+308
	ST r2 r0 cells+308
	LD r2 r0 next+304
	ST r2 r0 cells+304
	LD r2 r0 next+300
	ST r2 r0 cells+300
	LD r2 r0 next+296
	ST r2 r0 cells+296
	LD r2 r0 next+292
	ST r2 r0 cells+292
	LD r2 r0 next+288
	ST r2 r0 cells+288
	LD r2 r0 next+284
	ST r2 r0 cells+284
	LD r2 r0 next+280
	ST r2 r0 cells+280
	LD r2 r0 next+276
	ST r2 r0 cells+276
	LD r2 r0 next+272
	ST r2 r0 cells+272
	LD r2 r0 next+268
	ST r2 r0 cells+268
	LD r2 r0 next+264
	ST r2 r0 cells+264
	LD r2 r0 next+260
	ST r2 r0 cells+260
	LD r2 r0 next+256
	ST r2 r0 cells+256
	LD r2 r0 next+252
	ST r2 r0 cells+252
	LD r2 r0 next+248
	ST r2 r0 cells+248
	LD r2 r0 next+244
	ST r2 r0 cells+244
	LD r2 r0 next+240
	ST r2 r0 cells+240
	LD r2 r0 next+236
	ST r2 r0 cells+236
	LD r2 r0 next+232
	ST r2 r0 cells+232
	LD r2 r0 next+228
	ST r2 r0 cells+228
	LD r2 r0 next+224
	ST r2 r0 cells+224
	LD r2 r0 next+220
	ST r2 r0 cells+220
	LD r2 r0 next+216
	ST r2 r0 cells+216
	LD r2 r0 next+212
	ST r2 r0 cells+212
	LD r2 r0 next+208
	ST r2 r0 cells+208
	LD r2 r0 next+204
	ST r2 r0 cells+204
	LD r2 r0 next+200
	ST r2 r0 cells+200
	LD r2 r0 next+196
	ST r2 r0 cells+196
	LD r2 r0 next+192
	ST r2 r0 cells+192
	LD r2 r0 next+188
	ST r2 r0 cells+188
	LD r2 r0 next+184
	ST r2 r0 cells+184
	LD r2 r0 next+180
	ST r2 r0 cells+180
	LD r2 r0 next+176
	ST r2 r0 cells+176
	LD r2 r0 next+172
	ST r2 r0 cells+172
	LD r2 r0 next+168
	ST r2 r0 cells+168
	LD r2 r0 next+164
	ST r2 r0 cells+164
	LD r2 r0 next+160
	ST r2 r0 cells+160
	LD r2 r0 next+156
	ST r2 r0 cells+156
	LD r2 r0 next+152
	ST r2 r0 cells+152
	LD r2 r0 next+148
	ST r2 r0 cells+148
	LD r2 r0 next+144
	ST r2 r0 cells+144
	LD r2 r0 next+140
	ST r2 r0 cells+140
	LD r2 r0 next+136
	ST r2 r0 cells+136
	LD r2 r0 next+132
	ST r2 r0 cells+132
	LD r2 r0 next+128
	ST r2 r0 cells+128
	LD r2 r0 next+124
	ST r2 r0 cells+124
	LD r2 r0 next+120
	ST r2 r0 cells+120
	LD r2 r0 next+116
	ST r2 r0 cells+116
	LD r2 r0 next+112
	ST r2 r0 cells+112
	LD r2 r0 next+108
	ST r2 r0 cells+108
	LD r2 r0 next+104
	ST r2 r0 cells+104
	LD r2 r0 next+100
	ST r2 r0 cells+100
	LD r2 r0 next+96
	ST r2 r0 cells+96
	LD r2 r0 next+92
	ST r2 r0 cells+92
	LD r2 r0 next+88
	ST r2 r0 cells+88
	LD r2 r0 next+84
	ST r2 r0 cells+84
	LD r2 r0 next+80
	ST r2 r0 cells+80
	LD r2 r0 next+76
	ST r2 r0 cells+76
	LD r2 r0 next+72
	ST r2 r0 cells+72
	LD r2 r0 next+68
	ST r2 r0 cells+68
	LD r2 r0 next+64
	ST r2 r0 cells+64
	LD r2 r0 next+60
	ST r2 r0 cells+60
	LD r2 r0 next+56
	ST r2 r0 cells+56
	LD r2 r0 next+52
	ST r2 r0 cells+52
	LD r2 r0 next+48
	ST r2 r0 cells+48
	LD r2 r0 next+44
	ST r2 r0 cells+44
	LD r2 r0 next+40
	ST r2 r0 cells+40
	LD r2 r0 next+36
	ST r2 r0 cells+36
	LD r2 r0 next+32
	ST r2 r0 cells+32
	LD r2 r0 next+28
	ST r2 r0 cells+28
	LD r2 r0 next+24
	ST r2 r0 cells+24
	LD r2 r0 next+20
	ST r2 r0 cells+20
	LD r2 r0 next+16
	ST r2 r0 cells+16
	LD r2 r0 next+12
	ST r2 r0 cells+12
	LD r2 r0 next+8
	ST r2 r0 cells+8
	LD r2 r0 next+4
	ST r2 r0 cells+4
	LD r2 r0 next
	ST r2 r0 cells
	MOVli r2 0
	BR_COND r13 .LBB0_1
	B .LBB0_13
.LBB0_13:                               ; %done
	MOVli r9 0
	LD r8 r1 4
	LD r7 r1 8
	LD r6 r1 12
	LD r5 r1 16
	LD r3 r1 20
	BR r0
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        ; -- End function
	.type	cells,@object                   ; @cells
	.local	cells
	.comm	cells,1024,16
	.type	next,@object                    ; @next
	.local	next
	.comm	next,1024,16
	.section	".note.GNU-stack","",@progbits

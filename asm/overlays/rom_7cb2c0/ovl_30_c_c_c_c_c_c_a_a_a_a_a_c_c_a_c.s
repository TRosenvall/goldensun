	.include "macros.inc"

@ Cutscene: roughly 239 instructions of straight-line script --
@ 0 turns, 1 animation change, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x302, 0x911, 0x928.
@ Sets save bits 0x144, 0x902.
.thumb_func_start OvlFunc_945_200aff0
	push	{r5, lr}
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0xa2
	lsl	r0, #1
	bl	__SetFlag
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	ldr	r1, =gState
	add	r2, #0x49
	str	r2, [r3]
	sub	r2, #0x47
	add	r3, r1, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #1
	cmp	r3, #0x17
	bhi	.L30d6
	ldr	r2, =.L3028
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L3028:
	.word	.L3088
	.word	.L3088
	.word	.L30d6
	.word	.L30b2
	.word	.L30b8
	.word	.L30d6
	.word	.L30d6
	.word	.L30d6
	.word	.L30d6
	.word	.L30d6
	.word	.L3088
	.word	.L30b2
	.word	.L30d6
	.word	.L30d6
	.word	.L30b2
	.word	.L30b2
	.word	.L30b2
	.word	.L30b2
	.word	.L30b2
	.word	.L30b2
	.word	.L30b2
	.word	.L30d6
	.word	.L30b2
	.word	.L30b2
.L3088:
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	bne	.L30a2
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L30a2
	mov	r1, #2
	mov	r0, #9
	b	.L30d0
.L30a2:
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	beq	.L30d4
	mov	r1, #2
	mov	r0, #0xc
	b	.L30d0
.L30b2:
	mov	r1, #2
	mov	r0, #0x13
	b	.L30d0
.L30b8:
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	bne	.L30d4
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	beq	.L30d4
	mov	r1, #2
	mov	r0, #0xd
.L30d0:
	bl	__Func_8092950
.L30d4:
	ldr	r1, =gState
.L30d6:
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r1, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #1
	cmp	r3, #0x1d
	bls	.L30e8
	b	.L3320
.L30e8:
	ldr	r2, =.L30f0
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L30f0:
	.word	.L3168
	.word	.L3168
	.word	.L3320
	.word	.L316e
	.word	.L3174
	.word	.L3320
	.word	.L3320
	.word	.L3320
	.word	.L3320
	.word	.L317a
	.word	.L3190
	.word	.L31a6
	.word	.L31ac
	.word	.L31b2
	.word	.L31b8
	.word	.L320e
	.word	.L3214
	.word	.L3254
	.word	.L325a
	.word	.L32aa
	.word	.L32b0
	.word	.L32e4
	.word	.L32ea
	.word	.L32fa
	.word	.L3320
	.word	.L3320
	.word	.L3320
	.word	.L3320
	.word	.L3320
	.word	.L3300
.L3168:
	bl	OvlFunc_945_200b51c
	b	.L3320
.L316e:
	bl	OvlFunc_945_200b66c
	b	.L3320
.L3174:
	bl	OvlFunc_945_200b364
	b	.L3320
.L317a:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L318a
	bl	OvlFunc_945_200bdec
	b	.L3320
.L318a:
	bl	OvlFunc_945_200bd10
	b	.L3320
.L3190:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L31a0
	bl	OvlFunc_945_200beec
	b	.L3320
.L31a0:
	bl	OvlFunc_945_200be34
	b	.L3320
.L31a6:
	bl	OvlFunc_945_200bf94
	b	.L3320
.L31ac:
	bl	OvlFunc_945_200c0e8
	b	.L3320
.L31b2:
	bl	OvlFunc_945_200c13c
	b	.L3320
.L31b8:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3208
	bl	__CutsceneStart
	mov	r0, #0x19
	mov	r1, #1
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	mov	r2, #0
	mov	r0, #0x16
	mov	r1, #0
	bl	OvlFunc_945_200c8e8
	ldr	r5, =gScript_945__0200e8e4
	mov	r0, #0x24
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x25
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x26
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x24
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x25
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x26
	b	.L3298
.L3208:
	bl	OvlFunc_945_200c198
	b	.L3320
.L320e:
	bl	OvlFunc_945_200c218
	b	.L3320
.L3214:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L324e
	bl	__CutsceneStart
	mov	r0, #0x19
	mov	r1, #2
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	mov	r2, #0
	mov	r0, #0x16
	mov	r1, #0
	bl	OvlFunc_945_200c8e8
	ldr	r5, =gScript_945__0200e8e4
	mov	r0, #0x24
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x25
	mov	r1, r5
	bl	__MapActor_SetBehavior
	bl	__CutsceneEnd
	b	.L3320
.L324e:
	bl	OvlFunc_945_200d068
	b	.L3320
.L3254:
	bl	OvlFunc_945_200d684
	b	.L3320
.L325a:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L32a4
	bl	__CutsceneStart
	mov	r0, #0x19
	mov	r1, #3
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	mov	r2, #0
	mov	r0, #0x16
	mov	r1, #0
	bl	OvlFunc_945_200c8e8
	ldr	r5, =gScript_945__0200e8e4
	mov	r0, #0x24
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x25
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x24
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x25
.L3298:
	mov	r1, #3
	bl	__Func_8092950
	bl	__CutsceneEnd
	b	.L3320
.L32a4:
	bl	OvlFunc_945_200d6dc
	b	.L3320
.L32aa:
	bl	OvlFunc_945_200d780
	b	.L3320
.L32b0:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L32de
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3320
	ldr	r3, =.L7f84
	mov	r2, #0
	mov	r1, #0xc8
	lsl	r1, #4
	str	r2, [r3]
	ldr	r0, =OvlFunc_945_200dc48
	bl	__StartTask
	mov	r0, #9
	mov	r1, #5
	bl	__MapActor_SetAnim
	b	.L3320
.L32de:
	bl	OvlFunc_945_200d7ec
	b	.L3320
.L32e4:
	bl	OvlFunc_945_200dca4
	b	.L3320
.L32ea:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3320
	bl	OvlFunc_945_200dd10
	b	.L3320
.L32fa:
	bl	OvlFunc_945_200e110
	b	.L3320
.L3300:
	ldr	r1, =0x926
	ldr	r2, =0x92b
	mov	r0, #0x14
	bl	OvlFunc_945_200c8e8
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	ldr	r0, =0x902
	bl	__SetFlag
	mov	r0, #1
	bl	__Func_8091e9c
.L3320:
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_945_200aff0

@ 158 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, OvlFunc_45d0, TestSaveBit, OvlFunc_45d0
@   TestSaveBit x2, GetSlotEntityChecked, OvlFunc_48e8, OvlFunc_4890 x2
@   SetSlotEntitySpeed, GetSlotEntityChecked, TestSaveBit, OvlFunc_4890
@   TestSaveBit x2, OvlFunc_4890
@   ... and 5 more
@ reads save bits 0x109, 0x8a0, 0x911, 0x921, 0x925.
.thumb_func_start OvlFunc_945_200b364
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3380
	bl	OvlFunc_945_200c5d0
	b	.L34f2

	.pool_aligned

.L3380:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L338e
	bl	OvlFunc_945_200c5d0
.L338e:
	ldr	r0, =0x93e
	bl	__GetFlag
	mov	r7, r0
	cmp	r7, #0
	beq	.L339c
	b	.L34f2
.L339c:
	mov	r0, #0x8a
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3456
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, #0
	mov	r6, r0
	mov	r2, #0
	mov	r0, #0xd
	bl	OvlFunc_945_200c8e8
	mov	r1, #0xe4
	mov	r2, #0xa3
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #8
	mov	r3, #0
	bl	OvlFunc_945_200c890
	mov	r3, #0xf0
	lsl	r3, #1
	mov	r8, r3
	mov	r2, #0x96
	mov	r3, #0xb0
	lsl	r3, #8
	lsl	r2, #2
	mov	r0, #9
	mov	r1, r8
	bl	OvlFunc_945_200c890
	mov	r0, #9
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r3, r6
	add	r3, #0x66
	ldr	r5, .L3428	@ 0
	strh	r7, [r3]
	sub	r3, #3
	strb	r5, [r3]
	mov	r1, r6
	add	r1, #0x59
	ldrb	r2, [r1]
	mov	r3, #0x80
	orr	r3, r2
	strb	r3, [r1]
	ldr	r3, =OvlFunc_945_200812c
	mov	r0, #8
	str	r3, [r6, #0x6c]
	bl	__MapActor_GetActor
	mov	r3, r0
	add	r3, #0x62
	strb	r5, [r3]
	ldr	r3, =OvlFunc_945_2008284
	str	r3, [r0, #0x6c]
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L34f2
	mov	r3, #0xa0
	ldr	r2, =0x29a
	lsl	r3, #8
	b	.L344c

	.align	2, 0
.L3428:
	.word	0
	.pool

.L344c:
	mov	r0, #0
	mov	r1, r8
	bl	OvlFunc_945_200c890
	b	.L34f2
.L3456:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L346e
	mov	r1, #0xde
	mov	r3, #0xd0
	lsl	r1, #1
	ldr	r2, =0x266
	mov	r0, #8
	lsl	r3, #8
	b	.L3484
.L346e:
	ldr	r0, =0x925
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3494
	mov	r1, #0xe4
	mov	r2, #0xa2
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #8
	mov	r3, #0
.L3484:
	bl	OvlFunc_945_200c890
	mov	r0, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	b	.L34f2
.L3494:
	ldr	r0, =0x921
	bl	__GetFlag
	cmp	r0, #0
	beq	.L34f2
	mov	r3, #0x80
	ldr	r1, =0x1db
	ldr	r2, =0x256
	lsl	r3, #8
	mov	r0, #8
	mov	r5, #0xb0
	bl	OvlFunc_945_200c890
	lsl	r5, #8
	mov	r1, #0xe7
	lsl	r1, #1
	ldr	r2, =0x26a
	mov	r3, r5
	mov	r0, #9
	bl	OvlFunc_945_200c890
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r3, #0xc0
	lsl	r3, #6
	strh	r3, [r0, #6]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r1, #0xdb
	mov	r3, #0xd0
	strh	r5, [r0, #6]
	lsl	r1, #1
	ldr	r2, =0x293
	lsl	r3, #8
	mov	r0, #0xd
	bl	OvlFunc_945_200c890
	mov	r1, #0xf4
	mov	r2, #0xac
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #0xa
	mov	r3, r5
	bl	OvlFunc_945_200c890
.L34f2:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_200b364

@ 127 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, PlaceSlotAt x5, TestSaveBit, OvlFunc_4890
@   SetSlotScriptWithTurn, OvlFunc_4890 x6, SetSlotScriptWithTurn, TestSaveBit
@   OvlFunc_5004, TestSaveBit, OvlFunc_48e8, TestSaveBit x2
@   OvlFunc_48e8, PlaceSlotAt
@ reads save bits 0x8a0, 0x911, 0x922, 0x925, 0x928.
.thumb_func_start OvlFunc_945_200b51c
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3564
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xe
	b	.L360c
.L3564:
	mov	r0, #0x8a
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L35f0
	mov	r2, #0xde
	mov	r3, #0xc0
	lsl	r2, #1
	lsl	r3, #6
	mov	r0, #8
	mov	r1, #0x98
	bl	OvlFunc_945_200c890
	ldr	r1, =gScript_945__0200e958
	mov	r0, #8
	bl	__MapActor_SetBehavior
	mov	r3, #0xf0
	lsl	r3, #1
	mov	r10, r3
	mov	r3, #0xb0
	lsl	r3, #8
	mov	r5, #0xf4
	mov	r8, r3
	mov	r0, #0xa
	mov	r1, #0xb8
	mov	r2, r10
	lsl	r5, #1
	mov	r6, #0xd0
	bl	OvlFunc_945_200c890
	lsl	r6, #8
	mov	r0, #0xc
	mov	r1, #0xaa
	mov	r2, r5
	mov	r3, r8
	bl	OvlFunc_945_200c890
	mov	r0, #0xd
	mov	r1, #0x88
	mov	r2, r5
	mov	r3, r6
	bl	OvlFunc_945_200c890
	mov	r0, #0xf
	mov	r1, #0x78
	mov	r2, r10
	mov	r3, r6
	bl	OvlFunc_945_200c890
	ldr	r2, =0x20e
	mov	r0, #0xe
	mov	r1, #0xb8
	mov	r3, r8
	bl	OvlFunc_945_200c890
	mov	r2, #0x92
	mov	r3, #0x80
	mov	r0, #0xb
	mov	r1, #0x88
	lsl	r2, #2
	lsl	r3, #8
	bl	OvlFunc_945_200c890
	ldr	r1, =gScript_945__0200e840
	mov	r0, #0xb
	bl	__MapActor_SetBehavior
	b	.L363e
.L35f0:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3600
	bl	OvlFunc_945_200d004
	b	.L363e
.L3600:
	ldr	r0, =0x925
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3616
	mov	r0, #0x12
.L360c:
	mov	r1, #0
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	b	.L363e
.L3616:
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	beq	.L363e
	ldr	r0, =0x922
	bl	__GetFlag
	cmp	r0, #0
	beq	.L363e
	mov	r0, #0xe
	mov	r1, #0
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L363e:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_200b51c

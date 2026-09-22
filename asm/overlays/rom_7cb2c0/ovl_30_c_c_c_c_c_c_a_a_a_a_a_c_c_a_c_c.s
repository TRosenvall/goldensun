	.include "macros.inc"

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

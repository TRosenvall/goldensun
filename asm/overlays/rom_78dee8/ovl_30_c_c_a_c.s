	.include "macros.inc"

@ RevealPassageB
@ Takes no arguments. The passage-B reveal, gated on 0xF02 set and 0x821 clear.
@ Structurally the same scene as OvlFunc_258 with a different rectangle and
@ different line ids; see there for the step-by-step.
.thumb_func_start OvlFunc_895_2008420
	push	{r5, r6, lr}
	ldr	r0, =0xf02
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	bne	.L430
	b	.L54e
.L430:
	ldr	r0, =0x821
	bl	__GetFlag
	cmp	r0, #0
	beq	.L43c
	b	.L54e
.L43c:
	bl	__CutsceneStart
	bl	__Func_808e118
	mov	r0, #0xb6
	bl	__PlaySound
	mov	r5, #1
	mov	r2, #0x64
	mov	r3, #0x47
	mov	r1, #0x47
	mov	r0, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	bl	__Func_800fe9c
	mov	r0, #0x28
	bl	__CutsceneWait
	ldr	r6, =0x1032
	mov	r1, #1
	mov	r0, r6
	bl	__Func_801776c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xb7
	bl	__PlaySound
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r0, #0x7a
	mov	r1, #0x14
	mov	r2, #0x78
	mov	r3, #0x1e
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r3, #0x78
	mov	r2, #0x1e
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #2
	mov	r0, #0x7a
	mov	r1, #0x14
	mov	r2, #1
	bl	__Func_8010704
	bl	__Func_800fe9c
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #9
	lsl	r2, #9
	lsl	r0, #9
	bl	__Func_8012330
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #10
	lsl	r2, #9
	lsl	r0, #10
	bl	__Func_8012330
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r0, #0
	mov	r1, #4
	mov	r2, #0x14
	bl	__MapActor_Jump
	mov	r0, #0
	mov	r1, #6
	mov	r2, #0x28
	bl	__MapActor_Jump
	mov	r0, #1
	mov	r1, #1
	ldr	r2, =0xe666
	neg	r1, r1
	neg	r0, r0
	bl	__Func_8012330
	add	r6, #1
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, r6
	bl	__Func_801776c
	ldr	r0, =0x143
	bl	__SetFlag
	ldr	r0, =0x821
	bl	__SetFlag
	bl	__CutsceneEnd
.L54e:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_2008420

@ TrackBlock9West
@ Takes no arguments. Records where the block in slot 9 is resting, as a pair of
@ save bits, so the puzzle survives a save and reload.
@
@ Both bits 0x302 and 0x303 are cleared first, then exactly one is set from the
@ block's tile x (its +0x08 taken down with `asr #20`): x 0x5D sets 0x303,
@ x 0x5F sets 0x302, and any position between them leaves both clear. Slots 9
@ and 0xA are the two-block arrangement, used when the entrance id is 0x0B..0x0D.
.thumb_func_start OvlFunc_895_200856c
	push	{r5, lr}
	mov	r0, #9
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L59e
	ldr	r3, [r0, #8]
	ldr	r0, =0x302
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x303
	bl	__ClearFlag
	cmp	r5, #0x5d
	bne	.L594
	ldr	r0, =0x303
	bl	__SetFlag
	b	.L59e
.L594:
	cmp	r5, #0x5f
	bne	.L59e
	ldr	r0, =0x302
	bl	__SetFlag
.L59e:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_200856c

@ TrackBlockAWest
@ Takes no arguments. The slot-0xA partner of OvlFunc_56c: bits 0x300 / 0x301,
@ set from tile x 0x73 / 0x71 respectively. Note the sense is inverted relative
@ to slot 9 -- here the HIGHER x sets the LOWER-numbered bit.
.thumb_func_start OvlFunc_895_20085ac
	push	{r5, lr}
	mov	r0, #0xa
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L5e2
	ldr	r3, [r0, #8]
	mov	r0, #0xc0
	lsl	r0, #2
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x301
	bl	__ClearFlag
	cmp	r5, #0x73
	bne	.L5d8
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__SetFlag
	b	.L5e2
.L5d8:
	cmp	r5, #0x71
	bne	.L5e2
	ldr	r0, =0x301
	bl	__SetFlag
.L5e2:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_20085ac

@ TrackBlock9East
@ Takes no arguments. First of the six-block arrangement used when the entrance
@ id is 0x0E..0x10. Slot 9, bits 0x310 / 0x311, tile x 0x65 / 0x63.
@ Unlike the two-block pair above, each of these six ends by calling
@ OvlFunc_17c0(0) to re-test whether the whole puzzle is now solved.
.thumb_func_start OvlFunc_895_20085ec
	push	{r5, lr}
	mov	r0, #9
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L628
	ldr	r3, [r0, #8]
	mov	r0, #0xc4
	lsl	r0, #2
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x311
	bl	__ClearFlag
	cmp	r5, #0x63
	bne	.L616
	ldr	r0, =0x311
	bl	__SetFlag
	b	.L622
.L616:
	cmp	r5, #0x65
	bne	.L622
	mov	r0, #0xc4
	lsl	r0, #2
	bl	__SetFlag
.L622:
	mov	r0, #0
	bl	OvlFunc_895_20097c0
.L628:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_20085ec

@ TrackBlockAEast
@ Slot 0xA, bits 0x312 / 0x313, tile x 0x69 / 0x67. See OvlFunc_5ec.
.thumb_func_start OvlFunc_895_2008634
	push	{r5, lr}
	mov	r0, #0xa
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L66c
	ldr	r3, [r0, #8]
	ldr	r0, =0x312
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x313
	bl	__ClearFlag
	cmp	r5, #0x67
	bne	.L65c
	ldr	r0, =0x313
	bl	__SetFlag
	b	.L666
.L65c:
	cmp	r5, #0x69
	bne	.L666
	ldr	r0, =0x312
	bl	__SetFlag
.L666:
	mov	r0, #0
	bl	OvlFunc_895_20097c0
.L66c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_2008634

@ TrackBlockBEast
@ Slot 0xB, bits 0x314 / 0x315, tile x 0x6D / 0x6B. See OvlFunc_5ec.
.thumb_func_start OvlFunc_895_200867c
	push	{r5, lr}
	mov	r0, #0xb
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L6b8
	ldr	r3, [r0, #8]
	mov	r0, #0xc5
	lsl	r0, #2
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x315
	bl	__ClearFlag
	cmp	r5, #0x6b
	bne	.L6a6
	ldr	r0, =0x315
	bl	__SetFlag
	b	.L6b2
.L6a6:
	cmp	r5, #0x6d
	bne	.L6b2
	mov	r0, #0xc5
	lsl	r0, #2
	bl	__SetFlag
.L6b2:
	mov	r0, #0
	bl	OvlFunc_895_20097c0
.L6b8:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_200867c

@ TrackBlockCEast
@ Slot 0xC, bits 0x316 / 0x317, tile x 0x71 / 0x6F. See OvlFunc_5ec.
.thumb_func_start OvlFunc_895_20086c4
	push	{r5, lr}
	mov	r0, #0xc
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L6fc
	ldr	r3, [r0, #8]
	ldr	r0, =0x316
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x317
	bl	__ClearFlag
	cmp	r5, #0x6f
	bne	.L6ec
	ldr	r0, =0x317
	bl	__SetFlag
	b	.L6f6
.L6ec:
	cmp	r5, #0x71
	bne	.L6f6
	ldr	r0, =0x316
	bl	__SetFlag
.L6f6:
	mov	r0, #0
	bl	OvlFunc_895_20097c0
.L6fc:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_20086c4

@ TrackBlockDEast
@ Slot 0xD, bits 0x318 / 0x319, tile x 0x75 / 0x73. See OvlFunc_5ec.
.thumb_func_start OvlFunc_895_200870c
	push	{r5, lr}
	mov	r0, #0xd
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L748
	ldr	r3, [r0, #8]
	mov	r0, #0xc6
	lsl	r0, #2
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x319
	bl	__ClearFlag
	cmp	r5, #0x73
	bne	.L736
	ldr	r0, =0x319
	bl	__SetFlag
	b	.L742
.L736:
	cmp	r5, #0x75
	bne	.L742
	mov	r0, #0xc6
	lsl	r0, #2
	bl	__SetFlag
.L742:
	mov	r0, #0
	bl	OvlFunc_895_20097c0
.L748:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_200870c

@ TrackBlockEEast
@ Slot 0xE, bits 0x31A / 0x31B, tile x 0x79 / 0x77. See OvlFunc_5ec.
@ The six blocks sit on a regular grid: consecutive slots are 4 tiles apart and
@ each owns the next pair of save bits, so slot N uses 0x310 + (N-9)*2.
.thumb_func_start OvlFunc_895_2008754
	push	{r5, lr}
	mov	r0, #0xe
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L78c
	ldr	r3, [r0, #8]
	ldr	r0, =0x31a
	asr	r5, r3, #20
	bl	__ClearFlag
	ldr	r0, =0x31b
	bl	__ClearFlag
	cmp	r5, #0x77
	bne	.L77c
	ldr	r0, =0x31b
	bl	__SetFlag
	b	.L786
.L77c:
	cmp	r5, #0x79
	bne	.L786
	ldr	r0, =0x31a
	bl	__SetFlag
.L786:
	mov	r0, #0
	bl	OvlFunc_895_20097c0
.L78c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_2008754


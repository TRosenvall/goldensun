	.include "macros.inc"

@ 72 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, PlaySound, GetSlotEntityChecked, SetSlotEntitySpeed
@   MoveSlotBy, SetPendingMessageId, EndCutscene, PlayMapRectAnimation
@   OvlFunc_2b4, EndCutscene
.thumb_func_start OvlFunc_911_2008304
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r7, [r3]
	bl	__CutsceneStart
	mov	r0, #0x9e
	bl	__PlaySound
	mov	r2, #0xb6
	lsl	r2, #1
	add	r3, r7, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #5
	mov	r6, #0
	mov	r5, #0
	cmp	r3, #4
	bhi	.L39a
	ldr	r2, =.L330
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L330:
	.word	.L344
	.word	.L34a
	.word	.L350
	.word	.L356
	.word	.L35c
.L344:
	mov	r6, #0x47
	mov	r5, #9
	b	.L39a
.L34a:
	mov	r6, #0x49
	mov	r5, #0x11
	b	.L39a
.L350:
	mov	r6, #0x50
	mov	r5, #0x15
	b	.L39a
.L356:
	mov	r6, #0x54
	mov	r5, #0xc
	b	.L39a
.L35c:
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	mov	r1, #0x80
	mov	r2, #0x80
	strb	r3, [r0]
	lsl	r1, #8
	mov	r0, #0
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r0, #0
	mov	r1, #0
	mov	r2, #8
	bl	__Func_809228c
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe4
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0x10
	str	r2, [r3]
	mov	r0, #9
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	b	.L3b6
.L39a:
	ldr	r0, =.L2e48
	mov	r1, r6
	mov	r2, r5
	bl	__Func_8010560
	mov	r2, #0xb6
	lsl	r2, #1
	add	r3, r7, r2
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	bl	OvlFunc_911_20082b4
	bl	__CutsceneEnd
.L3b6:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_911_2008304

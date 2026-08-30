	.include "macros.inc"

@ 65 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x3, OvlFunc_1774 x2, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200a16c
	push	{r5, r6, lr}
	mov	r0, #0xd
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xd
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0xf
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	cmp	r6, #0x19
	bne	.L219e
	mov	r0, #0xd
	mov	r1, #0x60
	mov	r2, #0
	bl	OvlFunc_946_2009774
	mov	r0, #0xd
	mov	r1, #0x50
	b	.L21ba
.L219e:
	cmp	r6, #0x1f
	bne	.L21a8
	mov	r0, #0xd
	mov	r1, #0x50
	b	.L21ba
.L21a8:
	cmp	r6, #0x22
	bne	.L21b2
	mov	r0, #0xd
	mov	r1, #0x20
	b	.L21ba
.L21b2:
	cmp	r6, #0x23
	bne	.L21c2
	mov	r0, #0xd
	mov	r1, #0x10
.L21ba:
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L21c6
.L21c2:
	cmp	r6, #0x24
	beq	.L21f8
.L21c6:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xd
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	sub	r5, #1
	asr	r3, #20
	str	r3, [sp]
	mov	r0, r6
	mov	r1, r5
	mov	r2, #1
	mov	r3, #3
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	mov	r3, #3
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
.L21f8:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200a16c

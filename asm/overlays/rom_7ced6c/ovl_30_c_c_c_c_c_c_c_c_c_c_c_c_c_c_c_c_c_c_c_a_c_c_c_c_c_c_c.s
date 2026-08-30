	.include "macros.inc"

@ 72 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x3, OvlFunc_1774, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200add0
	push	{r5, r6, lr}
	mov	r0, #0x10
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0x10
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #9
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r6, #6
	bne	.L2e08
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2e02
	mov	r0, #0x10
	mov	r1, #0x20
	b	.L2e2a
.L2e02:
	mov	r0, #0x10
	mov	r1, #0x70
	b	.L2e2a
.L2e08:
	cmp	r6, #8
	bne	.L2e18
	sub	r3, #9
	cmp	r3, #2
	bls	.L2e68
	mov	r0, #0x10
	mov	r1, #0x50
	b	.L2e2a
.L2e18:
	cmp	r6, #9
	bne	.L2e22
	mov	r0, #0x10
	mov	r1, #0x40
	b	.L2e2a
.L2e22:
	cmp	r6, #0xc
	bne	.L2e32
	mov	r0, #0x10
	mov	r1, #0x10
.L2e2a:
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L2e36
.L2e32:
	cmp	r6, #0xd
	beq	.L2e68
.L2e36:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0x10
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
.L2e68:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200add0

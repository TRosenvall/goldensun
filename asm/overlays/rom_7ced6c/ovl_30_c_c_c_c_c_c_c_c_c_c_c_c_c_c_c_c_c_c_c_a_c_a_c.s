	.include "macros.inc"

@ 56 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x3, OvlFunc_1774 x2, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_2009ef4
	push	{r5, r6, lr}
	mov	r0, #0xb
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xb
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r5, r3, #20
	cmp	r6, #0x24
	beq	.L1f6e
	cmp	r6, #0x1e
	bne	.L1f2e
	mov	r0, #0xa
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r3, #0x12
	beq	.L1f6e
	mov	r0, #0xb
	mov	r1, #0x60
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L1f3c
.L1f2e:
	cmp	r6, #0x22
	bne	.L1f3c
	mov	r0, #0xb
	mov	r1, #0x20
	mov	r2, #0
	bl	OvlFunc_946_2009774
.L1f3c:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xb
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
.L1f6e:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_2009ef4

@ 61 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, OvlFunc_1774 x3, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_2009f78
	push	{r5, r6, r7, lr}
	mov	r0, #0xc
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xc
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r7, r3, #20
	cmp	r6, #0x24
	bne	.L1fa8
	mov	r5, #0x60
	neg	r5, r5
	mov	r0, #0xc
	mov	r1, r5
	mov	r2, #0
	bl	OvlFunc_946_2009774
	mov	r0, #0xc
	mov	r1, r5
	b	.L1fbe
.L1fa8:
	cmp	r6, #0x22
	bne	.L1fc6
	mov	r1, #0x60
	neg	r1, r1
	mov	r0, #0xc
	mov	r2, #0
	bl	OvlFunc_946_2009774
	mov	r1, #0x40
	neg	r1, r1
	mov	r0, #0xc
.L1fbe:
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L1fca
.L1fc6:
	cmp	r6, #0x18
	beq	.L1ffc
.L1fca:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xc
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	sub	r5, r7, #1
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
.L1ffc:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_2009f78

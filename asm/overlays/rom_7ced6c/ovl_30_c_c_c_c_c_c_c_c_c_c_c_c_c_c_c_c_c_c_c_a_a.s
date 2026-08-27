	.include "macros.inc"

@ 87 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x4, OvlFunc_1774 x3, OvlFunc_1b14, WaitFrames
@   GetSlotEntityChecked, CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_2009bbc
	push	{r5, r6, r7, lr}
	mov	r0, #8
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #8
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0xc
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xf
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r6, #0x13
	bne	.L1c18
	cmp	r5, #0x18
	bne	.L1bf4
	mov	r2, #0x50
	b	.L1c36
.L1bf4:
	cmp	r3, #0x18
	bne	.L1c08
	mov	r2, #0x70
	neg	r2, r2
	mov	r0, #8
	mov	r1, #0
	bl	OvlFunc_946_2009774
	mov	r2, #0x20
	b	.L1c36
.L1c08:
	mov	r2, #0x50
	neg	r2, r2
	mov	r0, #8
	mov	r1, #0
	bl	OvlFunc_946_2009774
	mov	r2, #0x70
	b	.L1c36
.L1c18:
	cmp	r6, #0xe
	bne	.L1c2c
	cmp	r5, #0x18
	beq	.L1c7a
	cmp	r3, #0x18
	bne	.L1c28
	mov	r2, #0x40
	b	.L1c36
.L1c28:
	mov	r2, #0x70
	b	.L1c36
.L1c2c:
	cmp	r6, #0xa
	bne	.L1c42
	cmp	r3, #0x18
	beq	.L1c7a
	mov	r2, #0x30
.L1c36:
	neg	r2, r2
	mov	r0, #8
	mov	r1, #0
	bl	OvlFunc_946_2009774
	b	.L1c48
.L1c42:
	bl	OvlFunc_946_2009b14
	b	.L1c7a
.L1c48:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	sub	r5, r7, #1
	asr	r3, #20
	str	r3, [sp, #4]
	mov	r0, r5
	mov	r1, r6
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r0, #0
	mov	r1, #0
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010704
.L1c7a:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_2009bbc

@ 72 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x3, OvlFunc_1774 x5, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_2009c84
	push	{r5, r6, lr}
	mov	r0, #8
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #8
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0xc
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r6, #7
	bne	.L1cd0
	cmp	r3, #0x18
	bne	.L1cba
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0x30
	bl	OvlFunc_946_2009774
	b	.L1cf2
.L1cba:
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0x50
	bl	OvlFunc_946_2009774
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0x70
	bl	OvlFunc_946_2009774
	b	.L1cf2
.L1cd0:
	cmp	r6, #0xa
	bne	.L1ce4
	cmp	r3, #0x18
	beq	.L1d24
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0x90
	bl	OvlFunc_946_2009774
	b	.L1cf2
.L1ce4:
	cmp	r6, #0xe
	bne	.L1d24
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0x50
	bl	OvlFunc_946_2009774
.L1cf2:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	sub	r5, #1
	asr	r3, #20
	str	r3, [sp, #4]
	mov	r0, r5
	mov	r1, r6
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r0, #0
	mov	r1, #0
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010704
.L1d24:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_2009c84

@ 80 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x4, OvlFunc_1774 x2, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_2009d2c
	push	{r5, r6, r7, lr}
	mov	r0, #0xa
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xa
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0xd
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xf
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r6, #0x12
	bne	.L1d82
	sub	r3, #0x1f
	cmp	r3, #2
	bhi	.L1d66
	mov	r2, #0x80
	b	.L1d96
.L1d66:
	mov	r3, r5
	sub	r3, #0x1f
	cmp	r3, #2
	bhi	.L1d72
	mov	r2, #0x80
	b	.L1d96
.L1d72:
	mov	r2, #0x70
	neg	r2, r2
	mov	r0, #0xa
	mov	r1, #0
	bl	OvlFunc_946_2009774
	mov	r2, #0x40
	b	.L1d96
.L1d82:
	cmp	r6, #0xa
	bne	.L1da2
	sub	r3, #0x1f
	cmp	r3, #2
	bls	.L1dd8
	mov	r3, r5
	sub	r3, #0x1f
	cmp	r3, #2
	bls	.L1dd8
	mov	r2, #0x30
.L1d96:
	neg	r2, r2
	mov	r0, #0xa
	mov	r1, #0
	bl	OvlFunc_946_2009774
	b	.L1da6
.L1da2:
	cmp	r6, #7
	beq	.L1dd8
.L1da6:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xa
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	sub	r5, r7, #1
	asr	r3, #20
	str	r3, [sp, #4]
	mov	r0, r5
	mov	r1, r6
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r0, #0
	mov	r1, #0
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010704
.L1dd8:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_2009d2c

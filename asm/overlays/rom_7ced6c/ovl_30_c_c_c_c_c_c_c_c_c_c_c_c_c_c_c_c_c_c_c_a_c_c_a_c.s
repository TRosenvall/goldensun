	.include "macros.inc"

@ 114 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x4, OvlFunc_1774 x4, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200a2c8
	push	{r5, r6, r7, lr}
	mov	r0, #0xf
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xf
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0xa
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0xd
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r6, #0x18
	bne	.L233e
	cmp	r5, #7
	beq	.L2300
	cmp	r3, #0x1f
	bne	.L2306
.L2300:
	mov	r0, #0xf
	mov	r1, #0x60
	b	.L237e
.L2306:
	cmp	r3, #0x22
	bne	.L231a
	mov	r0, #0xf
	mov	r1, #0x40
	mov	r2, #0
	bl	OvlFunc_946_2009774
	mov	r0, #0xf
	mov	r1, #0x50
	b	.L237e
.L231a:
	cmp	r3, #0x23
	bne	.L232e
	mov	r0, #0xf
	mov	r1, #0x50
	mov	r2, #0
	bl	OvlFunc_946_2009774
	mov	r0, #0xf
	mov	r1, #0x50
	b	.L237e
.L232e:
	mov	r0, #0xf
	mov	r1, #0x50
	mov	r2, #0
	bl	OvlFunc_946_2009774
	mov	r0, #0xf
	mov	r1, #0x60
	b	.L237e
.L233e:
	cmp	r6, #0x1e
	beq	.L2346
	cmp	r3, #0x1f
	bne	.L2364
.L2346:
	cmp	r5, #7
	beq	.L23bc
	cmp	r3, #0x22
	bne	.L2354
	mov	r0, #0xf
	mov	r1, #0x30
	b	.L237e
.L2354:
	cmp	r3, #0x23
	bne	.L235e
	mov	r0, #0xf
	mov	r1, #0x40
	b	.L237e
.L235e:
	mov	r0, #0xf
	mov	r1, #0x50
	b	.L237e
.L2364:
	cmp	r6, #0x21
	bne	.L2376
	cmp	r3, #0x22
	beq	.L23bc
	cmp	r3, #0x23
	beq	.L237a
	mov	r0, #0xf
	mov	r1, #0x20
	b	.L237e
.L2376:
	cmp	r6, #0x22
	bne	.L2386
.L237a:
	mov	r0, #0xf
	mov	r1, #0x10
.L237e:
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L238a
.L2386:
	cmp	r6, #0x23
	beq	.L23bc
.L238a:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xf
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
.L23bc:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200a2c8

@ 61 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x3, OvlFunc_1774, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200a3c4
	push	{r5, r6, lr}
	mov	r0, #0x11
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0x11
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0x13
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r6, #0x13
	bne	.L23f8
	sub	r3, #3
	cmp	r3, #2
	bhi	.L23f4
	mov	r2, #0x10
	b	.L2404
.L23f4:
	mov	r2, #0x40
	b	.L2404
.L23f8:
	cmp	r6, #0x12
	bne	.L2410
	sub	r3, #3
	cmp	r3, #2
	bls	.L2446
	mov	r2, #0x30
.L2404:
	neg	r2, r2
	mov	r0, #0x11
	mov	r1, #0
	bl	OvlFunc_946_2009774
	b	.L2414
.L2410:
	cmp	r6, #0xf
	beq	.L2446
.L2414:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0x11
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
.L2446:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200a3c4

	.include "macros.inc"

@ 151 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x5, OvlFunc_1774 x3, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200a700
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #9
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #9
	asr	r3, #20
	mov	r8, r3
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0x13
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xe
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0x10
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r2, r3, #20
	cmp	r7, #0x13
	bne	.L2774
	mov	r3, r6
	sub	r3, #9
	cmp	r3, #2
	bhi	.L274c
	mov	r2, #0x10
	b	.L27f6
.L274c:
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2758
	mov	r2, #0x40
	b	.L27f6
.L2758:
	mov	r3, r2
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2764
	mov	r2, #0x70
	b	.L27f6
.L2764:
	mov	r2, #0x50
	neg	r2, r2
	mov	r0, #9
	mov	r1, #0
	bl	OvlFunc_946_2009774
	mov	r2, #0x60
	b	.L27f6
.L2774:
	cmp	r7, #0x12
	bne	.L27a4
	mov	r3, r6
	sub	r3, #9
	cmp	r3, #2
	bls	.L283a
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bls	.L27f4
	mov	r3, r2
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2794
	mov	r2, #0x60
	b	.L27f6
.L2794:
	mov	r2, #0x60
	neg	r2, r2
	mov	r0, #9
	mov	r1, #0
	bl	OvlFunc_946_2009774
	mov	r2, #0x40
	b	.L27f6
.L27a4:
	cmp	r7, #0xf
	bne	.L27bc
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bls	.L283a
	mov	r3, r2
	sub	r3, #9
	cmp	r3, #2
	bls	.L27f4
	mov	r2, #0x70
	b	.L27f6
.L27bc:
	cmp	r7, #0xe
	bne	.L27d8
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bls	.L283a
	mov	r3, r2
	sub	r3, #9
	cmp	r3, #2
	bhi	.L27d4
	mov	r2, #0x20
	b	.L27f6
.L27d4:
	mov	r2, #0x60
	b	.L27f6
.L27d8:
	cmp	r7, #0xc
	bne	.L27e8
	mov	r3, r2
	sub	r3, #9
	cmp	r3, #2
	bls	.L283a
	mov	r2, #0x40
	b	.L27f6
.L27e8:
	cmp	r7, #0xb
	bne	.L2802
	mov	r3, r2
	sub	r3, #9
	cmp	r3, #2
	bls	.L283a
.L27f4:
	mov	r2, #0x30
.L27f6:
	neg	r2, r2
	mov	r0, #9
	mov	r1, #0
	bl	OvlFunc_946_2009774
	b	.L2806
.L2802:
	cmp	r7, #9
	bls	.L283a
.L2806:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #9
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r5, r8
	sub	r5, #1
	asr	r3, #20
	str	r3, [sp, #4]
	mov	r0, r5
	mov	r1, r7
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r0, #0
	mov	r1, #0
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	str	r7, [sp, #4]
	bl	__Func_8010704
.L283a:
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200a700

@ 139 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x5, OvlFunc_1774 x9, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200a848
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #9
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #9
	asr	r3, #20
	mov	r8, r3
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0x13
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xe
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0x10
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r7, #8
	bne	.L28b4
	sub	r3, #9
	cmp	r3, #2
	bls	.L2976
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bls	.L28c8
	mov	r3, r6
	sub	r3, #9
	cmp	r3, #2
	bls	.L28a8
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x50
	bl	OvlFunc_946_2009774
.L28a8:
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x60
	bl	OvlFunc_946_2009774
	b	.L2942
.L28b4:
	cmp	r7, #0xb
	bne	.L28e0
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bls	.L2976
	mov	r3, r6
	sub	r3, #9
	cmp	r3, #2
	bhi	.L28d4
.L28c8:
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x30
	bl	OvlFunc_946_2009774
	b	.L2942
.L28d4:
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x80
	bl	OvlFunc_946_2009774
	b	.L2942
.L28e0:
	cmp	r7, #0xc
	bne	.L290c
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bls	.L2976
	mov	r3, r6
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2900
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x20
	bl	OvlFunc_946_2009774
	b	.L2942
.L2900:
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x70
	bl	OvlFunc_946_2009774
	b	.L2942
.L290c:
	cmp	r7, #0xe
	bne	.L2924
	mov	r3, r6
	sub	r3, #9
	cmp	r3, #2
	bls	.L2976
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x50
	bl	OvlFunc_946_2009774
	b	.L2942
.L2924:
	cmp	r7, #0xf
	bne	.L2934
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x40
	bl	OvlFunc_946_2009774
	b	.L2942
.L2934:
	cmp	r7, #0x12
	bne	.L2942
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x10
	bl	OvlFunc_946_2009774
.L2942:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #9
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r5, r8
	sub	r5, #1
	asr	r3, #20
	str	r3, [sp, #4]
	mov	r0, r5
	mov	r1, r7
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r0, #0
	mov	r1, #0
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	str	r7, [sp, #4]
	bl	__Func_8010704
.L2976:
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200a848

	.include "macros.inc"

@ 93 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x4, OvlFunc_1774, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200ab80
	push	{r5, r6, r7, lr}
	mov	r0, #0xe
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xe
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0x12
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #9
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r6, #0xd
	bne	.L2bca
	sub	r3, #0xc
	cmp	r3, #2
	bhi	.L2bba
	mov	r1, #0x10
	b	.L2c02
.L2bba:
	mov	r3, r5
	sub	r3, #0xc
	cmp	r3, #2
	bhi	.L2bc6
	mov	r1, #0x40
	b	.L2c02
.L2bc6:
	mov	r1, #0x70
	b	.L2c02
.L2bca:
	cmp	r6, #0xc
	bne	.L2be4
	sub	r3, #0xc
	cmp	r3, #2
	bls	.L2c44
	mov	r3, r5
	sub	r3, #0xc
	cmp	r3, #2
	bhi	.L2be0
	mov	r1, #0x30
	b	.L2c02
.L2be0:
	mov	r1, #0x60
	b	.L2c02
.L2be4:
	cmp	r6, #9
	bne	.L2bf4
	mov	r3, r5
	sub	r3, #0xc
	cmp	r3, #2
	bls	.L2c44
	mov	r1, #0x30
	b	.L2c02
.L2bf4:
	cmp	r6, #8
	bne	.L2c0e
	mov	r3, r5
	sub	r3, #0xc
	cmp	r3, #2
	bls	.L2c44
	mov	r1, #0x20
.L2c02:
	neg	r1, r1
	mov	r0, #0xe
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L2c12
.L2c0e:
	cmp	r6, #6
	beq	.L2c44
.L2c12:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xe
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
.L2c44:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200ab80

@ 86 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x4, OvlFunc_1774, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200ac4c
	push	{r5, r6, r7, lr}
	mov	r0, #0xe
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0xe
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0x12
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #9
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r6, #6
	bne	.L2c9c
	sub	r3, #0xc
	cmp	r3, #2
	bhi	.L2c88
	mov	r0, #0xe
	mov	r1, #0x20
	b	.L2cc4
.L2c88:
	mov	r3, r5
	sub	r3, #0xc
	cmp	r3, #2
	bhi	.L2c96
	mov	r0, #0xe
	mov	r1, #0x40
	b	.L2cc4
.L2c96:
	mov	r0, #0xe
	mov	r1, #0x70
	b	.L2cc4
.L2c9c:
	cmp	r6, #8
	bne	.L2cac
	sub	r3, #0xc
	cmp	r3, #2
	bls	.L2d02
	mov	r0, #0xe
	mov	r1, #0x50
	b	.L2cc4
.L2cac:
	cmp	r6, #9
	bne	.L2cbc
	sub	r3, #0xc
	cmp	r3, #2
	bls	.L2d02
	mov	r0, #0xe
	mov	r1, #0x40
	b	.L2cc4
.L2cbc:
	cmp	r6, #0xc
	bne	.L2ccc
	mov	r0, #0xe
	mov	r1, #0x10
.L2cc4:
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L2cd0
.L2ccc:
	cmp	r6, #0xd
	beq	.L2d02
.L2cd0:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xe
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
.L2d02:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200ac4c

@ 89 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x4, OvlFunc_1774, WaitFrames, GetSlotEntityChecked
@   CopyMapRectAttributes x2
.thumb_func_start OvlFunc_946_200ad0c
	push	{r5, r6, r7, lr}
	mov	r0, #0x10
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	mov	r0, #0x10
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #0x12
	asr	r7, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r0, #9
	asr	r5, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r6, #0xd
	bne	.L2d56
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2d46
	mov	r1, #0x10
	b	.L2d86
.L2d46:
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2d52
	mov	r1, #0x40
	b	.L2d86
.L2d52:
	mov	r1, #0x70
	b	.L2d86
.L2d56:
	cmp	r6, #0xc
	bne	.L2d70
	sub	r3, #9
	cmp	r3, #2
	bls	.L2dc8
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bhi	.L2d6c
	mov	r1, #0x30
	b	.L2d86
.L2d6c:
	mov	r1, #0x60
	b	.L2d86
.L2d70:
	cmp	r6, #9
	bne	.L2d80
	mov	r3, r5
	sub	r3, #9
	cmp	r3, #2
	bls	.L2dc8
	mov	r1, #0x30
	b	.L2d86
.L2d80:
	cmp	r6, #8
	bne	.L2d92
	mov	r1, #0x20
.L2d86:
	neg	r1, r1
	mov	r0, #0x10
	mov	r2, #0
	bl	OvlFunc_946_2009774
	b	.L2d96
.L2d92:
	cmp	r6, #6
	beq	.L2dc8
.L2d96:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0x10
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
.L2dc8:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_946_200ad0c

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

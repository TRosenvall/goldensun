	.include "macros.inc"

@ 98 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectAttributes, OvlFunc_1050, CopyMapRectAttributes, OvlFunc_1154
@   CopyMapRectAttributes, OvlFunc_12cc, CopyMapRectAttributes x2, OvlFunc_15dc
.thumb_func_start OvlFunc_922_2009948
	push	{lr}
	ldr	r1, =gState
	mov	r0, #0xe0
	lsl	r0, #1
	add	r3, r1, r0
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r3, =0x3e
	sub	sp, #8
	cmp	r2, r3
	bne	.L1978
	mov	r3, #8
	mov	r2, #0x2a
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #8
	mov	r1, #0x1d
	mov	r2, #0xf
	mov	r3, #5
	bl	__Func_8010704
	bl	OvlFunc_922_2009050
	b	.L1a18
.L1978:
	ldr	r3, =0x3f
	cmp	r2, r3
	bne	.L1998
	mov	r3, #0
	mov	r2, #0x1c
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xc
	mov	r1, #8
	mov	r2, #0xa
	mov	r3, #0x12
	bl	__Func_8010704
	bl	OvlFunc_922_2009154
	b	.L1a18
.L1998:
	ldr	r3, =0x40
	cmp	r2, r3
	bne	.L19c6
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r1, r2
	mov	r0, #0
	ldrsh	r3, [r3, r0]
	cmp	r3, #1
	beq	.L19c6
	mov	r3, #0xc
	mov	r2, #3
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xc
	mov	r1, #0x15
	mov	r2, #9
	mov	r3, #0x10
	bl	__Func_8010704
	bl	OvlFunc_922_20092cc
	b	.L1a18
.L19c6:
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r1, r2
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r3, =0x41
	cmp	r2, r3
	bne	.L1a18
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r1, r2
	ldrh	r3, [r3]
	mov	r0, #0x80
	sub	r3, #1
	lsl	r3, #16
	lsl	r0, #9
	cmp	r3, r0
	bhi	.L1a00
	mov	r3, #0x16
	mov	r2, #0x14
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0xa
	mov	r2, #9
	mov	r3, #8
	bl	__Func_8010704
	b	.L1a14
.L1a00:
	mov	r3, #0x14
	mov	r2, #0x2d
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #7
	mov	r1, #0x2d
	mov	r2, #0xb
	mov	r3, #4
	bl	__Func_8010704
.L1a14:
	bl	OvlFunc_922_20095dc
.L1a18:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_922_2009948

@ 47 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_1ad0 x2, TestSaveBit, OvlFunc_1ad0
@ reads save bit 0x309.
.thumb_func_start OvlFunc_922_2009a34
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r1, #0xb6
	ldr	r3, [r3]
	lsl	r1, #1
	ldr	r5, =gState
	add	r3, r1
	add	r1, #0x54
	mov	r2, #0
	ldrsh	r6, [r3, r2]
	add	r3, r5, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x3f
	cmp	r2, r3
	bne	.L1a6e
	cmp	r6, #0x11
	bne	.L1a64
	mov	r1, #0x20
	neg	r1, r1
	mov	r0, #0
	bl	OvlFunc_922_2009ad0
	b	.L1a6e
.L1a64:
	mov	r0, #0x20
	neg	r0, r0
	mov	r1, #0
	bl	OvlFunc_922_2009ad0
.L1a6e:
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r5, r2
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x40
	cmp	r2, r3
	bne	.L1a94
	cmp	r6, #0x19
	bne	.L1a94
	ldr	r0, =0x309
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1a94
	mov	r0, #0
	mov	r1, #0x20
	bl	OvlFunc_922_2009ad0
.L1a94:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_922_2009a34

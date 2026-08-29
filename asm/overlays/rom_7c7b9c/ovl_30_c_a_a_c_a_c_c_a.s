	.include "macros.inc"

@ 73 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_3150, OvlFunc_3464, OvlFunc_3150, OvlFunc_3464
@   OvlFunc_3150, OvlFunc_3464, OvlFunc_3150, OvlFunc_3464
@   OvlFunc_3150, OvlFunc_3464
.thumb_func_start OvlFunc_943_200b3b8
	push	{r5, r6, r7, lr}
	ldr	r6, =.L5b70
	mov	r5, #0
	mov	r7, #3
.L33c0:
	mov	r0, r5
	bl	OvlFunc_943_200b150
	cmp	r0, #0
	beq	.L33d4
	mov	r0, r5
	bl	OvlFunc_943_200b464
	str	r0, [r6]
	b	.L33d6
.L33d4:
	str	r7, [r6]
.L33d6:
	add	r5, #1
	add	r6, #4
	cmp	r5, #3
	bls	.L33c0
	mov	r0, #0
	bl	OvlFunc_943_200b150
	cmp	r0, #0
	beq	.L33f4
	mov	r0, #0
	bl	OvlFunc_943_200b464
	ldr	r3, =.L5b70
	str	r0, [r3]
	b	.L33fa
.L33f4:
	ldr	r2, =.L5b70
	mov	r3, #3
	str	r3, [r2]
.L33fa:
	mov	r0, #2
	bl	OvlFunc_943_200b150
	cmp	r0, #0
	beq	.L3410
	mov	r0, #2
	bl	OvlFunc_943_200b464
	ldr	r3, =.L5b70
	str	r0, [r3, #4]
	b	.L3416
.L3410:
	ldr	r2, =.L5b70
	mov	r3, #3
	str	r3, [r2, #4]
.L3416:
	ldr	r6, =.L5b70
	mov	r5, #3
	str	r5, [r6, #8]
	str	r5, [r6, #0xc]
	mov	r0, #1
	bl	OvlFunc_943_200b150
	cmp	r0, #0
	beq	.L3432
	mov	r0, #1
	bl	OvlFunc_943_200b464
	str	r0, [r6, #0x10]
	b	.L3434
.L3432:
	str	r5, [r6, #0x10]
.L3434:
	mov	r0, #3
	bl	OvlFunc_943_200b150
	cmp	r0, #0
	beq	.L344a
	mov	r0, #3
	bl	OvlFunc_943_200b464
	ldr	r3, =.L5b70
	str	r0, [r3, #0x14]
	b	.L3450
.L344a:
	ldr	r2, =.L5b70
	mov	r3, #3
	str	r3, [r2, #0x14]
.L3450:
	ldr	r2, =.L5b70
	mov	r3, #3
	str	r3, [r2, #0x18]
	str	r3, [r2, #0x1c]
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_200b3b8

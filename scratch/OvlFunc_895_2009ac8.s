	.include "macros.inc"

.thumb_func_start OvlFunc_895_2009ac8
	push	{r5, lr}
	ldr	r5, =.L269c
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.L1aea
	sub	r3, #1
	str	r3, [r5]
	cmp	r3, #0x28
	bne	.L1b14
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_8012330
	b	.L1b14
.L1aea:
	bl	__Random
	lsl	r3, r0, #4
	sub	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	cmp	r3, #0
	bne	.L1b14
	mov	r0, #0x8a
	bl	__PlaySound
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #9
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_8012330
	mov	r3, #0x50
	str	r3, [r5]
.L1b14:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_895_2009ac8

	.include "macros.inc"

@ ResetNodeAnimation
@ r0 = node, or 0 for a no-op. Marks the node live (+0x05 = 1), copies its
@ resource id from +0x06 into the low 9 bits of +0x16, restarts the frame
@ counter by copying +0x08 to +0x14, and clears the top two bits of +0x17 and
@ the low two of +0x15. Called 43 times across the module, always to rewind a
@ sprite that is about to be reused rather than to build a new one.
.thumb_func_start Func_80a17c4  @ 0x080a17c4
	push	{lr}
	cmp	r0, #0
	beq	.La17f6
	mov	r3, #1
	strb	r3, [r0, #5]
	ldr	r2, =0x1ff
	ldrh	r3, [r0, #6]
	ldrh	r1, [r0, #0x16]
	and	r2, r3
	ldr	r3, =0xfffffe00
	and	r3, r1
	orr	r3, r2
	strh	r3, [r0, #0x16]
	ldrh	r3, [r0, #8]
	ldrb	r2, [r0, #0x17]
	strb	r3, [r0, #0x14]
	mov	r3, #0x3f
	neg	r3, r3
	and	r3, r2
	strb	r3, [r0, #0x17]
	ldrb	r2, [r0, #0x15]
	mov	r3, #4
	neg	r3, r3
	and	r3, r2
	strb	r3, [r0, #0x15]
.La17f6:
	pop	{r0}
	bx	r0
.func_end Func_80a17c4


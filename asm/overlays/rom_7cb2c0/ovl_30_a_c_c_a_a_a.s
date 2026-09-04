	.include "macros.inc"

@ Leaf helper, 24 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Writes offsets +0x4c.
.thumb_func_start OvlFunc_945_20080fc
	push	{lr}
	ldr	r3, [r0, #0x4c]
	cmp	r3, #0
	beq	.L10a
	sub	r3, #1
	str	r3, [r0, #0x4c]
	b	.L10e
.L10a:
	mov	r0, #1
	b	.L128
.L10e:
	mov	r2, #0x80
	ldr	r3, [r0, #0x38]
	lsl	r2, #24
	cmp	r3, r2
	bne	.L126
	ldr	r2, [r0, #0x3c]
	cmp	r2, r3
	bne	.L126
	ldr	r3, [r0, #0x40]
	mov	r0, #1
	cmp	r3, r2
	beq	.L128
.L126:
	mov	r0, #0
.L128:
	pop	{r1}
	bx	r1
.func_end OvlFunc_945_20080fc

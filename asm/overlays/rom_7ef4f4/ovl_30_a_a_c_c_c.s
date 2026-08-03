	.include "macros.inc"

@ Leaf helper, 14 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_965_2008fac
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xb0
	mov	r0, #0
	cmp	r2, r3
	bne	.Lfc4
	ldr	r0, =.L35b8
.Lfc4:
	pop	{r1}
	bx	r1
.func_end OvlFunc_965_2008fac

	.include "macros.inc"

@ Leaf helper, 14 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_911_20081ac
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x26
	mov	r0, #0
	cmp	r2, r3
	bne	.L1c4
	ldr	r0, =.L3010
.L1c4:
	pop	{r1}
	bx	r1
.func_end OvlFunc_911_20081ac

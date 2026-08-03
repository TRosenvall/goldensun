	.include "macros.inc"

@ Leaf helper, 14 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_907_20080dc
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x20
	mov	r0, #0
	cmp	r2, r3
	bne	.Lf4
	ldr	r0, =gOvl_020093fc
.Lf4:
	pop	{r1}
	bx	r1
.func_end OvlFunc_907_20080dc

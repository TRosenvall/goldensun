	.include "macros.inc"
	.include "gba.inc"

@ Leaf helper, 19 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_932_20081c8
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x55
	cmp	r2, r3
	bne	.L1e0
	ldr	r0, =gScript_943__0200c80c
	b	.L1ea
.L1e0:
	ldr	r3, =0x56
	mov	r0, #0
	cmp	r2, r3
	bne	.L1ea
	ldr	r0, =gOvl_0200c83c
.L1ea:
	pop	{r1}
	bx	r1
.func_end OvlFunc_932_20081c8

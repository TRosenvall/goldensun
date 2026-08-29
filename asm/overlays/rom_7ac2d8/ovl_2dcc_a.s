	.include "macros.inc"

@ Leaf helper, 22 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1e40
.thumb_func_start OvlFunc_924_200adcc
	push	{lr}
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #7
	and	r3, r2
	cmp	r3, #0
	bne	.L2df4
	ldr	r1, =0x50000c2
	ldr	r3, =0x50000ce
	ldrh	r2, [r1]
	strh	r2, [r3]
	ldr	r2, =0x50000c4
	mov	r0, #0
.L2de6:
	ldrh	r3, [r2]
	add	r0, #1
	strh	r3, [r1]
	add	r2, #2
	add	r1, #2
	cmp	r0, #5
	bls	.L2de6
.L2df4:
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_200adcc

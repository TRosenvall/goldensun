	.include "macros.inc"

@ Leaf helper, 31 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1e70
@ Writes offsets +0x8, +0xc, +0x1e.
.thumb_func_start OvlFunc_944_2008030
	push	{r5, lr}
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	ldr	r3, [r3]
	ldr	r4, =.L1938
	ldmia	r3!, {r1}
	ldr	r5, =.L1930
	ldr	r2, [r3]
	ldr	r3, [r4]
	sub	r1, r3
	ldr	r3, [r5]
	add	r3, r1
	str	r3, [r0, #8]
	ldr	r3, [r4, #4]
	sub	r2, r3
	lsr	r3, r2, #31
	add	r2, r3
	ldr	r3, [r5, #4]
	asr	r2, #1
	add	r3, r2
	str	r3, [r0, #0xc]
	ldr	r2, [r0, #0x50]
	mov	r1, #0xc0
	ldrh	r3, [r2, #0x1e]
	lsl	r1, #3
	add	r3, r1
	strh	r3, [r2, #0x1e]
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_944_2008030


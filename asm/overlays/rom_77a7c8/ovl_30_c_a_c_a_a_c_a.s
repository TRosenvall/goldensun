	.include "macros.inc"
	.include "gba.inc"

@ Leaf helper, 13 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1e70
@ Writes offsets +0x1e.
.thumb_func_start OvlFunc_881_20082cc
	ldr	r3, =iwram_3001e70
	mov	r1, #0x8d
	ldr	r2, [r3]
	lsl	r1, #1
	add	r2, r1
	ldr	r3, [r0, #0x50]
	ldrh	r2, [r2]
	ldr	r1, .L2e8	@ 0
	strh	r2, [r3, #0x1e]
	add	r3, #0x26
	strb	r1, [r3]
	mov	r0, #1
	bx	lr

	.align	2, 0
.L2e8:
	.word	0
.func_end OvlFunc_881_20082cc

@ Leaf helper, 15 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1e70
@ Writes offsets +0x1e.
.thumb_func_start OvlFunc_881_20082f0
	ldr	r3, =iwram_3001e70
	ldr	r4, [r0, #0x50]
	add	r0, #0x59
	ldrb	r2, [r0]
	ldr	r1, [r3]
	mov	r3, #1
	orr	r3, r2
	mov	r2, #0x8d
	lsl	r2, #1
	strb	r3, [r0]
	add	r3, r1, r2
	ldrh	r3, [r3]
	mov	r0, #1
	strh	r3, [r4, #0x1e]
	bx	lr
.func_end OvlFunc_881_20082f0

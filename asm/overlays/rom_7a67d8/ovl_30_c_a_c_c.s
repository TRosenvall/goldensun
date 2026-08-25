	.include "macros.inc"
	.include "gba.inc"

@ UpdateRasterSplit
@ The per-frame task that feeds OvlFunc_26c. Reads the camera record at
@ [iwram_1e70]+0x104: the halfword at +6 gives the horizon, stored as
@ .L610 = 0xC0 - that, and the halfword at +2 becomes the lower scroll .L614.
@ The upper scroll .L616 is that same value minus [iwram_1e40] >> 2, so the far
@ layer moves at a quarter rate -- the parallax.
.thumb_func_start OvlFunc_919_20082a0
	ldr	r3, =iwram_3001e70
	mov	r1, #0x82
	ldr	r2, [r3]
	lsl	r1, #1
	add	r2, r1
	mov	r3, #6
	ldrsh	r1, [r2, r3]
	ldr	r0, =.L610
	mov	r3, #0xc0
	sub	r3, r1
	str	r3, [r0]
	ldr	r3, =.L614
	mov	r1, #2
	ldrsh	r2, [r2, r1]
	strh	r2, [r3]
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	ldr	r1, =.L616
	lsr	r3, #2
	sub	r2, r3
	strh	r2, [r1]
	bx	lr
.func_end OvlFunc_919_20082a0

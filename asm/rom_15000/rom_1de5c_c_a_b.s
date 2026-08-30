	.include "macros.inc"
	.include "gba.inc"

@ SetTextInk
@ r0 = colour. Masks to 4 bits and stores at [iwram_1e8c]+0xEAE -- the ink
@ field Func_173ac defaults to 0x0F.
.thumb_func_start SetTextColor  @ 0x0801e71c
	ldr	r3, =iwram_3001e8c
	ldr	r2, .L1e72c	@ 0xf
	ldr	r3, [r3]
	and	r0, r2
	ldr	r2, =0xeae
	add	r3, r2
	strh	r0, [r3]
	bx	lr
	.align	2, 0
.L1e72c:
	.word	0xf
.func_end SetTextColor

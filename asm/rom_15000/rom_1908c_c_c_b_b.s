	.include "macros.inc"
	.include "gba.inc"

@ InitMenuLayer
@ Takes no arguments. Called from Func_15f30 during UI bring-up. Sets the two
@ halfwords at [iwram_1e8c]+0x12EC and +0x12EE to 0x3E7 (999) -- sentinel values
@ meaning "no selection", since real indices are small.
.thumb_func_start Func_8019d0c  @ 0x08019d0c
	ldr	r3, =iwram_3001e8c
	ldr	r0, =0x12ec
	ldr	r3, [r3]
	ldr	r2, .L19d20	@ 0x3e7
	add	r1, r3, r0
	add	r0, #2
	strh	r2, [r1]
	add	r1, r3, r0
	strh	r2, [r1]
	bx	lr
	.align	2, 0
.L19d20:
	.word	0x3e7
.func_end Func_8019d0c

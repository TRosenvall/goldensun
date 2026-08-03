	.include "macros.inc"

@ Leaf helper, 25 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_935_20080e0
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x60
	cmp	r2, r3
	bne	.Lf8
	ldr	r0, =.L1d34
	b	.L10e
.Lf8:
	ldr	r3, =0x61
	cmp	r2, r3
	bne	.L102
	ldr	r0, =.L1d4c
	b	.L10e
.L102:
	ldr	r3, =0x62
	cmp	r2, r3
	bne	.L10c
	ldr	r0, =gScript_887__02009ecc
	b	.L10e
.L10c:
	ldr	r0, =.L1d1c
.L10e:
	pop	{r1}
	bx	r1
.func_end OvlFunc_935_20080e0

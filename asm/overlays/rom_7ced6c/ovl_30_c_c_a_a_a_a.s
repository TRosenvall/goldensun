	.include "macros.inc"

@ Leaf helper, 28 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_946_2008d48
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x71
	cmp	r2, r3
	bne	.Ld60
	ldr	r0, =gScript_911__0200b610
	b	.Ld7c
.Ld60:
	ldr	r3, =0x7b
	cmp	r2, r3
	bne	.Ld6a
	ldr	r0, =.L3718
	b	.Ld7c
.Ld6a:
	ldr	r3, =0x86
	cmp	r2, r3
	bgt	.Ld7a
	ldr	r3, =0x7e
	cmp	r2, r3
	blt	.Ld7a
	ldr	r0, =.L3850
	b	.Ld7c
.Ld7a:
	ldr	r0, =gOvl_0200b5f8
.Ld7c:
	pop	{r1}
	bx	r1
.func_end OvlFunc_946_2008d48

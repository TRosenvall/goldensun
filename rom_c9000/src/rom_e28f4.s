	.include "macros.inc"
	.include "gba.inc"

@ Sub_e28f4
@ Battle animation routine, 16 instructions.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Func_e28f4
	push	{lr}
	ldr	r3, [r0, #0x18]
	cmp	r3, #0
	bne	.Le2904
	mov	r1, #6
	bl	Func_e2974
	b	.Le2916
.Le2904:
	cmp	r3, #1
	bne	.Le2910
	mov	r1, #7
	bl	Func_e2974
	b	.Le2916
.Le2910:
	mov	r1, #8
	bl	Func_e2974
.Le2916:
	pop	{r0}
	bx	r0
.func_end Func_e28f4

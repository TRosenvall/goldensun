	.include "macros.inc"
	.include "gba.inc"

@ Sub_d5258
@ Battle animation routine, 11 instructions.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Func_d5258
	push	{lr}
	ldr	r3, [r0, #0x18]
	cmp	r3, #0
	bne	.Ld5268
	mov	r1, #0
	bl	Func_d52c8
	b	.Ld526e
.Ld5268:
	mov	r1, #1
	bl	Func_d52c8
.Ld526e:
	pop	{r0}
	bx	r0
.func_end Func_d5258

	.include "macros.inc"
	.include "gba.inc"

@ SetSoundFlag
@ r0.. = parameters. Reads and writes the flag word at ewram_3040. No calls out.
.thumb_func_start Func_f9570
	push	{lr}
	mov	r3, #0x80
	and	r3, r0
	mov	r2, #0x7f
	and	r0, r2
	cmp	r3, #0
	beq	.Lf9588
	ldr	r2, =ewram_3040
	ldrb	r3, [r2]
	eor	r0, r3
	strb	r0, [r2]
	b	.Lf958c
.Lf9588:
	ldr	r3, =ewram_3040
	strb	r0, [r3]
.Lf958c:
	pop	{r0}
	bx	r0
.func_end Func_f9570

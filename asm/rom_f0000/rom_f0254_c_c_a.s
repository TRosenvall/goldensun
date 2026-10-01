	.include "macros.inc"
	.include "gba.inc"

@ FeedNextCreditLine
@ The per-frame task Func_f0678 registers at sort key 0xC80. Watches the scroll
@ position and, whenever it crosses into a new 8-pixel row, renders the next
@ credit line into the row that has just scrolled off the top.
@
@ The line index is `(row + 0x10) & 0x1F`, so the text bitmap is a 32-row ring
@ buffer half a screen ahead of what is showing. The string comes from the
@ pointer table .Lf1220 and goes through Func_f07f0 with alignment 1 (centred).
@ Does nothing while ewram_4c04 is set.
.thumb_func_start Func_80f0614  @ 0x080f0614
	push	{r5, r6, lr}
	ldr	r5, =ewram_2004c04
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	bne	.Lf0662
	ldr	r3, =ewram_2004c00
	mov	r6, #0
	ldrsh	r2, [r3, r6]
	ldrh	r4, [r3]
	mov	r3, r2
	cmp	r2, #0
	bge	.Lf0630
	add	r3, r2, #7
.Lf0630:
	ldr	r0, =ewram_2004c08
	asr	r1, r3, #3
	mov	r6, #0
	ldrsh	r3, [r0, r6]
	cmp	r3, #0
	bge	.Lf063e
	add	r3, #7
.Lf063e:
	asr	r3, #3
	cmp	r1, r3
	beq	.Lf0662
	ldr	r2, =.Lf1220
	lsl	r3, r1, #2
	strh	r4, [r0]
	ldr	r0, [r2, r3]
	mov	r3, r1
	mov	r2, #0x1f
	add	r3, #0x10
	and	r3, r2
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #3
	mov	r2, #1
	bl	Func_80f07f0
	strh	r0, [r5]
.Lf0662:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80f0614

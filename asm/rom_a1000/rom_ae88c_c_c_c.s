	.include "macros.inc"

@ AttachArrowSprite
@ r0 = x, r1 = y, r2 = priority, r3 = direction -- 0 up, non-zero down.
@ Attaches the arrow whose tiles .gcc2_compiled. loaded, picking state+0x392 for up
@ and +0x394 for down. Clears the node's +0x04 and +0x0C and marks it live.
@ Returns 1, or -1 when no node was free. This is the arrow Func_a15f0 puts
@ beside a stat that changes.
.thumb_func_start Func_80ae99c  @ 0x080ae99c
	push	{r5, r6, lr}
	mov	r5, r3
	ldr	r3, =iwram_3001f2c
	sub	sp, #4
	mov	r4, r0
	mov	r6, r1
	ldr	r3, [r3]
	cmp	r5, #0
	bne	.Lae9b2
	ldr	r1, =0x392
	b	.Lae9b6
.Lae9b2:
	mov	r1, #0xe5
	lsl	r1, #2
.Lae9b6:
	add	r3, r1
	ldrh	r0, [r3]
	mov	r1, #0x80
	str	r2, [sp]
	lsl	r1, #23
	mov	r2, r4
	mov	r3, r6
	bl	_Func_801eadc
	cmp	r0, #0
	bne	.Lae9d2
	mov	r0, #1
	neg	r0, r0
	b	.Lae9de
.Lae9d2:
	mov	r3, #0
	strb	r3, [r0, #4]
	strh	r3, [r0, #0xc]
	mov	r3, #1
	strb	r3, [r0, #5]
	mov	r0, #1
.Lae9de:
	add	sp, #4
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80ae99c

@ AttachArrowSpriteNudged
@ r0 = x, r1 = y, r2 = priority, r3 = direction. As Func_ae99c but with the
@ priority biased -- 3 lower for the up arrow, 4 for the down -- so the arrow
@ sorts behind the text it annotates rather than over it.
.thumb_func_start Func_80ae9f0  @ 0x080ae9f0
	push	{r5, r6, lr}
	mov	r5, r3
	ldr	r3, =iwram_3001f2c
	sub	sp, #4
	mov	r4, r0
	mov	r6, r1
	ldr	r3, [r3]
	cmp	r5, #0
	bne	.Laea0c
	ldr	r1, =0x392
	add	r3, r1
	ldrh	r0, [r3]
	sub	r2, #3
	b	.Laea16
.Laea0c:
	mov	r1, #0xe5
	lsl	r1, #2
	add	r3, r1
	ldrh	r0, [r3]
	sub	r2, #4
.Laea16:
	mov	r1, #0x80
	str	r2, [sp]
	lsl	r1, #23
	mov	r2, r4
	mov	r3, r6
	bl	_Func_801eadc
	cmp	r0, #0
	bne	.Laea2e
	mov	r0, #1
	neg	r0, r0
	b	.Laea3a
.Laea2e:
	mov	r3, #0
	strb	r3, [r0, #4]
	strh	r3, [r0, #0xc]
	mov	r3, #1
	strb	r3, [r0, #5]
	mov	r0, #1
.Laea3a:
	add	sp, #4
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80ae9f0

	.section .rodata
	.global .Laed4c
	.global .Laedcc

.Laed4c:
	.incrom 0xaed4c, 0xaedcc
.Laedcc:
	.incrom 0xaedcc, 0xaf08c

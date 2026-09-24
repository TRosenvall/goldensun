	.include "macros.inc"

@ WaitForAnimationEnd
@ r0=slot, r1=animation index. Blocks while the slot's actor is still playing
@ animation r1, checking the actor's current-animation byte at +0x24 once per
@ frame. Gives up after 0x5A (90) frames, and returns immediately if the slot is
@ empty or not draw kind 1.
.thumb_func_start MapActor_WaitAnim  @ 0x08091c44
	push	{r5, r6, r7, lr}
	mov	r7, r1
	bl	GetFieldActor
	cmp	r0, #0
	beq	.L91c76
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L91c76
	ldr	r3, [r0, #0x50]
	mov	r6, r3
	mov	r5, #0
	add	r6, #0x24
	b	.L91c66
.L91c64:
	add	r5, #1
.L91c66:
	cmp	r5, #0x59
	bgt	.L91c76
	mov	r0, #1
	bl	WaitFrames
	ldrb	r3, [r6]
	cmp	r7, r3
	beq	.L91c64
.L91c76:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end MapActor_WaitAnim

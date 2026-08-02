	.include "macros.inc"

@ WaitForSlotAnimationChange
@ r0=slot. Samples the actor's current-animation byte at +0x24 and blocks until
@ it changes, checking once per frame and giving up after 0x5A (90) frames.
@ Where MapActor_WaitAnim waits for a specific animation to end, this waits for
@ whatever is playing now to be replaced.
.thumb_func_start Func_8092504  @ 0x08092504
	push	{r5, r6, r7, lr}
	sub	sp, #4
	bl	GetFieldActor
	cmp	r0, #0
	beq	.L9253e
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L9253e
	ldr	r3, [r0, #0x50]
	mov	r6, r3
	add	r6, #0x24
	ldrb	r3, [r6]
	mov	r7, sp
	mov	r5, #0
	str	r3, [r7]
	b	.L9252c
.L9252a:
	add	r5, #1
.L9252c:
	cmp	r5, #0x59
	bgt	.L9253e
	mov	r0, #1
	bl	WaitFrames
	ldrb	r2, [r6]
	ldr	r3, [r7]
	cmp	r3, r2
	beq	.L9252a
.L9253e:
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8092504


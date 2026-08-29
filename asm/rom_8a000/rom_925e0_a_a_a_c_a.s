	.include "macros.inc"

@ SpawnSparkleParticle
@ r0=source entity, r1=non-zero to also run Func_929d8 on the new particle.
@ Spawns entity type 0xDE at the source's position with _Func_c150 and gives it
@ a randomised look and trajectory:
@   - a coin flip from Func_4458 picks animation 1 with script .L9fc04 or
@     animation 2 with script .L9fbec
@   - the movement mode byte at +0x55 is cleared so the mover leaves it alone
@   - vz at +0x34 is -(rand % 10 + 5) * 0x1CD, vx at +0x30 is
@     (rand % 15 - 7) * 2 * 0x1999, so it fans out sideways and rises
@   - Func_925e0 is installed as the hook at +0x6C
@ Finally the particle's palette bits (2-3 of actor byte +0x09) are copied from
@ the source actor so the sparkle tints to match whatever emitted it.
.thumb_func_start Func_8092624  @ 0x08092624
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r8, r1
	ldr	r2, [r7, #0xc]
	ldr	r1, [r7, #8]
	ldr	r3, [r7, #0x10]
	mov	r0, #0xde
	bl	_CreateActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L926fc
	ldr	r5, [r6, #0x50]
	bl	Random
	mov	r3, #1
	and	r0, r3
	cmp	r0, #1
	bne	.L92664
	mov	r0, r6
	mov	r1, #2
	bl	_Actor_SetAnim
	ldr	r1, =.L9fbec
	mov	r0, r6
	bl	_Actor_SetScript
	b	.L92674

	.pool_aligned

.L92664:
	mov	r0, r6
	mov	r1, #1
	bl	_Actor_SetAnim
	ldr	r1, =.L9fc04
	mov	r0, r6
	bl	_Actor_SetScript
.L92674:
	mov	r1, r8
	cmp	r1, #0
	beq	.L92680
	mov	r0, r6
	bl	Func_80929d8
.L92680:
	mov	r2, #0
	mov	r8, r2
	mov	r3, r6
	add	r3, #0x55
	mov	r1, r8
	strb	r1, [r3]
	bl	Random
	mov	r1, #0xa
	bl	__umodsi3
	add	r0, #5
	lsl	r2, r0, #1
	add	r2, r0
	lsl	r2, #2
	add	r2, r0
	lsl	r3, r2, #6
	sub	r3, r2
	lsl	r3, #3
	add	r3, r0
	neg	r3, r3
	str	r3, [r6, #0x34]
	bl	Random
	mov	r1, #0xf
	bl	__umodsi3
	ldr	r3, =0x1999
	sub	r0, #7
	lsl	r0, #1
	mul	r3, r0
	str	r3, [r6, #0x30]
	mov	r3, r6
	add	r3, #0x64
	mov	r1, r8
	strh	r1, [r3]
	ldr	r3, =Func_80925e0
	ldr	r2, .L926ec	@ 0
	str	r3, [r6, #0x6c]
	mov	r3, r5
	add	r3, #0x26
	strb	r2, [r3]
	ldr	r3, [r7, #0x50]
	ldrb	r3, [r3, #9]
	mov	r2, #0xc
	and	r2, r3
	ldrb	r1, [r5, #9]
	mov	r3, #0xd
	neg	r3, r3
	and	r3, r1
	orr	r3, r2
	strb	r3, [r5, #9]
	b	.L926fc

	.align	2, 0
.L926ec:
	.word	0
	.pool

.L926fc:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8092624

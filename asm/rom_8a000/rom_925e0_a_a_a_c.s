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

@ PlayLevitateSequence
@ r0=entity id, r1=animation index, r2=sparkle flag (-1 disables sparkles).
@ Blocking cutscene: lifts the entity into the air, holds it, and sets it back
@ down. Does nothing if the id does not resolve.
@   - sound 0x121, switch to animation r1, wait 10 frames
@   - switch to animation 1, set bit 1 of the mode byte at +0x55, give +0x28 an
@     upward velocity of 0x40000 and aim _Func_d14c at y - 0xC0000
@   - wait 6 frames, sound 0xD9, then .gcc2_compiled. for the lift effect
@   - 14 frames of rising 0x20000 per frame, spawning a sparkle through
@     Func_92624 on every other frame when the flag allows
@   - set mode 3, drop the velocity to 0x30000 and aim at y + 0x100000, then
@     block on _Func_ca6c until the descent finishes
@   - wait (up to 180 frames) until y falls back to the ground height at +0x14,
@     pause 2 frames and finish with .gcc2_compiled.
.thumb_func_start Func_8092708  @ 0x08092708
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r8, r2
	mov	r10, r0
	mov	r5, r1
	bl	MapActor_GetActor
	mov	r6, r0
	ldr	r2, [r6, #0x10]
	mov	r9, r2
	cmp	r6, #0
	beq	.L927f6
	ldr	r0, =0x121
	bl	_PlaySound
	mov	r1, r5
	mov	r0, r6
	bl	_Actor_SetAnim
	mov	r7, r6
	mov	r0, #0xa
	bl	WaitFrames
	add	r7, #0x55
	mov	r0, r6
	mov	r1, #1
	bl	_Actor_SetAnim
	ldrb	r2, [r7]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r7]
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r6, #0x28]
	mov	r3, #0xc0
	lsl	r3, #12
	ldr	r1, [r6, #8]
	ldr	r2, [r6, #0xc]
	add	r3, r9
	mov	r0, r6
	bl	_Actor_TravelTo
	mov	r0, #6
	bl	WaitFrames
	mov	r0, #0xd9
	bl	_PlaySound
	mov	r1, #0xa0
	mov	r5, #0
	lsl	r1, #7
	mov	r0, r10
	mov	r2, #0
	bl	Func_8092adc
	strb	r5, [r7]
.L92780:
	ldr	r3, [r6, #0xc]
	ldr	r2, =0xfffe0000
	add	r3, r2
	str	r3, [r6, #0xc]
	str	r3, [r6, #0x3c]
	mov	r0, #1
	bl	WaitFrames
	mov	r3, #1
	neg	r3, r3
	cmp	r8, r3
	beq	.L927a8
	mov	r3, #1
	and	r3, r5
	cmp	r3, #0
	beq	.L927a8
	mov	r0, r6
	mov	r1, r8
	bl	Func_8092624
.L927a8:
	add	r5, #1
	cmp	r5, #0xd
	bls	.L92780
	mov	r3, #3
	strb	r3, [r7]
	mov	r3, #0xc0
	lsl	r3, #10
	str	r3, [r6, #0x28]
	mov	r3, #0x80
	lsl	r3, #13
	ldr	r2, [r6, #0xc]
	add	r3, r9
	ldr	r1, [r6, #8]
	mov	r0, r6
	bl	_Actor_TravelTo
	mov	r0, r6
	bl	_Actor_WaitMovement
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	mov	r5, #0
	cmp	r2, r3
	ble	.L927ec
.L927d8:
	mov	r0, #1
	add	r5, #1
	bl	WaitFrames
	cmp	r5, #0xb3
	bhi	.L927ec
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	cmp	r2, r3
	bgt	.L927d8
.L927ec:
	mov	r0, #2
	bl	WaitFrames
	bl	Func_809202c
.L927f6:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8092708

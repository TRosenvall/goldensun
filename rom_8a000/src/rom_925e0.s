	.include "macros.inc"

@ ParticleArcHook
@ r0=particle entity. Installed as the per-frame hook at +0x6C by Func_92624.
@ Moves the particle on a decaying ballistic arc without going through the
@ normal seek logic -- it repurposes +0x30 and +0x34 as its own x and z velocity
@ and writes the new position into both the position words (+0x08/+0x0C/+0x10)
@ and the target words (+0x38/+0x3C/+0x40) so the mover has nothing left to do.
@ Each frame y falls by 0x400, x advances by vx and z by vz, then vx loses
@ 1/18th of itself (via Func_af0_from_thumb) and vz loses 1/16th, giving the
@ drift a soft ease-out.
.thumb_func_start Func_925e0
	push	{r5, r6, r7, lr}
	mov	r6, r0
	ldr	r5, [r6, #0x30]
	ldr	r3, [r6, #8]
	add	r3, r5
	str	r3, [r6, #8]
	str	r3, [r6, #0x38]
	ldr	r7, [r6, #0x34]
	ldr	r3, [r6, #0x10]
	add	r3, r7
	str	r3, [r6, #0x10]
	str	r3, [r6, #0x40]
	mov	r2, #0x80
	ldr	r3, [r6, #0xc]
	lsl	r2, #3
	add	r3, r2
	mov	r0, r5
	mov	r1, #0x12
	str	r3, [r6, #0xc]
	str	r3, [r6, #0x3c]
	bl	Func_af0_from_thumb
	sub	r5, r0
	str	r5, [r6, #0x30]
	mov	r3, r7
	cmp	r7, #0
	bge	.L92618
	add	r3, #0xf
.L92618:
	asr	r3, #4
	sub	r3, r7, r3
	str	r3, [r6, #0x34]
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_925e0

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
.thumb_func_start Func_92624
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r8, r1
	ldr	r2, [r7, #0xc]
	ldr	r1, [r7, #8]
	ldr	r3, [r7, #0x10]
	mov	r0, #0xde
	bl	_Func_c150
	mov	r6, r0
	cmp	r6, #0
	beq	.L926fc
	ldr	r5, [r6, #0x50]
	bl	Func_4458
	mov	r3, #1
	and	r0, r3
	cmp	r0, #1
	bne	.L92664
	mov	r0, r6
	mov	r1, #2
	bl	_Func_c300
	ldr	r1, =.L9fbec
	mov	r0, r6
	bl	_Func_c2d8
	b	.L92674

	.pool_aligned

.L92664:
	mov	r0, r6
	mov	r1, #1
	bl	_Func_c300
	ldr	r1, =.L9fc04
	mov	r0, r6
	bl	_Func_c2d8
.L92674:
	mov	r1, r8
	cmp	r1, #0
	beq	.L92680
	mov	r0, r6
	bl	Func_929d8
.L92680:
	mov	r2, #0
	mov	r8, r2
	mov	r3, r6
	add	r3, #0x55
	mov	r1, r8
	strb	r1, [r3]
	bl	Func_4458
	mov	r1, #0xa
	bl	Func_b50_from_thumb
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
	bl	Func_4458
	mov	r1, #0xf
	bl	Func_b50_from_thumb
	ldr	r3, =0x1999
	sub	r0, #7
	lsl	r0, #1
	mul	r3, r0
	str	r3, [r6, #0x30]
	mov	r3, r6
	add	r3, #0x64
	mov	r1, r8
	strh	r1, [r3]
	ldr	r3, =Func_925e0
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
.func_end Func_92624

@ PlayLevitateSequence
@ r0=entity id, r1=animation index, r2=sparkle flag (-1 disables sparkles).
@ Blocking cutscene: lifts the entity into the air, holds it, and sets it back
@ down. Does nothing if the id does not resolve.
@   - sound 0x121, switch to animation r1, wait 10 frames
@   - switch to animation 1, set bit 1 of the mode byte at +0x55, give +0x28 an
@     upward velocity of 0x40000 and aim _Func_d14c at y - 0xC0000
@   - wait 6 frames, sound 0xD9, then Func_92adc for the lift effect
@   - 14 frames of rising 0x20000 per frame, spawning a sparkle through
@     Func_92624 on every other frame when the flag allows
@   - set mode 3, drop the velocity to 0x30000 and aim at y + 0x100000, then
@     block on _Func_ca6c until the descent finishes
@   - wait (up to 180 frames) until y falls back to the ground height at +0x14,
@     pause 2 frames and finish with Func_9202c
.thumb_func_start Func_92708
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r8, r2
	mov	r10, r0
	mov	r5, r1
	bl	Func_92054
	mov	r6, r0
	ldr	r2, [r6, #0x10]
	mov	r9, r2
	cmp	r6, #0
	beq	.L927f6
	ldr	r0, =0x121
	bl	_Func_f9080
	mov	r1, r5
	mov	r0, r6
	bl	_Func_c300
	mov	r7, r6
	mov	r0, #0xa
	bl	Func_30f8
	add	r7, #0x55
	mov	r0, r6
	mov	r1, #1
	bl	_Func_c300
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
	bl	_Func_d14c
	mov	r0, #6
	bl	Func_30f8
	mov	r0, #0xd9
	bl	_Func_f9080
	mov	r1, #0xa0
	mov	r5, #0
	lsl	r1, #7
	mov	r0, r10
	mov	r2, #0
	bl	Func_92adc
	strb	r5, [r7]
.L92780:
	ldr	r3, [r6, #0xc]
	ldr	r2, =0xfffe0000
	add	r3, r2
	str	r3, [r6, #0xc]
	str	r3, [r6, #0x3c]
	mov	r0, #1
	bl	Func_30f8
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
	bl	Func_92624
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
	bl	_Func_d14c
	mov	r0, r6
	bl	_Func_ca6c
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	mov	r5, #0
	cmp	r2, r3
	ble	.L927ec
.L927d8:
	mov	r0, #1
	add	r5, #1
	bl	Func_30f8
	cmp	r5, #0xb3
	bhi	.L927ec
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	cmp	r2, r3
	bgt	.L927d8
.L927ec:
	mov	r0, #2
	bl	Func_30f8
	bl	Func_9202c
.L927f6:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92708

@ FaceEntityInstant
@ r0=slot A, r1=slot B, r2=argument for Func_9163c. Snaps A's facing angle at
@ +0x06 straight to the atan2 heading toward B -- no interpolation -- then calls
@ Func_9163c. Both slots must resolve through Func_8ba1c.
.thumb_func_start Func_9280c
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r6, r0
	mov	r0, r5
	bl	Func_8ba1c
	mov	r2, r0
	cmp	r6, #0
	beq	.L92840
	cmp	r2, #0
	beq	.L92840
	ldr	r3, [r6, #0x10]
	ldr	r0, [r2, #0x10]
	ldr	r1, [r2, #8]
	sub	r0, r3
	ldr	r3, [r6, #8]
	sub	r1, r3
	bl	Func_44d0
	strh	r0, [r6, #6]
	mov	r0, r7
	bl	Func_9163c
.L92840:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_9280c

@ TurnSlotsToFaceEachOther
@ r0=slot A, r1=slot B, r2=argument for Func_9163c. Resolves both slots and
@ hands them to Func_92878 for the gradual turn, then calls Func_9163c. This is
@ the slot-addressed wrapper; Func_92878 takes entities directly.
.thumb_func_start Func_92848
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r6, r0
	mov	r0, r5
	bl	Func_8ba1c
	mov	r1, r0
	cmp	r6, #0
	beq	.L92870
	cmp	r1, #0
	beq	.L92870
	mov	r0, r6
	bl	Func_92878
	mov	r0, r7
	bl	Func_9163c
.L92870:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92848

@ TurnToFaceEachOther
@ r0=entity A, r1=entity B. Blocking. Rotates A toward B and B toward A -- the
@ second heading is A's plus 0x8000, i.e. exactly opposite -- stepping each
@ facing angle at +0x06 by at most +/-0x1000 per frame and yielding with
@ Func_30f8(1) between steps.
@ Returns as soon as both have arrived, or after 60 frames, whichever comes
@ first. The counter r1 tracks how many are still turning, so a pair that is
@ already aligned costs nothing.
.thumb_func_start Func_92878
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r0
	sub	sp, #4
	mov	r5, r1
	cmp	r6, #0
	beq	.L92912
	cmp	r5, #0
	beq	.L92912
	ldr	r3, [r6, #0x10]
	ldr	r0, [r5, #0x10]
	ldr	r1, [r5, #8]
	sub	r0, r3
	ldr	r3, [r6, #8]
	sub	r1, r3
	bl	Func_44d0
	lsl	r0, #16
	lsr	r7, r0, #16
	mov	r0, #0x80
	lsl	r0, #8
	add	r0, r7
	mov	r8, r0
	mov	r4, #0
.L928aa:
	ldrh	r2, [r6, #6]
	sub	r3, r7, r2
	lsl	r3, #16
	asr	r3, #16
	mov	r1, #2
	cmp	r3, #0
	beq	.L928d2
	mov	r0, #0x80
	lsl	r0, #5
	cmp	r3, r0
	ble	.L928c4
	mov	r3, #0x80
	lsl	r3, #5
.L928c4:
	ldr	r0, =0xfffff000
	cmp	r3, r0
	bge	.L928cc
	ldr	r3, =0xfffff000
.L928cc:
	add	r3, r2, r3
	strh	r3, [r6, #6]
	b	.L928d4
.L928d2:
	mov	r1, #1
.L928d4:
	ldrh	r2, [r5, #6]
	mov	r0, r8
	sub	r3, r0, r2
	lsl	r3, #16
	asr	r3, #16
	cmp	r3, #0
	beq	.L928fc
	mov	r0, #0x80
	lsl	r0, #5
	cmp	r3, r0
	ble	.L928ee
	mov	r3, #0x80
	lsl	r3, #5
.L928ee:
	ldr	r0, =0xfffff000
	cmp	r3, r0
	bge	.L928f6
	ldr	r3, =0xfffff000
.L928f6:
	add	r3, r2, r3
	strh	r3, [r5, #6]
	b	.L928fe
.L928fc:
	sub	r1, #1
.L928fe:
	cmp	r1, #0
	beq	.L92912
	mov	r0, #1
	str	r4, [sp]
	bl	Func_30f8
	ldr	r4, [sp]
	add	r4, #1
	cmp	r4, #0x3b
	ble	.L928aa
.L92912:
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92878

@ DespawnSlotEntity
@ r0=slot. Destroys the slot's entity with _Func_c0f4 and clears the cached
@ pointer at iwram_1ebc + 0x14 + slot * 4 so nothing dereferences it afterwards.
@ No-op when the slot is already empty.
.thumb_func_start Func_92924
	push	{r5, r6, lr}
	mov	r5, r0
	bl	Func_8ba1c
	ldr	r3, =iwram_1ebc
	ldr	r6, [r3]
	cmp	r0, #0
	beq	.L92940
	bl	_Func_c0f4
	lsl	r3, r5, #2
	add	r3, #0x14
	mov	r2, #0
	str	r2, [r6, r3]
.L92940:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_92924

@ Nop -- empty handler, present to fill a table or callback slot.
.thumb_func_start Func_9294c
	bx	lr
.func_end Func_9294c

	.section .rodata

@ .L9fbec / .L9fc04 -- the two 0x18-byte sparkle particle scripts Func_92624
@ chooses between at random.
.L9fbec:
	.incrom 0x9fbec, 0x9fc04
.L9fc04:
	.incrom 0x9fc04, 0x9fc1c

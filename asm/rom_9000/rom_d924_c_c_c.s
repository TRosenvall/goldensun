	.include "macros.inc"

@ ScriptOp_PickWanderTarget
@ Script opcode handler. r0=entity. Chooses a random nearby point to wander to,
@ rejecting anything blocked or outside the entity's leash. Reads three script
@ operands: a base distance, a distance scale, and a leash radius (the third is
@ squared on entry and compared against the squared offset from the spawn tile
@ cached at +0x64/+0x66).
@ Up to 7 attempts. Each draws random values from Func_4458 to build a distance
@ and a heading offset around the current facing (+0x06), turns that into a
@ candidate point with vec3_translate, and rejects it if any of these fail:
@   - Func_d924 reports an entity collision at the candidate
@   - TestCollision reports terrain collision at the candidate, or at the two
@     probe headings +0x2000 and -0x2000 around it
@   - the candidate lands outside the leash radius of the spawn tile
@ On success it issues Actor_TravelTo toward the point, advances the cursor by 4 and
@ returns 1. After 7 failures it turns 180 degrees (+0x8000 on +0x06), sets the
@ wait timer at +0x5E to 1 and returns 0 so the handler retries next frame.
.thumb_func_start ActorCmd_Wander  @ 0x0800dd70
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r6, r0
	mov	r0, #4
	ldrsh	r2, [r6, r0]
	ldr	r3, [r6]
	lsl	r2, #2
	add	r3, r2
	add	r3, #4
	ldmia	r3!, {r2}
	sub	sp, #0x20
	str	r2, [sp, #4]
	ldmia	r3!, {r5}
	ldr	r3, [r3]
	mov	r11, r5
	cmp	r3, #0
	bge	.Ldda0
	ldr	r0, =0xffff
	add	r3, r0
.Ldda0:
	asr	r3, #16
	mov	r5, r3
	mul	r5, r3
	str	r3, [sp]
	mov	r2, #0
	str	r5, [sp]
	mov	r9, r2
.Lddae:
	mov	r0, #1
	add	r9, r0
	mov	r2, r9
	cmp	r2, #7
	ble	.Lddba
	b	.Ldebe
.Lddba:
	ldr	r3, [r6, #8]
	add	r7, sp, #0x14
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r7, #8]
	bl	Random
	ldr	r3, =Func_8000888
	mov	r1, r11
	.call_via r3
	ldr	r3, [sp, #4]
	add	r3, r0
	mov	r8, r3
	bl	Random
	mov	r5, r0
	bl	Random
	ldrh	r3, [r6, #6]
	lsr	r5, #2
	lsr	r0, #2
	add	r3, r5
	sub	r3, r0
	mov	r10, r3
	mov	r0, r8
	mov	r1, r10
	mov	r2, r7
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r7
	bl	Func_800d924
	cmp	r0, #0
	bne	.Lddae
	mov	r0, r6
	mov	r1, r7
	bl	TestCollision
	cmp	r0, #0
	bne	.Lddae
	mov	r5, #0x80
	ldr	r3, [r6, #8]
	lsl	r5, #12
	add	r8, r5
	add	r5, sp, #8
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, r8
	str	r3, [r5, #8]
	mov	r1, r10
	mov	r2, r5
	bl	vec3_translate
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	mov	r1, #0x80
	ldr	r3, [r6, #0x10]
	lsl	r1, #6
	add	r1, r10
	mov	r0, r8
	str	r3, [r5, #8]
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	cmp	r0, #0
	bne	.Lddae
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r1, =0xffffe000
	ldr	r3, [r6, #0x10]
	add	r1, r10
	mov	r0, r8
	str	r3, [r5, #8]
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	cmp	r0, #0
	bne	.Lddae
	ldr	r3, [r7]
	mov	r1, r3
	cmp	r3, #0
	bge	.Lde86
	ldr	r0, =0xffff
	add	r3, r0
.Lde86:
	mov	r2, r6
	add	r2, #0x64
	mov	r5, #0
	ldrsh	r2, [r2, r5]
	asr	r3, #16
	sub	r0, r3, r2
	ldr	r2, [r7, #8]
	mov	r4, r2
	cmp	r2, #0
	bge	.Lde9e
	ldr	r3, =0xffff
	add	r2, r3
.Lde9e:
	mov	r3, r6
	add	r3, #0x66
	mov	r5, #0
	ldrsh	r3, [r3, r5]
	asr	r2, #16
	sub	r2, r3
	mov	r3, r0
	mul	r3, r0
	mov	r0, r2
	mul	r0, r2
	mov	r2, r0
	add	r3, r2
	ldr	r2, [sp]
	cmp	r3, r2
	ble	.Lded4
	b	.Lddae
.Ldebe:
	ldrh	r3, [r6, #6]
	mov	r5, #0x80
	lsl	r5, #8
	add	r3, r5
	mov	r2, r6
	strh	r3, [r6, #6]
	add	r2, #0x5e
	mov	r3, #1
	strh	r3, [r2]
	mov	r0, #0
	b	.Ldee6
.Lded4:
	mov	r0, r6
	mov	r3, r4
	ldr	r2, [r7, #4]
	bl	Actor_TravelTo
	ldrh	r3, [r6, #4]
	add	r3, #4
	strh	r3, [r6, #4]
	mov	r0, #1
.Ldee6:
	add	sp, #0x20
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end ActorCmd_Wander

@ ScriptOp_PickWanderTargetLeashed
@ Script opcode handler. r0=entity. The leash-aware sibling of ActorCmd_Wander: same
@ three script operands (base distance, distance scale, leash radius squared)
@ and the same 7-attempt structure, but it checks the leash *first* and probes
@ more directions.
@ While the entity is still inside its leash radius of the spawn tile
@ (+0x64/+0x66), each attempt builds a random candidate via Func_4458 and
@ vec3_translate and rejects it on an entity hit (Func_d924) or a terrain hit
@ (TestCollision) at the candidate or at any of the probe headings +0x2000,
@ +0x4000, -0x2000 and -0x4000. A candidate that also stays within the leash is
@ accepted: bit 1 of the flag byte at +0x59 is set and Actor_TravelTo starts the move.
@ Once the entity is already outside its leash, the loop at .Le146 instead aims
@ back toward the spawn point -- heading = atan2 of the offset plus 0x8000 --
@ retries up to 7 times against the same collision tests, and on success clears
@ bit 1 of +0x59 before issuing Actor_TravelTo.
@ Either way the cursor advances by 4 and the handler returns 1.
.thumb_func_start ActorCmd_Unk9  @ 0x0800df04
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r6, r0
	mov	r0, #4
	ldrsh	r2, [r6, r0]
	ldr	r3, [r6]
	lsl	r2, #2
	add	r3, r2
	add	r3, #4
	ldmia	r3!, {r1}
	sub	sp, #0x34
	str	r1, [sp, #0x18]
	ldmia	r3!, {r2}
	str	r2, [sp, #0x14]
	ldr	r3, [r3]
	cmp	r3, #0
	bge	.Ldf34
	ldr	r4, =0xffff
	add	r3, r4
.Ldf34:
	asr	r3, #16
	mov	r1, r3
	mul	r1, r3
	mov	r2, #0
	mov	r0, #6
	ldrsh	r5, [r6, r0]
	mov	r10, r2
	ldr	r2, [r6, #8]
	str	r3, [sp, #0x10]
	str	r5, [sp, #0xc]
	str	r1, [sp, #0x10]
	cmp	r2, #0
	bge	.Ldf52
	ldr	r3, =0xffff
	add	r2, r3
.Ldf52:
	mov	r4, r6
	add	r4, #0x64
	str	r4, [sp, #8]
	mov	r5, #0
	ldrsh	r3, [r4, r5]
	asr	r2, #16
	sub	r2, r3
	mov	r11, r2
	ldr	r2, [r6, #0x10]
	cmp	r2, #0
	bge	.Ldf6c
	ldr	r0, =0xffff
	add	r2, r0
.Ldf6c:
	mov	r1, r6
	add	r1, #0x66
	str	r1, [sp, #4]
	mov	r4, #0
	ldrsh	r3, [r1, r4]
	asr	r2, #16
	sub	r2, r3
	mov	r9, r2
	mov	r5, r11
	mov	r0, r9
	mov	r3, r11
	mul	r3, r5
	mov	r2, r9
	mul	r2, r0
	ldr	r1, [sp, #0x10]
	add	r3, r2
	cmp	r3, r1
	ble	.Ldf92
	b	.Le146
.Ldf92:
	mov	r2, #1
	add	r10, r2
	mov	r3, r10
	cmp	r3, #7
	ble	.Ldf9e
	b	.Le146
.Ldf9e:
	bl	Random
	ldr	r3, =Func_8000888
	ldr	r1, [sp, #0x14]
	.call_via r3
	ldr	r4, [sp, #0x18]
	add	r4, r0
	mov	r8, r4
	bl	Random
	mov	r5, r0
	bl	Random
	ldr	r3, [r6, #8]
	add	r7, sp, #0x28
	ldr	r1, [sp, #0xc]
	str	r3, [r7]
	lsl	r2, r1, #16
	ldr	r3, [r6, #0xc]
	lsr	r5, #2
	lsr	r2, #16
	lsr	r0, #2
	str	r3, [r7, #4]
	add	r2, r5
	sub	r2, r0
	ldr	r3, [r6, #0x10]
	lsl	r2, #16
	lsr	r4, r2, #16
	mov	r0, #0x80
	mov	r1, r4
	str	r3, [r7, #8]
	lsl	r0, #12
	mov	r2, r7
	str	r4, [sp]
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r7
	bl	Func_800d924
	ldr	r4, [sp]
	cmp	r0, #0
	bne	.Ldf92
	ldr	r3, [r6, #8]
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	mov	r1, r4
	str	r3, [r7, #8]
	mov	r0, r8
	mov	r2, r7
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r7
	bl	TestCollision
	ldr	r4, [sp]
	cmp	r0, #0
	bne	.Ldf92
	ldr	r3, [r6, #8]
	add	r5, sp, #0x1c
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	mov	r2, #0x80
	ldr	r3, [r6, #0x10]
	lsl	r2, #12
	add	r8, r2
	mov	r1, r4
	str	r3, [r5, #8]
	mov	r0, r8
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	ldr	r4, [sp]
	cmp	r0, #0
	bne	.Ldf92
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #8]
	mov	r3, #0x80
	lsl	r3, #6
	add	r1, r4, r3
	mov	r0, r8
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	ldr	r4, [sp]
	cmp	r0, #0
	bne	.Ldf92
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r0, =0xffffe000
	ldr	r3, [r6, #0x10]
	add	r1, r4, r0
	str	r3, [r5, #8]
	mov	r0, r8
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	ldr	r4, [sp]
	cmp	r0, #0
	beq	.Le098
	b	.Ldf92
.Le098:
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	mov	r2, #0x80
	ldr	r3, [r6, #0x10]
	lsl	r2, #7
	add	r1, r4, r2
	str	r3, [r5, #8]
	mov	r0, r8
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	ldr	r4, [sp]
	cmp	r0, #0
	beq	.Le0c2
	b	.Ldf92
.Le0c2:
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #8]
	ldr	r3, =0xffffc000
	mov	r0, r8
	add	r1, r4, r3
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	cmp	r0, #0
	beq	.Le0e8
	b	.Ldf92
.Le0e8:
	ldr	r1, [r7]
	mov	r2, r1
	cmp	r1, #0
	bge	.Le0f4
	ldr	r4, =0xffff
	add	r2, r1, r4
.Le0f4:
	ldr	r0, [sp, #8]
	mov	r5, #0
	ldrsh	r3, [r0, r5]
	asr	r2, #16
	ldr	r4, [r7, #8]
	sub	r2, r3
	mov	r11, r2
	mov	r2, r4
	cmp	r4, #0
	bge	.Le10c
	ldr	r3, =0xffff
	add	r2, r4, r3
.Le10c:
	ldr	r0, [sp, #4]
	mov	r5, #0
	ldrsh	r3, [r0, r5]
	asr	r2, #16
	sub	r2, r3
	mov	r9, r2
	mov	r5, r9
	mov	r2, r11
	mov	r3, r11
	mul	r3, r2
	mov	r2, r9
	mul	r2, r5
	ldr	r0, [sp, #0x10]
	add	r3, r2
	cmp	r3, r0
	ble	.Le12e
	b	.Ldf92
.Le12e:
	mov	r0, r6
	add	r0, #0x59
	ldrb	r3, [r0]
	mov	r2, #2
	orr	r2, r3
	strb	r2, [r0]
	ldr	r2, [r7, #4]
	mov	r0, r6
	mov	r3, r4
	bl	Actor_TravelTo
	b	.Le1f6
.Le146:
	mov	r1, #0
	mov	r10, r1
	mov	r0, r9
	mov	r1, r11
	bl	atan2
	mov	r2, #0x80
	lsl	r2, #8
	add	r0, r2
	lsl	r0, #16
	asr	r0, #16
	str	r0, [sp, #0xc]
.Le15e:
	mov	r3, #1
	add	r10, r3
	mov	r4, r10
	cmp	r4, #7
	bgt	.Le1f6
	bl	Random
	ldr	r3, =Func_8000888
	ldr	r1, [sp, #0x14]
	.call_via r3
	ldr	r5, [sp, #0x18]
	add	r5, r0
	bl	Random
	mov	r8, r5
	mov	r5, r0
	bl	Random
	ldr	r1, [sp, #0xc]
	lsl	r2, r1, #16
	ldr	r3, [r6, #8]
	lsr	r5, #2
	lsr	r2, #16
	add	r2, r5
	add	r5, sp, #0x28
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	lsr	r0, #2
	str	r3, [r5, #4]
	sub	r2, r0
	ldr	r3, [r6, #0x10]
	lsl	r2, #16
	lsr	r7, r2, #16
	mov	r0, #0x80
	lsl	r0, #12
	mov	r1, r7
	str	r3, [r5, #8]
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	Func_800d924
	cmp	r0, #0
	bne	.Le15e
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, r8
	mov	r1, r7
	str	r3, [r5, #8]
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r5
	bl	TestCollision
	cmp	r0, #0
	bne	.Le15e
	mov	r1, r6
	add	r1, #0x59
	ldrb	r2, [r1]
	mov	r3, #0xfd
	and	r3, r2
	strb	r3, [r1]
	mov	r0, r6
	ldr	r1, [r5]
	ldr	r2, [r5, #4]
	ldr	r3, [r5, #8]
	bl	Actor_TravelTo
.Le1f6:
	ldrh	r3, [r6, #4]
	add	r3, #4
	mov	r0, #1
	strh	r3, [r6, #4]
	add	sp, #0x34
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end ActorCmd_Unk9

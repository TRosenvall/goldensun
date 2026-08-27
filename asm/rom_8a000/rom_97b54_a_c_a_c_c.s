	.include "macros.inc"

@ AnimateTargetRise
@ Takes no arguments. Steps the target object upward frame by frame, updating
@ its shadow and particles as it goes. The ~420-instruction body is
@ characterised structurally.
.thumb_func_start Func_8098698  @ 0x08098698
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f30
	ldr	r3, [r3]
	sub	sp, #0xc
	mov	r9, r3
	ldr	r7, [r3, #0x10]
	bl	Func_8097384
	mov	r0, #0x17
	mov	r8, sp
	mov	r10, r8
	mov	r11, r0
.L986bc:
	mov	r2, r9
	mov	r5, #0x80
	ldr	r3, [r2]
	lsl	r5, #7
	cmp	r3, r5
	bne	.L986d6
	ldr	r3, [r7, #8]
	mov	r0, r10
	str	r3, [r0]
	ldr	r3, [r7, #0xc]
	mov	r2, #0xa0
	lsl	r2, #12
	b	.L986ea
.L986d6:
	mov	r5, #0xc0
	lsl	r5, #8
	cmp	r3, r5
	bne	.L986f8
	ldr	r3, [r7, #8]
	mov	r0, r10
	str	r3, [r0]
	ldr	r3, [r7, #0xc]
	mov	r2, #0xc0
	lsl	r2, #13
.L986ea:
	add	r3, r2
	str	r3, [r0, #4]
	ldr	r3, [r7, #0x10]
	str	r3, [r0, #8]
	b	.L98716

	.pool_aligned

.L986f8:
	ldr	r3, [r7, #8]
	mov	r5, r10
	str	r3, [r5]
	ldr	r3, [r7, #0xc]
	mov	r0, #0xa0
	lsl	r0, #12
	add	r3, r0
	str	r3, [r5, #4]
	ldr	r3, [r7, #0x10]
	str	r3, [r5, #8]
	mov	r2, r9
	ldr	r1, [r2]
	mov	r2, r10
	bl	vec3_translate
.L98716:
	mov	r3, r10
	mov	r0, #0x8e
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	lsl	r0, #1
	ldr	r3, [r3, #8]
	bl	CreateParticleActor
	mov	r6, r0
	ldr	r4, [r6, #0x50]
	ldrb	r3, [r4, #5]
	mov	r0, r4
	add	r0, #0xc
	mov	r1, #0x20
	mov	r5, #0x21
	and	r1, r3
	neg	r5, r5
	ldrb	r3, [r0, #5]
	mov	r2, r5
	and	r3, r2
	orr	r3, r1
	strb	r3, [r0, #5]
	ldrb	r2, [r4, #5]
	mov	r1, #0x3f
	lsr	r2, #6
	lsl	r2, #6
	and	r3, r1
	orr	r3, r2
	strb	r3, [r0, #5]
	ldrb	r3, [r4, #7]
	ldrb	r2, [r0, #7]
	lsr	r3, #6
	lsl	r3, #6
	and	r1, r2
	orr	r1, r3
	strb	r1, [r0, #7]
	ldrh	r1, [r4, #8]
	ldrh	r3, [r0, #8]
	ldr	r2, =0xfffffc00
	lsl	r1, #22
	lsr	r1, #22
	and	r3, r2
	orr	r3, r1
	strh	r3, [r0, #8]
	ldrb	r2, [r4, #9]
	ldrb	r1, [r0, #9]
	lsr	r2, #4
	mov	r3, #0xf
	lsl	r2, #4
	and	r3, r1
	orr	r3, r2
	strb	r3, [r0, #9]
	cmp	r6, #0
	beq	.L98812
	ldr	r3, =0xb333
	str	r3, [r6, #0x1c]
	str	r3, [r6, #0x18]
	mov	r3, #0xc0
	lsl	r3, #9
	mov	r2, r6
	add	r2, #0x55
	str	r3, [r6, #0x34]
	str	r3, [r6, #0x30]
	mov	r3, #0
	strb	r3, [r2]
	mov	r0, r6
	b	.L987a4

	.pool_aligned

.L987a4:
	mov	r1, #0xb
	bl	_Actor_SetColorswap
	mov	r0, r6
	mov	r1, #7
	bl	_Actor_SetAnim
	mov	r0, r6
	ldr	r1, =.L9f0b4
	bl	_Actor_SetScript
	mov	r0, r6
	mov	r1, #1
	bl	_Actor_SetSpriteFlags
	mov	r0, r9
	ldr	r3, [r0, #4]
	mov	r2, r8
	str	r3, [r2]
	ldr	r3, [r0, #8]
	str	r3, [r2, #4]
	ldr	r3, [r0, #0xc]
	str	r3, [r2, #8]
	mov	r3, #0xc0
	ldr	r1, [r0]
	lsl	r3, #8
	cmp	r1, r3
	bne	.L987e4
	mov	r0, #0xe0
	lsl	r0, #12
	bl	vec3_translate
.L987e4:
	bl	Random
	lsl	r5, r0, #1
	add	r5, r0
	mov	r0, #0x80
	lsl	r0, #11
	lsl	r5, #1
	add	r5, r0
	bl	Random
	mov	r2, r8
	mov	r1, r0
	mov	r0, r5
	bl	vec3_translate
	mov	r5, r8
	mov	r2, r8
	ldr	r1, [r2]
	ldr	r3, [r5, #8]
	ldr	r2, [r2, #4]
	mov	r0, r6
	bl	_Actor_TravelTo
.L98812:
	mov	r0, #0x83
	bl	_PlaySound
	mov	r0, #2
	bl	WaitFrames
	mov	r0, #1
	neg	r0, r0
	add	r11, r0
	mov	r2, r11
	cmp	r2, #0
	blt	.L9882c
	b	.L986bc
.L9882c:
	mov	r0, #8
	bl	WaitFrames
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8098698

@ RunTargetSet
@ Takes no arguments. Sets a lifted object back down at its destination,
@ resolving the landing tile and clearing the lift state.
.thumb_func_start Field_Lift_Target  @ 0x08098848
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f30
	ldr	r6, [r3]
	ldr	r7, [r6, #0x14]
	sub	sp, #0x14
	ldr	r5, [r6, #0x10]
	cmp	r7, #0
	beq	.L98936
	bl	Func_8097384
	mov	r0, r5
	str	r7, [r5, #0x68]
	ldr	r1, =.L9f0bc
	bl	_Actor_SetScript
	ldr	r0, [r6, #4]
	add	r5, sp, #8
	str	r0, [r5]
	mov	r2, #0x80
	ldr	r1, [r6, #8]
	lsl	r2, #13
	add	r1, r2
	str	r1, [r5, #4]
	mov	r3, #0x80
	ldr	r2, [r6, #0xc]
	lsl	r3, #14
	add	r0, r3
	mov	r3, #0x80
	str	r2, [r5, #8]
	lsl	r3, #8
	bl	Func_8098a84
	ldr	r2, =0xffe00000
	str	r0, [sp]
	ldr	r0, [r5]
	ldr	r1, [r5, #4]
	add	r0, r2
	mov	r3, #0
	ldr	r2, [r5, #8]
	bl	Func_8098a84
	str	r0, [sp, #4]
	mov	r0, #0xf
	mov	r8, sp
	bl	WaitFrames
	mov	r6, r8
	mov	r5, #1
.L988ac:
	ldmia	r6!, {r0}
	cmp	r0, #0
	beq	.L988bc
	mov	r1, #0xe0
	ldrh	r2, [r0, #6]
	lsl	r1, #12
	bl	Func_8096bec
.L988bc:
	sub	r5, #1
	cmp	r5, #0
	bge	.L988ac
	ldr	r0, [sp]
	bl	_Actor_WaitMovement
	ldr	r3, =Func_8096b88
	mov	r0, #0x82
	str	r3, [r7, #0x6c]
	bl	_PlaySound
	mov	r2, r7
	add	r2, #0x55
	mov	r3, #4
	ldr	r0, [sp]
	strb	r3, [r2]
	ldr	r5, [r7, #0xc]
	cmp	r0, #0
	beq	.L98926
	mov	r2, r8
	ldr	r3, [r2, #4]
	cmp	r3, #0
	beq	.L98926
	mov	r2, #0x80
	lsl	r2, #14
	add	r3, r5, r2
	cmp	r5, r3
	bgt	.L98926
	b	.L988f8
.L988f6:
	ldr	r0, [sp]
.L988f8:
	ldr	r3, [r0, #0xc]
	mov	r1, #0x80
	lsl	r1, #7
	add	r3, r1
	str	r3, [r0, #0xc]
	mov	r3, r8
	ldr	r2, [r3, #4]
	ldr	r3, [r2, #0xc]
	add	r3, r1
	str	r3, [r2, #0xc]
	ldr	r3, [r7, #0xc]
	add	r3, r1
	str	r3, [r7, #0xc]
	mov	r0, #1
	bl	WaitFrames
	mov	r3, #0x80
	lsl	r3, #14
	add	r2, r5, r3
	ldr	r3, [r7, #0xc]
	cmp	r3, r2
	ble	.L988f6
	ldr	r0, [sp]
.L98926:
	bl	Func_80981b0
	mov	r2, r8
	ldr	r0, [r2, #4]
	bl	Func_80981b0
	bl	Func_809748c
.L98936:
	add	sp, #0x14
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Field_Lift_Target

@ RunTargetMove
@ Takes no arguments. Carries a lifted object sideways to a new tile before it
@ is set down. The ~340-instruction body is characterised structurally.
.thumb_func_start Field_Lift  @ 0x08098954
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f30
	ldr	r3, [r3]
	sub	sp, #0x14
	mov	r10, r3
	bl	Func_8097384
	mov	r3, r10
	ldr	r0, [r3, #4]
	add	r5, sp, #8
	str	r0, [r5]
	ldr	r1, [r3, #8]
	mov	r3, #0x80
	lsl	r3, #13
	add	r1, r3
	str	r1, [r5, #4]
	mov	r3, r10
	ldr	r2, [r3, #0xc]
	mov	r3, #0x80
	lsl	r3, #14
	add	r0, r3
	mov	r3, #0x80
	str	r2, [r5, #8]
	lsl	r3, #8
	bl	Func_8098a84
	ldr	r3, =0xffe00000
	str	r0, [sp]
	ldr	r0, [r5]
	ldr	r1, [r5, #4]
	add	r0, r3
	ldr	r2, [r5, #8]
	mov	r3, #0
	bl	Func_8098a84
	str	r0, [sp, #4]
	mov	r0, #0xf
	mov	r11, sp
	bl	WaitFrames
	mov	r0, #1
	mov	r7, r11
	mov	r8, r0
.L989b6:
	ldmia	r7!, {r6}
	cmp	r6, #0
	beq	.L989c8
	mov	r1, #0xc0
	ldrh	r2, [r6, #6]
	mov	r0, r6
	lsl	r1, #13
	bl	Func_8096bec
.L989c8:
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r0, r8
	cmp	r0, #0
	bge	.L989b6
	ldr	r0, [sp]
	bl	_Actor_WaitMovement
	mov	r0, #0x86
	bl	_PlaySound
	mov	r0, #0x80
	mov	r3, #0x17
	lsl	r0, #10
	mov	r7, r5
	mov	r8, r3
	mov	r9, r0
.L989ec:
	mov	r3, r10
	ldr	r1, [r3, #4]
	str	r1, [r7]
	mov	r0, #0x80
	ldr	r2, [r3, #8]
	lsl	r0, #13
	add	r2, r0
	str	r2, [r7, #4]
	ldr	r3, [r3, #0xc]
	ldr	r0, =0x11d
	str	r3, [r7, #8]
	bl	CreateParticleActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L98a44
	ldr	r1, =.L9f0d4
	bl	_Actor_SetScript
	bl	Random
	mov	r3, r9
	mov	r2, r6
	add	r2, #0x55
	str	r3, [r6, #0x34]
	add	r0, r9
	mov	r3, #0
	str	r0, [r6, #0x30]
	strb	r3, [r2]
	bl	Random
	lsl	r5, r0, #1
	add	r5, r0
	mov	r0, #0x80
	lsl	r0, #12
	lsl	r5, #3
	add	r5, r0
	bl	Random
	mov	r1, r5
	mov	r2, r0
	mov	r0, r6
	bl	Func_8096bec
.L98a44:
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r0, r8
	cmp	r0, #0
	bge	.L989ec
	ldr	r0, [sp]
	bl	_DeleteActor
	mov	r3, r11
	ldr	r0, [r3, #4]
	bl	_DeleteActor
	bl	Func_809748c
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Field_Lift

@ PlayAbilityImpact
@ r0, r1, r2, r3 = impact position and kind. Plays sound 0x8A and the impact
@ burst at that point.
.thumb_func_start Func_8098a84  @ 0x08098a84
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r5, r0
	mov	r6, r1
	mov	r8, r2
	mov	r0, #0x8a
	mov	r7, r3
	bl	_PlaySound
	mov	r1, r5
	mov	r0, #0xd7
	mov	r2, r6
	mov	r3, r8
	bl	CreateParticleActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L98b02
	mov	r3, #0x80
	lsl	r3, #7
	str	r3, [r5, #0x1c]
	str	r3, [r5, #0x18]
	mov	r3, #0xc0
	lsl	r3, #10
	str	r3, [r5, #0x30]
	str	r3, [r5, #0x34]
	ldr	r1, [r5, #0x50]
	mov	r3, #0xd
	ldrb	r2, [r1, #9]
	neg	r3, r3
	and	r3, r2
	strb	r3, [r1, #9]
	mov	r1, #3
	bl	_Actor_SetAnim
	mov	r2, #0x80
	ldr	r3, [r5, #0x18]
	lsl	r2, #9
	cmp	r3, r2
	bge	.L98b00
	ldr	r6, =0x2000
.L98ad8:
	mov	r2, #0x80
	lsl	r2, #4
	add	r3, r2
	str	r3, [r5, #0x1c]
	str	r3, [r5, #0x18]
	ldrh	r3, [r5, #6]
	add	r3, r6
	strh	r3, [r5, #6]
	mov	r0, #1
	bl	WaitFrames
	ldr	r3, [r5, #0x18]
	ldr	r2, =0xffff
	cmp	r3, r2
	ble	.L98ad8
	b	.L98b00

	.pool_aligned

.L98b00:
	strh	r7, [r5, #6]
.L98b02:
	mov	r0, r5
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8098a84

@ PlaceParticlesAroundTarget
@ r0=effect instance base. Spaces the ability's particles around the target,
@ using the per-instance offsets at +0x40.
.thumb_func_start Func_8098b10  @ 0x08098b10
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f30
	mov	r7, r0
	ldr	r3, [r3]
	mov	r1, #0x40
	add	r1, r7
	sub	sp, #0xc
	mov	r10, r3
	mov	r8, r1
.L98b28:
	mov	r2, r8
	mov	r6, #0
	ldrsb	r6, [r2, r6]
	cmp	r6, #0
	bne	.L98b68
	ldr	r3, [r7, #0x14]
	mov	r5, sp
	str	r3, [r5]
	ldr	r3, [r7, #0x18]
	str	r3, [r5, #8]
	bl	Random
	mov	r1, r0
	lsl	r1, #16
	mov	r0, #0xc8
	lsr	r1, #16
	lsl	r0, #13
	mov	r2, r5
	bl	vec3_translate
	ldr	r3, [r5]
	str	r3, [r7, #0xc]
	ldr	r3, [r5, #8]
	str	r3, [r7, #0x10]
	mov	r3, #0xc0
	lsl	r3, #10
	str	r3, [r7, #0x24]
	str	r3, [r7, #0x20]
	mov	r3, r7
	add	r3, #0x42
	strb	r6, [r3]
	b	.L98bd6
.L98b68:
	cmp	r6, #1
	bne	.L98b80
	mov	r0, r7
	bl	Func_809ba34
	cmp	r0, #0
	bne	.L98bf4
	mov	r2, r8
	ldrb	r3, [r2]
	add	r3, #1
	strb	r3, [r2]
	b	.L98b28
.L98b80:
	cmp	r6, #2
	bne	.L98be0
	mov	r3, r10
	ldr	r2, [r3, #0x10]
	ldr	r3, [r2, #8]
	mov	r5, sp
	str	r3, [r5]
	mov	r1, #0x80
	ldr	r3, [r2, #0xc]
	lsl	r1, #13
	add	r3, r1
	str	r3, [r5, #4]
	ldr	r3, [r2, #0x10]
	str	r3, [r5, #8]
	mov	r2, r10
	mov	r0, #0x80
	ldr	r1, [r2]
	lsl	r0, #12
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r5
	bl	Func_80974d8
	bl	Random
	mov	r1, r0
	mov	r0, #0x80
	mov	r2, r5
	lsl	r0, #11
	bl	vec3_translate
	ldr	r3, [r5]
	str	r3, [r7, #0xc]
	ldr	r3, [r5, #8]
	str	r3, [r7, #0x10]
	mov	r3, #0x80
	lsl	r3, #4
	mov	r2, r7
	strh	r3, [r7, #0x32]
	add	r2, #0x42
	mov	r3, #1
	strb	r3, [r2]
.L98bd6:
	mov	r1, r8
	ldrb	r3, [r1]
	add	r3, #1
	strb	r3, [r1]
	b	.L98bf4
.L98be0:
	cmp	r6, #3
	bne	.L98bf4
	mov	r0, r7
	bl	Func_809ba34
	cmp	r0, #0
	bne	.L98bf4
	mov	r0, r7
	bl	Func_809bb34
.L98bf4:
	add	sp, #0xc
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8098b10

@ RunShatterAbility
@ r0=target. Plays sound 0x86 and the break-apart effect on the target object,
@ removing it from the map when the animation finishes.
.thumb_func_start Func_8098c08  @ 0x08098c08
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	mov	r0, #0x86
	sub	sp, #0xc
	bl	_PlaySound
	ldr	r1, [r5, #8]
	mov	r6, sp
	str	r1, [r6]
	ldr	r2, [r5, #0xc]
	str	r2, [r6, #4]
	ldr	r4, =0xffe00000
	ldr	r3, [r5, #0x10]
	ldr	r0, =0x11b
	str	r3, [r6, #8]
	add	r2, r4
	bl	CreateParticleActor
	cmp	r0, #0
	beq	.L98c4a
	mov	r2, r0
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	add	r2, #9
	mov	r3, #0x14
	strh	r3, [r2]
	ldr	r1, =Data_9f0b0
	bl	_Actor_SetScript
.L98c4a:
	mov	r0, #0x80
	lsl	r0, #9
	mov	r8, r6
	mov	r7, #0xb
	mov	r10, r0
.L98c54:
	mov	r3, r8
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	ldr	r0, =0x11d
	ldr	r3, [r3, #8]
	bl	CreateParticleActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L98ca0
	ldr	r1, =.L9f0d4
	bl	_Actor_SetScript
	bl	Random
	mov	r2, r6
	add	r2, #0x55
	mov	r4, r10
	mov	r3, #0
	add	r0, r10
	str	r4, [r6, #0x34]
	str	r0, [r6, #0x30]
	strb	r3, [r2]
	bl	Random
	lsl	r5, r0, #1
	add	r5, r0
	mov	r0, #0x80
	lsl	r0, #12
	lsl	r5, #3
	add	r5, r0
	bl	Random
	mov	r1, r5
	mov	r2, r0
	mov	r0, r6
	bl	Func_8096bec
.L98ca0:
	sub	r7, #1
	cmp	r7, #0
	bge	.L98c54
	mov	r0, #0
	add	sp, #0xc
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8098c08

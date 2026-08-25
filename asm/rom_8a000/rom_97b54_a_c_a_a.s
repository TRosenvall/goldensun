	.include "macros.inc"

@ TrackTargetHook
@ r0=entity. Per-frame hook that keeps the particle pointed at the target held
@ at +0x68, recomputing the delta each frame. A null target makes it a no-op.
.thumb_func_start Func_8097b70  @ 0x08097b70
	push	{r5, r6, lr}
	mov	r5, r0
	ldr	r0, [r5, #0x68]
	sub	sp, #0xc
	cmp	r0, #0
	beq	.L97bba
	ldr	r2, [r0, #8]
	ldr	r3, [r5, #8]
	sub	r1, r2, r3
	ldr	r2, [r0, #0x10]
	ldr	r3, [r5, #0x10]
	sub	r0, r2, r3
	cmp	r1, #0
	bne	.L97b90
	cmp	r0, #0
	beq	.L97bb2
.L97b90:
	bl	atan2
	ldrh	r3, [r5, #6]
	sub	r0, r3
	lsl	r0, #16
	mov	r2, #0x80
	asr	r0, #16
	lsl	r2, #5
	cmp	r0, r2
	ble	.L97ba6
	mov	r0, r2
.L97ba6:
	ldr	r2, =0xfffff000
	cmp	r0, r2
	bge	.L97bae
	mov	r0, r2
.L97bae:
	add	r3, r0
	strh	r3, [r5, #6]
.L97bb2:
	mov	r2, r5
	add	r2, #0x5a
	mov	r3, #0
	strb	r3, [r2]
.L97bba:
	ldr	r3, [r5, #8]
	mov	r6, sp
	str	r3, [r6]
	bl	Random
	ldr	r3, [r5, #0xc]
	ldr	r2, =0xfff80000
	lsl	r0, #4
	sub	r3, r0
	add	r3, r2
	str	r3, [r6, #4]
	ldr	r3, [r5, #0x10]
	str	r3, [r6, #8]
	bl	Random
	lsl	r5, r0, #1
	add	r5, r0
	bl	Random
	lsl	r5, #4
	mov	r1, r0
	mov	r2, r6
	mov	r0, r5
	bl	vec3_translate
	ldr	r0, =0x11d
	ldr	r1, [r6]
	ldr	r2, [r6, #4]
	ldr	r3, [r6, #8]
	bl	CreateParticleActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L97c20
	mov	r2, r5
	add	r2, #0x55
	mov	r3, #2
	strb	r3, [r2]
	ldr	r3, =0x1999
	mov	r1, #0
	str	r3, [r5, #0x48]
	bl	_Actor_SetAnim
	mov	r2, r5
	add	r2, #0x5e
	mov	r3, #0xc
	strh	r3, [r2]
	ldr	r1, =Data_9f0b0
	mov	r0, r5
	bl	_Actor_SetScript
.L97c20:
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8097b70

@ RunGrowAbility
@ Takes no arguments. The plant/grow field ability: raises the target from its
@ tile, plays the growth stages and leaves the grown object in place. The
@ ~500-instruction body is characterised structurally.
.thumb_func_start Field_Move_Target  @ 0x08097c3c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f30
	ldr	r3, [r3]
	sub	sp, #0x34
	str	r3, [sp, #0x18]
	ldr	r0, [r3, #0x10]
	str	r0, [sp, #0x14]
	mov	r7, #0x80
	ldr	r6, [r3, #0x14]
	ldr	r3, [r3]
	lsl	r7, #8
	add	r3, r7
	mov	r1, #0
	str	r3, [sp, #8]
	str	r1, [sp, #4]
	cmp	r6, #0
	bne	.L97c6c
	b	.L97f3c
.L97c6c:
	bl	Func_8097384
	ldr	r2, [sp, #0x14]
	str	r6, [r2, #0x68]
	ldr	r0, [sp, #0x14]
	ldr	r1, =.L9f0bc
	bl	_Actor_SetScript
	ldr	r0, [sp, #0x14]
	bl	Func_8098070
	mov	r10, r0
	cmp	r0, #0
	bne	.L97c8e
	bl	Func_809748c
	b	.L97f3c
.L97c8e:
	mov	r3, r10
	str	r6, [r3, #0x68]
	mov	r0, #0x28
	ldr	r3, [r6, #8]
	add	r0, sp
	str	r3, [r0]
	mov	r5, #0x80
	ldr	r3, [r6, #0xc]
	lsl	r5, #13
	add	r3, r5
	str	r3, [r0, #4]
	ldr	r3, [r6, #0x10]
	mov	r9, r0
	str	r3, [r0, #8]
	ldr	r1, [sp, #8]
	mov	r0, r5
	mov	r2, r9
	bl	vec3_translate
	mov	r2, r9
	mov	r0, r9
	ldr	r1, [r2]
	ldr	r3, [r0, #8]
	ldr	r2, [r2, #4]
	mov	r0, r10
	bl	_Actor_TravelTo
	mov	r0, r10
	bl	Func_8098184
	mov	r3, #0x80
	mov	r1, r10
	lsl	r3, #11
	str	r3, [r1, #0x30]
	str	r7, [r1, #0x34]
	mov	r3, #4
	add	r1, #0x55
	str	r1, [sp]
	strb	r3, [r1]
	ldr	r3, =Func_8096b88
	str	r3, [r6, #0x6c]
	ldr	r3, =0x6666
	str	r3, [r6, #0x30]
	ldr	r3, =0x3333
	add	r2, sp, #4
	str	r3, [r6, #0x34]
	ldrb	r2, [r2]
	mov	r3, r6
	add	r3, #0x5a
	strb	r2, [r3]
	mov	r2, r6
	add	r2, #0x22
	mov	r3, #2
	mov	r7, r9
	mov	r11, r5
	strb	r3, [r2]
	b	.L97ee4
.L97d00:
	ldr	r3, =gKeyHeld
	ldr	r0, [r3]
	bl	Func_8097b54
	lsl	r0, #16
	lsr	r0, #16
	ldr	r3, =0xffff
	mov	r8, r0
	cmp	r8, r3
	bne	.L97d4a
	ldr	r3, [r6, #8]
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	add	r3, r11
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r7, #8]
	ldr	r1, [sp, #8]
	mov	r0, r11
	mov	r2, r7
	bl	vec3_translate
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	mov	r0, r10
	bl	_Actor_TravelTo
	mov	r0, r10
	mov	r1, #1
	bl	_Actor_SetAnim
	mov	r0, r10
	str	r5, [r0, #0x24]
	str	r5, [r0, #0x28]
	str	r5, [r0, #0x2c]
	b	.L97ee4
.L97d4a:
	ldr	r3, [r6, #8]
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	add	r3, r11
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r7, #8]
	ldr	r1, [sp, #8]
	mov	r0, r11
	mov	r2, r7
	bl	vec3_translate
	mov	r0, #0x80
	lsl	r0, #10
	mov	r1, r8
	mov	r2, r7
	bl	vec3_translate
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	mov	r0, r10
	bl	_Actor_TravelTo
	mov	r0, r10
	bl	_Actor_WaitMovement
	ldr	r3, [r6, #8]
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, r11
	str	r3, [r7, #8]
	mov	r1, r8
	mov	r2, r7
	bl	vec3_translate
	ldr	r3, [r6, #8]
	add	r5, sp, #0x1c
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, #0x80
	lsl	r0, #14
	mov	r1, r8
	str	r3, [r5, #8]
	mov	r2, r5
	bl	vec3_translate
	mov	r0, r6
	mov	r1, r7
	bl	_TestCollision
	cmp	r0, #0
	bgt	.L97e16
	mov	r0, r6
	mov	r1, r9
	bl	_Func_800d98c
	cmp	r0, #0
	beq	.L97e36
	ldr	r1, [sp, #0x14]
	cmp	r0, r1
	bne	.L97e16
	ldr	r2, [sp, #0x14]
	ldr	r3, [sp, #0x14]
	mov	r1, r9
	ldr	r0, [r2, #8]
	ldr	r4, [r3, #0x10]
	ldr	r2, =0xfff00000
	ldr	r3, [r1]
	and	r0, r2
	and	r3, r2
	and	r4, r2
	cmp	r0, r3
	bne	.L97dee
	ldr	r3, [r1, #8]
	and	r3, r2
	cmp	r4, r3
	beq	.L97e16
.L97dee:
	ldr	r1, [r5]
	ldr	r2, =0xfff00000
	mov	r3, r1
	and	r3, r2
	mov	r12, r2
	cmp	r0, r3
	bne	.L97e36
	ldr	r2, [r5, #8]
	mov	r0, r12
	mov	r3, r2
	and	r3, r0
	cmp	r4, r3
	bne	.L97e36
	ldr	r3, [sp, #0x14]
	add	r3, #0x22
	ldrb	r0, [r3]
	bl	_Func_8011fd8
	cmp	r0, #0
	beq	.L97e32
.L97e16:
	mov	r0, r10
	mov	r1, #4
	bl	_Actor_SetAnim
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #0xf
	and	r3, r2
	cmp	r3, #0
	bne	.L97ee4
	mov	r0, #0x72
	bl	_PlaySound
	b	.L97ee4
.L97e32:
	mov	r1, #1
	str	r1, [sp, #4]
.L97e36:
	mov	r0, #0xaf
	bl	_PlaySound
	ldr	r2, [r7]
	str	r2, [sp, #0x10]
	ldr	r0, [sp, #8]
	ldr	r3, [r7, #8]
	mov	r1, r8
	str	r3, [sp, #0xc]
	sub	r3, r0, r1
	ldr	r2, =.L9f118
	lsl	r3, #16
	lsr	r3, #30
	ldrb	r1, [r2, r3]
	mov	r0, r10
	bl	_Actor_SetAnim
	mov	r0, #0xf
	bl	WaitFrames
	mov	r3, r6
	add	r3, #0x5b
	mov	r2, #0
	strb	r2, [r3]
	ldr	r3, =0x3333
	str	r3, [r6, #0x30]
	str	r3, [r6, #0x34]
	mov	r9, r3
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	mov	r0, r6
	bl	_Actor_TravelTo
	ldr	r1, [sp]
	mov	r3, r10
	mov	r2, r9
	mov	r0, #0
	strb	r0, [r1]
	str	r2, [r3, #0x30]
	str	r2, [r3, #0x34]
	mov	r0, r11
	mov	r1, r8
	mov	r2, r7
	bl	vec3_translate
	ldr	r2, [r7, #4]
	mov	r0, r10
	ldr	r1, [r7]
	add	r2, r11
	ldr	r3, [r7, #8]
	bl	_Actor_TravelTo
	ldr	r0, [sp, #4]
	cmp	r0, #1
	bne	.L97ece
	ldr	r2, [sp, #0x18]
	mov	r1, #0x18
	ldrsh	r0, [r2, r1]
	bl	MapActor_GetActor
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #0xfe
	and	r3, r2
	strb	r3, [r0]
	ldr	r0, [sp, #0x14]
	mov	r3, r9
	str	r3, [r0, #0x30]
	str	r3, [r0, #0x34]
	ldr	r0, [sp, #0x14]
	ldr	r1, [r5]
	ldr	r2, [r5, #4]
	ldr	r3, [r5, #8]
	bl	_Actor_TravelTo
.L97ece:
	mov	r0, r6
	bl	_Actor_WaitMovement
	ldr	r1, [sp, #0x10]
	str	r1, [r6, #8]
	ldr	r2, [sp, #0xc]
	mov	r3, #0
	str	r2, [r6, #0x10]
	str	r3, [r6, #0x24]
	str	r3, [r6, #0x2c]
	b	.L97ef8
.L97ee4:
	mov	r0, #1
	bl	WaitFrames
	ldr	r3, =gKeyPress
	ldr	r5, [r3]
	ldr	r3, =0x303
	and	r5, r3
	cmp	r5, #0
	bne	.L97ef8
	b	.L97d00
.L97ef8:
	ldr	r3, [sp, #0x18]
	add	r3, #0x44
	ldrb	r1, [r3]
	mov	r0, r6
	bl	_Actor_SetColorswap
	ldr	r0, [sp, #0x18]
	ldr	r1, [r0, #0x3c]
	mov	r0, r6
	bl	_Actor_SetScript
	ldr	r1, [sp, #0x18]
	ldr	r3, [r1, #0x38]
	str	r3, [r6, #0x6c]
	bl	Func_8097174
	ldr	r2, [sp, #4]
	cmp	r2, #1
	bne	.L97f32
	ldr	r1, [sp, #0x18]
	mov	r3, #0x18
	ldrsh	r0, [r1, r3]
	bl	MapActor_GetActor
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r0]
.L97f32:
	bl	Func_809748c
	mov	r0, r10
	bl	Func_80981b0
.L97f3c:
	add	sp, #0x34
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Field_Move_Target

.thumb_Func_start Func_97f80
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r0
	mov	r2, #0x40
	add	r2, r6
	sub	sp, #0xc
	mov	r8, r2
.L97f90:
	mov	r3, r8
	mov	r7, #0
	ldrsb	r7, [r3, r7]
	cmp	r7, #0
	bne	.L97fd0
	ldr	r3, [r6, #0x14]
	mov	r5, sp
	str	r3, [r5]
	ldr	r3, [r6, #0x18]
	str	r3, [r5, #8]
	bl	Random
	mov	r1, r0
	lsl	r1, #16
	mov	r0, #0xf0
	mov	r2, r5
	lsl	r0, #13
	lsr	r1, #16
	bl	vec3_translate
	ldr	r3, [r5]
	str	r3, [r6, #0xc]
	ldr	r3, [r5, #8]
	str	r3, [r6, #0x10]
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r6, #0x24]
	str	r3, [r6, #0x20]
	mov	r3, r6
	add	r3, #0x42
	strb	r7, [r3]
	b	.L98002
.L97fd0:
	cmp	r7, #1
	bne	.L97fe8
	mov	r0, r6
	bl	Func_809ba34
	cmp	r0, #0
	bne	.L98020
	mov	r2, r8
	ldrb	r3, [r2]
	add	r3, #1
	strb	r3, [r2]
	b	.L97f90
.L97fe8:
	cmp	r7, #2
	bne	.L9800c
	ldr	r3, [r6, #0x14]
	str	r3, [r6, #0xc]
	ldr	r3, [r6, #0x18]
	str	r3, [r6, #0x10]
	mov	r3, #0x80
	lsl	r3, #3
	mov	r2, r6
	strh	r3, [r6, #0x32]
	add	r2, #0x42
	mov	r3, #1
	strb	r3, [r2]
.L98002:
	mov	r2, r8
	ldrb	r3, [r2]
	add	r3, #1
	strb	r3, [r2]
	b	.L98020
.L9800c:
	cmp	r7, #3
	bne	.L98020
	mov	r0, r6
	bl	Func_809ba34
	cmp	r0, #0
	bne	.L98020
	mov	r0, r6
	bl	Func_809bb34
.L98020:
	add	sp, #0xc
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_97f80

@ RunAbilityWithPaletteSetup
@ Takes no arguments. Uploads the ability palettes with Func_97384, then runs
@ Func_98070 against the caster from [iwram_1f30]+0x10.
.thumb_func_start Field_Move  @ 0x0809802c
	push	{r5, lr}
	ldr	r3, =iwram_3001f30
	ldr	r3, [r3]
	sub	sp, #0xc
	ldr	r5, [r3, #0x10]
	bl	Func_8097384
	mov	r0, r5
	bl	Func_8098070
	mov	r5, r0
	bl	Func_8098184
	cmp	r5, #0
	beq	.L98058
	mov	r0, r5
	mov	r1, #4
	bl	_Actor_SetAnim
	mov	r0, #0x1e
	bl	WaitFrames
.L98058:
	bl	Func_809748c
	mov	r0, r5
	bl	Func_80981b0
	add	sp, #0xc
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Field_Move

@ AnimateCasterRotation
@ r0=caster entity. Spins the caster's facing at +0x06 through a full turn over
@ a fixed number of frames, the wind-up shared by several abilities.
.thumb_func_start Func_8098070  @ 0x08098070
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldrh	r3, [r0, #6]
	mov	r8, r0
	mov	r0, #0x80
	lsl	r0, #6
	mov	r2, r8
	add	r5, r3, r0
	ldr	r1, [r2, #8]
	mov	r3, #0xc0
	ldr	r2, [r2, #0xc]
	mov	r6, #0x80
	lsl	r3, #8
	mov	r0, r8
	lsl	r6, #13
	and	r5, r3
	add	r2, r6
	ldr	r3, [r0, #0x10]
	mov	r0, #0xd7
	bl	CreateParticleActor
	mov	r10, r0
	cmp	r0, #0
	bne	.L980aa
	mov	r0, #0
	b	.L98166
.L980aa:
	mov	r3, #0x80
	mov	r2, r10
	lsl	r3, #7
	str	r3, [r2, #0x1c]
	str	r3, [r2, #0x18]
	ldr	r3, =Func_8097b70
	str	r3, [r2, #0x6c]
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r2, #0x30]
	str	r3, [r2, #0x34]
	mov	r3, #0
	add	r2, #0x55
	strb	r3, [r2]
	mov	r0, r10
	mov	r1, #3
	bl	_Actor_SetAnim
	mov	r0, r10
	mov	r1, r6
	mov	r2, r5
	bl	Func_8096bec
	mov	r3, #7
	mov	r9, r3
.L980dc:
	mov	r0, r8
	ldr	r2, [r0, #0xc]
	mov	r3, #0x80
	lsl	r3, #13
	ldr	r1, [r0, #8]
	add	r2, r3
	ldr	r3, [r0, #0x10]
	ldr	r0, =0x11d
	bl	CreateParticleActor
	mov	r7, r0
	cmp	r7, #0
	beq	.L98152
	ldr	r1, =.L9f0d4
	bl	_Actor_SetScript
	bl	Random
	mov	r3, #0x80
	lsl	r3, #9
	mov	r2, r7
	add	r2, #0x55
	add	r0, r3
	str	r3, [r7, #0x34]
	mov	r3, #2
	str	r0, [r7, #0x30]
	strb	r3, [r2]
	ldr	r3, =0x51e
	str	r3, [r7, #0x48]
	bl	Random
	mov	r5, r0
	bl	Random
	sub	r5, r0
	str	r5, [r7, #0x28]
	bl	Random
	lsl	r6, r0, #1
	add	r6, r0
	mov	r0, #0x80
	lsl	r0, #12
	lsl	r6, #3
	add	r6, r0
	bl	Random
	mov	r5, r0
	bl	Random
	mov	r2, r8
	sub	r5, r0
	ldrh	r3, [r2, #6]
	lsr	r5, #3
	add	r5, r3
	mov	r0, r7
	mov	r1, r6
	mov	r2, r5
	bl	Func_8096bec
.L98152:
	mov	r3, #1
	neg	r3, r3
	add	r9, r3
	mov	r0, r9
	cmp	r0, #0
	bge	.L980dc
	mov	r0, #0x8a
	bl	_PlaySound
	mov	r0, r10
.L98166:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8098070

@ ClampEffectHeight
@ r0=effect instance. Clamps the height word at +0x18 to 0xFFFF and mirrors it
@ into +0x1C, keeping a rising particle from overshooting. Null-safe.
.thumb_func_start Func_8098184  @ 0x08098184
	push	{lr}
	cmp	r0, #0
	beq	.L981a8
	ldr	r2, [r0, #0x18]
	ldr	r3, =0xffff
	cmp	r2, r3
	bgt	.L981a4
	mov	r1, #0x80
	lsl	r1, #5
	mov	r12, r3
.L98198:
	add	r3, r2, r1
	mov	r2, r3
	cmp	r3, r12
	ble	.L98198
	str	r3, [r0, #0x18]
	str	r3, [r0, #0x1c]
.L981a4:
	bl	_Actor_WaitMovement
.L981a8:
	pop	{r0}
	bx	r0
.func_end Func_8098184

@ RunLaunchAbility
@ r0=caster. Plays sound 0x9A and launches the effect upward from the caster,
@ stepping the height by -0x800 per frame. The ~120-instruction body is
@ characterised structurally.
.thumb_func_start Func_80981b0  @ 0x080981b0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r7, r0
	mov	r0, #0x9a
	bl	_PlaySound
	ldr	r5, =0xfffff800
	mov	r2, #0x1e
	mov	r8, r2
.L981c6:
	ldr	r3, [r7, #0xc]
	mov	r2, #0x80
	lsl	r2, #9
	add	r3, r2
	str	r3, [r7, #0xc]
	mov	r2, #0x80
	ldrh	r3, [r7, #6]
	lsl	r2, #6
	add	r3, r2
	strh	r3, [r7, #6]
	ldr	r3, [r7, #0x18]
	add	r3, r5
	str	r3, [r7, #0x18]
	ldr	r3, [r7, #0x1c]
	add	r3, r5
	str	r3, [r7, #0x1c]
	mov	r0, #1
	bl	WaitFrames
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r2, r8
	cmp	r2, #0
	bge	.L981c6
	mov	r2, #0x80
	mov	r3, #7
	lsl	r2, #9
	mov	r8, r3
	mov	r10, r2
.L98202:
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0xc]
	ldr	r3, [r7, #0x10]
	ldr	r0, =0x11d
	bl	CreateParticleActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L9825e
	ldr	r1, =.L9f0d4
	bl	_Actor_SetScript
	bl	Random
	mov	r3, r10
	mov	r2, r6
	add	r2, #0x55
	str	r3, [r6, #0x34]
	add	r0, r10
	mov	r3, #2
	str	r0, [r6, #0x30]
	strb	r3, [r2]
	ldr	r3, =0xa3d
	str	r3, [r6, #0x48]
	bl	Random
	mov	r5, r0
	bl	Random
	sub	r5, r0
	str	r5, [r6, #0x28]
	bl	Random
	lsl	r5, r0, #1
	add	r5, r0
	mov	r2, #0x80
	lsl	r2, #12
	lsl	r5, #3
	add	r5, r2
	bl	Random
	mov	r1, r5
	mov	r2, r0
	mov	r0, r6
	bl	Func_8096bec
.L9825e:
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r2, r8
	cmp	r2, #0
	bge	.L98202
	mov	r0, #0x83
	bl	_PlaySound
	mov	r0, r7
	bl	_DeleteActor
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80981b0

@ FindEntityNearPosition
@ r0=position. Scans the 0x40 entities at [iwram_1e64] for one close to r0 and
@ returns it, or 0. The ability targeting counterpart to Func_d924's collision
@ scan in rom_9000.
.thumb_func_start Func_8098294  @ 0x08098294
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001e64
	ldr	r3, [r3]
	mov	r5, r0
	mov	r12, r3
	mov	r0, #0x3f
.L982a0:
	mov	r2, r12
	ldr	r3, [r2]
	cmp	r3, #0
	beq	.L982c6
	mov	r3, r12
	add	r3, #0x54
	ldrb	r4, [r3]
	cmp	r4, #1
	bne	.L982c6
	ldr	r1, [r2, #0x50]
	ldr	r2, [r1, #0x28]
	mov	r6, #0
	ldrsh	r3, [r2, r6]
	cmp	r3, #0xc8
	bne	.L982c6
	mov	r3, r1
	add	r3, #0x25
	strb	r5, [r2, #5]
	strb	r4, [r3]
.L982c6:
	mov	r2, #0x70
	sub	r0, #1
	add	r12, r2
	cmp	r0, #0
	bge	.L982a0
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8098294

@ PositionCasterForAbility
@ Takes no arguments. Moves the player entity (ewram_240+0x1F4) into the
@ standing position an ability cast requires, aligning it to the target tile.
.thumb_func_start Func_80982dc  @ 0x080982dc
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r0, #0xfa
	ldr	r5, [r3]
	ldr	r3, =gState
	lsl	r0, #1
	add	r3, r0
	ldr	r0, [r3]
	bl	GetFieldActor
	mov	r1, #0xcc
	lsl	r1, #4
	add	r3, r5, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	mov	r4, r0
	cmp	r3, #0
	beq	.L98312
	ldr	r3, =0xcba
	add	r2, r5, r3
	mov	r0, #0
	ldrsh	r3, [r2, r0]
	ldrh	r1, [r2]
	cmp	r3, #0
	beq	.L98312
	sub	r3, r1, #1
	strh	r3, [r2]
.L98312:
	ldr	r1, =0xcbc
	add	r3, r5, r1
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r0, [r4, #8]
	cmp	r0, #0
	bge	.L98324
	ldr	r1, =0xffff
	add	r0, r1
.L98324:
	asr	r0, #16
	ldr	r3, =Func_8000888
	sub	r0, r2, r0
	ldr	r1, =0xd105
	.call_via r3
	ldr	r2, =0xcbe
	add	r3, r5, r2
	mov	r1, r0
	ldr	r2, [r4, #0x10]
	mov	r0, #0
	ldrsh	r6, [r3, r0]
	ldr	r3, [r4, #0xc]
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.L98348
	ldr	r2, =0xffff
	add	r0, r2
.L98348:
	asr	r3, r0, #16
	sub	r3, r6, r3
	mov	r0, r3
	mul	r0, r3
	mov	r2, r1
	mul	r2, r1
	mov	r3, r0
	mov	r1, #0xe1
	add	r2, r3
	lsl	r1, #4
	cmp	r2, r1
	bge	.L9836c
	ldr	r2, =0xcba
	add	r3, r5, r2
	mov	r0, #0
	ldrsh	r3, [r3, r0]
	cmp	r3, #0
	bne	.L98376
.L9836c:
	mov	r1, #0xbf
	lsl	r1, #1
	ldr	r3, =0x2090
	add	r2, r5, r1
	strh	r3, [r2]
.L98376:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80982dc

@ SpawnAbilityParticles
@ Takes no arguments. Creates the particle instances an ability needs, binding
@ each to the effect state and giving it its starting offset. The
@ ~190-instruction body is characterised structurally.
.thumb_func_start Field_Reveal  @ 0x080983a0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r5, =iwram_3001ebc
	mov	r0, #6
	sub	sp, #4
	ldr	r6, [r5]
	bl	Func_8098294
	mov	r0, #8
	bl	Func_808fe38
	ldr	r3, =gState
	mov	r0, #0xfa
	lsl	r0, #1
	add	r0, r3
	mov	r10, r0
	ldr	r0, [r0]
	ldr	r7, [r5, #0x10]
	bl	GetFieldActor
	ldr	r2, =0x52c
	ldr	r3, [r0, #8]
	add	r5, r7, r2
	str	r3, [r5]
	mov	r3, #0xa6
	lsl	r3, #3
	add	r3, r7
	ldr	r2, [r0, #0xc]
	mov	r8, r3
	ldr	r3, [r0, #0x10]
	mov	r0, r8
	sub	r3, r2
	str	r3, [r0]
	mov	r0, #0x80
	lsl	r0, #9
	mov	r1, #0
	bl	Func_8091220
	mov	r1, #1
	ldr	r0, =0x10001
	bl	Func_8091200
	mov	r0, #1
	bl	Func_8091254
	mov	r0, #1
	bl	WaitFrames
	ldr	r0, =0x50000005
	mov	r2, sp
	mov	r1, #8
	bl	Func_808e4b4
	cmp	r0, #0
	beq	.L9841c
	mov	r2, r10
	ldr	r1, [r2]
	ldr	r2, [sp]
	bl	Func_8096b28
.L9841c:
	mov	r0, #0x83
	bl	_PlaySound
	ldr	r0, =0xcb8
	mov	r1, #1
	add	r3, r6, r0
	strh	r1, [r3]
	ldr	r2, [r5]
	cmp	r2, #0
	bge	.L98434
	ldr	r3, =0xffff
	add	r2, r3
.L98434:
	ldr	r0, =0xcbc
	asr	r2, #16
	add	r3, r6, r0
	strh	r2, [r3]
	mov	r3, r8
	ldr	r2, [r3]
	cmp	r2, #0
	bge	.L98448
	ldr	r0, =0xffff
	add	r2, r0
.L98448:
	ldr	r0, =0xcbe
	asr	r2, #16
	add	r3, r6, r0
	strh	r2, [r3]
	ldr	r3, =0xcba
	add	r2, r6, r3
	mov	r3, #0x96
	lsl	r3, #2
	add	r0, #2
	strh	r3, [r2]
	add	r3, r6, r0
	strh	r1, [r3]
	bl	Func_808f32c
	ldr	r2, =0x52a
	mov	r5, #0
	add	r6, r7, r2
.L9846a:
	mov	r0, #1
	bl	WaitFrames
	strh	r5, [r6]
	add	r5, #1
	cmp	r5, #0x12
	ble	.L9846a
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Func_80982dc
	bl	StartTask
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Field_Reveal

@ RunAbilityIntro
@ Takes no arguments. The common opening beat: freezes the player, plays the
@ cast pose and brings the particles up before the ability's own body runs.
.thumb_func_start Func_80984c0  @ 0x080984c0
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f30
	mov	r2, r3
	ldr	r1, [r3]
	sub	r2, #0x64
	sub	r3, #0x74
	ldr	r6, [r3]
	ldr	r7, [r2]
	ldr	r2, =0xcb8
	add	r5, r6, r2
	mov	r8, r1
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	sub	sp, #4
	cmp	r3, #0
	beq	.L9859c
	mov	r0, #0xa7
	bl	_PlaySound
	ldr	r0, =Func_80982dc
	bl	StopTask
	ldr	r1, =0xcba
	mov	r2, #0
	strh	r2, [r5]
	add	r3, r6, r1
	mov	r5, #0x80
	strh	r2, [r3]
	mov	r0, #0
	lsl	r5, #9
	bl	Func_8098294
	mov	r1, #1
	mov	r0, r5
	bl	Func_8091200
	mov	r0, #1
	bl	Func_8091254
	mov	r0, #0
	mov	r1, #0
	bl	Func_8091220
	mov	r1, #0
	mov	r0, r5
	bl	Func_8091200
	mov	r0, #0x1e
	bl	Func_8091254
	mov	r0, #1
	bl	WaitFrames
	ldr	r0, =0x40000005
	mov	r2, sp
	mov	r1, #8
	bl	Func_808e4b4
	cmp	r0, #0
	beq	.L9854c
	ldr	r3, =gState
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r1, [r3]
	ldr	r2, [sp]
	bl	Func_8096b28
.L9854c:
	mov	r3, r8
	add	r3, #0x34
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	bne	.L9859c
	ldr	r3, =0x53e
	ldr	r1, =0x53c
	add	r2, r7, r3
	ldr	r3, .L98578	@ 0
	strb	r3, [r2]
	add	r3, r7, r1
	mov	r2, #1
	add	r1, #1
	strb	r2, [r3]
	add	r3, r7, r1
	strb	r2, [r3]
	mov	r0, #0xa
	bl	WaitFrames
	b	.L9859c

	.align	2, 0
.L98578:
	.word	0
	.pool

.L9859c:
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80984c0

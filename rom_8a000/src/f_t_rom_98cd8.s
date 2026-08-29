	.include "macros.inc"
	.include "gba.inc"

@ RunFreezeAbility
@ Takes no arguments. The freeze field ability: forms the frozen pillar at the
@ target and leaves it as standing geometry. The ~600-instruction body is
@ characterised structurally.
.thumb_func_start Func_98cd8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f30
	ldr	r3, [r3]
	ldr	r0, [r3, #0x14]
	sub	sp, #0x2c
	mov	r9, r3
	str	r0, [sp, #8]
	bl	Func_97384
	mov	r0, #0x82
	bl	_Func_f9080
	add	r1, sp, #0x10
	mov	r5, r9
	mov	r10, r1
	mov	r2, #0xb
	add	r5, #0x58
	mov	r6, r10
	mov	r8, r2
.L98d0a:
	mov	r3, r9
	ldr	r2, [r3, #0x10]
	ldr	r3, [r2, #8]
	str	r3, [r6]
	mov	r0, #0x80
	ldr	r3, [r2, #0xc]
	lsl	r0, #13
	add	r3, r0
	str	r3, [r6, #4]
	ldr	r3, [r2, #0x10]
	mov	r0, r6
	str	r3, [r6, #8]
	bl	Func_974d8
	mov	r1, #0x8e
	ldr	r2, [r6]
	ldr	r3, [r6, #8]
	mov	r0, r5
	lsl	r1, #1
	bl	Func_9ba90
	mov	r0, r5
	ldr	r1, =Func_98b10
	bl	Func_9ba7c
	mov	r0, r5
	mov	r1, #7
	bl	Func_9ba70
	ldr	r0, [r5]
	mov	r1, #9
	bl	_Func_b684
	ldr	r3, =0xb333
	mov	r0, #2
	str	r3, [r5, #0x2c]
	str	r3, [r5, #0x28]
	bl	Func_30f8
	mov	r1, #1
	neg	r1, r1
	add	r8, r1
	mov	r2, r8
	add	r5, #0x48
	cmp	r2, #0
	bge	.L98d0a
	mov	r3, r9
	ldr	r2, [r3, #0x10]
	ldr	r3, [r2, #8]
	mov	r0, r10
	str	r3, [r0]
	mov	r1, #0x80
	ldr	r3, [r2, #0xc]
	lsl	r1, #13
	add	r3, r1
	str	r3, [r0, #4]
	ldr	r3, [r2, #0x10]
	str	r3, [r0, #8]
	mov	r2, r9
	mov	r0, #0x80
	ldr	r1, [r2]
	lsl	r0, #12
	mov	r2, r10
	bl	Func_447c
	mov	r3, r10
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	mov	r0, #0xd7
	ldr	r3, [r3, #8]
	bl	Func_96c80
	mov	r6, r0
	cmp	r6, #0
	bne	.L98db4
	bl	Func_9748c
	b	.L98ff2

	.pool_aligned

.L98db4:
	mov	r3, #0x80
	lsl	r3, #7
	str	r3, [r6, #0x1c]
	str	r3, [r6, #0x18]
	mov	r0, r9
	ldr	r3, [r0]
	strh	r3, [r6, #6]
	mov	r3, #0x80
	lsl	r3, #11
	ldr	r2, =0
	str	r3, [r6, #0x30]
	str	r3, [r6, #0x34]
	mov	r3, r6
	add	r3, #0x55
	strb	r2, [r3]
	mov	r0, r6
	mov	r1, #5
	bl	_Func_c300
	mov	r1, #3
	mov	r0, r6
	bl	_Func_c598
	mov	r1, #0x80
	ldr	r3, [r6, #0x18]
	lsl	r1, #9
	cmp	r3, r1
	bge	.L98e0c
	b	.L98df4

	.pool_aligned

.L98df4:
	mov	r2, #0xa0
	lsl	r2, #3
	add	r3, r2
	str	r3, [r6, #0x1c]
	str	r3, [r6, #0x18]
	mov	r0, #1
	bl	Func_30f8
	ldr	r3, [r6, #0x18]
	ldr	r0, =0xffff
	cmp	r3, r0
	ble	.L98df4
.L98e0c:
	mov	r0, #3
	bl	Func_30f8
	mov	r3, sp
	add	r3, #0x1c
	mov	r1, #0
	mov	r2, #2
	str	r3, [sp, #4]
	mov	r11, r1
	mov	r8, r2
	add	r7, sp, #0x24
.L98e22:
	ldr	r1, [r6, #8]
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x10]
	mov	r0, #0xd7
	bl	Func_96c80
	mov	r5, r0
	str	r0, [r7]
	sub	r7, #4
	cmp	r5, #0
	beq	.L98e70
	mov	r3, #0xf0
	lsl	r3, #8
	str	r3, [r5, #0x1c]
	str	r3, [r5, #0x18]
	mov	r0, r9
	ldr	r3, [r0]
	strh	r3, [r5, #6]
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r5, #0x30]
	str	r3, [r5, #0x34]
	mov	r3, r5
	add	r3, #0x55
	mov	r1, #0
	strb	r1, [r3]
	mov	r0, r5
	mov	r1, #5
	bl	_Func_c300
	mov	r0, r5
	mov	r1, #2
	bl	_Func_c598
	mov	r1, r11
	ldr	r0, [r5, #0x50]
	bl	Func_96c48
	mov	r11, r0
.L98e70:
	mov	r2, #1
	neg	r2, r2
	add	r8, r2
	mov	r3, r8
	cmp	r3, #0
	bge	.L98e22
	mov	r3, r9
	mov	r0, r11
	add	r3, #0x20
	ldrb	r0, [r0, #0x1c]
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	mov	r11, r0
	cmp	r3, #0
	beq	.L98eb8
	mov	r1, r9
	ldr	r2, [r1, #0x10]
	ldr	r3, [r2, #8]
	mov	r0, r10
	str	r3, [r0]
	mov	r1, #0x80
	ldr	r3, [r2, #0xc]
	lsl	r1, #13
	add	r3, r1
	str	r3, [r0, #4]
	ldr	r3, [r2, #0x10]
	str	r3, [r0, #8]
	mov	r2, r9
	mov	r0, #0xe0
	ldr	r1, [r2]
	lsl	r0, #14
	mov	r2, r10
	bl	Func_447c
	b	.L98ece
.L98eb8:
	mov	r0, r9
	ldr	r3, [r0, #4]
	mov	r1, r10
	str	r3, [r1]
	mov	r2, #0x80
	ldr	r3, [r0, #8]
	lsl	r2, #13
	add	r3, r2
	str	r3, [r1, #4]
	ldr	r3, [r0, #0xc]
	str	r3, [r1, #8]
.L98ece:
	mov	r3, r10
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	mov	r0, r6
	ldr	r3, [r3, #8]
	bl	_Func_d14c
	ldr	r1, =L9f12c
	mov	r0, r6
	bl	_Func_c2d8
	ldr	r0, [sp, #4]
	mov	r1, #2
	str	r0, [sp]
	mov	r7, r10
	mov	r8, r1
.L98eee:
	ldr	r3, [sp]
	ldmia	r3!, {r5}
	mov	r2, r3
	str	r2, [sp]
	cmp	r5, #0
	beq	.L98f14
	mov	r0, #3
	bl	Func_30f8
	ldr	r1, [r7]
	mov	r0, r5
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	bl	_Func_d14c
	mov	r0, r5
	ldr	r1, =L9f0b4
	bl	_Func_c2d8
.L98f14:
	mov	r0, #1
	neg	r0, r0
	add	r8, r0
	mov	r1, r8
	cmp	r1, #0
	bge	.L98eee
	ldr	r3, [r6]
	mov	r2, #0
	mov	r8, r2
	cmp	r3, #0
	beq	.L98f40
.L98f2a:
	mov	r0, #1
	bl	Func_30f8
	mov	r3, #1
	add	r8, r3
	mov	r0, r8
	cmp	r0, #0x3b
	bgt	.L98f40
	ldr	r3, [r6]
	cmp	r3, #0
	bne	.L98f2a
.L98f40:
	ldr	r1, [sp, #8]
	cmp	r1, #0
	beq	.L98fb8
	mov	r3, r9
	add	r3, #0x35
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	bne	.L98fb8
	mov	r3, r9
	add	r3, #0x34
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	beq	.L98f68
	mov	r3, #0x80
	lsl	r3, #12
	str	r3, [r1, #0x28]
.L98f68:
	ldr	r2, [sp, #8]
	ldr	r3, [r2, #8]
	mov	r0, r10
	str	r3, [r0]
	ldr	r3, [r2, #0xc]
	str	r3, [r0, #4]
	ldr	r3, [r2, #0x10]
	str	r3, [r0, #8]
	mov	r2, r9
	mov	r0, #0x80
	ldr	r1, [r2]
	lsl	r0, #13
	mov	r2, r10
	bl	Func_447c
	mov	r1, r10
	ldr	r0, [sp, #8]
	bl	_Func_120dc
	cmp	r0, #0
	bne	.L98fb8
	ldr	r0, [sp, #8]
	mov	r1, r10
	bl	_Func_d924
	cmp	r0, #0
	bne	.L98fb8
	ldr	r0, [sp, #8]
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r0, #0x34]
	str	r3, [r0, #0x30]
	mov	r2, r10
	mov	r0, r10
	ldr	r1, [r2]
	ldr	r3, [r0, #8]
	ldr	r2, [r2, #4]
	ldr	r0, [sp, #8]
	bl	_Func_d14c
.L98fb8:
	ldr	r0, =0x50000005
	add	r2, sp, #0xc
	mov	r1, #4
	bl	Func_8e4b4
	cmp	r0, #0
	beq	.L98fd6
	ldr	r3, =ewram_240
	mov	r1, #0xfa
	lsl	r1, #1
	add	r3, r1
	ldr	r1, [r3]
	ldr	r2, [sp, #0xc]
	bl	Func_96b28
.L98fd6:
	mov	r0, #0xa
	bl	Func_30f8
	bl	Func_9748c
	mov	r0, #0x14
	bl	Func_30f8
	mov	r2, r11
	cmp	r2, #0x60
	beq	.L98ff2
	mov	r0, r11
	bl	Func_3f3c
.L98ff2:
	add	sp, #0x2c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_98cd8

@ FlickerEveryEighthFrame
@ r0=entity. Applies a palette change on every eighth frame (bits 0-2 of
@ iwram_1e40), the slow shimmer used on a completed ice pillar.
.thumb_func_start Func_99018
	push	{lr}
	ldr	r3, =iwram_1e40
	ldr	r2, [r3]
	mov	r3, #7
	and	r2, r3
	cmp	r2, #0
	bne	.L9902e
	mov	r1, #2
	bl	_Func_c598
	b	.L99038
.L9902e:
	cmp	r2, #2
	bne	.L99038
	mov	r1, #0
	bl	_Func_c598
.L99038:
	pop	{r0}
	bx	r0
.func_end Func_99018

@ SinkEffectHook
@ r0=entity. Per-frame hook that lowers the entity: subtracts 0x1000 from the
@ height words at +0x18 and +0x1C each frame. Null-safe.
.thumb_func_start Func_99040
	push	{lr}
	cmp	r0, #0
	beq	.L99062
	ldr	r1, =0xfffff000
	ldr	r2, [r0, #0x1c]
	ldr	r3, [r0, #0x18]
	add	r2, r1
	str	r2, [r0, #0x1c]
	mov	r2, #0x80
	add	r3, r1
	lsl	r2, #5
	str	r3, [r0, #0x18]
	cmp	r3, r2
	bgt	.L99062
	ldr	r1, =Data_9f0b0
	bl	_Func_c2d8
.L99062:
	pop	{r0}
	bx	r0
.func_end Func_99040

@ OrbitHookSlow
@ r0=entity. Per-frame hook that advances the phase at +0x64 and moves the
@ particle around its orbit; the slower of the two variants here.
.thumb_func_start Func_99070
	push	{r5, r6, lr}
	mov	r6, r0
	sub	sp, #0xc
	cmp	r6, #0
	beq	.L990c0
	mov	r2, r6
	add	r2, #0x64
	ldrh	r3, [r2]
	sub	r3, #1
	strh	r3, [r2]
	lsl	r3, #16
	asr	r2, r3, #16
	cmp	r2, #0
	beq	.L990bc
	ldr	r3, [r6, #0x38]
	mov	r5, sp
	str	r3, [r5]
	ldr	r3, [r6, #0x3c]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x40]
	str	r3, [r5, #8]
	mov	r3, r6
	add	r3, #0x66
	mov	r4, #0
	ldrsh	r1, [r3, r4]
	lsl	r3, r2, #11
	lsl	r0, r2, #17
	add	r1, r3
	mov	r2, r5
	bl	Func_447c
	ldr	r3, [r5]
	str	r3, [r6, #8]
	ldr	r3, [r5, #4]
	str	r3, [r6, #0xc]
	ldr	r3, [r5, #8]
	str	r3, [r6, #0x10]
	b	.L990c0
.L990bc:
	ldr	r3, =Func_99040
	str	r3, [r6, #0x6c]
.L990c0:
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_99070

@ OrbitHookFast
@ r0=entity. The faster sibling of Func_99070 -- same phase word at +0x64, a
@ larger step per frame.
.thumb_func_start Func_990cc
	push	{r5, r6, lr}
	mov	r6, r0
	sub	sp, #0xc
	cmp	r6, #0
	beq	.L9911c
	mov	r2, r6
	add	r2, #0x64
	ldrh	r3, [r2]
	sub	r3, #1
	strh	r3, [r2]
	lsl	r3, #16
	asr	r2, r3, #16
	cmp	r2, #0
	beq	.L99118
	ldr	r3, [r6, #0x38]
	mov	r5, sp
	str	r3, [r5]
	ldr	r3, [r6, #0x3c]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x40]
	str	r3, [r5, #8]
	mov	r3, r6
	add	r3, #0x66
	mov	r4, #0
	ldrsh	r1, [r3, r4]
	lsl	r3, r2, #11
	lsl	r0, r2, #17
	sub	r1, r3
	mov	r2, r5
	bl	Func_447c
	ldr	r3, [r5]
	str	r3, [r6, #8]
	ldr	r3, [r5, #4]
	str	r3, [r6, #0xc]
	ldr	r3, [r5, #8]
	str	r3, [r6, #0x10]
	b	.L9911c
.L99118:
	ldr	r3, =Func_99040
	str	r3, [r6, #0x6c]
.L9911c:
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_990cc

@ MarkTargetHeld
@ Takes no arguments. Sets the held flag at +0x35 of the effect state when a
@ target is present at +0x14, recording that an object is currently lifted.
.thumb_func_start Func_99128
	push	{lr}
	ldr	r3, =iwram_1f30
	ldr	r2, [r3]
	ldr	r1, [r2, #0x14]
	cmp	r1, #0
	beq	.L99156
	mov	r3, r2
	add	r3, #0x35
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	beq	.L99148
	add	r2, #0x20
	mov	r3, #1
	strb	r3, [r2]
.L99148:
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r1]
	bl	Func_99160
.L99156:
	pop	{r0}
	bx	r0
.func_end Func_99128

@ RunCarryAbility
@ Takes no arguments. Carries a held object with the player as they walk,
@ updating its position each frame until it is released. The
@ ~400-instruction body is characterised structurally.
.thumb_func_start Func_99160
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_1f30
	ldr	r3, [r3]
	ldr	r2, [r3, #0x14]
	sub	sp, #0x10
	mov	r8, r3
	mov	r9, r2
	bl	Func_97384
	mov	r0, #0x73
	bl	_Func_f9080
	mov	r3, #0xf
	add	r7, sp, #4
	mov	r10, r3
.L99186:
	mov	r0, #0xe8
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	bl	Func_96c80
	mov	r6, r0
	cmp	r6, #0
	beq	.L99212
	bl	Func_4458
	mov	r2, #0x80
	lsl	r2, #8
	lsr	r0, #1
	add	r0, r2
	str	r0, [r6, #0x1c]
	str	r0, [r6, #0x18]
	bl	Func_4458
	mov	r3, #1
	and	r0, r3
	cmp	r0, #0
	beq	.L991b8
	ldr	r3, =Func_99070
	b	.L991ba
.L991b8:
	ldr	r3, =Func_990cc
.L991ba:
	str	r3, [r6, #0x6c]
	bl	Func_4458
	mov	r2, r6
	add	r2, #0x64
	mov	r3, #0x3c
	strh	r0, [r6, #6]
	strh	r3, [r2]
	bl	Func_4458
	mov	r3, r6
	add	r3, #0x66
	mov	r1, #9
	strh	r0, [r3]
	mov	r0, r6
	bl	_Func_c598
	mov	r2, r8
	ldr	r3, [r2, #4]
	str	r3, [r7]
	ldr	r3, [r2, #8]
	str	r3, [r7, #4]
	ldr	r3, [r2, #0xc]
	str	r3, [r7, #8]
	bl	Func_4458
	mov	r3, #0x80
	mov	r5, r0
	lsl	r3, #10
	lsl	r5, #2
	add	r5, r3
	bl	Func_4458
	mov	r2, r7
	mov	r1, r0
	mov	r0, r5
	bl	Func_447c
	ldr	r3, [r7]
	str	r3, [r6, #0x38]
	ldr	r3, [r7, #4]
	str	r3, [r6, #0x3c]
	ldr	r3, [r7, #8]
	str	r3, [r6, #0x40]
.L99212:
	mov	r0, #3
	bl	Func_30f8
	mov	r2, #1
	neg	r2, r2
	add	r10, r2
	mov	r3, r10
	cmp	r3, #0
	bge	.L99186
	mov	r0, #0xa
	bl	Func_30f8
	mov	r0, #0x73
	bl	_Func_f9080
	mov	r0, #0x32
	bl	Func_30f8
	mov	r2, r9
	cmp	r2, #0
	beq	.L992c2
	mov	r3, r8
	add	r3, #0x20
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	bne	.L992c2
	mov	r0, #0xd4
	bl	_Func_f9080
	mov	r3, #0xf
	mov	r10, r3
.L99254:
	mov	r1, #7
	mov	r0, r9
	bl	_Func_c598
	mov	r0, #1
	bl	Func_30f8
	mov	r0, r9
	mov	r1, #0
	bl	_Func_c598
	mov	r0, #4
	bl	Func_30f8
	mov	r2, #1
	neg	r2, r2
	add	r10, r2
	mov	r3, r10
	cmp	r3, #0
	bge	.L99254
	mov	r3, r8
	add	r3, #0x34
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	bne	.L99298
	mov	r0, #0xdc
	bl	_Func_f9080
	mov	r0, r9
	mov	r1, #2
	bl	_Func_c300
.L99298:
	ldr	r3, =Func_99018
	mov	r2, r9
	str	r3, [r2, #0x6c]
	ldr	r0, =0x50000005
	mov	r2, sp
	mov	r1, #6
	bl	Func_8e4b4
	cmp	r0, #0
	beq	.L992bc
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r1, [r3]
	ldr	r2, [sp]
	bl	Func_96b28
.L992bc:
	mov	r0, #0x14
	bl	Func_30f8
.L992c2:
	bl	Func_9748c
	add	sp, #0x10
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_99160

@ ComputeOrbitOffset
@ r0=entity. Converts the phase counter at +0x64 into a position offset through
@ Func_2322 (the sine table), giving the particle its circular path.
.thumb_func_start Func_992f0
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r6, r5
	add	r6, #0x64
	mov	r3, #0
	ldrsh	r0, [r6, r3]
	lsl	r0, #9
	bl	Func_2322
	mov	r1, r0
	mov	r0, #0x80
	ldr	r3, =Func_888
	lsl	r0, #11
	.call_via r3
	ldr	r3, [r5, #0x38]
	add	r3, r0
	str	r3, [r5, #8]
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	lsl	r3, #16
	asr	r1, r3, #16
	mov	r2, r1
	add	r2, #0x80
	mov	r3, r2
	cmp	r2, #0
	bge	.L9932e
	mov	r3, r1
	add	r3, #0xff
.L9932e:
	asr	r3, #7
	lsl	r3, #7
	sub	r3, r2, r3
	strh	r3, [r6]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_992f0

@ ApplyParticleOffset
@ r0=entity. Adds the computed orbit offset to the particle's position, relative
@ to the effect origin in [iwram_1f30]. Null-safe.
.thumb_func_start Func_99340
	push	{r5, r6, lr}
	ldr	r3, =iwram_1f30
	mov	r6, r0
	sub	sp, #0xc
	ldr	r1, [r3]
	cmp	r6, #0
	beq	.L9939e
	mov	r2, r6
	add	r2, #0x64
	ldrh	r3, [r2]
	sub	r3, #1
	strh	r3, [r2]
	lsl	r3, #16
	asr	r2, r3, #16
	cmp	r2, #0
	beq	.L99396
	ldr	r3, [r1, #4]
	mov	r5, sp
	str	r3, [r5]
	mov	r0, #0xa0
	ldr	r3, [r1, #8]
	lsl	r0, #12
	add	r3, r0
	str	r3, [r5, #4]
	ldr	r3, [r1, #0xc]
	str	r3, [r5, #8]
	mov	r3, r6
	add	r3, #0x66
	mov	r4, #0
	ldrsh	r1, [r3, r4]
	lsl	r3, r2, #11
	lsl	r0, r2, #16
	add	r1, r3
	mov	r2, r5
	bl	Func_447c
	ldr	r3, [r5]
	str	r3, [r6, #8]
	ldr	r3, [r5, #4]
	str	r3, [r6, #0xc]
	ldr	r3, [r5, #8]
	str	r3, [r6, #0x10]
	b	.L9939e
.L99396:
	ldr	r1, =Data_9f0b0
	mov	r0, r6
	bl	_Func_c2d8
.L9939e:
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_99340

@ AnimateParticleSpiral
@ r0=entity. Advances the particle along a spiral -- phase at +0x64 driving both
@ the angle and a growing radius. The ~110-instruction body is characterised
@ structurally.
.thumb_func_start Func_993b0
	push	{r5, r6, r7, lr}
	mov	r5, r0
	ldr	r3, =iwram_1f30
	mov	r7, r5
	add	r7, #0x64
	ldr	r6, [r3]
	mov	r2, #0
	ldrsh	r0, [r7, r2]
	mov	r3, #1
	neg	r3, r3
	sub	sp, #0xc
	cmp	r0, r3
	beq	.L99410
	lsl	r0, #10
	bl	Func_2322
	mov	r1, r0
	mov	r0, #0xc0
	ldr	r3, =Func_888
	lsl	r0, #11
	.call_via r3
	ldr	r3, [r6, #4]
	add	r3, r0
	str	r3, [r5, #8]
	mov	r2, #0x80
	ldr	r3, [r6, #8]
	lsl	r2, #13
	add	r3, r2
	str	r3, [r5, #0xc]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #0x10]
	ldrh	r3, [r7]
	add	r3, #1
	strh	r3, [r7]
	lsl	r3, #16
	asr	r1, r3, #16
	mov	r2, r1
	add	r2, #0x40
	mov	r3, r2
	cmp	r2, #0
	bge	.L99408
	mov	r3, r1
	add	r3, #0x7f
.L99408:
	asr	r3, #6
	lsl	r3, #6
	sub	r3, r2, r3
	strh	r3, [r7]
.L99410:
	ldr	r3, =iwram_1e40
	mov	r1, #3
	ldr	r0, [r3]
	bl	Func_b50_from_thumb
	cmp	r0, #0
	bne	.L9949c
	ldr	r3, [r5, #8]
	mov	r6, sp
	str	r3, [r6]
	mov	r2, #0x80
	ldr	r3, [r5, #0xc]
	lsl	r2, #10
	add	r3, r2
	str	r3, [r6, #4]
	ldr	r3, [r5, #0x10]
	str	r3, [r6, #8]
	bl	Func_4458
	lsl	r5, r0, #1
	add	r5, r0
	bl	Func_4458
	lsl	r5, #1
	mov	r1, r0
	mov	r2, r6
	mov	r0, r5
	bl	Func_447c
	ldr	r0, =0x11d
	ldr	r1, [r6]
	ldr	r2, [r6, #4]
	ldr	r3, [r6, #8]
	bl	Func_96c80
	mov	r5, r0
	cmp	r5, #0
	beq	.L9949c
	ldr	r3, =Func_992f0
	str	r3, [r5, #0x6c]
	ldr	r3, =0x9999
	mov	r2, r5
	add	r2, #0x55
	str	r3, [r5, #0x1c]
	str	r3, [r5, #0x18]
	mov	r3, #2
	strb	r3, [r2]
	mov	r3, #0xe5
	lsl	r3, #1
	str	r3, [r5, #0x48]
	bl	Func_4458
	mov	r3, r5
	lsr	r0, #9
	add	r3, #0x64
	strh	r0, [r3]
	ldr	r3, [r5, #8]
	mov	r0, r5
	str	r3, [r5, #0x38]
	mov	r1, #9
	bl	_Func_c598
	mov	r2, r5
	add	r2, #0x5e
	mov	r3, #0x48
	strh	r3, [r2]
	ldr	r1, =Data_9f0b0
	mov	r0, r5
	bl	_Func_c2d8
.L9949c:
	add	sp, #0xc
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_993b0

@ RunAbilityOutro_Wrapper
@ Runs Func_994d0 and then Func_97174, restoring the caster's hook and palette
@ once the ability finishes.
.thumb_func_start Func_994c0
	push	{lr}
	bl	Func_994d0
	bl	Func_97174
	pop	{r0}
	bx	r0
.func_end Func_994c0

@ RunAbilityOutro
@ Takes no arguments. The common closing beat: retracts the particles, returns
@ the caster to idle and releases the effect instances. The ~400-instruction
@ body is characterised structurally.
.thumb_func_start Func_994d0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_1f30
	ldr	r3, [r3]
	mov	r10, r3
	ldr	r6, [r3, #0x10]
	bl	Func_97384
	mov	r2, #0
	mov	r8, r2
	mov	r7, #0
.L994ec:
	ldr	r2, [r6, #0xc]
	mov	r3, #0x80
	lsl	r3, #14
	add	r2, r3
	ldr	r1, [r6, #8]
	ldr	r3, [r6, #0x10]
	mov	r0, #0xe9
	bl	Func_96c80
	mov	r5, r0
	cmp	r5, #0
	beq	.L9952c
	ldr	r3, =0xb333
	str	r3, [r5, #0x1c]
	str	r3, [r5, #0x18]
	ldr	r3, =Func_99340
	mov	r2, r5
	str	r3, [r5, #0x6c]
	add	r2, #0x64
	mov	r3, #0x78
	strh	r3, [r2]
	lsl	r3, r7, #13
	add	r2, #2
	strh	r3, [r2]
	sub	r2, #0x11
	mov	r3, #4
	strb	r3, [r2]
	mov	r1, r8
	ldr	r0, [r5, #0x50]
	bl	Func_96c48
	mov	r8, r0
.L9952c:
	mov	r0, #1
	add	r7, #1
	bl	Func_30f8
	cmp	r7, #7
	ble	.L994ec
	mov	r2, r8
	ldrb	r2, [r2, #0x1c]
	mov	r0, #0x82
	mov	r9, r2
	bl	_Func_f9080
	mov	r0, #0x6e
	bl	Func_30f8
	mov	r0, #0xe9
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0
	bl	Func_96c80
	mov	r6, r0
	mov	r5, r6
	cmp	r6, #0
	beq	.L99588
	ldr	r3, =0xb333
	str	r3, [r6, #0x1c]
	str	r3, [r6, #0x18]
	mov	r2, r10
	ldr	r3, [r2, #4]
	str	r3, [r6, #8]
	ldr	r3, [r2, #8]
	mov	r2, #0x80
	lsl	r2, #13
	add	r3, r2
	str	r3, [r6, #0xc]
	mov	r2, r10
	ldr	r3, [r2, #0xc]
	mov	r2, r6
	str	r3, [r6, #0x10]
	add	r2, #0x55
	mov	r3, #4
	strb	r3, [r2]
	mov	r1, #7
	bl	_Func_c598
.L99588:
	mov	r0, #0x83
	bl	_Func_f9080
	mov	r0, #0xc
	bl	Func_30f8
	cmp	r6, #0
	beq	.L995c6
	mov	r3, #3
	mov	r7, #0
	mov	r8, r3
.L9959e:
	mov	r3, r7
	mov	r2, r8
	and	r3, r2
	cmp	r3, #0
	beq	.L995b2
	mov	r0, r5
	mov	r1, #9
	bl	_Func_c598
	b	.L995ba
.L995b2:
	mov	r0, r5
	mov	r1, #0xa
	bl	_Func_c598
.L995ba:
	mov	r0, #2
	add	r7, #1
	bl	Func_30f8
	cmp	r7, #0x1d
	ble	.L9959e
.L995c6:
	mov	r0, r5
	mov	r1, #0
	bl	_Func_c598
	mov	r0, #0x54
	bl	_Func_f9080
	cmp	r5, #0
	beq	.L99600
	ldr	r3, =Func_993b0
	mov	r2, r6
	str	r3, [r6, #0x6c]
	add	r2, #0x64
	mov	r3, #0
	strh	r3, [r2]
	mov	r3, r10
	add	r3, #0x20
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	beq	.L995fa
	mov	r0, #0x80
	bl	Func_30f8
	b	.L99600
.L995fa:
	mov	r0, #0xc0
	bl	Func_30f8
.L99600:
	cmp	r6, #0
	beq	.L99636
	ldr	r3, =0xffff
	mov	r2, r6
	add	r2, #0x64
	strh	r3, [r2]
	mov	r3, #0xa0
	lsl	r3, #11
	str	r3, [r6, #0x30]
	ldr	r3, =0x6666
	sub	r2, #0xa
	str	r3, [r6, #0x34]
	mov	r3, #0
	strb	r3, [r2]
	mov	r1, #0xc0
	mov	r2, #0xe8
	lsl	r1, #16
	lsl	r2, #8
	mov	r0, r6
	bl	Func_96bec
	mov	r0, r6
	bl	_Func_ca6c
	mov	r0, r6
	bl	_Func_c0f4
.L99636:
	mov	r3, r9
	cmp	r3, #0x60
	beq	.L99642
	mov	r0, r9
	bl	Func_3f3c
.L99642:
	mov	r2, r10
	ldr	r3, [r2, #0x24]
	cmp	r3, #0
	beq	.L9964e
	bl	_call_via_r3
.L9964e:
	bl	Func_9748c
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_994d0

@ RestorePlayerAfterAbility
@ Takes no arguments. Returns the player entity (ewram_240+0x1F4) to normal
@ control after a cast: clears the freeze flag, restores the idle animation and
@ re-enables input.
.thumb_func_start Func_99678
	push	{r5, r6, lr}
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r5, =iwram_1ebc
	ldr	r0, [r3]
	ldr	r6, [r5]
	bl	Func_8ba1c
	mov	r2, #0xcf
	lsl	r2, #1
	add	r3, r6, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r5, #0x4c
	ldr	r5, [r5]
	cmp	r3, #3
	bne	.L996c8
	ldr	r3, [r0, #8]
	cmp	r3, #0
	bge	.L996a8
	ldr	r2, =0x1fffff
	add	r3, r2
.L996a8:
	ldr	r0, [r0, #0x10]
	asr	r2, r3, #21
	mov	r1, #0x1f
	and	r2, r1
	cmp	r0, #0
	bge	.L996b8
	ldr	r3, =0x1fffff
	add	r0, r3
.L996b8:
	asr	r3, r0, #21
	and	r3, r1
	lsl	r3, #5
	add	r3, r2, r3
	ldr	r2, =ewram_20000
	lsl	r3, #2
	add	r5, r3, r2
	b	.L99706
.L996c8:
	mov	r2, r0
	add	r2, #0x22
	ldrb	r3, [r2]
	cmp	r3, #2
	bhi	.L996e4
	mov	r2, r3
	lsl	r3, r2, #1
	add	r3, r2
	mov	r2, #0x98
	lsl	r3, #4
	lsl	r2, #1
	add	r3, r2
	ldr	r5, [r5, r3]
	b	.L996e6
.L996e4:
	ldr	r5, =ewram_10000
.L996e6:
	ldr	r3, [r0, #8]
	cmp	r3, #0
	bge	.L996f0
	ldr	r2, =0xfffff
	add	r3, r2
.L996f0:
	ldr	r0, [r0, #0x10]
	asr	r2, r3, #20
	cmp	r0, #0
	bge	.L996fc
	ldr	r3, =0xfffff
	add	r0, r3
.L996fc:
	asr	r3, r0, #20
	lsl	r3, #7
	add	r3, r2, r3
	lsl	r3, #2
	add	r5, r3
.L99706:
	ldrb	r3, [r5, #2]
	cmp	r3, #0xfb
	beq	.L99716
	mov	r3, #0xbf
	lsl	r3, #1
	add	r2, r6, r3
	ldr	r3, =0x2092
	strh	r3, [r2]
.L99716:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_99678

@ SaveAbilityResult
@ Takes no arguments. Records the outcome of the cast into ewram_240 so the map
@ script and save state reflect whatever the ability changed.
.thumb_func_start Func_99738
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =ewram_240
	mov	r1, #0xfa
	lsl	r1, #1
	add	r3, r1
	ldr	r0, [r3]
	bl	Func_8ba1c
	mov	r5, r0
	ldr	r6, [r5, #0x50]
	ldr	r2, [r6, #0x28]
	mov	r0, #0x9a
	mov	r10, r2
	bl	_Func_f9080
	ldr	r0, =Func_99678
	bl	Func_4278
	mov	r0, r5
	mov	r1, #0
	bl	_Func_c300
	mov	r3, #0
	str	r3, [r5, #0x6c]
	mov	r3, #0x25
	add	r3, r6
	add	r6, #0x26
	mov	r9, r6
	mov	r11, r3
	mov	r1, #1
	mov	r7, #0
	mov	r6, r11
	mov	r8, r1
	mov	r5, r9
.L99788:
	mov	r2, #7
	mov	r3, r10
	strb	r2, [r3, #5]
	mov	r1, r8
	mov	r3, #2
	strb	r1, [r6]
	mov	r0, #2
	strb	r3, [r5]
	bl	Func_30f8
	mov	r2, r8
	mov	r3, #0
	strb	r2, [r6]
	mov	r0, #2
	strb	r3, [r5]
	add	r7, #1
	bl	Func_30f8
	cmp	r7, #4
	bls	.L99788
	mov	r1, #0
	mov	r2, #7
	mov	r5, r11
	mov	r7, #0
	mov	r8, r1
	mov	r6, #1
	mov	r11, r2
.L997be:
	mov	r1, r10
	mov	r3, r11
	strb	r3, [r1, #5]
	mov	r2, r8
	mov	r3, r9
	strb	r6, [r5]
	mov	r0, #2
	strb	r2, [r3]
	bl	Func_30f8
	mov	r1, r8
	mov	r2, r10
	strb	r1, [r2, #5]
	strb	r6, [r5]
	mov	r0, #2
	add	r7, #1
	bl	Func_30f8
	cmp	r7, #4
	bls	.L997be
	mov	r1, r9
	mov	r3, #1
	strb	r3, [r1]
	ldr	r3, =ewram_240
	mov	r1, #0x93
	lsl	r1, #2
	mov	r2, #0
	add	r3, r1
	strh	r2, [r3]
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_99738

@ HasPendingAbilityResult
@ Takes no arguments. Returns whether the pending-result halfword at
@ ewram_240+0x24C is non-zero -- i.e. whether a cast left something for the map
@ script to act on.
.thumb_func_start Func_99810
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0x93
	lsl	r2, #2
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	beq	.L9982c
	mov	r1, #0xc8
	ldr	r0, =Func_99678
	lsl	r1, #4
	bl	Func_41d8
.L9982c:
	pop	{r0}
	bx	r0
.func_end Func_99810

@ ApplyAbilityResultToScene
@ Takes no arguments. Applies the recorded ability result to the live scene:
@ updates the affected slots and map tiles so the change persists. The
@ ~230-instruction body is characterised structurally.
.thumb_func_start Func_99838
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	sub	sp, #8
	str	r3, [sp, #4]
	mov	r2, #0xfa
	ldr	r3, =ewram_240
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	Func_8ba1c
	mov	r5, r0
	ldr	r6, [r5, #0x50]
	ldr	r3, [r6, #0x28]
	mov	r0, #0x82
	mov	r9, r3
	bl	_Func_f9080
	mov	r0, r5
	mov	r1, #0
	bl	_Func_c300
	mov	r2, r6
	mov	r3, #0
	add	r2, #0x25
	add	r6, #0x26
	str	r3, [r5, #0x6c]
	mov	r11, r6
	mov	r8, r3
	str	r2, [sp]
	mov	r3, #1
	mov	r6, r2
	mov	r10, r3
	mov	r5, r11
.L9988a:
	mov	r3, r9
	mov	r2, #7
	strb	r2, [r3, #5]
	mov	r7, #2
	mov	r2, r10
	strb	r2, [r6]
	mov	r0, #2
	strb	r7, [r5]
	bl	Func_30f8
	mov	r3, r10
	mov	r2, #0
	strb	r3, [r6]
	mov	r0, #2
	strb	r2, [r5]
	bl	Func_30f8
	mov	r3, #1
	add	r8, r3
	mov	r2, r8
	cmp	r2, #9
	bls	.L9988a
	mov	r3, #0
	mov	r8, r3
	mov	r2, r8
	mov	r3, r9
	strb	r2, [r3, #5]
	mov	r2, r11
	strb	r7, [r2]
	ldr	r3, [sp]
	ldr	r5, =Func_99678
	mov	r6, #1
	mov	r1, #0xc8
	strb	r6, [r3]
	lsl	r1, #4
	mov	r0, r5
	bl	Func_41d8
	ldr	r3, =ewram_240
	mov	r2, #0x93
	lsl	r2, #2
	add	r3, r2
	strh	r6, [r3]
	bl	_call_via_r5
	mov	r2, #0xbf
	ldr	r3, [sp, #4]
	lsl	r2, #1
	add	r5, r3, r2
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	ldr	r2, =0x2092
	cmp	r3, r2
	bne	.L998fe
	bl	Func_99738
	mov	r3, r8
	strh	r3, [r5]
.L998fe:
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_99838

@ FallToGroundHook
@ r0=entity. Per-frame hook that drops the entity until its y (+0x0C) reaches
@ the ground height cached at +0x14, then sets the wait timer at +0x5E to end
@ the sequence.
.thumb_func_start Func_99920
	push	{r5, r6, r7, lr}
	mov	r6, r0
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	cmp	r2, r3
	bgt	.L9999a
	mov	r2, r6
	add	r2, #0x5e
	mov	r3, #2
	strh	r3, [r2]
	ldr	r1, =Data_9f0b0
	bl	_Func_c2d8
	mov	r5, #0
	mov	r7, #0
	str	r5, [r6, #0x6c]
	b	.L99984
.L99942:
	mov	r3, #0x80
	lsl	r3, #8
	mov	r2, r5
	add	r2, #0x55
	str	r3, [r5, #0x1c]
	str	r3, [r5, #0x18]
	mov	r3, #2
	strb	r3, [r2]
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r5, #0x28]
	bl	Func_4458
	ldr	r3, =0x13333
	add	r0, r3
	str	r0, [r5, #0x30]
	bl	Func_4458
	mov	r1, #0x80
	mov	r2, r0
	lsl	r1, #14
	mov	r0, r5
	bl	Func_96bec
	mov	r2, r5
	add	r2, #0x5e
	mov	r3, #6
	strh	r3, [r2]
	mov	r0, r5
	ldr	r1, =Data_9f0b0
	bl	_Func_c2d8
	add	r7, #1
.L99984:
	cmp	r7, #2
	bgt	.L9999a
	ldr	r1, [r6, #8]
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x10]
	mov	r0, #0xf0
	bl	Func_96c80
	mov	r5, r0
	cmp	r5, #0
	bne	.L99942
.L9999a:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_99920

@ BounceScatterHook
@ r0=entity. Per-frame hook that drops the entity by 0x4CCC and gives it a
@ random sideways nudge from Func_4458 -- the debris scatter after a break.
.thumb_func_start Func_999a8
	push	{r5, r6, lr}
	mov	r6, r0
	ldr	r2, =0xffffb334
	ldr	r3, [r6, #0xc]
	add	r3, r2
	str	r3, [r6, #0xc]
	bl	Func_4458
	mov	r5, r0
	bl	Func_4458
	ldr	r3, [r6, #8]
	sub	r5, r0
	add	r3, r5
	str	r3, [r6, #8]
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x14]
	cmp	r2, r3
	bgt	.L999d6
	ldr	r1, =Data_9f0b0
	mov	r0, r6
	bl	_Func_c2d8
.L999d6:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_999a8

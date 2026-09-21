	.include "macros.inc"
	.include "gba.inc"

@ BuildRewardSummary
@ r0.. = parameters. Collects the post-battle rewards from each combatant's
@ record via _Func_77394, _Func_78b9c and _Func_7a5b0.
.thumb_func_start Func_80b9470  @ 0x080b9470
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r7, r1
	sub	sp, #0x10
	mov	r11, r0
	cmp	r7, #0
	ble	.Lb94e4
	mov	r5, r11
	mov	r6, r7
.Lb948c:
	mov	r1, #6
	ldrsh	r3, [r5, r1]
	cmp	r3, #5
	bne	.Lb94dc
	mov	r2, #0
	ldrsh	r0, [r5, r2]
	bl	_GetUnit
	ldrh	r2, [r5, #8]
	ldr	r3, =0xf
	lsl	r0, r2, #16
	asr	r0, #24
	mov	r1, #0xff
	and	r1, r2
	and	r0, r3
	bl	_Func_807a5b0
	bl	_GetMoveInfo
	ldrb	r2, [r0, #3]
	mov	r3, r2
	add	r3, #0xd2
	mov	r1, #0x80
	lsl	r3, #24
	lsl	r1, #17
	cmp	r3, r1
	bls	.Lb94c8
	mov	r3, r2
	cmp	r3, #0x35
	bne	.Lb94dc
.Lb94c8:
	ldrh	r3, [r5, #4]
	ldr	r2, =0x2710
	add	r3, r2
	strh	r3, [r5, #4]
	b	.Lb94dc

	.pool_aligned

.Lb94dc:
	sub	r6, #1
	add	r5, #0x10
	cmp	r6, #0
	bne	.Lb948c
.Lb94e4:
	sub	r7, #1
	mov	r9, r7
.Lb94e8:
	mov	r3, #0
	mov	r7, r9
	mov	r10, r3
	cmp	r7, #0
	ble	.Lb9538
	lsl	r3, r7, #4
	add	r3, r11
	ldr	r1, =Func_8001af8
	mov	r5, r3
	mov	r8, r1
	sub	r5, #0x10
	mov	r6, r3
.Lb9500:
	mov	r3, #0x14
	ldrsh	r2, [r5, r3]
	mov	r1, #4
	ldrsh	r3, [r5, r1]
	cmp	r2, r3
	ble	.Lb952e
	mov	r0, sp
	mov	r1, r6
	mov	r2, #0x10
	bl	_call_via_r8
	mov	r1, r5
	mov	r2, #0x10
	mov	r0, r6
	bl	_call_via_r8
	mov	r2, #0x10
	mov	r0, r5
	mov	r1, sp
	bl	_call_via_r8
	mov	r2, #1
	add	r10, r2
.Lb952e:
	sub	r7, #1
	sub	r5, #0x10
	sub	r6, #0x10
	cmp	r7, #0
	bgt	.Lb9500
.Lb9538:
	mov	r3, r10
	cmp	r3, #0
	bne	.Lb94e8
	add	sp, #0x10
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80b9470

@ FadeBattleMusicOut
@ r0.. = parameters. Ramps the music down through Func_63bc / .gcc2_compiled. a frame
@ at a time.
.thumb_func_start Func_80b9554  @ 0x080b9554
	push	{r5, r6, r7, lr}
	mov	r7, r9
	push	{r7}
	sub	sp, #4
	mov	r3, sp
	mov	r2, r9
	str	r2, [r3]
	mov	r7, r2
	sub	r3, r7, #4
	ldr	r0, [r3]
	mov	r1, #0x14
	bl	Func_80063bc
	mov	r3, #1
	mov	r5, #0x96
	neg	r3, r3
	mov	r6, #0
	lsl	r5, #1
	cmp	r0, r3
	bne	.Lb95a0
	b	.Lb95f4
.Lb957e:
	mov	r0, #1
	sub	r5, #1
	bl	WaitFrames
	cmp	r5, #0
	blt	.Lb95e2
	ldr	r3, =iwram_3001f64
	ldrh	r2, [r3]
	mov	r3, #3
	and	r3, r2
	cmp	r3, #3
	beq	.Lb959e
	add	r6, #1
	cmp	r6, #0x18
	ble	.Lb95a0
	b	.Lb95e2
.Lb959e:
	mov	r6, #0
.Lb95a0:
	bl	Func_80064f4
	cmp	r0, #0
	bne	.Lb957e
	mov	r3, r7
	sub	r3, #8
	ldr	r1, [r3]
	cmp	r1, #0
	beq	.Lb95f2
	sub	r3, #4
	ldr	r0, [r3]
	bl	Func_80063bc
	mov	r2, #1
	neg	r2, r2
	cmp	r0, r2
	bne	.Lb95ea
	b	.Lb95f4
.Lb95c4:
	mov	r0, #1
	sub	r5, #1
	bl	WaitFrames
	cmp	r5, #0
	blt	.Lb95e2
	ldr	r3, =iwram_3001f64
	ldrh	r2, [r3]
	mov	r3, #3
	and	r3, r2
	cmp	r3, #3
	beq	.Lb95e8
	add	r6, #1
	cmp	r6, #0x18
	ble	.Lb95ea
.Lb95e2:
	mov	r0, #1
	neg	r0, r0
	b	.Lb95f4
.Lb95e8:
	mov	r6, #0
.Lb95ea:
	bl	Func_80064f4
	cmp	r0, #0
	bne	.Lb95c4
.Lb95f2:
	mov	r0, #0
.Lb95f4:
	add	sp, #4
	pop	{r3}
	mov	r9, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80b9554

@ FadeBattleMusicIn
@ r0.. = parameters. The counterpart to Func_b9554, using Func_6408.
.thumb_func_start Func_80b9604  @ 0x080b9604
	push	{r5, r6, r7, lr}
	mov	r7, r9
	mov	r6, r8
	push	{r6, r7}
	mov	r1, r9
	sub	sp, #4
	mov	r8, r1
	mov	r3, sp
	mov	r7, r8
	str	r1, [r3]
	sub	r7, #4
	ldr	r0, [r7]
	bl	Func_8006408
	mov	r2, #1
	mov	r5, #0x96
	neg	r2, r2
	mov	r6, #0
	lsl	r5, #1
	cmp	r0, r2
	bne	.Lb965a
	b	.Lb970c
.Lb9630:
	ldr	r3, =ewram_2002238
	ldrh	r3, [r3]
	cmp	r3, #0x14
	bhi	.Lb9704
	mov	r0, #1
	sub	r5, #1
	bl	WaitFrames
	cmp	r5, #0
	blt	.Lb9704
	ldr	r3, =iwram_3001f64
	ldrh	r2, [r3]
	mov	r3, #3
	and	r3, r2
	cmp	r3, #3
	beq	.Lb9658
	add	r6, #1
	cmp	r6, #0x18
	ble	.Lb965a
	b	.Lb9704
.Lb9658:
	mov	r6, #0
.Lb965a:
	bl	Func_80064f4
	cmp	r0, #0
	bne	.Lb9630
	ldr	r3, =ewram_2002238
	ldrh	r3, [r3]
	cmp	r3, #0x14
	bne	.Lb9704
	mov	r3, #0x10
	neg	r3, r3
	add	r3, r8
	mov	r9, r3
	ldr	r3, [r7]
	ldr	r2, [r3]
	mov	r1, r9
	str	r2, [r1]
	ldr	r3, [r3]
	cmp	r3, #0
	beq	.Lb970a
	mov	r3, r8
	mov	r2, r8
	sub	r3, #0x14
	sub	r2, #0xc
	ldr	r3, [r3]
	ldr	r0, [r2]
	lsl	r3, #4
	add	r0, r3
	bl	Func_8006408
	mov	r2, #1
	neg	r2, r2
	cmp	r0, r2
	bne	.Lb96de
	b	.Lb970c
.Lb969e:
	ldr	r3, =ewram_2002238
	ldrh	r3, [r3]
	mov	r8, r3
	mov	r3, r9
	ldr	r0, [r3]
	lsl	r0, #4
	add	r0, #0x13
	mov	r1, #0x14
	bl	__udivsi3
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #2
	cmp	r8, r3
	bhi	.Lb9704
	mov	r0, #1
	sub	r5, #1
	bl	WaitFrames
	cmp	r5, #0
	blt	.Lb9704
	ldr	r3, =iwram_3001f64
	ldrh	r2, [r3]
	mov	r3, #3
	and	r3, r2
	cmp	r3, #3
	beq	.Lb96dc
	add	r6, #1
	cmp	r6, #0x18
	ble	.Lb96de
	b	.Lb9704
.Lb96dc:
	mov	r6, #0
.Lb96de:
	bl	Func_80064f4
	cmp	r0, #0
	bne	.Lb969e
	mov	r1, r9
	ldr	r0, [r1]
	ldr	r3, =ewram_2002238
	lsl	r0, #4
	ldrh	r3, [r3]
	add	r0, #0x13
	mov	r1, #0x14
	mov	r8, r3
	bl	__udivsi3
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #2
	cmp	r8, r3
	beq	.Lb970a
.Lb9704:
	mov	r0, #1
	neg	r0, r0
	b	.Lb970c
.Lb970a:
	mov	r0, #0
.Lb970c:
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r9, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80b9604

@ RunBattleMusicSequence
@ r0.. = parameters. Sequences the battle music through Func_b9554 and
@ Func_b9604, with a Func_4970 scratch released by free.
.thumb_func_start Func_80b9724  @ 0x080b9724
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e74
	ldr	r3, [r3]
	sub	sp, #0x14
	mov	r9, r3
	add	r3, sp, #4
	add	r2, sp, #8
	mov	r8, r3
	mov	r7, sp
	str	r0, [r2]
	mov	r3, #0
	mov	r0, r8
	str	r1, [r7]
	str	r3, [r0]
	ldr	r0, [r7]
	lsl	r0, #4
	mov	r1, #0x14
	add	r0, #0x13
	mov	r11, r2
	bl	__udivsi3
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #2
	mov	r0, #0x28
	str	r3, [sp, #0xc]
	bl	Func_8004970
	ldr	r3, [r7]
	add	r5, sp, #0x10
	mov	r10, r5
	str	r0, [r5]
	cmp	r3, #0
	ble	.Lb97b0
	mov	r2, r11
	mov	r6, r9
	ldr	r1, [r2]
	mov	r0, #1
	add	r6, #0x50
	mov	r4, r3
.Lb9780:
	mov	r2, #0
	ldrsh	r3, [r1, r2]
	mov	r2, r9
	add	r3, #0x48
	ldrb	r3, [r2, r3]
	strh	r3, [r1, #2]
	ldrb	r3, [r6]
	cmp	r3, #0
	bne	.Lb97a0
	ldrh	r2, [r1, #4]
	mov	r3, r0
	and	r3, r2
	cmp	r3, #0
	beq	.Lb97a8
	add	r3, r2, #1
	b	.Lb97a6
.Lb97a0:
	ldrh	r2, [r1, #4]
	mov	r3, r0
	orr	r3, r2
.Lb97a6:
	strh	r3, [r1, #4]
.Lb97a8:
	sub	r4, #1
	add	r1, #0x10
	cmp	r4, #0
	bne	.Lb9780
.Lb97b0:
	mov	r3, r9
	add	r3, #0x52
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.Lb9890
	mov	r3, r9
	add	r3, #0x50
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.Lb980e
	mov	r3, r10
	ldr	r2, [r3]
	ldr	r3, [r7]
	str	r3, [r2]
	bl	_RPGRandom
	mov	r1, r10
	ldr	r3, [r1]
	str	r0, [r3, #4]
	ldr	r2, =REG_IME
	ldrh	r1, [r2]
	strh	r2, [r2]
	ldr	r4, =sRPGRNGState
	ldr	r3, =gRNGState
	add	r5, sp, #0x10
	ldr	r3, [r3]
	ldr	r0, [r5]
	str	r3, [r0, #8]
	str	r3, [r4]
	strh	r1, [r2]
	add	r2, sp, #0x14
	mov	r9, r2
	bl	Func_80b9554
	cmp	r0, #0
	blt	.Lb9890
	add	r3, sp, #0x14
	mov	r9, r3
	bl	Func_80b9604
	cmp	r0, #0
	blt	.Lb9890
	ldr	r3, [r5]
	ldr	r3, [r3]
	mov	r0, r8
	str	r3, [r0]
	b	.Lb9848
.Lb980e:
	add	r1, sp, #0x14
	mov	r9, r1
	bl	Func_80b9604
	cmp	r0, #0
	blt	.Lb9890
	mov	r3, r10
	ldr	r2, [r3]
	ldr	r3, [r2]
	mov	r0, r8
	str	r3, [r0]
	ldr	r3, [r7]
	add	r1, sp, #0x14
	str	r3, [r2]
	mov	r9, r1
	bl	Func_80b9554
	cmp	r0, #0
	blt	.Lb9890
	bl	_RPGRandom
	mov	r2, r10
	ldr	r1, [r2]
	ldr	r3, [r1, #4]
	cmp	r0, r3
	bne	.Lb9890
	ldr	r2, =sRPGRNGState
	ldr	r3, [r1, #8]
	str	r3, [r2]
.Lb9848:
	mov	r3, r8
	ldr	r1, [r3]
	cmp	r1, #0
	ble	.Lb9870
	mov	r0, r11
	ldr	r3, [r7]
	ldr	r2, [r0]
	lsl	r3, #4
	ldr	r6, .Lb987c	@ 0x80
	add	r0, r3, r2
	mov	r4, r1
.Lb985e:
	ldrh	r3, [r0, #2]
	strh	r3, [r0]
	ldrh	r3, [r0, #0xa]
	sub	r4, #1
	eor	r3, r6
	strh	r3, [r0, #0xa]
	add	r0, #0x10
	cmp	r4, #0
	bne	.Lb985e
.Lb9870:
	ldr	r0, [r5]
	bl	free
	mov	r1, r8
	ldr	r0, [r1]
	b	.Lb98a2

	.align	2, 0
.Lb987c:
	.word	0x80
	.pool

.Lb9890:
	bl	Func_800651c
	bl	Func_8006358
	ldr	r0, [r5]
	bl	free
	mov	r0, #1
	neg	r0, r0
.Lb98a2:
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80b9724

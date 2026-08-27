	.include "macros.inc"
	.include "gba.inc"

@ RunRangedSequence
@ r0.. = parameters. The projectile counterpart to Func_b9ec0, aiming with
@ atan2 (atan2) and pacing with Func_af0. 328 lines; traced structurally.
.thumb_func_start Func_80ba2c0  @ 0x080ba2c0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x6c
	str	r0, [sp, #0xc]
	ldr	r3, =iwram_3001f00
	ldr	r3, [r3]
	ldr	r1, [sp, #0xc]
	str	r3, [sp, #4]
	ldrb	r0, [r1]
	bl	GetBattleActor
	ldr	r3, [r0]
	ldr	r1, [r3, #0x10]
	ldr	r0, [r3, #8]
	bl	atan2
	ldr	r3, =0xffffe000
	lsl	r0, #16
	ldr	r1, [sp, #0xc]
	lsr	r0, #16
	add	r2, r0, r3
	ldrb	r3, [r1]
	cmp	r3, #7
	bls	.Lba300
	mov	r3, #0xc0
	lsl	r3, #7
	add	r2, r0, r3
.Lba300:
	ldr	r3, =0x7fff
	ldr	r1, =0xffffe000
	and	r2, r3
	add	r3, r2, r1
	lsr	r2, r3, #31
	add	r3, r2
	mov	r1, #0x80
	asr	r3, #1
	lsl	r1, #6
	add	r2, r3, r1
	ldr	r1, [sp, #4]
	ldr	r3, [r1]
	cmp	r3, r2
	bne	.Lba326
	str	r2, [r1]
	mov	r0, #5
	bl	WaitFrames
	b	.Lba330
.Lba326:
	ldr	r3, [sp, #4]
	mov	r0, #0xa
	str	r2, [r3]
	bl	WaitFrames
.Lba330:
	mov	r0, #0
	mov	r1, #0
	bl	Func_80c10e8
	add	r7, sp, #0x18
	ldr	r0, [sp, #0xc]
	mov	r1, r7
	bl	InitAnimContext
	ldr	r3, [r7]
	cmp	r3, #0x87
	bne	.Lba35a
	ldr	r3, =iwram_3001e74
	ldr	r3, [r3]
	add	r3, #0x41
	ldrb	r0, [r3]
	mov	r3, #2
	neg	r3, r3
	and	r0, r3
	bl	_Func_801f200
.Lba35a:
	ldr	r0, [r7, #8]
	bl	_GetUnit
	mov	r5, r0
	mov	r1, #0x24
	ldrsh	r0, [r7, r1]
	bl	_GetUnit
	ldr	r3, [sp, #0xc]
	add	r3, #0x2c
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	ldr	r2, [sp, #0xc]
	str	r3, [sp, #8]
	mov	r3, #0x1e
	ldrsb	r3, [r2, r3]
	mov	r6, #0
	cmp	r3, #0
	bne	.Lba384
	mov	r6, #1
.Lba384:
	ldr	r3, [sp, #0xc]
	ldrb	r0, [r3]
	bl	GetBattleActor
	mov	r1, #0
	ldr	r0, [r0]
	bl	Func_80b7f70
	ldr	r3, [r0, #0x28]
	mov	r2, #1
	mov	r1, #0
	ldrsh	r0, [r3, r1]
	mov	r1, #2
	bl	_Func_800be20
	mov	r2, #0x94
	lsl	r2, #1
	add	r3, r5, r2
	mov	r9, r0
	ldrb	r0, [r3]
	bl	GetEnemyAttackAnimParam
	mov	r3, r0
	lsl	r3, #16
	mov	r2, #0x24
	ldrsh	r1, [r7, r2]
	ldr	r0, [r7, #8]
	mov	r2, r9
	bl	Func_80b82c4
	ldr	r0, [r7, #8]
	bl	GetBattleActor
	mov	r1, #0x10
	ldr	r0, [r0]
	bl	_Actor_SetAnimSpeed
	mov	r3, #0x24
	ldrsh	r0, [r7, r3]
	bl	GetBattleActor
	ldrh	r3, [r7, #0x24]
	cmp	r3, #7
	bhi	.Lba3f0
	mov	r3, #1
	b	.Lba3f2

	.pool_aligned

.Lba3f0:
	mov	r3, #0
.Lba3f2:
	str	r3, [r7, #4]
	ldr	r1, .Lba424	@ 0xf0
	ldr	r3, =REG_WIN0H
	ldr	r2, .Lba428	@ 0x1088
	strh	r1, [r3]
	add	r3, #4
	strh	r2, [r3]
	sub	r3, #2
	strh	r1, [r3]
	add	r3, #4
	strh	r2, [r3]
	ldr	r2, =REG_WININ
	ldr	r3, .Lba42c	@ 0x3537
	strh	r3, [r2]
	ldr	r3, .Lba430	@ 0x3f21
	add	r2, #2
	strh	r3, [r2]
	mov	r1, #0x80
	lsl	r1, #19
	ldrh	r3, [r1]
	ldr	r2, .Lba434	@ 0x6000
	orr	r3, r2
	strh	r3, [r1]
	b	.Lba440

	.align	2, 0
.Lba424:
	.word	0xf0
.Lba428:
	.word	0x1088
.Lba42c:
	.word	0x3537
.Lba430:
	.word	0x3f21
.Lba434:
	.word	0x6000
	.pool

.Lba440:
	cmp	r6, #0
	beq	.Lba484
	mov	r0, #0xa
	bl	WaitFrames
	mov	r1, #0x24
	ldrsh	r0, [r7, r1]
	bl	Func_80b8178
	mov	r0, #2
	bl	WaitFrames
	mov	r0, #4
	bl	WaitFrames
	mov	r0, #0xa
	bl	WaitFrames
	ldr	r2, [sp, #0xc]
	mov	r0, #0
	ldrb	r1, [r2, #2]
	bl	Func_80bbabc
	ldr	r1, =0x853
	mov	r0, #4
	bl	Func_80bbabc
	bl	Func_80bb938
	mov	r3, #0x24
	ldrsh	r0, [r7, r3]
	bl	Func_80b8000
	b	.Lba562
.Lba484:
	mov	r1, #0
	str	r1, [sp]
	str	r1, [r7, #0x1c]
	ldr	r2, [sp, #0xc]
	ldr	r3, [r2, #0x58]
	cmp	r3, #0
	beq	.Lba496
	mov	r3, #1
	str	r3, [r7, #0x1c]
.Lba496:
	ldr	r3, [sp, #8]
	cmp	r3, #0
	beq	.Lba4be
	ldr	r3, [r7]
	add	r3, #0xc8
	str	r3, [r7]
	ldr	r2, [sp, #4]
	mov	r1, #1
	str	r1, [sp]
	str	r1, [r2, #0x14]
	ldr	r3, [r7, #8]
	add	r0, sp, #0x10
	strh	r3, [r0]
	ldr	r3, [r7, #0xc]
	strh	r3, [r0, #2]
	mov	r3, #0xff
	strh	r3, [r0, #4]
	mov	r1, #0
	bl	CreateBattleSpriteOverlays
.Lba4be:
	mov	r3, #8
	neg	r3, r3
	add	r9, r3
	mov	r1, r9
	cmp	r1, #0
	bgt	.Lba4ce
	mov	r2, #1
	mov	r9, r2
.Lba4ce:
	mov	r3, #0
	mov	r1, r9
	mov	r11, r3
	cmp	r1, #0
	beq	.Lba50e
	mov	r8, r7
	mov	r10, r3
.Lba4dc:
	ldr	r2, [sp]
	cmp	r2, #0
	beq	.Lba4fc
	mov	r3, r8
	mov	r1, r9
	mov	r0, r10
	ldr	r5, [r3, #8]
	ldr	r6, [r3, #0xc]
	bl	__divsi3
	mov	r2, r0
	add	r2, #0x64
	mov	r0, r5
	mov	r1, r6
	bl	Func_80c0df4
.Lba4fc:
	mov	r0, #1
	bl	WaitFrames
	mov	r2, #1
	mov	r1, #0x1e
	add	r11, r2
	add	r10, r1
	cmp	r11, r9
	bne	.Lba4dc
.Lba50e:
	mov	r1, #0xc8
	ldr	r0, =Func_80bd898
	lsl	r1, #4
	bl	StartTask
	ldr	r3, [r7]
	cmp	r3, #0
	beq	.Lba53a
	ldr	r1, [sp, #0xc]
	mov	r2, #0x80
	ldr	r3, [r1, #0x58]
	lsl	r2, #7
	and	r3, r2
	cmp	r3, #0
	beq	.Lba534
	mov	r0, r7
	bl	_Anim_Attack
	b	.Lba53a
.Lba534:
	mov	r0, r7
	bl	_Anim_Func
.Lba53a:
	bl	Func_80be02c
	ldr	r2, [sp, #8]
	cmp	r2, #0
	beq	.Lba55a
	ldr	r1, [sp, #4]
	mov	r3, #0
	str	r3, [r1, #0x14]
	bl	Func_80b6cb0
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0x64
	bl	Func_80c0cec
.Lba55a:
	mov	r2, #0x24
	ldrsh	r0, [r7, r2]
	bl	Func_80b8000
.Lba562:
	ldr	r0, [r7, #8]
	bl	Func_80b8000
	mov	r0, #0
	add	sp, #0x6c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80ba2c0

@ RunImpactSequence
@ r0.. = parameters. Plays the moment of impact -- orientation via Func_b8000,
@ shadow via .gcc2_compiled., animation via Func_b82c4.
.thumb_func_start Func_80ba584  @ 0x080ba584
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f00
	mov	r6, r0
	ldrb	r0, [r6]
	sub	sp, #0x54
	mov	r8, r1
	ldr	r5, [r3]
	bl	GetBattleActor
	ldr	r3, [r0]
	ldr	r1, [r3, #0x10]
	ldr	r0, [r3, #8]
	bl	atan2
	ldr	r1, =0xffffe000
	lsl	r0, #16
	ldrb	r3, [r6]
	lsr	r0, #16
	add	r2, r0, r1
	cmp	r3, #7
	bls	.Lba5b8
	mov	r3, #0xc0
	lsl	r3, #7
	add	r2, r0, r3
.Lba5b8:
	ldr	r3, =0x7fff
	ldr	r1, =0xffffe000
	and	r2, r3
	add	r3, r2, r1
	lsr	r2, r3, #31
	add	r3, r2
	mov	r1, #0x80
	asr	r3, #1
	lsl	r1, #6
	add	r2, r3, r1
	ldr	r3, [r5]
	cmp	r3, r2
	bne	.Lba5dc
	str	r2, [r5]
	mov	r0, #5
	bl	WaitFrames
	b	.Lba5e4
.Lba5dc:
	str	r2, [r5]
	mov	r0, #0x14
	bl	WaitFrames
.Lba5e4:
	mov	r0, #0
	mov	r1, #0
	bl	Func_80c10e8
	mov	r5, sp
	mov	r1, r5
	mov	r0, r6
	bl	InitAnimContext
	ldr	r0, [r5, #8]
	bl	_GetUnit
	ldrb	r0, [r6, #2]
	bl	_GetUnit
	mov	r7, #2
	mov	r2, r8
	ldrb	r0, [r6]
	and	r7, r2
	bl	GetBattleActor
	mov	r1, #0
	ldr	r0, [r0]
	bl	Func_80b7f70
	ldr	r3, [r0, #0x28]
	mov	r2, #1
	mov	r1, #0
	ldrsh	r0, [r3, r1]
	mov	r1, #2
	bl	_Func_800be20
	mov	r3, #0
	mov	r2, r0
	ldrb	r1, [r6, #2]
	ldr	r0, [r5, #8]
	bl	Func_80b82c4
	ldr	r0, [r5, #8]
	bl	GetBattleActor
	mov	r1, #0x10
	ldr	r0, [r0]
	bl	_Actor_SetAnimSpeed
	ldrb	r0, [r6, #2]
	bl	GetBattleActor
	ldrb	r3, [r6, #2]
	cmp	r3, #7
	bhi	.Lba64e
	mov	r3, #1
	b	.Lba650
.Lba64e:
	mov	r3, #0
.Lba650:
	str	r3, [r5, #4]
	cmp	r7, #0
	beq	.Lba67c
	mov	r0, #0xa
	bl	WaitFrames
	ldrb	r0, [r6, #2]
	bl	Func_80b8178
	mov	r0, #2
	bl	WaitFrames
	mov	r0, #4
	bl	WaitFrames
	mov	r0, #0xa
	bl	WaitFrames
	ldrb	r0, [r6, #2]
	bl	Func_80b8000
	b	.Lba68c
.Lba67c:
	mov	r0, r5
	bl	_Anim_Attack
	bl	Func_80bb938
	ldrb	r0, [r6, #2]
	bl	Func_80b8000
.Lba68c:
	ldr	r0, [r5, #8]
	bl	Func_80b8000
	mov	r0, #0
	add	sp, #0x54
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80ba584

@ RunSpecialSequence
@ r0.. = parameters. A longer action sequence with its own per-frame task
@ (StartTask). 290 lines; traced structurally.
.thumb_func_start Func_80ba6ac  @ 0x080ba6ac
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_3001f00
	ldr	r1, [r3]
	ldrb	r3, [r0]
	mov	r8, r2
	sub	sp, #0x58
	mov	r10, r0
	ldr	r2, =0xffffe000
	cmp	r3, #4
	bhi	.Lba6cc
	mov	r2, #0x80
	lsl	r2, #6
.Lba6cc:
	ldr	r3, [r1]
	cmp	r3, r2
	beq	.Lba6d4
	str	r2, [r1]
.Lba6d4:
	add	r5, sp, #4
	mov	r1, r5
	mov	r0, r10
	bl	InitAnimContext
	mov	r1, #0
	mov	r0, #0
	bl	Func_80c10e8
	ldr	r0, [r5, #8]
	bl	GetBattleActor
	ldr	r0, [r0]
	mov	r1, #3
	mov	r9, r0
	bl	_Actor_SetAnim
	mov	r1, #0x10
	mov	r0, r9
	bl	_Actor_SetAnimSpeed
	mov	r1, r10
	ldrb	r3, [r1, #2]
	cmp	r3, #7
	bhi	.Lba70a
	mov	r3, #1
	b	.Lba70c
.Lba70a:
	mov	r3, #0
.Lba70c:
	str	r3, [r5, #4]
	ldr	r3, [r5, #0x14]
	mov	r7, #0
	mov	r2, r5
	cmp	r3, #0
	beq	.Lba75e
	mov	r6, #0
.Lba71a:
	lsl	r3, r7, #1
	add	r3, #0x24
	ldrsh	r0, [r2, r3]
	bl	GetBattleActor
	mov	r1, #0
	ldr	r0, [r0]
	bl	Func_80b7f70
	mov	r3, r0
	add	r3, #0x27
	ldrb	r3, [r3]
	sub	r3, #1
	mov	r1, #0
	cmp	r3, #0
	beq	.Lba752
	mov	r12, r3
	add	r3, r6, r5
	mov	r2, r3
	add	r2, #0x34
	add	r0, #0x28
.Lba744:
	ldmia	r0!, {r3}
	ldrb	r3, [r3, #5]
	add	r1, #1
	strb	r3, [r2]
	add	r2, #1
	cmp	r1, r12
	bne	.Lba744
.Lba752:
	ldr	r3, [r5, #0x14]
	add	r7, #1
	add	r6, #4
	mov	r2, r5
	cmp	r7, r3
	bne	.Lba71a
.Lba75e:
	mov	r1, #0xc8
	ldr	r0, =Func_80bd898
	lsl	r1, #4
	bl	StartTask
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.Lba7c0
	mov	r7, #0
	mov	r6, #0
.Lba772:
	ldr	r3, =iwram_3001e74
	ldr	r3, [r3]
	cmp	r7, #0x13
	bgt	.Lba792
	ldr	r2, =0x544
	ldr	r1, =0x644
	add	r0, r3, r2
	mov	r2, #0x80
	lsl	r2, #9
	add	r3, r1
	sub	r2, r6
	str	r2, [r3]
	ldr	r1, =0x50000c0
	mov	r3, #0x80
	bl	UploadBGPalette
.Lba792:
	mov	r0, #1
	bl	WaitFrames
	ldr	r2, =0x444
	add	r7, #1
	add	r6, r2
	cmp	r7, #0x13
	ble	.Lba772
	mov	r6, r10
	ldr	r3, [r6, #0x58]
	mov	r2, #0x80
	lsl	r2, #7
	and	r3, r2
	cmp	r3, #0
	beq	.Lba7b8
	mov	r0, r5
	bl	_Anim_Attack
	b	.Lba7c6
.Lba7b8:
	mov	r0, r5
	bl	_Anim_Func
	b	.Lba7c6
.Lba7c0:
	mov	r0, #0x3c
	bl	WaitFrames
.Lba7c6:
	bl	Func_80be02c
	mov	r6, r5
	mov	r0, r9
	mov	r1, #1
	bl	_Actor_SetAnim
	ldr	r3, [r6, #0x14]
	mov	r7, #0
	cmp	r3, #0
	beq	.Lba7f4
	mov	r2, #0x24
.Lba7de:
	ldrsh	r0, [r6, r2]
	str	r2, [sp]
	bl	Func_80b8000
	mov	r5, r6
	ldr	r2, [sp]
	ldr	r3, [r5, #0x14]
	add	r7, #1
	add	r2, #2
	cmp	r7, r3
	bne	.Lba7de
.Lba7f4:
	mov	r3, r8
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	bl	_GetUnit
	mov	r1, r8
	mov	r7, #8
	ldrsh	r3, [r1, r7]
	lsl	r3, #1
	add	r3, #0xd8
	mov	r6, r0
	ldrh	r5, [r6, r3]
	mov	r0, r5
	bl	_GetItemInfo
	ldrb	r2, [r0, #0xc]
	mov	r3, r2
	cmp	r3, #1
	bne	.Lba87e
	mov	r3, r8
	mov	r6, #8
	ldrsh	r1, [r3, r6]
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	bl	_Func_80788c4
	mov	r1, r8
	mov	r7, #8
	ldrsh	r5, [r1, r7]
	cmp	r0, #2
	bne	.Lba8da
	ldr	r3, =iwram_3001e74
	ldr	r0, [r3]
	mov	r4, #0
.Lba838:
	mov	r2, #0xbc
	lsl	r1, r4, #4
	lsl	r2, #2
	add	r3, r1, r2
	add	r3, r0, r3
	mov	r6, #2
	ldrsh	r3, [r3, r6]
	cmp	r3, #2
	bne	.Lba876
	mov	r7, #0xbb
	lsl	r7, #2
	add	r3, r1, r7
	ldrsh	r2, [r0, r3]
	mov	r6, r8
	mov	r7, #0
	ldrsh	r3, [r6, r7]
	cmp	r2, r3
	bne	.Lba876
	mov	r7, #0xbd
	lsl	r7, #2
	add	r1, r7
	ldrsh	r2, [r0, r1]
	ldrh	r3, [r0, r1]
	cmp	r2, r5
	bne	.Lba86e
	ldr	r3, =0xffff
	b	.Lba874
.Lba86e:
	cmp	r2, r5
	ble	.Lba876
	sub	r3, #1
.Lba874:
	strh	r3, [r0, r1]
.Lba876:
	add	r4, #1
	cmp	r4, #0x13
	bls	.Lba838
	b	.Lba8da
.Lba87e:
	lsl	r3, r2, #24
	lsr	r3, #24
	cmp	r3, #2
	bne	.Lba8c0
	bl	_RPGRandom
	mov	r3, #7
	and	r0, r3
	cmp	r0, #0
	bne	.Lba8da
	mov	r1, r8
	mov	r7, #8
	ldrsh	r3, [r1, r7]
	lsl	r3, #1
	add	r3, #0xd8
	ldrh	r1, [r6, r3]
	mov	r0, #2
	bl	Func_80bbabc
	ldr	r1, =0x81c
	mov	r0, #4
	bl	Func_80bbabc
	mov	r3, r8
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	mov	r6, #8
	ldrsh	r1, [r3, r6]
	bl	_BreakItem
	bl	Func_80bb938
	b	.Lba8da
.Lba8c0:
	cmp	r3, #4
	bne	.Lba8da
	ldr	r3, =0x1ff
	and	r3, r5
	cmp	r3, #0xb8
	bne	.Lba8ce
	mov	r5, #0xb9
.Lba8ce:
	mov	r1, r8
	mov	r7, #8
	ldrsh	r3, [r1, r7]
	lsl	r3, #1
	add	r3, #0xd8
	strh	r5, [r6, r3]
.Lba8da:
	mov	r0, #0
	add	sp, #0x58
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80ba6ac

@ CountActorParts
@ r0 = actor. Iterates .gcc2_compiled. until it returns 0 and reports how many parts
@ the actor has.
.thumb_func_start Func_80ba918  @ 0x080ba918
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r3, #0xff
	mov	r8, r0
	mov	r7, r1
	mov	r6, #0
	mov	r10, r3
	b	.Lba95e
.Lba92c:
	ldr	r2, [r0, #0x28]
	ldrb	r3, [r2, #0x16]
	mov	r4, r10
	orr	r3, r4
	strb	r3, [r2, #0x16]
	mov	r3, r0
	add	r3, #0x27
	mov	r1, r0
	ldrb	r0, [r3]
	add	r1, #0x2c
	strb	r7, [r2, #5]
	cmp	r0, #1
	ble	.Lba95c
	mov	r5, #0
	mov	r4, #0xff
	sub	r0, #1
.Lba94c:
	ldmia	r1!, {r2}
	ldrb	r3, [r2, #0x16]
	sub	r0, #1
	orr	r3, r4
	strb	r5, [r2, #5]
	strb	r3, [r2, #0x16]
	cmp	r0, #0
	bne	.Lba94c
.Lba95c:
	add	r6, #1
.Lba95e:
	mov	r0, r8
	mov	r1, r6
	bl	Func_80b7f70
	cmp	r0, #0
	bne	.Lba92c
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80ba918

@ RunSummonSequence
@ r0.. = parameters. The largest of the action sequences at 298 lines, with its
@ own task and atan2 aiming. Traced structurally.
.thumb_func_start Func_80ba978  @ 0x080ba978
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f00
	mov	r7, r0
	ldr	r5, [r3]
	mov	r2, #0x80
	ldr	r3, [r7, #0x58]
	lsl	r2, #11
	and	r3, r2
	sub	sp, #0x58
	mov	r10, r1
	cmp	r3, #0
	beq	.Lba9aa
	ldrb	r3, [r7]
	ldr	r2, =0xffffe000
	cmp	r3, #7
	bls	.Lba9a2
	mov	r2, #0xa0
	lsl	r2, #7
.Lba9a2:
	mov	r3, #0x3c
	str	r2, [r5]
	str	r3, [r5, #4]
	b	.Lbaa28
.Lba9aa:
	ldrb	r0, [r7]
	bl	GetBattleActor
	ldr	r3, [r0]
	ldr	r1, [r3, #0x10]
	ldr	r0, [r3, #8]
	bl	atan2
	ldrb	r4, [r7]
	lsl	r0, #16
	ldr	r2, =0xffffe800
	lsr	r0, #16
	mov	r3, r4
	add	r1, r0, r2
	cmp	r3, #7
	bls	.Lba9d0
	mov	r3, #0xc0
	lsl	r3, #5
	add	r1, r0, r3
.Lba9d0:
	lsl	r3, r1, #16
	asr	r1, r3, #16
	mov	r3, r4
	cmp	r3, #7
	bhi	.Lba9e0
	mov	r3, #0x80
	lsl	r3, #6
	b	.Lba9e2
.Lba9e0:
	ldr	r3, =0xffffe000
.Lba9e2:
	sub	r3, r1
	lsl	r2, r3, #1
	add	r2, r3
	cmp	r2, #0
	bge	.Lba9ee
	add	r2, #3
.Lba9ee:
	asr	r3, r2, #2
	add	r1, r3
	ldrb	r3, [r7, #2]
	mov	r2, r4
	cmp	r3, #7
	bhi	.Lbaa08
	mov	r3, #0
	cmp	r2, #7
	bhi	.Lbaa02
	mov	r3, #1
.Lbaa02:
	cmp	r3, #0
	bne	.Lbaa14
	b	.Lbaa20
.Lbaa08:
	mov	r3, #0
	cmp	r2, #7
	bls	.Lbaa10
	mov	r3, #1
.Lbaa10:
	cmp	r3, #0
	beq	.Lbaa20
.Lbaa14:
	mov	r1, #0x90
	mov	r3, r4
	lsl	r1, #6
	cmp	r3, #7
	bls	.Lbaa20
	ldr	r1, =0xffffdc00
.Lbaa20:
	ldr	r3, [r5]
	cmp	r3, r1
	beq	.Lbaa28
	str	r1, [r5]
.Lbaa28:
	ldr	r3, [r7, #0x58]
	mov	r2, #0x80
	lsl	r2, #12
	and	r3, r2
	cmp	r3, #0
	beq	.Lbaa46
	ldrb	r3, [r7]
	ldr	r2, =0xffffe000
	cmp	r3, #7
	bls	.Lbaa40
	mov	r2, #0x80
	lsl	r2, #6
.Lbaa40:
	mov	r3, #0x3c
	str	r2, [r5]
	str	r3, [r5, #4]
.Lbaa46:
	add	r5, sp, #4
	mov	r0, r7
	mov	r1, r5
	bl	InitAnimContext
	mov	r6, r10
	mov	r3, #1
	and	r6, r3
	cmp	r6, #0
	beq	.Lbaa5c
	str	r3, [r5, #0x1c]
.Lbaa5c:
	mov	r1, #0
	mov	r0, #0
	bl	Func_80c10e8
	ldr	r3, =iwram_3001e74
	ldr	r3, [r3]
	add	r3, #0x41
	ldrb	r0, [r3]
	mov	r3, #2
	neg	r3, r3
	and	r0, r3
	bl	_Func_801f200
	ldr	r0, [r5, #8]
	bl	GetBattleActor
	ldr	r0, [r0]
	mov	r1, #3
	mov	r8, r0
	bl	_Actor_SetAnim
	mov	r1, #0x10
	mov	r0, r8
	bl	_Actor_SetAnimSpeed
	mov	r0, #0x9a
	bl	_PlaySound
	mov	r3, #2
	mov	r1, r10
	and	r3, r1
	cmp	r3, #0
	beq	.Lbaaac
	ldr	r0, [r5, #8]
	ldr	r1, [r7, #0x50]
	mov	r2, #1
	mov	r3, #0
	bl	Anim_MoveIntro
	b	.Lbaabc
.Lbaaac:
	cmp	r6, #0
	bne	.Lbaabc
	ldr	r0, [r5, #8]
	ldr	r1, [r7, #0x50]
	mov	r2, #0
	mov	r3, #0
	bl	Anim_MoveIntro
.Lbaabc:
	ldrb	r3, [r7, #2]
	cmp	r3, #7
	bhi	.Lbaac6
	mov	r3, #1
	b	.Lbaac8
.Lbaac6:
	mov	r3, #0
.Lbaac8:
	str	r3, [r5, #4]
	ldr	r3, [r5, #0x14]
	mov	r4, #0
	mov	r2, r5
	cmp	r3, #0
	beq	.Lbab1e
	mov	r6, #0
.Lbaad6:
	lsl	r3, r4, #1
	add	r3, #0x24
	ldrsh	r0, [r2, r3]
	str	r4, [sp]
	bl	GetBattleActor
	mov	r1, #0
	ldr	r0, [r0]
	bl	Func_80b7f70
	mov	r3, r0
	add	r3, #0x27
	ldrb	r3, [r3]
	sub	r3, #1
	mov	r1, #0
	ldr	r4, [sp]
	cmp	r3, #0
	beq	.Lbab12
	mov	r12, r3
	add	r3, r6, r5
	mov	r2, r3
	add	r2, #0x34
	add	r0, #0x28
.Lbab04:
	ldmia	r0!, {r3}
	ldrb	r3, [r3, #5]
	add	r1, #1
	strb	r3, [r2]
	add	r2, #1
	cmp	r1, r12
	bne	.Lbab04
.Lbab12:
	ldr	r3, [r5, #0x14]
	add	r4, #1
	add	r6, #4
	mov	r2, r5
	cmp	r4, r3
	bne	.Lbaad6
.Lbab1e:
	ldr	r3, [r7, #0x5c]
	cmp	r3, #0
	beq	.Lbab4c
	cmp	r3, #1
	bne	.Lbab3a
	ldrb	r1, [r7]
	mov	r0, #0
	bl	Func_80bbabc
	ldr	r1, =0x856
	mov	r0, #4
	bl	Func_80bbabc
	b	.Lbab42
.Lbab3a:
	ldr	r1, =0x855
	mov	r0, #4
	bl	Func_80bbabc
.Lbab42:
	bl	Func_80bb938
	bl	Func_80c1a14
	b	.Lbabaa
.Lbab4c:
	mov	r1, #0xc8
	ldr	r0, =Func_80bd898
	lsl	r1, #4
	bl	StartTask
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.Lbab78
	ldr	r3, [r7, #0x58]
	mov	r2, #0x80
	lsl	r2, #7
	and	r3, r2
	cmp	r3, #0
	beq	.Lbab70
	mov	r0, r5
	bl	_Anim_Attack
	b	.Lbab7c
.Lbab70:
	mov	r0, r5
	bl	_Anim_Func
	b	.Lbab7c
.Lbab78:
	bl	Func_80c1a14
.Lbab7c:
	bl	Func_80be02c
	mov	r6, r5
	mov	r0, r8
	mov	r1, #1
	bl	_Actor_SetAnim
	ldr	r3, [r6, #0x14]
	mov	r4, #0
	cmp	r3, #0
	beq	.Lbabaa
	mov	r7, #0x24
.Lbab94:
	ldrsh	r0, [r6, r7]
	str	r4, [sp]
	bl	Func_80b8000
	mov	r5, r6
	ldr	r4, [sp]
	ldr	r3, [r5, #0x14]
	add	r4, #1
	add	r7, #2
	cmp	r4, r3
	bne	.Lbab94
.Lbabaa:
	mov	r0, #0
	add	sp, #0x58
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80ba978

@ ShowDamageNumbers
@ r0.. = parameters. Puts the damage overlay up through rom_15000's _Func_1f200,
@ counting the actor's parts with Func_ba918 and pricing with _Func_2281c.
.thumb_func_start Func_80babdc  @ 0x080babdc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	sub	sp, #4
	bl	_GetUnit
	mov	r0, r7
	bl	GetBattleActor
	mov	r1, #5
	ldr	r0, [r0]
	bl	_Actor_SetAnim
	mov	r3, #1
	mov	r6, sp
	mov	r8, r3
.Lbabfe:
	mov	r3, #0xff
	mov	r0, r6
	strh	r3, [r6, #2]
	strh	r7, [r6]
	bl	_Func_802281c
	mov	r0, r7
	bl	GetBattleActor
	mov	r1, #7
	ldr	r0, [r0]
	bl	Func_80ba918
	mov	r0, #2
	bl	WaitFrames
	mov	r0, r6
	strh	r7, [r6]
	bl	_Func_802281c
	mov	r0, r7
	bl	GetBattleActor
	mov	r5, r0
	mov	r0, r7
	bl	Func_80b6cd0
	mov	r1, r0
	ldr	r0, [r5]
	bl	Func_80ba918
	mov	r0, #2
	bl	WaitFrames
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r3, r8
	cmp	r3, #0
	bge	.Lbabfe
	ldr	r3, =iwram_3001e74
	ldr	r3, [r3]
	add	r3, #0x41
	ldrb	r0, [r3]
	bl	_Func_801f200
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80babdc

@ GetCombatantScale
@ r0 = combatant id. Derives the display scale from the record and Func_c1ebc.
.thumb_func_start Func_80bac6c  @ 0x080bac6c
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001e74
	mov	r6, r0
	ldr	r5, [r3]
	bl	_GetUnit
	mov	r3, #0x95
	lsl	r3, #1
	add	r2, r0, r3
	ldr	r1, .Lbac94	@ 0xfe
	mov	r3, #0
	strb	r3, [r2]
	mov	r2, #0x58
	b	.Lbac8a
.Lbac88:
	add	r2, #2
.Lbac8a:
	ldrsh	r3, [r2, r5]
	cmp	r3, r6
	bne	.Lbac9c
	strh	r1, [r2, r5]
	b	.Lbacc4

	.align	2, 0
.Lbac94:
	.word	0xfe
	.pool

.Lbac9c:
	cmp	r3, #0xff
	bne	.Lbac88
	mov	r1, #0
	add	r0, r5, #2
.Lbaca4:
	lsl	r3, r1, #1
	mov	r2, r3
	add	r2, #0x64
	ldrsh	r3, [r0, r2]
	cmp	r3, r6
	bne	.Lbacb6
	ldr	r3, =0xfe
	strh	r3, [r0, r2]
	b	.Lbacc4
.Lbacb6:
	add	r1, #1
	cmp	r3, #0xff
	bne	.Lbaca4
	b	.Lbace2

	.pool_aligned

.Lbacc4:
	mov	r0, r6
	bl	Func_80c1ebc
	mov	r2, #0xbb
	mov	r1, #0
	mov	r0, #0xff
	lsl	r2, #2
.Lbacd2:
	ldrsh	r3, [r2, r5]
	cmp	r3, r6
	bne	.Lbacda
	strh	r0, [r2, r5]
.Lbacda:
	add	r1, #1
	add	r2, #0x10
	cmp	r1, #0x13
	bls	.Lbacd2
.Lbace2:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80bac6c

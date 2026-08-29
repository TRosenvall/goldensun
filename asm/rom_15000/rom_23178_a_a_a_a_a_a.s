	.include "macros.inc"
	.include "gba.inc"

@ RunPsynergyScreen
@ r0.. = parameters. 1574 lines. A full-screen menu built from the shared
@ pieces: CreateUIBox opens windows, Func_16738 fills the text scratch,
@ Func_17aa4 emits runs, Func_19000 clips, Func_17248 saves the tilemap, and
@ CloseUIBox / .gcc2_compiled. close. Traced structurally.
.thumb_func_start Func_8023178  @ 0x08023178
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x160
	str	r0, [sp, #0x5c]
	str	r2, [sp, #0x54]
	str	r1, [sp, #0x58]
	ldr	r5, =iwram_3001e8c
	mov	r3, #1
	ldr	r1, [r5]
	mov	r0, #0x80
	mov	r2, #1
	neg	r3, r3
	lsl	r0, #2
	str	r1, [sp, #0x50]
	mov	r10, r2
	str	r3, [sp, #0x44]
	bl	AllocUploadSpriteGFX
	mov	r4, #0
	str	r0, [sp, #0x34]
	ldr	r0, [sp, #0x44]
	str	r4, [sp, #0x30]
	bl	_GetNumDjinn
	str	r0, [sp, #0x2c]
	add	r5, #0xa8
	ldr	r5, [r5]
	ldr	r0, [r5, #0x44]
	mov	r1, r10
	str	r1, [r5, #0x48]
	cmp	r0, #0
	beq	.L231ca
	bl	CloseUIBox
	ldr	r2, [sp, #0x30]
	str	r2, [r5, #0x44]
.L231ca:
	mov	r0, #0x70
	bl	_PlaySound
	mov	r6, #4
	mov	r2, #0
	add	r3, sp, #0x64
.L231d6:
	sub	r6, #1
	strb	r2, [r3]
	sub	r3, #1
	cmp	r6, #0
	bge	.L231d6
	mov	r3, #0
	add	r2, sp, #0x60
	str	r3, [r2, #8]
	str	r3, [r2, #0xc]
	str	r3, [r2, #0x10]
	mov	r0, #0x80
	bl	AllocUploadSpriteGFX
	mov	r3, #0x8e
	mov	r4, #0x88
	lsl	r3, #1
	lsl	r4, #1
	add	r3, sp
	add	r4, sp
	mov	r1, #1
	neg	r1, r1
	str	r0, [sp, #0x40]
	str	r3, [sp, #0x10]
	str	r4, [sp, #0x14]
	mov	r8, r1
	mov	r5, r4
	mov	r7, r3
	mov	r6, #0xa
.L2320e:
	mov	r0, #0x80
	bl	AllocUploadSpriteGFX
	sub	r6, #1
	mov	r2, r8
	strb	r2, [r5]
	stmia	r7!, {r0}
	add	r5, #1
	cmp	r6, #0
	bge	.L2320e
	ldr	r3, [sp, #0x58]
	cmp	r3, #0
	beq	.L23258
	ldr	r4, [sp, #0x5c]
	ldrh	r3, [r4]
	mov	r6, #0
	cmp	r3, #0xff
	beq	.L23258
	cmp	r3, #0xfe
	beq	.L2323c
	ldr	r1, [sp, #0x54]
	cmp	r3, r1
	beq	.L23256
.L2323c:
	add	r6, #1
	cmp	r6, #5
	bgt	.L23258
	ldr	r2, [sp, #0x5c]
	lsl	r3, r6, #1
	ldrh	r3, [r3, r2]
	cmp	r3, #0xff
	beq	.L23258
	cmp	r3, #0xfe
	beq	.L2323c
	ldr	r4, [sp, #0x54]
	cmp	r3, r4
	bne	.L2323c
.L23256:
	str	r6, [sp, #0x44]
.L23258:
	mov	r3, #6
	mov	r1, #0
	str	r3, [sp]
	mov	r2, #0x1e
	mov	r3, #0x14
	mov	r0, #0
	str	r1, [sp, #0x38]
	str	r1, [sp, #0x3c]
	bl	CreateUIBox
	mov	r3, #0xa
	str	r0, [sp, #0x4c]
	str	r3, [sp]
	mov	r1, #0xe
	mov	r2, #0x1e
	mov	r3, #6
	mov	r0, #0
	bl	CreateUIBox
	str	r0, [sp, #0x48]
	bl	Func_801e318
	mov	r3, #0xaa
	mov	r4, #0xa4
	mov	r2, sp
	lsl	r3, #1
	lsl	r4, #1
	mov	r1, sp
	add	r2, #0x80
	add	r3, sp
	add	r4, sp
	add	r1, #0x8c
	str	r2, [sp, #0x1c]
	str	r3, [sp, #8]
	str	r4, [sp, #0xc]
	str	r1, [sp, #0x18]
.L232a0:
	ldr	r3, =gKeyRepeat
	ldr	r3, [r3]
	ldr	r2, [sp, #0x1c]
	str	r3, [sp, #0x28]
	ldr	r3, =0x80000400
	str	r3, [r2, #4]
	mov	r3, #0
	str	r3, [r2, #8]
	ldr	r1, [sp, #0x34]
	ldr	r0, [sp, #0x54]
	bl	Func_8021b80
	ldr	r3, .L232f0	@ 0x3ff
	and	r0, r3
	ldr	r3, [sp, #0x1c]
	ldrh	r2, [r3, #8]
	ldr	r3, =0xfffffc00
	ldr	r4, [sp, #0x1c]
	and	r3, r2
	orr	r3, r0
	strh	r3, [r4, #8]
	ldrh	r2, [r4, #6]
	ldr	r3, =0xfffffe00
	and	r3, r2
	ldr	r2, .L232f4	@ 8
	ldr	r1, [sp, #0x1c]
	orr	r3, r2
	strh	r3, [r1, #6]
	ldrb	r2, [r1, #9]
	mov	r3, #0x18
	strb	r3, [r1, #4]
	mov	r3, #0xf
	and	r3, r2
	mov	r2, #0xe0
	orr	r3, r2
	strb	r3, [r1, #9]
	ldr	r0, [sp, #0x1c]
	mov	r1, #0xf0
	b	.L2330c

	.align	2, 0
.L232f0:
	.word	0x3ff
.L232f4:
	.word	8
	.pool

.L2330c:
	bl	Func_8003dec
	ldr	r3, [sp, #0x68]
	cmp	r3, #0x15
	bhi	.L23378
	ldr	r2, =.L23320
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L23320:
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23378
	.word	.L23390
	.word	.L23390
	.word	.L23390
	.word	.L23390
	.word	.L2339c
	.word	.L2339c
	.word	.L2339c
	.word	.L2339c
	.word	.L2339c
	.word	.L2339c
	.word	.L2339c
	.word	.L2339c
.L23378:
	mov	r3, #0
	add	r2, sp, #0x60
	str	r3, [r2, #0x10]
	ldr	r3, [sp, #0x2c]
	cmp	r3, #0
	beq	.L2338a
	mov	r3, #9
	str	r3, [r2, #0x14]
	b	.L233a6
.L2338a:
	mov	r3, #7
	str	r3, [r2, #0x14]
	b	.L233a6
.L23390:
	add	r2, sp, #0x60
	mov	r3, #1
	str	r3, [r2, #0x10]
	mov	r3, #4
	str	r3, [r2, #0x14]
	b	.L233a6
.L2339c:
	add	r2, sp, #0x60
	mov	r3, #2
	str	r3, [r2, #0x10]
	ldr	r4, [sp, #0x30]
	str	r4, [r2, #0x14]
.L233a6:
	ldr	r1, [sp, #0x4c]
	ldr	r3, [sp, #0x4c]
	ldrh	r0, [r1, #0xc]
	ldrh	r2, [r3, #8]
	mov	r4, #0xf
	ldrh	r1, [r1, #0xe]
	ldrh	r3, [r3, #0xa]
	str	r4, [sp]
	bl	Func_8022768
	ldr	r4, [sp, #0x30]
	cmp	r4, #0
	bne	.L233c2
	b	.L234ce
.L233c2:
	add	r6, sp, #0x60
	ldr	r3, [r6, #0x10]
	cmp	r3, #1
	bhi	.L23458
	ldr	r1, [sp, #0x28]
	ldrsb	r5, [r6, r3]
	mov	r3, #0x80
	and	r3, r1
	cmp	r3, #0
	beq	.L233ee
	mov	r2, #0
	mov	r0, #0x6f
	str	r2, [sp, #0x28]
	bl	_PlaySound
	ldr	r3, [r6, #0x14]
	add	r5, #1
	cmp	r5, r3
	blt	.L234c8
	ldr	r3, [r6, #0x10]
	mov	r5, #0
	b	.L2340e
.L233ee:
	ldr	r4, [sp, #0x28]
	mov	r3, #0x40
	and	r3, r4
	cmp	r3, #0
	beq	.L2341a
	mov	r1, #0
	mov	r0, #0x6f
	sub	r5, #1
	str	r1, [sp, #0x28]
	bl	_PlaySound
	cmp	r5, #0
	bge	.L234c8
	ldr	r3, [r6, #0x14]
	sub	r5, r3, #1
	ldr	r3, [r6, #0x10]
.L2340e:
	cmp	r3, #1
	bne	.L234c8
	mov	r3, #2
	str	r3, [r6, #0x10]
	ldrsb	r5, [r6, r3]
	b	.L234c8
.L2341a:
	ldr	r2, [sp, #0x28]
	mov	r3, #0x31
	and	r3, r2
	cmp	r3, #0
	beq	.L234c8
	mov	r3, #0
	mov	r0, #0x6f
	str	r3, [sp, #0x28]
	bl	_PlaySound
	ldr	r1, [r6, #0x10]
	mov	r3, #2
	eor	r1, r3
	str	r1, [r6, #0x10]
	ldr	r0, =gKeyRepeat
	ldr	r3, [r0]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L23446
	ldrsb	r5, [r6, r1]
	b	.L234c8
.L23446:
	ldr	r2, [r0]
	mov	r3, #0x20
	ldr	r5, [sp, #0x30]
	and	r2, r3
	sub	r5, #1
	cmp	r2, #0
	bne	.L234c8
	mov	r5, #0
	b	.L234c8
.L23458:
	cmp	r3, #2
	bne	.L234ce
	ldrsb	r5, [r6, r3]
	ldr	r2, [r6, #0x14]
	cmp	r5, r2
	blt	.L23466
	sub	r5, r2, #1
.L23466:
	cmp	r5, #0
	bge	.L23474
	mov	r3, #0
	str	r3, [r6, #0x10]
	mov	r5, #0
	ldrsb	r5, [r6, r5]
	b	.L234c8
.L23474:
	ldr	r4, [sp, #0x28]
	mov	r3, #0x10
	and	r3, r4
	cmp	r3, #0
	beq	.L2348c
	mov	r1, #0
	add	r5, #1
	str	r1, [sp, #0x28]
	cmp	r5, r2
	blt	.L234a6
	str	r1, [r6, #0x10]
	b	.L234a2
.L2348c:
	ldr	r2, [sp, #0x28]
	mov	r3, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.L234ae
	mov	r3, #0
	sub	r5, #1
	str	r3, [sp, #0x28]
	cmp	r5, #0
	bge	.L234a6
	str	r3, [r6, #0x10]
.L234a2:
	mov	r5, #0
	ldrsb	r5, [r6, r5]
.L234a6:
	mov	r0, #0x6f
	bl	_PlaySound
	b	.L234c8
.L234ae:
	ldr	r4, [sp, #0x28]
	mov	r3, #0xc1
	and	r3, r4
	cmp	r3, #0
	beq	.L234c8
	mov	r1, #0
	str	r1, [sp, #0x28]
	mov	r0, #0x6f
	str	r1, [r6, #0x10]
	mov	r5, #0
	ldrsb	r5, [r6, r5]
	bl	_PlaySound
.L234c8:
	add	r2, sp, #0x60
	ldr	r3, [r2, #0x10]
	strb	r5, [r2, r3]
.L234ce:
	add	r0, sp, #0x60
	ldr	r2, [r0, #0x10]
	cmp	r2, #0
	bne	.L23512
	ldr	r3, [sp, #0x2c]
	ldrsb	r2, [r0, r2]
	cmp	r3, #0
	bne	.L234e0
	add	r2, #9
.L234e0:
	ldr	r3, =.L37328
	lsl	r2, #3
	add	r2, r3
	ldrb	r3, [r2]
	str	r3, [r0, #8]
	ldrb	r3, [r2, #1]
	str	r3, [r0, #0x18]
	ldrb	r3, [r2, #2]
	str	r3, [r0, #0x1c]
	ldr	r4, [sp, #0x4c]
	ldrb	r3, [r2, #3]
	ldrh	r0, [r4, #0xc]
	ldrh	r1, [r4, #0xe]
	add	r0, r3
	ldrb	r3, [r2, #4]
	add	r1, r3
	mov	r3, #0xe
	ldrb	r2, [r2, #5]
	add	r0, #1
	str	r3, [sp]
	add	r1, #1
	mov	r3, #1
	bl	Func_8022768
	b	.L23544
.L23512:
	cmp	r2, #1
	bne	.L2352c
	ldrsb	r3, [r0, r2]
	ldr	r1, =.L373a8
	b	.L23534

	.pool_aligned

.L2352c:
	cmp	r2, #2
	bne	.L23544
	ldrsb	r3, [r0, r2]
	ldr	r1, =.L373b8
.L23534:
	lsl	r3, #2
	ldrb	r2, [r1, r3]
	add	r3, r1
	str	r2, [r0, #8]
	ldrb	r2, [r3, #1]
	ldrb	r3, [r3, #2]
	str	r2, [r0, #0x18]
	str	r3, [r0, #0x1c]
.L23544:
	add	r2, sp, #0x60
	ldr	r3, [r2, #0xc]
	ldr	r1, [r2, #8]
	cmp	r3, r1
	beq	.L23554
	str	r1, [r2, #0xc]
	mov	r1, #2
	mov	r10, r1
.L23554:
	ldr	r3, [r2, #0x18]
	lsl	r3, #3
	str	r3, [sp, #0x38]
	ldr	r3, [r2, #0x1c]
	ldr	r0, [sp, #0x54]
	lsl	r3, #3
	str	r3, [sp, #0x3c]
	bl	_GetBattleActor
	ldr	r3, [r0]
	ldr	r4, [sp, #8]
	ldr	r2, [r3, #0x50]
	ldr	r3, =0xc0002400
	mov	r7, #0
	str	r3, [r4, #4]
	str	r7, [r4, #8]
	ldr	r1, =0x3ff
	ldrh	r2, [r2, #8]
	ldr	r5, =0xfffffc00
	mov	r8, r1
	ldrh	r1, [r4, #8]
	mov	r3, r5
	lsl	r2, #22
	lsr	r2, #22
	and	r3, r1
	orr	r3, r2
	ldr	r2, [sp, #8]
	strh	r3, [r2, #8]
	ldr	r3, [sp, #8]
	ldr	r6, =0xfffffe00
	ldrh	r2, [r3, #6]
	mov	r3, r6
	and	r3, r2
	ldr	r2, .L235cc	@ 0xac
	ldr	r4, [sp, #8]
	orr	r3, r2
	strh	r3, [r4, #6]
	mov	r3, #0x38
	strb	r3, [r4, #4]
	ldr	r0, [sp, #8]
	mov	r1, #0xf0
	bl	Func_8003dec
	ldr	r3, =0x40000400
	ldr	r1, [sp, #0xc]
	str	r3, [r1, #4]
	str	r7, [r1, #8]
	ldr	r0, [sp, #0x40]
	ldr	r1, =Data_310a4
	bl	UploadSprite2
	ldr	r4, [sp, #0xc]
	ldrh	r3, [r4, #8]
	mov	r2, r8
	and	r2, r0
	and	r5, r3
	mov	r1, r4
	orr	r5, r2
	strh	r5, [r1, #8]
	b	.L235ec

	.align	2, 0
.L235cc:
	.word	0xac
	.pool

.L235ec:
	ldr	r3, [sp, #0x4c]
	ldr	r0, =iwram_3001e40
	ldrh	r2, [r3, #0xc]
	ldr	r3, [r0]
	ldr	r4, [sp, #0x38]
	mov	r1, #4
	and	r3, r1
	lsl	r2, #3
	add	r2, r4, r2
	lsr	r3, #2
	ldr	r4, [sp, #0xc]
	sub	r2, r3
	ldr	r3, .L23640	@ 0x1ff
	add	r2, #0x10
	and	r2, r3
	ldrh	r3, [r4, #6]
	and	r6, r3
	orr	r6, r2
	mov	r2, r4
	strh	r6, [r2, #6]
	ldr	r3, [sp, #0x4c]
	ldrh	r2, [r3, #0xe]
	ldr	r3, [r0]
	ldr	r4, [sp, #0x3c]
	and	r3, r1
	lsl	r2, #3
	add	r2, r4, r2
	lsr	r3, #2
	ldr	r1, [sp, #0xc]
	sub	r2, r3
	add	r2, #0x10
	strb	r2, [r1, #4]
	mov	r3, #0x3f
	ldrb	r2, [r1, #7]
	neg	r3, r3
	and	r3, r2
	mov	r2, #0x10
	orr	r3, r2
	strb	r3, [r1, #7]
	ldr	r0, [sp, #0xc]
	b	.L23648

	.align	2, 0
.L23640:
	.word	0x1ff
	.pool

.L23648:
	mov	r1, #0xf1
	bl	Func_8003dec
	mov	r2, r10
	cmp	r2, #0
	bne	.L23656
	b	.L23cfa
.L23656:
	ldr	r0, [sp, #0x54]
	bl	_GetUnit
	mov	r9, r0
	bl	Func_801e318
	bl	Func_8016738
	mov	r3, #1
	mov	r4, r10
	and	r3, r4
	cmp	r3, #0
	bne	.L23672
	b	.L23b42
.L23672:
	ldr	r0, [sp, #0x4c]
	bl	Func_8016498
	ldr	r1, [sp, #0x48]
	ldr	r3, [sp, #0x48]
	ldrh	r0, [r1, #0xc]
	ldrh	r2, [r3, #8]
	ldrh	r1, [r1, #0xe]
	ldrh	r3, [r3, #0xa]
	str	r7, [sp]
	bl	Func_8017248
	mov	r3, #0xe
	str	r3, [sp]
	ldr	r0, [sp, #0x4c]
	mov	r1, #0
	mov	r2, #0xe
	mov	r3, #0x1d
	bl	Func_801e41c
	mov	r0, r9
	ldr	r1, [sp, #0x4c]
	mov	r2, #0
	mov	r3, #0
	bl	Func_801e8b0
	ldr	r0, =.L373d8
	ldr	r1, [sp, #0x4c]
	mov	r2, #0x38
	mov	r3, #0
	bl	Func_801e8b0
	mov	r4, r9
	ldr	r2, [sp, #0x4c]
	ldrb	r0, [r4, #0xf]
	mov	r1, #2
	mov	r3, #0x48
	str	r7, [sp]
	bl	Func_801ea08
	ldr	r7, =0x8ba
	ldr	r1, [sp, #0x4c]
	mov	r0, r7
	mov	r2, #0
	mov	r3, #8
	bl	Func_801e7c0
	mov	r3, #0x92
	lsl	r3, #1
	add	r3, r9
	ldr	r0, [r3]
	mov	r3, #8
	str	r3, [sp]
	ldr	r2, [sp, #0x4c]
	mov	r1, #8
	mov	r3, #0x28
	bl	Func_801ea08
	ldr	r0, =.L373dc
	ldr	r1, [sp, #0x4c]
	mov	r2, #0x28
	mov	r3, #0x18
	bl	Func_801e8b0
	mov	r3, #0x18
	mov	r2, r9
	mov	r1, #0x38
	ldrsh	r0, [r2, r1]
	mov	r8, r3
	str	r3, [sp]
	ldr	r2, [sp, #0x4c]
	mov	r1, #4
	mov	r3, #0x38
	bl	Func_801ea08
	ldr	r5, =.L373e0
	ldr	r1, [sp, #0x4c]
	mov	r0, r5
	mov	r2, #0x58
	mov	r3, #0x18
	bl	Func_801e8b0
	mov	r1, r9
	mov	r2, r8
	mov	r4, #0x34
	ldrsh	r0, [r1, r4]
	mov	r3, #0x60
	str	r2, [sp]
	mov	r1, #4
	ldr	r2, [sp, #0x4c]
	bl	Func_801ea08
	mov	r3, #0x20
	ldr	r1, [sp, #0x4c]
	ldr	r0, =.L373e4
	mov	r2, #0x28
	bl	Func_801e8b0
	mov	r4, r9
	ldr	r2, [sp, #0x4c]
	mov	r3, #0x3a
	ldrsh	r0, [r4, r3]
	mov	r6, #0x20
	mov	r1, #4
	mov	r3, #0x38
	str	r6, [sp]
	bl	Func_801ea08
	mov	r0, r5
	ldr	r1, [sp, #0x4c]
	mov	r2, #0x58
	mov	r3, #0x20
	bl	Func_801e8b0
	mov	r2, r9
	mov	r1, #0x36
	ldrsh	r0, [r2, r1]
	mov	r3, #0x60
	ldr	r2, [sp, #0x4c]
	mov	r1, #4
	str	r6, [sp]
	bl	Func_801ea08
	mov	r0, r7
	ldr	r1, [sp, #0x4c]
	sub	r0, #0xa
	mov	r2, #0x88
	mov	r3, #0x10
	bl	Func_801e7c0
	mov	r3, r9
	ldrh	r0, [r3, #0x3c]
	mov	r3, #0x10
	str	r3, [sp]
	ldr	r2, [sp, #0x4c]
	mov	r1, #3
	mov	r3, #0xb8
	bl	Func_801ea08
	mov	r0, r7
	ldr	r1, [sp, #0x4c]
	sub	r0, #9
	mov	r2, #0x88
	mov	r3, #0x18
	bl	Func_801e7c0
	mov	r4, r9
	mov	r1, r8
	ldrh	r0, [r4, #0x3e]
	ldr	r2, [sp, #0x4c]
	str	r1, [sp]
	mov	r3, #0xb8
	mov	r1, #3
	bl	Func_801ea08
	mov	r0, r7
	ldr	r1, [sp, #0x4c]
	sub	r0, #8
	mov	r2, #0x88
	mov	r3, #0x20
	bl	Func_801e7c0
	mov	r3, r9
	add	r3, #0x40
	ldr	r2, [sp, #0x4c]
	ldrh	r0, [r3]
	mov	r1, #3
	mov	r3, #0xb8
	str	r6, [sp]
	bl	Func_801ea08
	sub	r0, r7, #7
	ldr	r1, [sp, #0x4c]
	mov	r2, #0x88
	mov	r3, #0x28
	bl	Func_801e7c0
	mov	r3, r9
	add	r3, #0x42
	ldrb	r0, [r3]
	mov	r3, #0x28
	str	r3, [sp]
	ldr	r2, [sp, #0x4c]
	mov	r1, #3
	mov	r3, #0xb8
	bl	Func_801ea08
	ldr	r3, =0x129
	add	r3, r9
	ldrb	r0, [r3]
	ldr	r3, =0x741
	mov	r2, #0
	add	r0, r3
	ldr	r1, [sp, #0x4c]
	mov	r3, #0x30
	bl	Func_801e7c0
	ldr	r2, [sp, #0x2c]
	cmp	r2, #0
	beq	.L2380e
	sub	r0, r7, #1
	ldr	r1, [sp, #0x4c]
	mov	r2, #0
	mov	r3, #0x48
	bl	Func_801e7c0
.L2380e:
	ldr	r1, [sp, #0x4c]
	sub	r0, r7, #5
	mov	r2, #0
	mov	r3, #0x50
	bl	Func_801e7c0
	sub	r0, r7, #4
	ldr	r1, [sp, #0x4c]
	mov	r2, #0
	mov	r3, #0x58
	bl	Func_801e7c0
	mov	r6, #0x8c
	sub	r0, r7, #3
	ldr	r1, [sp, #0x4c]
	mov	r2, #0
	mov	r3, #0x60
	bl	Func_801e7c0
	lsl	r6, #1
	mov	r3, #0x48
	mov	r1, #0x28
	mov	r2, #7
	mov	r4, #0
	mov	r11, r3
	mov	r8, r1
	add	r6, r9
	mov	r7, #0x30
	mov	r10, r2
.L23848:
	ldr	r1, [sp, #0x2c]
	mov	r3, #1
	cmp	r1, #0
	beq	.L23852
	mov	r3, #0
.L23852:
	ldr	r2, =0x5001
	add	r1, r4, r2
	mov	r2, #0
	add	r3, #8
	str	r2, [sp]
	ldr	r0, [sp, #0x4c]
	mov	r2, r10
	lsl	r5, r4, #2
	str	r4, [sp, #4]
	bl	Func_8019000
	ldr	r3, [sp, #0x2c]
	ldr	r4, [sp, #4]
	cmp	r3, #0
	beq	.L238a0
	mov	r1, r11
	ldr	r2, [sp, #0x4c]
	ldrb	r0, [r6, #4]
	mov	r3, r8
	str	r1, [sp]
	mov	r1, #1
	bl	Func_801ea08
	ldr	r0, =.L373e0
	ldr	r1, [sp, #0x4c]
	mov	r2, r7
	mov	r3, #0x48
	bl	Func_801e8b0
	mov	r2, r11
	mov	r3, r7
	ldrb	r0, [r6]
	add	r3, #8
	str	r2, [sp]
	mov	r1, #1
	ldr	r2, [sp, #0x4c]
	bl	Func_801ea08
	ldr	r4, [sp, #4]
.L238a0:
	mov	r1, r4
	ldr	r0, [sp, #0x54]
	str	r4, [sp, #4]
	bl	_Func_807987c
	mov	r3, #0x50
	str	r3, [sp]
	ldr	r2, [sp, #0x4c]
	mov	r1, #2
	mov	r3, r7
	bl	Func_801ea08
	add	r5, #0x48
	mov	r3, r9
	ldrsh	r0, [r3, r5]
	mov	r3, #0x58
	ldr	r2, [sp, #0x4c]
	str	r3, [sp]
	mov	r1, #3
	mov	r3, r8
	bl	Func_801ea08
	add	r5, r9
	mov	r3, #0x60
	mov	r2, #2
	ldrsh	r0, [r5, r2]
	mov	r1, #3
	str	r3, [sp]
	ldr	r2, [sp, #0x4c]
	mov	r3, r8
	bl	Func_801ea08
	ldr	r4, [sp, #4]
	mov	r3, #0x20
	mov	r1, #4
	add	r4, #1
	add	r8, r3
	add	r6, #1
	add	r7, #0x20
	add	r10, r1
	cmp	r4, #3
	ble	.L23848
	mov	r4, r9
	mov	r2, #0x38
	ldrsh	r3, [r4, r2]
	mov	r6, #0
	cmp	r3, #0
	bne	.L23908
	ldr	r1, [sp, #0x14]
	mov	r3, #0x10
	strb	r3, [r1]
	mov	r6, #1
.L23908:
	ldr	r3, [sp, #0x14]
	add	r2, r6, r3
	b	.L23aa0

	.pool_aligned

.L23930:
	ldr	r1, =0x131
	add	r1, r9
	mov	r3, #0
	ldrsb	r3, [r1, r3]
	cmp	r3, #1
	bne	.L23942
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23942:
	cmp	r6, #7
	ble	.L23948
	b	.L23ad4
.L23948:
	mov	r3, #0
	ldrsb	r3, [r1, r3]
	cmp	r3, #2
	bne	.L23956
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23956:
	cmp	r6, #7
	ble	.L2395c
	b	.L23ad4
.L2395c:
	ldr	r3, =0x13d
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L2396e
	mov	r3, #4
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L2396e:
	cmp	r6, #7
	ble	.L23974
	b	.L23ad4
.L23974:
	ldr	r3, =0x13b
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L23986
	mov	r3, #3
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23986:
	cmp	r6, #7
	ble	.L2398c
	b	.L23ad4
.L2398c:
	mov	r3, #0x9e
	lsl	r3, #1
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L239a0
	mov	r3, #5
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L239a0:
	cmp	r6, #7
	ble	.L239a6
	b	.L23ad4
.L239a6:
	mov	r3, #0xa0
	lsl	r3, #1
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L239ba
	mov	r3, #7
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L239ba:
	cmp	r6, #7
	ble	.L239c0
	b	.L23ad4
.L239c0:
	mov	r3, #0x9c
	lsl	r3, #1
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L239d4
	mov	r3, #6
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L239d4:
	cmp	r6, #7
	bgt	.L23ad4
	mov	r3, #0x99
	lsl	r3, #1
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L23a0a
	ldr	r3, =0x133
	add	r3, r9
	ldrb	r1, [r3]
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	ble	.L239fc
	mov	r3, #9
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L239fc:
	lsl	r3, r1, #24
	cmp	r3, #0
	bge	.L23a0a
	mov	r3, #0xa
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23a0a:
	cmp	r6, #7
	bgt	.L23ad4
	mov	r3, #0x9a
	lsl	r3, #1
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L23a40
	ldr	r3, =0x135
	add	r3, r9
	ldrb	r1, [r3]
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	ble	.L23a32
	mov	r3, #0xb
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23a32:
	lsl	r3, r1, #24
	cmp	r3, #0
	bge	.L23a40
	mov	r3, #0xc
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23a40:
	cmp	r6, #7
	bgt	.L23ad4
	mov	r3, #0x9b
	lsl	r3, #1
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L23a76
	ldr	r3, =0x137
	add	r3, r9
	ldrb	r1, [r3]
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	ble	.L23a68
	mov	r3, #0xd
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23a68:
	lsl	r3, r1, #24
	cmp	r3, #0
	bge	.L23a76
	mov	r3, #0xe
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23a76:
	cmp	r6, #7
	bgt	.L23ad4
	ldr	r3, =0x147
	add	r3, r9
	ldrb	r1, [r3]
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	ble	.L23a92
	mov	r3, #0x11
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23a92:
	lsl	r3, r1, #24
	cmp	r3, #0
	bge	.L23ad4
	mov	r3, #0x12
	strb	r3, [r2]
	add	r6, #1
	b	.L23ad4
.L23aa0:
	mov	r3, #0x98
	lsl	r3, #1
	add	r3, r9
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	beq	.L23ab8
	mov	r3, #0xf
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23ab8:
	cmp	r6, #7
	bgt	.L23ad4
	ldr	r3, =0x141
	add	r3, r9
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L23ace
	mov	r3, #8
	strb	r3, [r2]
	add	r6, #1
	add	r2, #1
.L23ace:
	cmp	r6, #7
	bgt	.L23ad4
	b	.L23930
.L23ad4:
	cmp	r6, #0
	ble	.L23af6
	ldr	r7, [sp, #0x10]
	ldr	r5, [sp, #0x14]
	mov	r4, r6
.L23ade:
	ldrb	r0, [r5]
	lsl	r0, #24
	asr	r0, #24
	ldmia	r7!, {r1}
	str	r4, [sp, #4]
	bl	Func_8021ab0
	ldr	r4, [sp, #4]
	sub	r4, #1
	add	r5, #1
	cmp	r4, #0
	bne	.L23ade
.L23af6:
	cmp	r6, #0
	bne	.L23b00
	ldr	r4, [sp, #0x14]
	strb	r6, [r4]
	mov	r6, #1
.L23b00:
	cmp	r6, #0xa
	bgt	.L23b1c
	ldr	r3, [sp, #0x14]
	mov	r2, #1
	neg	r2, r2
	mov	r1, r2
	add	r2, r6, r3
	mov	r3, #0xb
	sub	r4, r3, r6
.L23b12:
	sub	r4, #1
	strb	r1, [r2]
	add	r2, #1
	cmp	r4, #0
	bne	.L23b12
.L23b1c:
	str	r6, [sp, #0x30]
	ldr	r4, [sp, #0x14]
	mov	r3, #0
	ldrsb	r3, [r4, r3]
	cmp	r3, #0
	bne	.L23b42
	mov	r2, r9
	mov	r1, #0x38
	ldrsh	r3, [r2, r1]
	cmp	r3, #0
	beq	.L23b36
	ldr	r0, =0x8bd
	b	.L23b38
.L23b36:
	ldr	r0, =0x8be
.L23b38:
	ldr	r1, [sp, #0x4c]
	mov	r2, #0x70
	mov	r3, #0
	bl	Func_801e7c0
.L23b42:
	mov	r0, #0x80
	lsl	r0, #1
	bl	Func_8004938
	ldr	r3, [sp, #0x68]
	str	r0, [sp, #0x24]
	cmp	r3, #0xd
	bhi	.L23b54
	b	.L23c94
.L23b54:
	ldr	r4, [sp, #0x14]
	sub	r3, #0xe
	ldrsb	r3, [r4, r3]
	str	r3, [sp, #0x20]
	cmp	r3, #0
	bne	.L23b6e
	mov	r2, r9
	mov	r1, #0x38
	ldrsh	r3, [r2, r1]
	cmp	r3, #0
	bne	.L23b6e
	mov	r3, #0x10
	str	r3, [sp, #0x20]
.L23b6e:
	mov	r4, #0xa6
	lsl	r4, #1
	mov	r8, r4
	mov	r0, r8
	bl	Func_8004938
	mov	r4, #0
	str	r4, [sp, #4]
	mov	r1, r9
	mov	r2, r8
	ldr	r3, =Func_8001af8
	mov	r6, r0
	bl	_call_via_r3
	ldr	r3, =0x133
	ldr	r4, [sp, #4]
	mov	r5, r9
	add	r5, #0x40
	add	r3, r9
	mov	r1, r9
	mov	r2, r9
	ldrh	r1, [r1, #0x3c]
	ldrh	r2, [r2, #0x3e]
	ldrh	r7, [r5]
	strb	r4, [r3]
	ldr	r3, =0x135
	add	r3, r9
	strb	r4, [r3]
	ldr	r3, =0x147
	add	r3, r9
	strb	r4, [r3]
	ldr	r0, [sp, #0x54]
	mov	r10, r1
	mov	r11, r2
	bl	_CalcStats
	mov	r1, r9
	ldrh	r3, [r1, #0x3c]
	mov	r2, r10
	sub	r2, r3
	ldrh	r3, [r1, #0x3e]
	mov	r1, r11
	sub	r1, r3
	ldrh	r3, [r5]
	mov	r10, r2
	mov	r11, r1
	mov	r2, r8
	mov	r1, r6
	sub	r7, r3
	mov	r0, r9
	ldr	r3, =Func_8001af8
	bl	_call_via_r3
	mov	r0, r6
	bl	free
	ldr	r3, [sp, #0x20]
	sub	r3, #8
	ldr	r4, [sp, #4]
	cmp	r3, #0xa
	bhi	.L23c4e
	ldr	r2, =.L23bf0
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L23bf0:
	.word	.L23c1c
	.word	.L23c24
	.word	.L23c28
	.word	.L23c2e
	.word	.L23c32
	.word	.L23c40
	.word	.L23c40
	.word	.L23c4e
	.word	.L23c4e
	.word	.L23c38
	.word	.L23c3c
.L23c1c:
	ldr	r3, =0x141
	add	r3, r9
	ldrb	r4, [r3]
	b	.L23c4e
.L23c24:
	mov	r4, r10
	b	.L23c4e
.L23c28:
	mov	r1, r10
	neg	r4, r1
	b	.L23c4e
.L23c2e:
	mov	r4, r11
	b	.L23c4e
.L23c32:
	mov	r2, r11
	neg	r4, r2
	b	.L23c4e
.L23c38:
	mov	r4, r7
	b	.L23c4e
.L23c3c:
	neg	r4, r7
	b	.L23c4e
.L23c40:
	ldr	r3, =0x137
	add	r3, r9
	mov	r2, #0
	ldrsb	r2, [r3, r2]
	lsl	r3, r2, #2
	add	r3, r2
	lsl	r4, r3, #2
.L23c4e:
	mov	r0, r4
	mov	r1, #5
	bl	Func_8019908
	ldr	r0, =0x8d2
	ldr	r3, [sp, #0x20]
	ldr	r1, [sp, #0x24]
	add	r0, r3, r0
	b	.L23cbe

	.pool_aligned

.L23c94:
	cmp	r3, #2
	bne	.L23ccc
	mov	r4, r9
	ldrb	r3, [r4, #0xf]
	cmp	r3, #0x62
	bhi	.L23ccc
	mov	r1, r3
	add	r1, #1
	ldr	r0, [sp, #0x54]
	bl	_Func_8079008
	mov	r3, #0x92
	lsl	r3, #1
	add	r3, r9
	ldr	r3, [r3]
	mov	r1, #5
	sub	r0, r3
	bl	Func_8019908
	ldr	r0, =0x8bf
	ldr	r1, [sp, #0x24]
.L23cbe:
	mov	r2, #0x80
	bl	Func_801965c
	b	.L23cda

	.pool_aligned

.L23ccc:
	ldr	r0, [sp, #0x68]
	ldr	r3, =0x8c0
	ldr	r1, [sp, #0x24]
	add	r0, r3
	mov	r2, #0x80
	bl	Func_801965c
.L23cda:
	ldr	r0, [sp, #0x24]
	ldr	r1, [sp, #0x48]
	mov	r2, #0
	mov	r3, #4
	bl	Func_8017aa4
	ldr	r0, [sp, #0x24]
	bl	free
	ldr	r4, =0xea3
	ldr	r3, [sp, #0x50]
	add	r2, r3, r4
	mov	r3, #1
	mov	r1, #0
	strb	r3, [r2]
	mov	r10, r1
.L23cfa:
	ldr	r1, =gSpriteSlots
	ldr	r5, [sp, #0x18]
	ldr	r7, [sp, #0x10]
	mov	r6, #0
	mov	r8, r1
	mov	r4, #0x70
.L23d06:
	ldr	r3, =0x40000400
	str	r3, [r5, #4]
	mov	r3, #0
	str	r3, [r5, #8]
	ldmia	r7!, {r3}
	lsl	r3, #2
	add	r3, r8
	ldrh	r2, [r3, #2]
	ldr	r1, .L23d44	@ 0xfffffc00
	ldrh	r3, [r5, #8]
	lsl	r2, #17
	lsr	r2, #22
	and	r3, r1
	orr	r3, r2
	strh	r3, [r5, #8]
	ldr	r3, .L23d48	@ 0x1ff
	mov	r1, r4
	and	r1, r3
	ldr	r2, .L23d4c	@ 0xfffffe00
	ldrh	r3, [r5, #6]
	and	r3, r2
	orr	r3, r1
	strh	r3, [r5, #6]
	ldr	r2, [sp, #0x4c]
	ldrh	r3, [r2, #0xe]
	lsl	r3, #3
	add	r3, #8
	strb	r3, [r5, #4]
	ldr	r1, [sp, #0x14]
	ldrsb	r3, [r1, r6]
	b	.L23d60

	.align	2, 0
.L23d44:
	.word	0xfffffc00
.L23d48:
	.word	0x1ff
.L23d4c:
	.word	0xfffffe00
	.pool

.L23d60:
	cmp	r3, #0
	ble	.L23d70
	mov	r0, r5
	mov	r1, #0xf0
	str	r4, [sp, #4]
	bl	Func_8003dec
	ldr	r4, [sp, #4]
.L23d70:
	add	r6, #1
	add	r4, #0xf
	add	r5, #0xc
	cmp	r6, #0xa
	ble	.L23d06
	ldr	r3, =iwram_3001f34
	ldr	r3, [r3]
	ldr	r3, [r3, #0x4c]
	cmp	r3, #0
	beq	.L23e00
	ldr	r3, =gKeyPress
	ldr	r3, [r3]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	bne	.L23e00
	ldr	r2, [sp, #0x58]
	cmp	r2, #0
	beq	.L23df6
	mov	r3, #0x80
	ldr	r4, [sp, #0x28]
	lsl	r3, #1
	and	r3, r4
	cmp	r3, #0
	beq	.L23dc6
	ldr	r1, [sp, #0x44]
	add	r1, #1
	str	r1, [sp, #0x44]
	cmp	r1, r2
	blt	.L23db0
	mov	r2, #0
	str	r2, [sp, #0x44]
.L23db0:
	ldr	r4, [sp, #0x44]
	ldr	r1, [sp, #0x5c]
	lsl	r3, r4, #1
	ldrh	r3, [r3, r1]
	mov	r2, #1
	mov	r0, #0x6f
	str	r3, [sp, #0x54]
	mov	r10, r2
	bl	_PlaySound
	b	.L23df6
.L23dc6:
	mov	r3, #0x80
	ldr	r4, [sp, #0x28]
	lsl	r3, #2
	and	r3, r4
	cmp	r3, #0
	beq	.L23df6
	ldr	r1, [sp, #0x44]
	sub	r1, #1
	str	r1, [sp, #0x44]
	cmp	r1, #0
	bge	.L23de2
	ldr	r2, [sp, #0x58]
	sub	r2, #1
	str	r2, [sp, #0x44]
.L23de2:
	ldr	r4, [sp, #0x44]
	ldr	r1, [sp, #0x5c]
	lsl	r3, r4, #1
	ldrh	r3, [r3, r1]
	mov	r2, #1
	mov	r0, #0x6f
	str	r3, [sp, #0x54]
	mov	r10, r2
	bl	_PlaySound
.L23df6:
	mov	r0, #1
	bl	WaitFrames
	bl	.L232a0
.L23e00:
	ldr	r5, [sp, #0x10]
	mov	r6, #0xa
.L23e04:
	ldmia	r5!, {r0}
	sub	r6, #1
	bl	Func_8003f3c
	cmp	r6, #0
	bge	.L23e04
	ldr	r0, [sp, #0x34]
	bl	Func_8003f3c
	ldr	r0, [sp, #0x40]
	bl	Func_8003f3c
	mov	r0, #1
	bl	WaitFrames
	bl	Func_801e318
	mov	r1, #1
	ldr	r0, [sp, #0x4c]
	bl	CloseUIBox
	mov	r1, #1
	ldr	r0, [sp, #0x48]
	bl	CloseUIBox
	ldr	r5, =iwram_3001e74
	ldr	r3, [r5]
	add	r3, #0x41
	ldrb	r0, [r3]
	add	r5, #0xc0
	bl	Func_801f200
	ldr	r2, [r5]
	mov	r3, #0
	str	r3, [r2, #0x48]
	mov	r0, #1
	bl	WaitFrames
	mov	r0, #0
	add	sp, #0x160
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8023178

@ RunItemManageScreen
@ r0.. = parameters. 1309 lines, same construction as Func_23178 with node
@ release through .gcc2_compiled. added. Traced structurally.
.thumb_func_start Func_8023e70  @ 0x08023e70
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0xe0
	str	r0, [sp, #0x4c]
	ldr	r5, =iwram_3001e8c
	mov	r3, #1
	ldr	r1, [r5]
	mov	r2, #0
	neg	r3, r3
	mov	r0, #0x80
	str	r1, [sp, #0x48]
	str	r2, [sp, #0x44]
	str	r3, [sp, #0x40]
	bl	AllocUploadSpriteGFX
	str	r0, [sp, #0x3c]
	mov	r0, #0xa8
	lsl	r0, #1
	bl	Func_8004970
	ldr	r2, [sp, #0x40]
	mov	r1, #0
	mov	r3, #0x2a
	mov	r6, #0
	str	r0, [sp, #0x38]
	str	r1, [sp, #0x34]
	str	r2, [sp, #0x30]
	str	r3, [sp]
	mov	r1, #4
	mov	r2, #0x1e
	mov	r3, #4
	mov	r0, #0
	str	r6, [sp, #0x28]
	str	r6, [sp, #0x20]
	str	r6, [sp, #0x50]
	str	r6, [sp, #0x1c]
	str	r6, [sp, #0x18]
	bl	CreateUIBox
	str	r0, [sp, #0x2c]
	mov	r0, #1
	bl	Func_801e3c8
	mov	r3, #6
	str	r3, [sp]
	mov	r1, #9
	mov	r2, #9
	mov	r3, #0xb
	mov	r0, #0x15
	bl	CreateUIBox
	mov	r9, r0
	add	r5, #0xa8
	ldr	r3, [r5]
	ldr	r1, [r3, #0x34]
	ldr	r2, [r3, #0x30]
	ldr	r3, [r3, #0x38]
	mov	r11, r1
	mov	r10, r2
	str	r3, [sp, #0x24]
	ldr	r0, [sp, #0x4c]
	bl	_GetUnit
	add	r0, #0xf8
	mov	r7, #0
	mov	r8, r0
.L23efe:
	ldr	r1, [sp, #0x34]
	ldr	r2, [sp, #0x38]
	lsl	r3, r1, #2
	mov	r6, #0
	add	r5, r3, r2
.L23f08:
	mov	r1, r8
	mov	r2, #1
	ldr	r3, [r1, #0x10]
	lsl	r2, r6
	and	r3, r2
	cmp	r3, #0
	beq	.L23f24
	lsl	r3, r7, #8
	orr	r3, r6
	stmia	r5!, {r3}
	ldr	r2, [sp, #0x34]
	add	r2, #1
	str	r2, [sp, #0x34]
	b	.L23fba
.L23f24:
	mov	r1, r8
	ldr	r3, [r1]
	and	r3, r2
	cmp	r3, #0
	beq	.L23fba
	ldr	r2, [sp, #0x4c]
	mov	r0, #0
	cmp	r2, #7
	bls	.L23f38
	mov	r0, #1
.L23f38:
	bl	_Func_8077330
	mov	r2, #0x84
	mov	r3, r0
	lsl	r2, #1
	add	r3, r2
	ldr	r3, [r3]
	mov	r1, #0
	add	r0, #8
	mov	r4, #0
	cmp	r1, r3
	bge	.L23f94
	ldrb	r3, [r0, #2]
	ldr	r2, [sp, #0x4c]
	cmp	r3, r2
	bne	.L23f64
	ldrb	r3, [r0]
	cmp	r3, r7
	bne	.L23f64
	ldrb	r3, [r0, #1]
	cmp	r3, r6
	beq	.L23f8e
.L23f64:
	mov	r2, #0x80
	lsl	r2, #1
	add	r3, r0, r2
	ldr	r3, [r3]
	add	r1, #1
	cmp	r1, r3
	bge	.L23f92
	lsl	r4, r1, #2
	add	r2, r0, r4
	ldrb	r3, [r2, #2]
	mov	r12, r3
	ldr	r3, [sp, #0x4c]
	cmp	r12, r3
	bne	.L23f64
	ldrb	r3, [r2]
	cmp	r3, r7
	bne	.L23f64
	ldrb	r3, [r2, #1]
	cmp	r3, r6
	bne	.L23f64
	b	.L23f94
.L23f8e:
	mov	r4, #0
	b	.L23f94
.L23f92:
	lsl	r4, r1, #2
.L23f94:
	lsl	r2, r7, #8
	mov	r3, #0x80
	lsl	r3, #9
	orr	r2, r6
	orr	r2, r3
	str	r2, [r5]
	add	r3, r0, r4
	ldrb	r3, [r3, #3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	ble	.L23fb2
	lsl	r3, #17
	orr	r2, r3
	str	r2, [r5]
.L23fb2:
	ldr	r1, [sp, #0x34]
	add	r1, #1
	str	r1, [sp, #0x34]
	add	r5, #4
.L23fba:
	add	r6, #1
	cmp	r6, #0x13
	ble	.L23f08
	mov	r2, #4
	add	r7, #1
	add	r8, r2
	cmp	r7, #3
	ble	.L23efe
	ldr	r3, [sp, #0x34]
	ldr	r1, [sp, #0x38]
	lsl	r2, r3, #2
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r2, r1]
	ldr	r3, [sp, #0x48]
	ldr	r1, =0xea3
	add	r2, r3, r1
	mov	r3, #1
	strb	r3, [r2]
	mov	r2, sp
	add	r2, #0xd4
	str	r2, [sp, #4]
	ldr	r1, [sp, #0x48]
	ldr	r2, =0xea3
	mov	r3, sp
	add	r3, #0x54
	add	r2, r1, r2
	str	r3, [sp, #0xc]
	str	r2, [sp, #8]
.L23ff4:
	ldr	r3, [sp, #0x30]
	cmp	r11, r3
	bne	.L24008
	ldr	r1, [sp, #0x40]
	cmp	r10, r1
	bne	.L24008
	ldr	r2, [sp, #0x1c]
	cmp	r2, #0
	bne	.L24008
	b	.L24328
.L24008:
	mov	r3, r11
	ldr	r1, [sp, #0x38]
	add	r3, r10
	lsl	r3, #2
	ldr	r5, [r3, r1]
	ldr	r3, [sp, #0x48]
	ldr	r1, =0xea6
	mov	r2, #0
	str	r2, [sp, #0x28]
	add	r2, r3, r1
	mov	r3, #1
	strb	r3, [r2]
	mov	r2, r9
	ldrh	r0, [r2, #0xc]
	ldrh	r1, [r2, #0xe]
	ldr	r2, [sp, #0x40]
	lsl	r3, r2, #1
	add	r1, r3
	mov	r3, r9
	ldrh	r2, [r3, #8]
	mov	r3, #0xf
	add	r1, #1
	str	r3, [sp]
	add	r0, #1
	sub	r2, #2
	mov	r3, #1
	bl	Func_8022768
	ldr	r1, [sp, #0x20]
	cmp	r1, #0
	beq	.L24064
	ldr	r0, [sp, #0x2c]
	mov	r1, #1
	bl	CloseUIBox
	mov	r3, #0x2a
	str	r3, [sp]
	mov	r0, #0
	mov	r1, #4
	mov	r2, #0x1e
	mov	r3, #4
	bl	CreateUIBox
	str	r0, [sp, #0x2c]
	bl	Func_8016738
.L24064:
	ldr	r3, [sp, #0x34]
	mov	r2, #0
	str	r2, [sp, #0x1c]
	cmp	r3, #0
	bne	.L24070
	b	.L241a0
.L24070:
	bl	Func_80198dc
	mov	r1, #0
	str	r1, [sp, #0x18]
	ldr	r0, =iwram_3001f34
	ldr	r3, [r0]
	mov	r2, #0xe4
	ldr	r3, [r3, r2]
	ldr	r1, [sp, #0x1c]
	cmp	r3, r5
	bne	.L2408c
	mov	r2, #1
	str	r2, [sp, #0x18]
	b	.L240a2
.L2408c:
	add	r1, #1
	cmp	r1, #7
	bgt	.L240a2
	ldr	r3, [r0]
	lsl	r2, r1, #2
	add	r2, #0xe4
	ldr	r3, [r3, r2]
	cmp	r3, r5
	bne	.L2408c
	mov	r3, #1
	str	r3, [sp, #0x18]
.L240a2:
	ldr	r1, [sp, #0x18]
	cmp	r1, #0
	beq	.L240ca
	ldr	r6, [sp, #0xc]
	mov	r2, #0x34
	ldr	r0, =0x8ef
	mov	r1, r6
	bl	Func_801965c
	ldr	r2, [sp, #0x44]
	cmp	r2, #0
	beq	.L241ac
	mov	r0, r2
	mov	r1, #1
	bl	CloseUIBox
	mov	r3, #0
	str	r3, [sp, #0x44]
	str	r3, [sp, #0x20]
	b	.L241ac
.L240ca:
	mov	r3, #0x80
	lsl	r3, #9
	and	r3, r5
	cmp	r3, #0
	beq	.L2414c
	mov	r0, #0xf8
	lsl	r0, #14
	and	r0, r5
	cmp	r0, #0
	beq	.L2411e
	lsr	r0, #17
	mov	r1, #5
	bl	Func_8019908
	mov	r3, #0xf0
	lsl	r3, #4
	and	r3, r5
	lsr	r3, #8
	lsl	r0, r3, #2
	add	r0, r3
	mov	r3, #0xff
	and	r5, r3
	ldr	r6, [sp, #0xc]
	ldr	r3, =0x666
	lsl	r0, #2
	add	r0, r5
	mov	r1, r6
	add	r0, r3
	mov	r2, #0x34
	bl	Func_801965c
	ldr	r1, [sp, #0x44]
	cmp	r1, #0
	beq	.L241ac
	mov	r0, r1
	mov	r1, #1
	bl	CloseUIBox
	mov	r2, #0
	str	r2, [sp, #0x44]
	str	r2, [sp, #0x20]
	b	.L241ac
.L2411e:
	add	r3, sp, #0x50
	str	r3, [sp]
	ldr	r1, [sp, #0x4c]
	mov	r2, r5
	ldr	r3, [sp, #0x20]
	ldr	r0, [sp, #0x44]
	bl	Func_8022b44
	ldr	r6, [sp, #0xc]
	str	r0, [sp, #0x44]
	mov	r1, r6
	ldr	r0, =0x899
	mov	r2, #0x34
	bl	Func_801965c
	mov	r3, #0xf0
	lsl	r3, #4
	and	r5, r3
	lsr	r3, r5, #8
	mov	r1, #1
	lsl	r1, r3
	str	r1, [sp, #0x28]
	b	.L241ac
.L2414c:
	add	r3, sp, #0x50
	mov	r2, r5
	str	r3, [sp]
	ldr	r1, [sp, #0x4c]
	ldr	r3, [sp, #0x20]
	ldr	r0, [sp, #0x44]
	bl	Func_8022b44
	mov	r3, #0xf0
	lsl	r3, #4
	and	r3, r5
	lsr	r3, #8
	str	r0, [sp, #0x44]
	lsl	r0, r3, #2
	add	r0, r3
	mov	r3, #0xff
	and	r5, r3
	lsl	r0, #2
	ldr	r3, =0x666
	ldr	r6, [sp, #0xc]
	add	r0, r5
	add	r0, r3
	mov	r1, r6
	mov	r2, #0x34
	bl	Func_801965c
	b	.L241ac

	.pool_aligned

.L241a0:
	ldr	r6, [sp, #0xc]
	ldr	r0, =0x8ed
	mov	r1, r6
	mov	r2, #0x34
	bl	Func_801965c
.L241ac:
	ldr	r2, [sp, #0x48]
	ldr	r3, =0xea6
	mov	r1, #0
	add	r5, r2, r3
	strb	r1, [r5]
	ldr	r2, [sp, #0x20]
	cmp	r2, #0
	bne	.L241e0
	mov	r3, #1
	strb	r3, [r5]
	ldr	r0, [sp, #0x2c]
	mov	r1, #1
	bl	CloseUIBox
	mov	r3, #0x2a
	str	r3, [sp]
	mov	r0, #0
	mov	r3, #4
	mov	r1, #4
	mov	r2, #0x1e
	bl	CreateUIBox
	str	r0, [sp, #0x2c]
	add	r3, sp, #0x20
	ldrb	r3, [r3]
	strb	r3, [r5]
.L241e0:
	ldr	r1, [sp, #0x2c]
	mov	r2, #0
	mov	r0, r6
	mov	r3, #4
	bl	Func_8017aa4
	ldr	r2, [sp, #0x30]
	mov	r1, r10
	str	r1, [sp, #0x40]
	cmp	r11, r2
	beq	.L242ae
	mov	r0, r9
	bl	Func_8016498
	mov	r1, r11
	ldr	r2, [sp, #0x38]
	lsl	r3, r1, #2
	add	r3, r2
	mov	r1, #0x80
	ldr	r6, [r3]
	lsl	r1, #24
	mov	r7, #0
	cmp	r6, r1
	beq	.L242a8
	mov	r8, r3
.L24212:
	mov	r2, #0xf0
	lsl	r2, #4
	mov	r1, r6
	and	r1, r2
	ldr	r3, =0x5001
	lsr	r1, #8
	add	r1, r3
	mov	r2, #0
	lsl	r3, r7, #1
	mov	r0, r9
	str	r2, [sp]
	bl	Func_8019000
	mov	r3, #0xf8
	lsl	r3, #14
	and	r3, r6
	cmp	r3, #0
	beq	.L2423e
	mov	r0, #4
	bl	SetTextColor
	b	.L2424e
.L2423e:
	mov	r3, #0x80
	lsl	r3, #9
	and	r3, r6
	cmp	r3, #0
	beq	.L2424e
	mov	r0, #2
	bl	SetTextColor
.L2424e:
	mov	r1, #0xf0
	lsl	r1, #4
	mov	r3, r6
	and	r3, r1
	lsr	r3, #8
	lsl	r0, r3, #2
	add	r0, r3
	mov	r3, #0xff
	and	r3, r6
	lsl	r0, #2
	add	r0, r3
	ldr	r3, =0x45f
	lsl	r5, r7, #4
	add	r0, r3
	mov	r1, r9
	mov	r2, #8
	mov	r3, r5
	bl	Func_801e7c0
	mov	r0, #0xf8
	lsl	r0, #14
	and	r0, r6
	cmp	r0, #0
	beq	.L2428c
	lsr	r0, #17
	mov	r1, #1
	mov	r2, r9
	mov	r3, #0x30
	str	r5, [sp]
	bl	Func_801e9d4
.L2428c:
	mov	r0, #0xf
	add	r7, #1
	bl	SetTextColor
	cmp	r7, #4
	bgt	.L242a8
	mov	r2, #4
	add	r8, r2
	mov	r3, r8
	mov	r1, #0x80
	ldr	r6, [r3]
	lsl	r1, #24
	cmp	r6, r1
	bne	.L24212
.L242a8:
	mov	r12, r11
	mov	r2, r12
	str	r2, [sp, #0x30]
.L242ae:
	ldr	r1, [sp, #0x34]
	cmp	r1, #5
	ble	.L242f8
	mov	r7, #0
	add	r1, #4
	mov	r8, r1
	b	.L242ea
.L242bc:
	ldr	r2, =0xf301
	mov	r0, r11
	mov	r1, #5
	add	r6, r7, r2
	bl	__divsi3
	cmp	r7, r0
	bne	.L242d0
	ldr	r3, =0xf30b
	add	r6, r7, r3
.L242d0:
	mov	r1, r9
	ldrh	r2, [r1, #8]
	sub	r2, r5
	add	r2, r7
	mov	r3, #0
	str	r3, [sp]
	sub	r2, #2
	mov	r0, r9
	mov	r1, r6
	sub	r3, #1
	bl	Func_8019000
	add	r7, #1
.L242ea:
	mov	r0, r8
	mov	r1, #5
	bl	__divsi3
	mov	r5, r0
	cmp	r7, r5
	blt	.L242bc
.L242f8:
	mov	r1, r9
	ldrh	r0, [r1, #0xc]
	mov	r2, r10
	ldrh	r1, [r1, #0xe]
	lsl	r3, r2, #1
	add	r1, r3
	mov	r3, r9
	ldrh	r2, [r3, #8]
	mov	r3, #0xe
	add	r1, #1
	sub	r2, #2
	str	r3, [sp]
	add	r0, #1
	mov	r3, #1
	bl	Func_8022768
	ldr	r1, [sp, #8]
	mov	r3, #1
	strb	r3, [r1]
	ldr	r2, [sp, #0x48]
	ldr	r1, =0xea6
	add	r3, r2, r1
	mov	r2, #0
	strb	r2, [r3]
.L24328:
	ldr	r3, [sp, #0x34]
	cmp	r3, #5
	ble	.L24412
	mov	r7, #0
	add	r3, #4
	mov	r8, r3
	b	.L2438a
.L24336:
	ldr	r3, =gKeyHeld
	mov	r2, #0x80
	ldr	r3, [r3]
	ldr	r1, =0xf301
	lsl	r2, #1
	and	r3, r2
	add	r6, r7, r1
	cmp	r3, #0
	bne	.L24354
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #0xf
	and	r3, r2
	cmp	r3, #0xb
	bhi	.L24364
.L24354:
	mov	r0, r11
	mov	r1, #5
	bl	__divsi3
	cmp	r7, r0
	bne	.L24364
	ldr	r2, =0xf30b
	add	r6, r7, r2
.L24364:
	mov	r3, r9
	mov	r1, #5
	mov	r0, r8
	ldrh	r5, [r3, #8]
	bl	__divsi3
	sub	r5, r0
	add	r5, r7
	mov	r1, #0
	sub	r5, #2
	mov	r3, #1
	str	r1, [sp]
	mov	r0, r9
	mov	r1, r6
	mov	r2, r5
	neg	r3, r3
	bl	Func_8019000
	add	r7, #1
.L2438a:
	mov	r0, r8
	mov	r1, #5
	bl	__divsi3
	cmp	r7, r0
	blt	.L24336
	ldr	r3, =gKeyHeld
	ldr	r5, [r3]
	mov	r3, #0x80
	lsl	r3, #1
	and	r5, r3
	cmp	r5, #0
	bne	.L243d0
	mov	r3, r9
	ldrh	r2, [r3, #8]
	mov	r3, #1
	sub	r2, r0
	sub	r2, #3
	mov	r0, r9
	ldr	r1, =0xf334
	neg	r3, r3
	str	r5, [sp]
	bl	Func_8019000
	mov	r1, r9
	ldrh	r2, [r1, #8]
	mov	r3, #1
	sub	r2, #2
	mov	r0, r9
	ldr	r1, =0xf335
	neg	r3, r3
	str	r5, [sp]
	bl	Func_8019000
	b	.L243fe
.L243d0:
	mov	r3, r9
	ldrh	r2, [r3, #8]
	mov	r1, #0
	sub	r2, r0
	mov	r3, #1
	sub	r2, #3
	str	r1, [sp]
	mov	r0, r9
	ldr	r1, =0xf011
	neg	r3, r3
	bl	Func_8019000
	mov	r3, r9
	ldrh	r2, [r3, #8]
	mov	r1, #0
	mov	r3, #1
	str	r1, [sp]
	sub	r2, #2
	mov	r0, r9
	ldr	r1, =0xf012
	neg	r3, r3
	bl	Func_8019000
.L243fe:
	mov	r2, r9
	ldrh	r3, [r2, #0xe]
	ldr	r1, [sp, #8]
	sub	r3, #1
	lsr	r3, #2
	mov	r2, #2
	lsl	r2, r3
	ldrb	r3, [r1]
	orr	r2, r3
	strb	r2, [r1]
.L24412:
	ldr	r3, =gKeyPress
	ldr	r1, [r3]
	ldr	r3, =gKeyRepeat
	ldr	r0, =iwram_3001f34
	ldr	r7, [r3]
	ldr	r3, =gKeyHeld
	ldr	r2, [r0]
	ldr	r3, [r3]
	mov	r8, r3
	mov	r3, r2
	add	r3, #0xd8
	ldr	r3, [r3]
	cmp	r3, #0
	beq	.L2444a
	add	r2, #0xdc
	ldr	r3, [r2]
	mov	r1, #0
	mov	r7, #0
	mov	r8, r1
	cmp	r3, #0
	bne	.L24446
	mov	r3, #0x3c
	str	r3, [r2]
	mov	r7, #1
	mov	r1, #1
	b	.L2444a
.L24446:
	sub	r3, #1
	str	r3, [r2]
.L2444a:
	ldr	r2, [r0]
	ldr	r3, [r2, #0x4c]
	cmp	r3, #0
	beq	.L2445a
	mov	r3, #2
	and	r3, r1
	cmp	r3, #0
	beq	.L24466
.L2445a:
	mov	r0, #0x71
	mov	r6, #1
	bl	_PlaySound
	neg	r6, r6
	b	.L248a0
.L24466:
	mov	r3, #1
	and	r3, r1
	cmp	r3, #0
	beq	.L2451c
	ldr	r3, [sp, #0x34]
	cmp	r3, #0
	beq	.L244d6
	mov	r3, r11
	add	r3, r10
	ldr	r1, [sp, #0x38]
	lsl	r3, #2
	ldr	r0, [r3, r1]
	mov	r6, #0xf8
	lsl	r6, #14
	mov	r5, r0
	and	r5, r6
	cmp	r5, #0
	bne	.L244a0
	ldr	r3, [sp, #0x18]
	cmp	r3, #0
	bne	.L244ce
	mov	r1, r11
	mov	r3, r10
	str	r1, [r2, #0x34]
	str	r3, [r2, #0x30]
	ldr	r1, [sp, #0x24]
	mov	r6, r0
	str	r1, [r2, #0x38]
	b	.L248a0
.L244a0:
	ldr	r2, [sp, #0x18]
	cmp	r2, #0
	bne	.L244ce
	and	r5, r6
	bl	Func_8016738
	bl	Func_80198dc
	lsr	r0, r5, #17
	mov	r1, #5
	bl	Func_8019908
	mov	r2, #0x34
	ldr	r1, [sp, #0xc]
	ldr	r0, =0x898
	bl	Func_801965c
	mov	r2, #0
	ldr	r0, [sp, #0xc]
	ldr	r1, [sp, #0x2c]
	mov	r3, #4
	bl	Func_8017aa4
.L244ce:
	mov	r0, #0x72
	bl	_PlaySound
	b	.L2451c
.L244d6:
	mov	r6, #1
	neg	r6, r6
	b	.L248a0

	.pool_aligned

.L2451c:
	ldr	r3, [sp, #0x34]
	cmp	r3, #0
	bne	.L24524
	b	.L24766
.L24524:
	mov	r3, #0x80
	and	r3, r7
	cmp	r3, #0
	beq	.L24550
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r1, #1
	add	r10, r1
	mov	r2, r10
	cmp	r2, #5
	beq	.L24546
	mov	r3, r11
	ldr	r1, [sp, #0x34]
	add	r3, r10
	cmp	r3, r1
	bne	.L2454a
.L24546:
	mov	r2, #0
	mov	r10, r2
.L2454a:
	mov	r3, r10
	str	r3, [sp, #0x24]
	b	.L24766
.L24550:
	mov	r3, #0x40
	and	r3, r7
	cmp	r3, #0
	beq	.L24590
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r1, #1
	neg	r1, r1
	add	r10, r1
	mov	r2, r10
	cmp	r2, #0
	bge	.L2458a
	ldr	r0, [sp, #0x34]
	mov	r1, #5
	sub	r0, #1
	bl	__divsi3
	lsl	r3, r0, #2
	add	r3, r0
	cmp	r11, r3
	bne	.L24586
	ldr	r1, [sp, #0x34]
	mov	r2, r11
	sub	r3, r1, r2
	sub	r3, #1
	b	.L24588
.L24586:
	mov	r3, #4
.L24588:
	mov	r10, r3
.L2458a:
	mov	r1, r10
	str	r1, [sp, #0x24]
	b	.L24766
.L24590:
	mov	r3, #0x80
	lsl	r3, #1
	mov	r2, r8
	and	r3, r2
	cmp	r3, #0
	beq	.L24694
	ldr	r3, [sp, #0x44]
	cmp	r3, #0
	beq	.L24622
	ldr	r0, [sp, #0x50]
	mov	r5, #0
	cmp	r5, r0
	bge	.L245e6
.L245aa:
	ldr	r3, =iwram_3001e40
	ldr	r2, =0xf301
	ldr	r3, [r3]
	add	r1, r5, r2
	mov	r2, #0xf
	and	r3, r2
	cmp	r3, #0xb
	bhi	.L245c8
	ldr	r3, [sp, #0x20]
	sub	r3, #1
	cmp	r5, r3
	bne	.L245c8
	ldr	r3, [sp, #0x20]
	ldr	r2, =0xf30a
	add	r1, r3, r2
.L245c8:
	ldr	r3, [sp, #0x44]
	ldrh	r2, [r3, #8]
	sub	r2, r0
	add	r2, r5
	mov	r3, #0
	str	r3, [sp]
	ldr	r0, [sp, #0x44]
	sub	r2, #2
	sub	r3, #1
	bl	Func_8019000
	ldr	r0, [sp, #0x50]
	add	r5, #1
	cmp	r5, r0
	blt	.L245aa
.L245e6:
	ldr	r1, [sp, #0x44]
	ldrh	r2, [r1, #8]
	mov	r3, #0
	sub	r2, r0
	str	r3, [sp]
	mov	r0, r1
	sub	r2, #3
	ldr	r1, =0xf334
	sub	r3, #1
	bl	Func_8019000
	ldr	r1, [sp, #0x44]
	ldrh	r2, [r1, #8]
	mov	r3, #0
	str	r3, [sp]
	mov	r0, r1
	sub	r2, #2
	ldr	r1, =0xf335
	sub	r3, #1
	bl	Func_8019000
	ldr	r1, [sp, #0x44]
	ldrh	r2, [r1, #0xe]
	ldr	r1, [sp, #8]
	lsr	r2, #2
	mov	r3, #2
	lsl	r3, r2
	ldrb	r2, [r1]
	orr	r3, r2
	strb	r3, [r1]
.L24622:
	ldr	r2, [sp, #0x20]
	cmp	r2, #0
	bne	.L24642
	ldr	r0, [sp, #0x50]
	cmp	r0, #0
	beq	.L24644
	ldr	r3, [sp, #0x44]
	cmp	r3, #0
	beq	.L2463a
	mov	r0, r3
	bl	Func_80164ac
.L2463a:
	mov	r1, #1
	str	r1, [sp, #0x20]
	str	r1, [sp, #0x1c]
	b	.L24766
.L24642:
	ldr	r0, [sp, #0x50]
.L24644:
	ldr	r2, [sp, #0x20]
	cmp	r2, r0
	ble	.L2464c
	str	r0, [sp, #0x20]
.L2464c:
	ldr	r3, [sp, #0x20]
	cmp	r3, #0
	bne	.L24654
	b	.L24766
.L24654:
	mov	r3, #0x10
	and	r3, r7
	cmp	r3, #0
	beq	.L24672
	mov	r0, #0x6f
	bl	_PlaySound
	ldr	r1, [sp, #0x20]
	ldr	r3, [sp, #0x50]
	add	r1, #1
	str	r1, [sp, #0x20]
	cmp	r1, r3
	ble	.L2468e
	mov	r2, #1
	b	.L2468c
.L24672:
	mov	r3, #0x20
	and	r3, r7
	cmp	r3, #0
	beq	.L24766
	mov	r0, #0x6f
	bl	_PlaySound
	ldr	r1, [sp, #0x20]
	sub	r1, #1
	str	r1, [sp, #0x20]
	cmp	r1, #0
	bgt	.L2468e
	ldr	r2, [sp, #0x50]
.L2468c:
	str	r2, [sp, #0x20]
.L2468e:
	mov	r3, #1
	str	r3, [sp, #0x1c]
	b	.L24766
.L24694:
	ldr	r1, [sp, #0x20]
	cmp	r1, #0
	beq	.L246b0
	ldr	r2, [sp, #0x44]
	cmp	r2, #0
	beq	.L246a6
	mov	r0, r2
	bl	Func_80164ac
.L246a6:
	mov	r3, #0
	mov	r1, #1
	str	r3, [sp, #0x20]
	str	r1, [sp, #0x1c]
	b	.L24766
.L246b0:
	mov	r3, #0x10
	and	r3, r7
	cmp	r3, #0
	beq	.L24708
	mov	r0, #0x6f
	bl	_PlaySound
	bl	Func_800352c
	mov	r3, r11
	ldr	r2, [sp, #0x34]
	add	r3, #5
	cmp	r3, r2
	blt	.L246dc
	mov	r3, r11
	cmp	r3, #0
	beq	.L24766
	ldr	r2, [sp, #0x24]
	mov	r1, #0
	mov	r11, r1
	mov	r10, r2
	b	.L24766
.L246dc:
	ldr	r0, [sp, #0x34]
	mov	r11, r3
	ldr	r3, [sp, #0x24]
	sub	r0, #1
	mov	r1, #5
	mov	r10, r3
	bl	__divsi3
	lsl	r3, r0, #2
	add	r3, r0
	cmp	r11, r3
	bne	.L24766
	ldr	r1, [sp, #0x34]
	mov	r2, r11
	sub	r3, r1, r2
	sub	r3, #1
	mov	r10, r3
	ldr	r3, [sp, #0x24]
	cmp	r10, r3
	ble	.L24766
	mov	r10, r3
	b	.L24766
.L24708:
	mov	r3, #0x20
	and	r3, r7
	cmp	r3, #0
	beq	.L24766
	mov	r0, #0x6f
	bl	_PlaySound
	bl	Func_800352c
	mov	r1, r11
	cmp	r1, #0
	beq	.L24740
	mov	r2, #5
	ldr	r3, [sp, #0x24]
	neg	r2, r2
	add	r11, r2
	mov	r10, r3
	b	.L24766

	.pool_aligned

.L24740:
	ldr	r0, [sp, #0x34]
	mov	r1, #5
	sub	r0, #1
	bl	__divsi3
	lsl	r3, r0, #2
	ldr	r1, [sp, #0x24]
	add	r3, r0
	mov	r11, r3
	mov	r10, r1
	cmp	r3, #0
	beq	.L24766
	ldr	r2, [sp, #0x34]
	sub	r3, r2, r3
	sub	r3, #1
	mov	r10, r3
	cmp	r10, r1
	ble	.L24766
	mov	r10, r1
.L24766:
	mov	r1, r9
	ldrh	r3, [r1, #0xc]
	lsl	r3, #3
	mov	r2, r10
	sub	r3, #2
	str	r3, [sp, #0x10]
	lsl	r3, r2, #1
	ldrh	r2, [r1, #0xe]
	add	r3, r2
	lsl	r3, #3
	add	r3, #0x14
	ldr	r1, [sp, #4]
	str	r3, [sp, #0x14]
	mov	r3, #0x80
	lsl	r3, #23
	mov	r2, #0
	str	r3, [r1, #4]
	str	r2, [r1, #8]
	ldr	r0, [sp, #0x3c]
	ldr	r1, =Data_310a4
	bl	UploadSprite2
	ldr	r3, .L247c8	@ 0x3ff
	ldr	r1, [sp, #4]
	and	r0, r3
	ldr	r2, .L247cc	@ 0xfffffc00
	ldrh	r3, [r1, #8]
	ldr	r6, =iwram_3001e40
	and	r3, r2
	orr	r3, r0
	mov	r2, r1
	strh	r3, [r2, #8]
	ldr	r2, [r6]
	mov	r5, #4
	ldr	r3, [sp, #0x10]
	and	r2, r5
	ldr	r1, =0xfffa
	lsr	r2, #1
	add	r2, r3, r2
	add	r2, r1
	ldr	r3, .L247d0	@ 0x1ff
	ldr	r1, [sp, #4]
	and	r2, r3
	ldrh	r3, [r1, #6]
	ldr	r1, .L247d4	@ 0xfffffe00
	and	r3, r1
	orr	r3, r2
	b	.L247e4

	.align	2, 0
.L247c8:
	.word	0x3ff
.L247cc:
	.word	0xfffffc00
.L247d0:
	.word	0x1ff
.L247d4:
	.word	0xfffffe00
	.pool

.L247e4:
	ldr	r2, [sp, #4]
	strh	r3, [r2, #6]
	ldr	r3, [r6]
	ldr	r1, [sp, #0x14]
	and	r3, r5
	lsr	r3, #2
	sub	r3, r1, r3
	add	r3, #0xf8
	strb	r3, [r2, #4]
	ldr	r2, [sp, #0x34]
	cmp	r2, #0
	beq	.L24804
	ldr	r0, [sp, #4]
	mov	r1, #0xf2
	bl	Func_8003dec
.L24804:
	ldr	r3, =iwram_3001e90
	ldr	r3, [r3]
	ldrh	r2, [r3, #0xc]
	ldr	r6, [r6]
	ldr	r7, [r3]
	mov	r3, #2
	and	r3, r2
	and	r6, r5
	cmp	r3, #0
	beq	.L24858
	mov	r5, #0
.L2481a:
	neg	r3, r6
	orr	r3, r6
	lsr	r3, #31
	mov	r2, r3
	mov	r3, #0xf
	sub	r2, r3, r2
	ldr	r1, [sp, #0x28]
	mov	r3, #1
	lsl	r3, r5
	and	r3, r1
	cmp	r3, #0
	bne	.L24834
	mov	r2, #0xf
.L24834:
	ldr	r3, =.L373e7
	ldrh	r0, [r7, #0xc]
	ldrb	r3, [r3, r5]
	add	r0, r3
	ldr	r3, =.L373eb
	ldrh	r1, [r7, #0xe]
	ldrb	r3, [r3, r5]
	add	r1, r3
	str	r2, [sp]
	add	r0, #1
	add	r1, #1
	mov	r2, #2
	mov	r3, #2
	add	r5, #1
	bl	Func_8022768
	cmp	r5, #3
	ble	.L2481a
.L24858:
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #4
	and	r3, r2
	cmp	r3, #0
	beq	.L2487e
	ldr	r5, =.L37308
	mov	r2, #0x20
	mov	r1, r5
	ldr	r6, =Func_8001af8
	ldr	r0, =0x6006500
	bl	_call_via_r6
	ldr	r0, =0x6006520
	mov	r1, r5
	mov	r2, #0x20
	bl	_call_via_r6
	b	.L24896
.L2487e:
	ldr	r3, =Func_80008d8
	mov	r1, #0x20
	ldr	r2, =0x44444444
	ldr	r0, =0x6006500
	bl	_call_via_r3
	ldr	r3, =Func_8001af8
	ldr	r0, =0x6006520
	ldr	r1, =.L37308
	mov	r2, #0x20
	bl	_call_via_r3
.L24896:
	mov	r0, #1
	bl	WaitFrames
	bl	.L23ff4
.L248a0:
	ldr	r3, =iwram_3001e90
	ldr	r1, [r3]
	ldrh	r2, [r1, #0xc]
	mov	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.L248c4
	ldr	r3, [r1]
	ldrh	r0, [r3, #0xc]
	ldrh	r1, [r3, #0xe]
	mov	r3, #0xf
	str	r3, [sp]
	add	r0, #1
	add	r1, #1
	mov	r2, #4
	mov	r3, #4
	bl	Func_8022768
.L248c4:
	ldr	r0, [sp, #0x3c]
	bl	Func_8003f3c
	mov	r1, #1
	ldr	r0, [sp, #0x2c]
	bl	CloseUIBox
	mov	r1, #1
	ldr	r0, [sp, #0x44]
	bl	CloseUIBox
	mov	r1, #1
	mov	r0, r9
	bl	CloseUIBox
	bl	Func_801e318
	mov	r0, #0
	bl	Func_801e3c8
	ldr	r0, [sp, #0x38]
	bl	free
	mov	r0, #1
	bl	WaitFrames
	mov	r0, r6
	add	sp, #0xe0
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8023e70

@ RunDjinnScreen
@ r0.. = parameters. 1059 lines, same construction with glyph nodes allocated
@ through Func_18efc. Traced structurally.
.thumb_func_start Func_8024934  @ 0x08024934
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x174
	str	r2, [sp, #0x54]
	ldr	r5, =iwram_3001e8c
	ldr	r0, [r5]
	mov	r1, #1
	str	r0, [sp, #0x48]
	neg	r1, r1
	mov	r0, #0x80
	str	r1, [sp, #0x44]
	mov	r9, r1
	bl	AllocUploadSpriteGFX
	lsl	r0, #16
	asr	r0, #16
	mov	r3, #0x2a
	str	r0, [sp, #0x40]
	str	r3, [sp]
	mov	r1, #4
	mov	r2, #0x1e
	mov	r3, #4
	mov	r0, #0
	bl	CreateUIBox
	mov	r6, #6
	str	r0, [sp, #0x3c]
	mov	r1, #8
	mov	r2, #0xa
	mov	r3, #3
	mov	r0, #0x14
	str	r6, [sp]
	bl	CreateUIBox
	mov	r2, #0
	str	r0, [sp, #0x38]
	str	r2, [sp, #0x34]
	add	r5, #0xa8
	ldr	r3, [r5]
	ldr	r0, [r3, #0x34]
	ldr	r1, [r3, #0x30]
	ldr	r3, [r3, #0x38]
	mov	r11, r0
	mov	r10, r1
	str	r3, [sp, #0x30]
	str	r6, [sp]
	mov	r2, #0x11
	mov	r3, #9
	mov	r0, #0xd
	mov	r1, #0xb
	bl	CreateUIBox
	mov	r2, #0x9c
	lsl	r2, #1
	add	r2, sp
	ldr	r3, =0xfffffe00
	mov	r7, #0x80
	str	r0, [sp, #0x4c]
	str	r2, [sp, #0x1c]
	mov	r4, #0
	mov	r12, r3
	mov	r5, r2
	lsl	r7, #23
	mov	r6, #0
.L249be:
	lsl	r0, r4, #1
	str	r7, [r5, #4]
	str	r6, [r5, #8]
	ldr	r1, [sp, #0x4c]
	ldrh	r2, [r1, #0xc]
	ldr	r3, .L249fc	@ 0x1ff
	lsl	r2, #3
	ldrh	r1, [r5, #6]
	add	r2, #8
	and	r2, r3
	mov	r3, r12
	and	r3, r1
	orr	r3, r2
	strh	r3, [r5, #6]
	ldr	r2, [sp, #0x4c]
	ldrh	r3, [r2, #0xe]
	add	r0, r3
	lsl	r0, #3
	add	r0, #4
	add	r4, #1
	strb	r0, [r5, #4]
	add	r5, #0xc
	cmp	r4, #3
	ble	.L249be
	ldr	r3, =0xfffffc00
	ldr	r7, [sp, #0x1c]
	mov	r5, #8
	add	r6, sp, #0x60
	mov	r8, r3
	mov	r4, #3
	b	.L24a0c

	.align	2, 0
.L249fc:
	.word	0x1ff
	.pool

.L24a0c:
	mov	r0, #0x80
	str	r4, [sp, #4]
	bl	AllocUploadSpriteGFX
	mov	r1, #1
	neg	r1, r1
	stmia	r6!, {r0}
	bl	UploadSprite2
	ldr	r3, =0x3ff
	and	r0, r3
	ldrh	r3, [r5, r7]
	mov	r1, r8
	ldr	r4, [sp, #4]
	and	r3, r1
	orr	r3, r0
	sub	r4, #1
	strh	r3, [r5, r7]
	add	r5, #0xc
	cmp	r4, #0
	bge	.L24a0c
	b	.L24a3c

	.pool_aligned

.L24a3c:
	mov	r2, #0x8a
	lsl	r2, #1
	add	r2, sp
	mov	r8, r2
	mov	r0, r8
	bl	_Func_807977c
	str	r0, [sp, #0x50]
	mov	r7, #0
	mov	r3, r0
	sub	r3, #1
	str	r3, [sp, #0x14]
	cmp	r3, #0
	blt	.L24aa0
	mov	r0, sp
	add	r0, #0xf0
	mov	r5, r3
	str	r0, [sp, #0x20]
	add	r5, r8
.L24a62:
	ldrb	r6, [r5]
	mov	r0, r6
	bl	_GetSummonInfo
	ldr	r1, [sp, #0x54]
	add	r0, #4
	ldrb	r2, [r0]
	ldrb	r3, [r1]
	mov	r4, #0
	cmp	r2, r3
	bhi	.L24a8a
.L24a78:
	add	r4, #1
	cmp	r4, #3
	bgt	.L24a8a
	add	r0, #1
	add	r1, #1
	ldrb	r2, [r0]
	ldrb	r3, [r1]
	cmp	r2, r3
	bls	.L24a78
.L24a8a:
	cmp	r4, #4
	bne	.L24a98
	ldr	r2, [sp, #0x20]
	mov	r3, #0x20
	strb	r6, [r2, r7]
	strb	r3, [r5]
	add	r7, #1
.L24a98:
	sub	r5, #1
	cmp	r5, r8
	bge	.L24a62
	b	.L24aa6
.L24aa0:
	mov	r3, sp
	add	r3, #0xf0
	str	r3, [sp, #0x20]
.L24aa6:
	ldr	r0, [sp, #0x50]
	cmp	r0, #0
	ble	.L24ac8
	ldr	r2, [sp, #0x20]
	add	r1, r7, r2
	ldr	r2, [sp, #0x50]
	mov	r0, r8
.L24ab4:
	ldrb	r3, [r0]
	add	r0, #1
	cmp	r3, #0x20
	beq	.L24ac2
	strb	r3, [r1]
	add	r7, #1
	add	r1, #1
.L24ac2:
	sub	r2, #1
	cmp	r2, #0
	bne	.L24ab4
.L24ac8:
	ldr	r0, [sp, #0x20]
	mov	r3, #0x20
	strb	r3, [r0, r7]
	mov	r1, #0xb4
	ldr	r3, [sp, #0x40]
	lsl	r1, #1
	mov	r2, r10
	add	r1, sp
	lsl	r2, #1
	lsl	r3, #16
	str	r1, [sp, #0x18]
	str	r2, [sp, #0x10]
	str	r3, [sp, #0xc]
.L24ae2:
	cmp	r11, r9
	bne	.L24aee
	ldr	r0, [sp, #0x44]
	cmp	r10, r0
	bne	.L24aee
	b	.L24d90
.L24aee:
	ldr	r1, [sp, #0x48]
	ldr	r2, =0xea6
	mov	r0, #1
	add	r3, r1, r2
	strb	r0, [r3]
	ldr	r1, [sp, #0x4c]
	ldr	r2, [sp, #0x44]
	ldrh	r0, [r1, #0xc]
	ldrh	r1, [r1, #0xe]
	lsl	r3, r2, #1
	add	r1, r3
	ldr	r3, [sp, #0x4c]
	ldrh	r2, [r3, #8]
	mov	r3, #0xf
	str	r3, [sp]
	sub	r2, #2
	add	r1, #1
	mov	r3, #1
	add	r0, #1
	bl	Func_8022768
	bl	Func_8016738
	mov	r3, r11
	ldr	r1, [sp, #0x20]
	add	r3, r10
	ldrb	r0, [r1, r3]
	bl	_GetSummonInfo
	mov	r6, r0
	ldrh	r0, [r6]
	ldr	r3, =0x53a
	add	r5, sp, #0x70
	add	r0, r3
	mov	r1, r5
	mov	r2, #0x34
	bl	Func_801965c
	mov	r2, #0
	ldr	r1, [sp, #0x3c]
	mov	r3, #4
	mov	r0, r5
	bl	Func_8017aa4
	mov	r3, #0
	mov	r2, r10
	str	r3, [sp, #0x34]
	str	r2, [sp, #0x44]
	mov	r1, #1
	mov	r2, #0
	add	r6, #4
.L24b54:
	ldrb	r3, [r6]
	add	r6, #1
	cmp	r3, #0
	beq	.L24b66
	ldr	r0, [sp, #0x34]
	mov	r3, r1
	lsl	r3, r2
	orr	r0, r3
	str	r0, [sp, #0x34]
.L24b66:
	add	r2, #1
	cmp	r2, #3
	ble	.L24b54
	cmp	r11, r9
	bne	.L24b72
	b	.L24d06
.L24b72:
	ldr	r0, [sp, #0x4c]
	bl	Func_8016498
	mov	r5, #0
	mov	r7, #0
	mov	r6, #1
.L24b7e:
	ldr	r2, =0x5001
	ldr	r0, [sp, #0x38]
	add	r1, r5, r2
	mov	r3, #0
	lsl	r2, r5, #1
	str	r7, [sp]
	bl	Func_8019000
	ldr	r3, [sp, #0x54]
	ldrb	r1, [r3, r5]
	mov	r2, r6
	add	r1, #0x30
	ldr	r0, [sp, #0x38]
	mov	r3, #0
	add	r5, #1
	str	r7, [sp]
	add	r6, #2
	bl	Func_8018efc
	cmp	r5, #3
	ble	.L24b7e
	ldr	r0, [sp, #0x20]
	mov	r1, r11
	ldrb	r6, [r0, r1]
	mov	r4, #0
	cmp	r6, #0x20
	bne	.L24bb6
	b	.L24ce8
.L24bb6:
	mov	r2, sp
	add	r2, #0x58
	str	r2, [sp, #8]
.L24bbc:
	mov	r0, r6
	str	r4, [sp, #4]
	bl	_GetSummonInfo
	str	r0, [sp, #0x24]
	mov	r1, r0
	ldr	r0, [sp, #0x54]
	add	r1, #4
	ldrb	r2, [r1]
	ldrb	r3, [r0]
	mov	r7, #0
	ldr	r4, [sp, #4]
	cmp	r2, r3
	bhi	.L24bea
.L24bd8:
	add	r7, #1
	cmp	r7, #3
	bgt	.L24bea
	add	r1, #1
	add	r0, #1
	ldrb	r2, [r1]
	ldrb	r3, [r0]
	cmp	r2, r3
	bls	.L24bd8
.L24bea:
	mov	r3, #4
	eor	r3, r7
	ldr	r2, [sp, #0x24]
	neg	r5, r3
	orr	r5, r3
	ldr	r0, .L24c28	@ 0x3fff
	ldrh	r3, [r2]
	mov	r1, #1
	and	r0, r3
	add	r2, sp, #0x60
	lsl	r3, r4, #2
	lsr	r5, #31
	add	r2, r3
	str	r1, [sp]
	ldr	r3, [sp, #8]
	sub	r5, r1, r5
	mov	r1, #0
	str	r4, [sp, #4]
	bl	LoadMoveIcon
	ldr	r4, [sp, #4]
	lsl	r3, r4, #1
	add	r1, r3, r4
	ldr	r2, [sp, #0x1c]
	mov	r8, r3
	lsl	r1, #2
	ldr	r3, .L24c2c	@ 0x3ff
	ldr	r0, [sp, #0x58]
	add	r1, #8
	and	r0, r3
	b	.L24c3c

	.align	2, 0
.L24c28:
	.word	0x3fff
.L24c2c:
	.word	0x3ff
	.pool

.L24c3c:
	ldrh	r3, [r2, r1]
	ldr	r2, =0xfffffc00
	and	r3, r2
	orr	r3, r0
	ldr	r0, [sp, #0x1c]
	strh	r3, [r0, r1]
	cmp	r5, #0
	bne	.L24c54
	mov	r0, #2
	bl	SetTextColor
	ldr	r4, [sp, #4]
.L24c54:
	mov	r0, r6
	str	r4, [sp, #4]
	bl	_GetSummonInfo
	ldr	r3, =0x333
	ldr	r4, [sp, #4]
	ldrh	r0, [r0]
	ldr	r1, [sp, #0x4c]
	add	r0, r3
	mov	r2, #0x10
	lsl	r3, r4, #4
	bl	Func_801e7c0
	mov	r1, #0
	ldr	r6, [sp, #0x24]
	lsl	r3, r1, #1
	b	.L24c80

	.pool_aligned

.L24c80:
	mov	r5, r3
	ldr	r4, [sp, #4]
	mov	r7, #0
	mov	r9, r1
	add	r6, #4
	add	r5, #0xd
.L24c8c:
	ldrb	r3, [r6]
	cmp	r3, #0
	beq	.L24cbc
	ldr	r2, =0x5001
	mov	r3, r9
	add	r1, r7, r2
	str	r3, [sp]
	ldr	r0, [sp, #0x4c]
	mov	r2, r5
	mov	r3, r8
	str	r4, [sp, #4]
	bl	Func_8019000
	ldrb	r1, [r6]
	mov	r0, r9
	add	r2, r5, #1
	str	r0, [sp]
	add	r1, #0x30
	ldr	r0, [sp, #0x4c]
	mov	r3, r8
	bl	Func_8018efc
	ldr	r4, [sp, #4]
	add	r5, #2
.L24cbc:
	add	r7, #1
	add	r6, #1
	cmp	r7, #3
	ble	.L24c8c
	mov	r0, #0xf
	str	r4, [sp, #4]
	bl	SetTextColor
	ldr	r4, [sp, #4]
	add	r3, sp, #0x5c
	mov	r1, #1
	strb	r1, [r3, r4]
	add	r4, #1
	cmp	r4, #3
	bgt	.L24d04
	mov	r2, r11
	ldr	r0, [sp, #0x20]
	add	r3, r2, r4
	ldrb	r6, [r0, r3]
	cmp	r6, #0x20
	beq	.L24ce8
	b	.L24bbc
.L24ce8:
	cmp	r4, #3
	bgt	.L24d04
	add	r2, sp, #0x174
	ldr	r0, =0xfffffee8
	add	r3, r4, r2
	add	r2, r3, r0
	mov	r3, #4
	mov	r1, #0
	sub	r4, r3, r4
.L24cfa:
	sub	r4, #1
	strb	r1, [r2]
	add	r2, #1
	cmp	r4, #0
	bne	.L24cfa
.L24d04:
	mov	r9, r11
.L24d06:
	ldr	r1, [sp, #0x50]
	cmp	r1, #4
	ble	.L24d60
	mov	r4, #0
	mov	r5, r1
	add	r5, #3
	b	.L24d50

	.pool_aligned

.L24d1c:
	ldr	r2, =0xf301
	mov	r3, r11
	add	r1, r4, r2
	cmp	r3, #0
	bge	.L24d28
	add	r3, #3
.L24d28:
	asr	r3, #2
	cmp	r4, r3
	bne	.L24d32
	ldr	r3, =0xf30b
	add	r1, r4, r3
.L24d32:
	ldr	r3, [sp, #0x4c]
	ldrh	r2, [r3, #8]
	sub	r2, r0
	mov	r0, #0
	add	r2, r4
	str	r0, [sp]
	mov	r0, r3
	mov	r3, #1
	sub	r2, #2
	neg	r3, r3
	str	r4, [sp, #4]
	bl	Func_8019000
	ldr	r4, [sp, #4]
	add	r4, #1
.L24d50:
	mov	r3, r5
	cmp	r5, #0
	bge	.L24d5a
	ldr	r3, [sp, #0x50]
	add	r3, #6
.L24d5a:
	asr	r0, r3, #2
	cmp	r4, r0
	blt	.L24d1c
.L24d60:
	ldr	r1, [sp, #0x4c]
	ldr	r2, [sp, #0x10]
	ldrh	r0, [r1, #0xc]
	ldr	r3, [sp, #0x4c]
	ldrh	r1, [r1, #0xe]
	add	r1, r2
	ldrh	r2, [r3, #8]
	mov	r3, #0xe
	add	r0, #1
	add	r1, #1
	sub	r2, #2
	str	r3, [sp]
	mov	r3, #1
	bl	Func_8022768
	ldr	r1, =0xea3
	ldr	r0, [sp, #0x48]
	mov	r2, #1
	add	r3, r0, r1
	add	r1, #3
	strb	r2, [r3]
	add	r3, r0, r1
	mov	r2, #0
	strb	r2, [r3]
.L24d90:
	ldr	r6, [sp, #0x1c]
	mov	r4, #0
	add	r5, sp, #0x5c
.L24d96:
	ldrb	r3, [r5]
	add	r5, #1
	cmp	r3, #0
	beq	.L24daa
	mov	r0, r6
	mov	r1, #0xf0
	str	r4, [sp, #4]
	bl	Func_8003dec
	ldr	r4, [sp, #4]
.L24daa:
	add	r4, #1
	add	r6, #0xc
	cmp	r4, #3
	ble	.L24d96
	ldr	r0, [sp, #0x4c]
	ldrh	r3, [r0, #0xc]
	lsl	r3, #3
	sub	r3, #2
	ldr	r1, [sp, #0x10]
	str	r3, [sp, #0x28]
	ldrh	r3, [r0, #0xe]
	add	r3, r1, r3
	lsl	r3, #3
	add	r3, #0x14
	ldr	r2, [sp, #0x18]
	str	r3, [sp, #0x2c]
	mov	r3, #0x80
	lsl	r3, #23
	str	r3, [r2, #4]
	mov	r3, #0
	str	r3, [r2, #8]
	ldr	r1, [sp, #0xc]
	lsr	r0, r1, #16
	ldr	r1, =Data_310a4
	bl	UploadSprite2
	ldr	r3, .L24e10	@ 0x3ff
	ldr	r2, [sp, #0x18]
	and	r0, r3
	ldrh	r3, [r2, #8]
	ldr	r2, .L24e14	@ 0xfffffc00
	ldr	r1, =iwram_3001e40
	and	r3, r2
	orr	r3, r0
	ldr	r0, [sp, #0x18]
	ldr	r2, [r1]
	strh	r3, [r0, #8]
	mov	r0, #4
	ldr	r3, [sp, #0x28]
	and	r2, r0
	ldr	r1, =0xfffc
	lsr	r2, #1
	add	r2, r3, r2
	add	r2, r1
	ldr	r3, .L24e18	@ 0x1ff
	ldr	r1, [sp, #0x18]
	and	r2, r3
	ldrh	r3, [r1, #6]
	ldr	r1, .L24e1c	@ 0xfffffe00
	and	r3, r1
	b	.L24e38

	.align	2, 0
.L24e10:
	.word	0x3ff
.L24e14:
	.word	0xfffffc00
.L24e18:
	.word	0x1ff
.L24e1c:
	.word	0xfffffe00
	.pool

.L24e38:
	orr	r3, r2
	ldr	r1, =iwram_3001e40
	ldr	r2, [sp, #0x18]
	strh	r3, [r2, #6]
	ldr	r3, [r1]
	ldr	r2, [sp, #0x2c]
	and	r3, r0
	lsr	r3, #2
	ldr	r0, [sp, #0x18]
	sub	r3, r2, r3
	add	r3, #0xf8
	strb	r3, [r0, #4]
	mov	r1, #0xf2
	ldr	r0, [sp, #0x18]
	bl	Func_8003dec
	ldr	r1, =iwram_3001e40
	ldr	r6, [r1]
	mov	r3, #8
	and	r6, r3
	mov	r5, #0
.L24e62:
	neg	r3, r6
	orr	r3, r6
	lsr	r3, #31
	mov	r2, r3
	mov	r3, #0xf
	sub	r2, r3, r2
	ldr	r0, [sp, #0x34]
	mov	r3, #1
	lsl	r3, r5
	and	r3, r0
	cmp	r3, #0
	bne	.L24e7c
	mov	r2, #0xf
.L24e7c:
	ldr	r1, [sp, #0x38]
	ldrh	r0, [r1, #0xc]
	lsl	r3, r5, #1
	ldrh	r1, [r1, #0xe]
	add	r0, r3
	str	r2, [sp]
	add	r0, #1
	add	r1, #1
	mov	r2, #2
	mov	r3, #1
	add	r5, #1
	bl	Func_8022768
	cmp	r5, #3
	ble	.L24e62
	ldr	r2, [sp, #0x50]
	cmp	r2, #4
	ble	.L24f48
	mov	r4, #0
	mov	r5, r2
	add	r5, #3
	b	.L24ef2
.L24ea8:
	ldr	r3, =0xf301
	ldr	r0, =iwram_3001e40
	add	r1, r4, r3
	ldr	r3, [r0]
	mov	r2, #0xf
	and	r3, r2
	cmp	r3, #0xb
	bhi	.L24eca
	mov	r3, r11
	cmp	r3, #0
	bge	.L24ec0
	add	r3, #3
.L24ec0:
	asr	r3, #2
	cmp	r4, r3
	bne	.L24eca
	ldr	r2, =0xf30b
	add	r1, r4, r2
.L24eca:
	ldr	r0, [sp, #0x4c]
	mov	r2, r5
	ldrh	r3, [r0, #8]
	cmp	r5, #0
	bge	.L24ed8
	ldr	r2, [sp, #0x50]
	add	r2, #6
.L24ed8:
	asr	r2, #2
	sub	r2, r3, r2
	add	r2, r4
	mov	r3, #0
	str	r3, [sp]
	sub	r2, #2
	ldr	r0, [sp, #0x4c]
	sub	r3, #1
	str	r4, [sp, #4]
	bl	Func_8019000
	ldr	r4, [sp, #4]
	add	r4, #1
.L24ef2:
	mov	r3, r5
	cmp	r5, #0
	bge	.L24efc
	ldr	r3, [sp, #0x50]
	add	r3, #6
.L24efc:
	asr	r2, r3, #2
	cmp	r4, r2
	blt	.L24ea8
	ldr	r0, [sp, #0x4c]
	ldrh	r3, [r0, #8]
	mov	r5, #1
	neg	r5, r5
	sub	r2, r3, r2
	mov	r1, #0
	str	r1, [sp]
	ldr	r0, [sp, #0x4c]
	mov	r3, r5
	sub	r2, #3
	ldr	r1, =0xf334
	bl	Func_8019000
	ldr	r3, [sp, #0x4c]
	ldrh	r2, [r3, #8]
	mov	r0, #0
	str	r0, [sp]
	sub	r2, #2
	mov	r0, r3
	ldr	r1, =0xf335
	mov	r3, r5
	bl	Func_8019000
	ldr	r2, [sp, #0x48]
	ldr	r3, =0xea3
	ldr	r0, [sp, #0x4c]
	add	r1, r2, r3
	ldrh	r3, [r0, #0xe]
	sub	r3, #1
	lsr	r3, #2
	mov	r2, #2
	lsl	r2, r3
	ldrb	r3, [r1]
	orr	r2, r3
	strb	r2, [r1]
.L24f48:
	ldr	r3, =iwram_3001f34
	ldr	r2, [r3]
	mov	r1, r11
	mov	r3, r10
	str	r1, [r2, #0x34]
	str	r3, [r2, #0x30]
	ldr	r0, [sp, #0x30]
	str	r0, [r2, #0x38]
	ldr	r3, =gKeyPress
	ldr	r1, [r3]
	ldr	r3, =gKeyRepeat
	ldr	r0, [r3]
	mov	r3, r2
	add	r3, #0xd8
	ldr	r3, [r3]
	cmp	r3, #0
	beq	.L24f84
	add	r2, #0xdc
	ldr	r3, [r2]
	mov	r0, #0
	mov	r1, #0
	cmp	r3, #0
	bne	.L24f80
	mov	r3, #0x78
	str	r3, [r2]
	mov	r0, #1
	mov	r1, #1
	b	.L24f84
.L24f80:
	sub	r3, #1
	str	r3, [r2]
.L24f84:
	mov	r3, r1
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L24f98
	mov	r3, r11
	ldr	r0, [sp, #0x20]
	add	r3, r10
	ldrb	r6, [r0, r3]
	b	.L25106
.L24f98:
	ldr	r3, =iwram_3001f34
	ldr	r3, [r3]
	ldr	r3, [r3, #0x4c]
	cmp	r3, #0
	beq	.L24faa
	mov	r3, #2
	and	r3, r1
	cmp	r3, #0
	beq	.L24fb6
.L24faa:
	mov	r0, #0x71
	mov	r6, #1
	bl	_PlaySound
	neg	r6, r6
	b	.L25106
.L24fb6:
	mov	r3, #0x80
	and	r3, r0
	cmp	r3, #0
	beq	.L24fe6
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r1, #1
	add	r10, r1
	mov	r2, r10
	cmp	r2, #4
	beq	.L24fd8
	mov	r3, r11
	ldr	r0, [sp, #0x50]
	add	r3, r10
	cmp	r3, r0
	bne	.L24fdc
.L24fd8:
	mov	r1, #0
	mov	r10, r1
.L24fdc:
	mov	r3, r10
	mov	r2, r10
	lsl	r3, #1
	str	r2, [sp, #0x30]
	b	.L250fc
.L24fe6:
	mov	r3, #0x40
	and	r3, r0
	cmp	r3, #0
	beq	.L2502c
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r0, #1
	neg	r0, r0
	add	r10, r0
	mov	r1, r10
	cmp	r1, #0
	bge	.L25022
	ldr	r3, [sp, #0x14]
	cmp	r3, #0
	bge	.L2500a
	ldr	r3, [sp, #0x50]
	add	r3, #2
.L2500a:
	asr	r3, #2
	lsl	r3, #2
	cmp	r11, r3
	bne	.L2501e
	ldr	r2, [sp, #0x50]
	mov	r0, r11
	sub	r3, r2, r0
	sub	r3, #1
	mov	r10, r3
	b	.L25022
.L2501e:
	mov	r1, #3
	mov	r10, r1
.L25022:
	mov	r3, r10
	mov	r2, r10
	lsl	r3, #1
	str	r2, [sp, #0x30]
	b	.L250fc
.L2502c:
	mov	r3, #0x10
	and	r3, r0
	cmp	r3, #0
	beq	.L2508a
	mov	r0, #0x6f
	bl	_PlaySound
	bl	Func_800352c
	mov	r3, r11
	ldr	r0, [sp, #0x50]
	add	r3, #4
	cmp	r3, r0
	blt	.L2505e
	mov	r1, r11
	cmp	r1, #0
	beq	.L250fe
	ldr	r3, [sp, #0x30]
	mov	r10, r3
	mov	r0, r10
	mov	r2, #0
	lsl	r0, #1
	mov	r11, r2
	str	r0, [sp, #0x10]
	b	.L250fe
.L2505e:
	mov	r11, r3
	ldr	r1, [sp, #0x30]
	ldr	r3, [sp, #0x14]
	mov	r10, r1
	cmp	r3, #0
	bge	.L2506e
	ldr	r3, [sp, #0x50]
	add	r3, #2
.L2506e:
	asr	r3, #2
	lsl	r3, #2
	cmp	r11, r3
	bne	.L250e0
	ldr	r2, [sp, #0x50]
	mov	r0, r11
	sub	r3, r2, r0
	sub	r3, #1
	ldr	r1, [sp, #0x30]
	mov	r10, r3
	cmp	r10, r1
	ble	.L250e8
	mov	r10, r1
	b	.L250f0
.L2508a:
	mov	r3, #0x20
	and	r3, r0
	cmp	r3, #0
	beq	.L250fe
	mov	r0, #0x6f
	bl	_PlaySound
	bl	Func_800352c
	mov	r3, r11
	cmp	r3, #0
	beq	.L250b4
	ldr	r1, [sp, #0x30]
	mov	r10, r1
	mov	r0, #4
	mov	r2, r10
	neg	r0, r0
	lsl	r2, #1
	add	r11, r0
	str	r2, [sp, #0x10]
	b	.L250fe
.L250b4:
	ldr	r3, [sp, #0x14]
	cmp	r3, #0
	bge	.L250be
	ldr	r3, [sp, #0x50]
	add	r3, #2
.L250be:
	asr	r3, #2
	lsl	r3, #2
	mov	r11, r3
	ldr	r3, [sp, #0x30]
	mov	r0, r11
	mov	r10, r3
	cmp	r0, #0
	beq	.L250f0
	ldr	r1, [sp, #0x50]
	sub	r3, r1, r0
	sub	r3, #1
	ldr	r2, [sp, #0x30]
	mov	r10, r3
	cmp	r10, r2
	ble	.L250f8
	mov	r10, r2
	b	.L250f8
.L250e0:
	mov	r0, r10
	lsl	r0, #1
	str	r0, [sp, #0x10]
	b	.L250fe
.L250e8:
	mov	r1, r10
	lsl	r1, #1
	str	r1, [sp, #0x10]
	b	.L250fe
.L250f0:
	mov	r2, r10
	lsl	r2, #1
	str	r2, [sp, #0x10]
	b	.L250fe
.L250f8:
	mov	r3, r10
	lsl	r3, #1
.L250fc:
	str	r3, [sp, #0x10]
.L250fe:
	mov	r0, #1
	bl	WaitFrames
	b	.L24ae2
.L25106:
	mov	r0, #1
	bl	WaitFrames
	mov	r4, #3
	add	r5, sp, #0x60
.L25110:
	ldmia	r5!, {r0}
	str	r4, [sp, #4]
	bl	Func_8003f3c
	ldr	r4, [sp, #4]
	sub	r4, #1
	cmp	r4, #0
	bge	.L25110
	ldr	r1, [sp, #0xc]
	lsr	r0, r1, #16
	bl	Func_8003f3c
	mov	r1, #1
	ldr	r0, [sp, #0x38]
	bl	CloseUIBox
	mov	r1, #1
	ldr	r0, [sp, #0x3c]
	bl	CloseUIBox
	mov	r1, #1
	ldr	r0, [sp, #0x4c]
	bl	CloseUIBox
	mov	r0, #1
	bl	WaitFrames
	mov	r0, r6
	add	sp, #0x174
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8024934

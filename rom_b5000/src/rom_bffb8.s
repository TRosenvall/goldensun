	.include "macros.inc"
	.include "gba.inc"

@ RunEncounterIntro
@ r0.. = parameters. Plays the encounter's opening flourish a frame at a time
@ (Func_30f8), rolling variation with Func_4458 and reserving with Func_3b70.
.thumb_func_start Func_bffb8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	sub	sp, #8
	ldr	r2, =REG_BG0CNT
	mov	r0, #6
	add	r0, sp
	ldrh	r3, [r2]
	mov	r9, r0
	mov	r1, r9
	strh	r3, [r1]
	ldr	r1, .Lc0008	@ 0x40
	orr	r3, r1
	strh	r3, [r2]
	add	r3, sp, #4
	add	r2, #2
	mov	r10, r3
	ldrh	r3, [r2]
	mov	r0, r10
	strh	r3, [r0]
	orr	r3, r1
	strh	r3, [r2]
	mov	r3, #2
	add	r3, sp
	add	r2, #2
	mov	r8, r3
	ldrh	r3, [r2]
	mov	r0, r8
	strh	r3, [r0]
	orr	r3, r1
	strh	r3, [r2]
	add	r2, #2
	ldrh	r3, [r2]
	mov	r7, sp
	strh	r3, [r7]
	orr	r3, r1
	b	.Lc0010

	.align	2, 0
.Lc0008:
	.word	0x40
	.pool

.Lc0010:
	strh	r3, [r2]
	ldr	r3, .Lc004c	@ 0x3eee
	add	r2, #0x42
	strh	r3, [r2]
	mov	r0, #0x10
	bl	Func_3b70
	ldr	r6, =REG_MOSAIC
	mov	r5, #0
.Lc0022:
	bl	Func_4458
	bl	Func_4458
	bl	Func_4458
	bl	Func_4458
	lsl	r3, r5, #8
	orr	r3, r5
	strh	r3, [r6]
	mov	r0, #1
	add	r5, #1
	bl	Func_30f8
	cmp	r5, #0xf
	ble	.Lc0022
	ldr	r3, .Lc0050	@ 1
	mov	r2, #0x80
	b	.Lc0058

	.align	2, 0
.Lc004c:
	.word	0x3eee
.Lc0050:
	.word	1
	.pool

.Lc0058:
	lsl	r2, #19
	strh	r3, [r2]
	mov	r0, #4
	bl	Func_30f8
	mov	r1, r9
	ldrh	r3, [r1]
	ldr	r2, =REG_BG0CNT
	strh	r3, [r2]
	mov	r0, r10
	ldrh	r3, [r0]
	add	r2, #2
	strh	r3, [r2]
	mov	r1, r8
	ldrh	r3, [r1]
	add	r2, #2
	strh	r3, [r2]
	ldrh	r3, [r7]
	add	r2, #2
	mov	r0, #0
	strh	r3, [r2]
	add	sp, #8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_bffb8

@ SetBlendRegisters
@ r0.. = parameters. Writes REG_BLDCNT and REG_BLDALPHA for the battle scene.
.thumb_func_start Func_c0098
	push	{lr}
	ldr	r2, =0x3020100
	ldr	r1, =0x4040404
	mov	r3, #0
.Lc00a0:
	add	r3, #1
	stmia	r0!, {r2}
	add	r2, r1
	cmp	r3, #0x3f
	bls	.Lc00a0
	ldr	r2, =0x3020100
	ldr	r1, =0x4040404
	mov	r3, #0
.Lc00b0:
	add	r3, #1
	stmia	r0!, {r2}
	add	r2, r1
	cmp	r3, #0x37
	bls	.Lc00b0
	mov	r1, #0x88
	mov	r2, #1
	ldr	r3, =Func_8d8
	lsl	r1, #2
	neg	r2, r2
	bl	_call_via_r3
	pop	{r0}
	bx	r0
.func_end Func_c0098

@ SetWindowRegisters
@ r0.. = parameters. Writes the window registers (REG_WIN0H, REG_WINOUT) for
@ the battle scene.
.thumb_func_start Func_c00d8
	push	{r5, r6, lr}
	mov	r1, #0x80
	mov	r2, #1
	ldr	r5, =Func_8d8
	lsl	r1, #1
	neg	r2, r2
	mov	r6, r0
	bl	_call_via_r5
	mov	r3, #0x80
	lsl	r3, #1
	add	r6, r3
	mov	r0, r6
	mov	r1, #0x80
	ldr	r2, =0x3ff03ff
	bl	_call_via_r5
	ldr	r2, =ewram_10200
	ldr	r1, =0x20002
	add	r6, #0x80
	mov	r3, #0
.Lc0102:
	add	r3, #1
	stmia	r6!, {r2}
	add	r2, r1
	cmp	r3, #0xef
	bls	.Lc0102
	mov	r1, #0xa0
	ldr	r3, =Func_8d8
	mov	r0, r6
	lsl	r1, #2
	ldr	r2, =0x3ff03ff
	bl	_call_via_r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c00d8

@ SetBackgroundRegisters
@ r0.. = parameters. Writes REG_BG0CNT / REG_BG1CNT for the battle scene.
.thumb_func_start Func_c0130
	push	{lr}
	ldr	r2, =iwram_1f00
	ldr	r3, [r2]
	ldr	r3, [r3, #8]
	cmp	r3, #2
	bne	.Lc016a
	mov	r3, r2
	sub	r3, #0x88
	ldr	r4, [r3]
	ldr	r3, [r4]
	lsl	r0, r3, #2
	add	r0, r3
	lsl	r0, #6
	add	r0, r4, r0
	ldrh	r3, [r0, #0x20]
	ldr	r1, =REG_BG2CNT
	strh	r3, [r1]
	ldr	r3, =REG_DMA0SAD
	ldr	r2, =0xa2600001
	add	r0, #0x22
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r4
	add	r3, #0x24
	add	r0, #0x10
	add	r1, #0x14
	ldr	r2, =0x84000004
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.Lc016a:
	pop	{r0}
	bx	r0
.func_end Func_c0130

@ GetSceneField
@ r0 = index. Returns one of the scene configuration fields.
.thumb_func_start Func_c0184
	push	{lr}
	ldr	r3, =iwram_1ef8
	ldr	r3, [r3]
	ldr	r3, [r3]
	sub	r0, r3, #1
	cmp	r0, #0x1f
	bhi	.Lc01a4
	ldr	r2, =Lc5a30
	lsr	r0, #2
	lsl	r0, #5
	add	r0, r2
	ldr	r3, =REG_DMA3SAD
	ldr	r1, =0x6005000
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.Lc01a4:
	pop	{r0}
	bx	r0
.func_end Func_c0184

@ ApplySceneTransform
@ r0.. = parameters. Applies the scene transform via Func_c0cec.
.thumb_func_start Func_c01bc
	push	{lr}
	ldr	r3, =iwram_1ef8
	ldr	r0, [r3]
	sub	r3, #0x78
	ldr	r1, [r0]
	ldr	r4, [r3]
	mov	r3, #0x34
	sub	r2, r3, r1
	cmp	r2, #0x20
	ble	.Lc01d2
	mov	r2, #0x20
.Lc01d2:
	cmp	r2, #0
	bge	.Lc01d8
	mov	r2, #0
.Lc01d8:
	ldr	r3, =iwram_1ad0
	strh	r2, [r3, #2]
	cmp	r1, #0x50
	bhi	.Lc01f0
	lsl	r2, r1, #1
	add	r2, r1
	lsl	r3, r2, #4
	sub	r3, r2
	ldr	r2, =0xaf80
	lsl	r3, #3
	add	r3, r2
	strh	r3, [r4, #0x36]
.Lc01f0:
	ldr	r3, [r0]
	add	r2, r3, #1
	str	r2, [r0]
	cmp	r2, #0x50
	bhi	.Lc020a
	mov	r3, #0xb4
	sub	r3, r2
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	Func_c0cec
	b	.Lc0216
.Lc020a:
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0x64
	bl	Func_c0cec
.Lc0216:
	pop	{r0}
	bx	r0
.func_end Func_c01bc

@ ComputeSceneGeometry
@ r0.. = parameters. Derives the scene's geometry; no calls out.
.thumb_func_start Func_c0228
	push	{r5, lr}
	ldr	r3, =iwram_1ef8
	ldr	r3, [r3]
	ldr	r0, [r3]
	cmp	r0, #0x4f
	bhi	.Lc0286
	mov	r3, #7
	ldr	r2, =0xf081
	and	r3, r0
	add	r4, r3, r2
	mov	r3, r0
	cmp	r0, #0
	bge	.Lc0244
	add	r3, r0, #7
.Lc0244:
	asr	r3, #3
	mov	r2, #0xd
	sub	r2, r3
	ldr	r5, =0x6006000
	lsl	r3, r2, #6
	mov	r1, #0
	add	r2, r3, r5
.Lc0252:
	add	r1, #1
	strh	r4, [r2]
	add	r2, #2
	cmp	r1, #0x20
	bne	.Lc0252
	mov	r3, #0x80
	lsl	r3, #4
	orr	r4, r3
	mov	r3, r0
	cmp	r3, #0
	bge	.Lc026a
	add	r3, #7
.Lc026a:
	asr	r3, #3
	mov	r2, r3
	add	r2, #0xd
	cmp	r2, #0x14
	bhi	.Lc0286
	ldr	r0, =0x6006000
	lsl	r3, r2, #6
	mov	r1, #0
	add	r2, r3, r0
.Lc027c:
	add	r1, #1
	strh	r4, [r2]
	add	r2, #2
	cmp	r1, #0x20
	bne	.Lc027c
.Lc0286:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_c0228

@ ResetBg0Scroll
@ Takes no arguments. REG_BG0VOFS = 0. Identical to Func_b5b08 in rom_b5a0c.s --
@ the same two instructions duplicated in two files.
.thumb_func_start Func_c0298
	ldr	r3, =REG_BG0VOFS
	mov	r2, #0
	strh	r2, [r3]
	bx	lr
.func_end Func_c0298

@ RunSceneSetup
@ r0.. = parameters. 485 lines. Builds the battle scene: allocates and frees
@ with Func_2dd8, arms a scanline trigger with Func_307c, registers a task with
@ Func_41d8, reserves OBJ tiles with Func_393c / Func_39fc, and paces with
@ Func_30f8. Traced structurally.
.thumb_func_start Func_c02a4
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x94
	str	r0, [sp]
	ldr	r6, =iwram_1f00
	mov	r5, r1
	mov	r0, #0x2a
	mov	r1, #4
	ldr	r7, [r6]
	bl	Func_48b0
	ldr	r1, =0x15b
	mov	r11, r0
	cmp	r5, r1
	bne	.Lc02ce
	b	.Lc04c8
.Lc02ce:
	ldr	r2, =Lc5b30
	mov	r12, r2
	ldr	r3, =REG_DMA3SAD
	mov	r0, r12
	ldr	r1, =0x6005020
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r12
	add	r0, #0x20
	add	r1, #0x20
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r12
	add	r0, #0x40
	add	r1, #0x20
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r12
	add	r0, #0x60
	add	r1, #0x20
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r12
	add	r0, #0x80
	add	r1, #0x20
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r12
	add	r0, #0xa0
	add	r1, #0x20
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r12
	add	r0, #0xc0
	add	r1, #0x20
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	sub	r3, #0xd4
	mov	r2, #1
	strh	r2, [r3]
	ldr	r3, =0x33333333
	add	r4, sp, #0x90
	mov	r5, #0
	str	r2, [r7, #0xc]
	str	r2, [r7, #8]
	str	r5, [r7, #0x10]
	mov	r0, r4
	str	r3, [r4]
	sub	r1, #0xe0
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r5, [r4]
	mov	r0, r4
	ldr	r1, =0x6005100
	ldr	r2, =0x85000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r2, =REG_BG1CNT
	ldr	r3, .Lc0378	@ 0xc04
	ldr	r1, =REG_BG0CNT
	strh	r3, [r2]
	ldr	r2, .Lc037c
	ldrh	r3, [r1]
	orr	r3, r2
	strh	r3, [r1]
	mov	r3, #2
	str	r3, [r7, #8]
	ldr	r2, =0x6006000
	mov	r6, #0
.Lc036a:
	ldr	r1, =0xf080
	cmp	r6, #0x14
	bls	.Lc0372
	ldr	r1, =0xf088
.Lc0372:
	mov	r3, #0
	b	.Lc03b8

	.align	2, 0
.Lc0378:
	.word	0xc04
.Lc037c:
	.word	2
	.pool

.Lc03b8:
	add	r3, #1
	strh	r1, [r2]
	add	r2, #2
	cmp	r3, #0x1f
	bls	.Lc03b8
	add	r6, #1
	cmp	r6, #0x1f
	bls	.Lc036a
	ldr	r6, =iwram_1ad0
	mov	r3, #0
	mov	r5, #0x20
	mov	r9, r3
	mov	r3, #8
	strh	r5, [r6, #2]
	strh	r5, [r6, #6]
	strh	r3, [r6, #4]
	mov	r0, #1
	bl	Func_30f8
	ldr	r1, .Lc0414	@ 0xf0
	ldr	r3, =REG_WIN0H
	ldr	r2, .Lc0418	@ 0x88
	strh	r1, [r3]
	add	r3, #4
	strh	r2, [r3]
	sub	r3, #2
	strh	r1, [r3]
	add	r3, #4
	strh	r2, [r3]
	ldr	r2, =REG_WININ
	ldr	r3, .Lc041c	@ 0x3537
	strh	r3, [r2]
	ldr	r3, .Lc0420	@ 0x3f21
	add	r2, #2
	mov	r0, #0x80
	strh	r3, [r2]
	lsl	r0, #19
	ldr	r1, =0x7741
	bl	Func_387c
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0xb4
	b	.Lc0434

	.align	2, 0
.Lc0414:
	.word	0xf0
.Lc0418:
	.word	0x88
.Lc041c:
	.word	0x3537
.Lc0420:
	.word	0x3f21
	.pool

.Lc0434:
	bl	Func_c0cec
	ldr	r3, =Func_c01bc
	mov	r2, r11
	mov	r1, r9
	mov	r10, r3
	str	r1, [r2]
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r10
	bl	Func_41d8
	ldr	r1, =Func_c0228
	mov	r8, r1
	mov	r1, #0x90
	lsl	r1, #3
	mov	r0, r8
	bl	Func_41d8
	ldr	r2, =Func_c0298
	mov	r1, #0x20
	mov	r0, #2
	bl	Func_307c
	strh	r5, [r6, #2]
	mov	r0, #1
	bl	Func_30f8
	ldr	r3, =iwram_1e74
	ldr	r3, [r3]
	add	r3, #0x41
	ldrb	r0, [r3]
	ldr	r5, =REG_BG0CNT
	bl	_Func_1ef08
	mov	r0, #0x14
	bl	Func_30f8
	mov	r0, r5
	mov	r1, #2
	bl	Func_39fc
	mov	r1, #0
	mov	r0, r5
	bl	Func_393c
	ldr	r0, [sp]
	bl	Func_b595c
	mov	r0, r10
	bl	Func_4278
	mov	r0, r8
	bl	Func_4278
	mov	r2, r9
	strh	r2, [r6, #2]
	mov	r0, #2
	mov	r1, #0
	mov	r2, #0
	bl	Func_307c
	b	.Lc069c

	.pool_aligned

.Lc04c8:
	mov	r3, r6
	sub	r3, #0x8c
	ldr	r3, [r3]
	mov	r10, r3
	mov	r3, #1
	str	r3, [r7, #0xc]
	mov	r3, #0
	str	r3, [r7, #0x10]
	add	r1, sp, #0x20
	mov	r0, #3
	bl	Func_b6c08
	mov	r6, #0
	mov	r8, r0
	cmp	r0, #0
	beq	.Lc0516
.Lc04e8:
	mov	r5, r6
	add	r5, #0x78
	cmp	r6, #7
	bgt	.Lc04f2
	mov	r5, r6
.Lc04f2:
	mov	r0, r5
	bl	Func_b7dd0
	mov	r7, r0
	mov	r0, r5
	bl	_Func_77394
	mov	r3, #0x94
	lsl	r3, #1
	add	r0, r3
	ldrb	r3, [r0]
	cmp	r3, #0x94
	beq	.Lc0510
	ldr	r3, =0xb333
	str	r3, [r7, #0x18]
.Lc0510:
	add	r6, #1
	cmp	r6, r8
	bne	.Lc04e8
.Lc0516:
	ldr	r1, =ewram_2090
	ldr	r0, =REG_IME
	ldrh	r3, [r0]
	mov	r4, r3
	strh	r0, [r0]
	ldrh	r2, [r1]
	cmp	r2, #0x1f
	bgt	.Lc0544
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r1
	strh	r2, [r1]
	ldr	r2, =0x6041
	add	r3, #4
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #19
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.Lc0544:
	strh	r4, [r0]
	mov	r0, #1
	bl	Func_30f8
	mov	r3, #0xc9
	ldr	r2, =0x21
	lsl	r3, #3
	add	r6, sp, #0x60
	add	r3, r10
	mov	r1, #0
	mov	r8, r1
	strh	r2, [r3]
	mov	r1, r6
	mov	r0, #2
	add	r5, sp, #0x3c
	bl	Func_b6c08
	ldr	r2, .Lc05a4	@ 0xff
	str	r0, [r5, #0x14]
	mov	r10, r2
	lsl	r0, #1
	mov	r3, r10
	add	r0, #0x24
	strh	r3, [r5, r0]
	mov	r1, #0
	mov	r0, r6
	bl	Func_b7b6c
	mov	r0, r5
	bl	_Func_cbc0c
	mov	r3, #0x64
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	Func_c0cec
	mov	r1, r8
	mov	r2, r11
	str	r1, [r2]
	mov	r0, #2
	ldr	r2, =Func_c0298
	mov	r1, #0x20
	bl	Func_307c
	mov	r0, #1
	b	.Lc05c0

	.align	2, 0
.Lc05a4:
	.word	0xff
	.pool

.Lc05c0:
	bl	Func_30f8
	mov	r0, #0x14
	bl	Func_30f8
	ldr	r3, =iwram_1e74
	ldr	r3, [r3]
	ldr	r5, =REG_BG0CNT
	add	r3, #0x41
	ldrb	r0, [r3]
	bl	_Func_1ef08
	mov	r0, r5
	mov	r1, #2
	bl	Func_39fc
	mov	r0, r5
	mov	r1, #0
	bl	Func_393c
	ldr	r2, =REG_BLDCNT
	ldr	r3, .Lc0620	@ 0x3f40
	strh	r3, [r2]
	add	r3, sp, #4
	mov	r8, r3
	mov	r1, r8
	mov	r0, #3
	bl	Func_b6c08
	mov	r7, r0
	lsl	r3, r7, #1
	mov	r2, r10
	mov	r1, r8
	strh	r2, [r1, r3]
	mov	r0, r8
	mov	r1, #0
	bl	Func_b7b6c
	mov	r0, #1
	mov	r1, r8
	bl	Func_b6c08
	mov	r7, r0
	mov	r6, #0
	cmp	r7, #0
	beq	.Lc0640
	mov	r5, r8
	b	.Lc0630

	.align	2, 0
.Lc0620:
	.word	0x3f40
	.pool

.Lc0630:
	ldrh	r0, [r5]
	mov	r1, #1
	add	r6, #1
	add	r5, #2
	bl	Func_c0f98
	cmp	r6, r7
	bne	.Lc0630
.Lc0640:
	ldr	r3, =REG_BLDALPHA
	ldr	r5, .Lc0668	@ 0x1000
	mov	r6, #0
	mov	r10, r3
.Lc0648:
	mov	r3, r6
	orr	r3, r5
	mov	r1, r10
	strh	r3, [r1]
	mov	r0, #1
	add	r6, #1
	bl	Func_30f8
	cmp	r6, #0x10
	bne	.Lc0648
	mov	r6, #0
	cmp	r7, #0
	beq	.Lc0680
	mov	r5, r8
	b	.Lc0670

	.align	2, 0
.Lc0668:
	.word	0x1000
	.pool

.Lc0670:
	ldrh	r0, [r5]
	mov	r1, #0
	add	r6, #1
	add	r5, #2
	bl	Func_c0f98
	cmp	r6, r7
	bne	.Lc0670
.Lc0680:
	ldr	r0, [sp]
	bl	Func_b595c
	ldr	r2, =iwram_1ad0
	mov	r3, #0
	strh	r3, [r2, #2]
	mov	r0, #1
	bl	Func_30f8
	mov	r0, #2
	mov	r1, #0
	mov	r2, #0
	bl	Func_307c
.Lc069c:
	ldr	r6, =REG_BG1CNT
	ldr	r5, .Lc06d8	@ 0x1f83
	mov	r1, #0
	mov	r2, #0
	mov	r0, #2
	bl	Func_307c
	strh	r5, [r6]
	mov	r0, #1
	bl	Func_30f8
	strh	r5, [r6]
	ldr	r1, =REG_BG0CNT
	ldr	r3, =0xfffd
	ldrh	r2, [r1]
	and	r3, r2
	ldr	r2, =iwram_1ad0
	strh	r3, [r1]
	mov	r3, #8
	strh	r3, [r2, #4]
	ldr	r3, .Lc06dc	@ 0x1541
	mov	r2, #0x80
	lsl	r2, #19
	strh	r3, [r2]
	mov	r0, #0x2a
	bl	Func_2dd8
	add	sp, #0x94
	b	.Lc06f0

	.align	2, 0
.Lc06d8:
	.word	0x1f83
.Lc06dc:
	.word	0x1541
	.pool

.Lc06f0:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c02a4

@ BlendSceneLayer
@ r0.. = parameters. Applies Func_c1724's blend over a scene layer. Exported.
.thumb_func_start Func_c0700
	push	{r5, r6, lr}
	ldr	r3, =iwram_1e74
	ldr	r0, [r3]
	ldr	r3, =0x544
	sub	sp, #4
	mov	r4, r1
	add	r5, r0, r3
	mov	r6, sp
	ldr	r3, =REG_IME
	ldrh	r2, [r3]
	str	r2, [r6]
	strh	r3, [r3]
	cmp	r4, #0
	bne	.Lc072a
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	ldr	r1, =0x50000c0
	ldr	r2, =0x80000080
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	b	.Lc074a
.Lc072a:
	ldr	r3, =0x644
	add	r0, r3
	lsl	r3, r4, #4
	add	r3, r4
	lsl	r3, #4
	add	r3, r4
	mov	r2, #0x80
	lsl	r3, #2
	lsl	r2, #9
	sub	r2, r3
	str	r2, [r0]
	ldr	r1, =0x50000c0
	mov	r0, r5
	mov	r3, #0x80
	bl	Func_c1724
.Lc074a:
	ldr	r2, [r6]
	ldr	r3, =REG_IME
	add	sp, #4
	strh	r2, [r3]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c0700

@ LoadSceneGraphics
@ r0.. = parameters. Loads the battle scene's graphics and registers the
@ per-frame task with Func_41d8, configuring the blend and window registers
@ through Func_c0098 and Func_c00d8. Exported; rom_c9000 calls it during
@ animation setup.
.thumb_func_start Func_c0774
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1f00
	ldr	r6, [r3]
	ldr	r3, [r6, #8]
	mov	r7, r0
	mov	r5, r2
	cmp	r3, #0
	bne	.Lc078c
	ldr	r0, =Func_c0130
	ldr	r1, =0x4ff
	bl	Func_41d8
.Lc078c:
	str	r7, [r6, #8]
	cmp	r7, #1
	bne	.Lc07c0
	ldr	r1, =ewram_2090
	ldr	r0, =REG_IME
	ldrh	r3, [r0]
	mov	r4, r3
	strh	r0, [r0]
	ldrh	r2, [r1]
	cmp	r2, #0x1f
	bgt	.Lc07be
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r1
	strh	r2, [r1]
	ldr	r2, =0x1f83
	add	r3, #4
	stmia	r3!, {r2}
	ldr	r2, =REG_BG1CNT
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.Lc07be:
	strh	r4, [r0]
.Lc07c0:
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =0x5000200
	ldr	r1, =0x50000a0
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r2, =0x50001e8
	ldr	r3, =0x50000bc
	ldrh	r2, [r2]
	strh	r2, [r3]
	cmp	r5, #0x80
	bne	.Lc07ec
	ldr	r3, =iwram_1e74
	ldr	r2, =0x544
	ldr	r0, [r3]
	add	r1, #0x20
	add	r0, r2
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x80000080
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	b	.Lc088e
.Lc07ec:
	cmp	r5, #0
	beq	.Lc088e
	ldr	r3, =iwram_1e74
	ldr	r2, =0x544
	ldr	r3, [r3]
	ldr	r7, .Lc081c	@ 0x1f
	add	r2, r3
	ldr	r4, =0x50000c0
	mov	r12, r2
	mov	r6, #0
	mov	r0, #0
.Lc0802:
	mov	r2, r12
	ldrh	r3, [r0, r2]
	mov	r1, #0x1f
	and	r1, r3
	lsl	r3, #16
	lsr	r2, r3, #21
	lsr	r3, #26
	and	r2, r7
	and	r3, r7
	cmp	r1, r5
	ble	.Lc0864
	sub	r1, r5
	b	.Lc0866

	.align	2, 0
.Lc081c:
	.word	0x1f
	.pool

.Lc0864:
	mov	r1, #0
.Lc0866:
	cmp	r2, r5
	ble	.Lc086e
	sub	r2, r5
	b	.Lc0870
.Lc086e:
	mov	r2, #0
.Lc0870:
	cmp	r3, r5
	ble	.Lc0878
	sub	r3, r5
	b	.Lc087a
.Lc0878:
	mov	r3, #0
.Lc087a:
	lsl	r3, #10
	lsl	r2, #5
	orr	r3, r2
	orr	r3, r1
	add	r6, #1
	strh	r3, [r4]
	add	r0, #2
	add	r4, #2
	cmp	r6, #0x80
	bne	.Lc0802
.Lc088e:
	ldr	r0, =0x6003800
	bl	Func_c0098
	ldr	r0, =0x600f800
	bl	Func_c00d8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c0774

@ AllocSceneBuffer
@ r0 = size. Allocates the scene buffer with Func_48f4.
.thumb_func_start Func_c08a8
	push	{r5, lr}
	mov	r1, #0xa8
	lsl	r1, #2
	mov	r0, #0xa
	sub	sp, #4
	bl	Func_48f4
	ldr	r3, =iwram_1f00
	mov	r1, r0
	mov	r4, #0
	mov	r0, sp
	ldr	r5, [r3]
	str	r4, [r0]
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x850000a8
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r4, [r5, #8]
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_c08a8

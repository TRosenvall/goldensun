	.include "macros.inc"
	.include "gba.inc"

@ Playd9ae8Impl
@ r0=action descriptor, r1=variant. The shared implementation behind the
@ 4 thin wrappers in this file, which exist so the animation table can hold
@ one address per variant:
@     0=Func_d9ab8 1=Func_d9ac4 2=Func_d9ad0 3=Func_d9adc
@ Works from the battle state at [iwram_1eec]; the variant selects timing,
@ colours and which arm of the sequence runs. Body characterised
@ structurally -- see the wrappers for the variant numbering.
.thumb_func_start BaseAnim_StatDown  @ 0x080d9ae8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x64
	str	r1, [sp, #0x34]
	ldr	r3, =iwram_3001eec
	ldmia	r3!, {r1}
	str	r1, [sp, #0x30]
	ldr	r2, =0x7828
	ldr	r3, [r3]
	str	r3, [sp, #0x2c]
	add	r3, r1, r2
	str	r0, [r3]
	mov	r0, #0
	bl	AnimStart
	ldr	r3, .Ld9b2c	@ 0x100
	ldr	r2, =REG_BG2PA
	strh	r3, [r2]
	ldr	r3, [sp, #0x34]
	cmp	r3, #0
	bne	.Ld9b40
	ldr	r0, =_FILE_9c
	ldr	r1, [sp, #0x30]
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	b	.Ld9b4c

	.align	2, 0
.Ld9b2c:
	.word	0x100
	.pool

.Ld9b40:
	ldr	r0, =_FILE_9b
	ldr	r1, [sp, #0x30]
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
.Ld9b4c:
	ldr	r5, [sp, #0x34]
	cmp	r5, #0
	beq	.Ld9b5c
	ldr	r0, [sp, #0x34]
	cmp	r0, #1
	bne	.Ld9b5c
	ldr	r0, =_FILE_b7
	b	.Ld9b5e
.Ld9b5c:
	ldr	r0, =_FILE_bb
.Ld9b5e:
	bl	GetFile
	mov	r1, r0
	mov	r0, #0xa0
	ldr	r3, =Func_8001af8
	mov	r2, #0x80
	lsl	r0, #19
	bl	_call_via_r3
	mov	r3, #0x96
	ldr	r2, [sp, #0x30]
	lsl	r3, #6
	add	r1, r2, r3
	ldr	r0, =_FILE_9d
	mov	r2, #0
	mov	r3, #0
	bl	LoadVFXFile
	mov	r0, #0xae
	mov	r5, #1
	lsl	r0, #2
	mov	r9, r5
	mov	r14, r0
	mov	r4, #0x39
.Ld9b8e:
	ldr	r3, [sp, #0x30]
	mov	r2, #0x96
	mov	r1, #0
	lsl	r2, #6
	str	r1, [sp, #0x24]
	add	r1, r3, r2
	add	r3, r0, r3
	mov	r12, r4
	add	r3, r2
.Ld9ba0:
	ldrb	r2, [r1]
	add	r1, #1
	cmp	r2, r12
	ble	.Ld9baa
	mov	r2, r12
.Ld9baa:
	cmp	r2, #0
	bge	.Ld9bb0
	mov	r2, #0
.Ld9bb0:
	strb	r2, [r3]
	ldr	r5, [sp, #0x24]
	add	r5, #1
	add	r3, #1
	str	r5, [sp, #0x24]
	cmp	r5, r14
	bne	.Ld9ba0
	mov	r2, #1
	mov	r1, #0xae
	add	r9, r2
	lsl	r1, #2
	mov	r3, r9
	add	r0, r1
	sub	r4, #7
	cmp	r3, #8
	bne	.Ld9b8e
	ldr	r5, [sp, #0x30]
	ldr	r0, =0x7828
	add	r3, r5, r0
	ldr	r3, [r3]
	ldr	r3, [r3, #4]
	cmp	r3, #1
	bne	.Ld9bec
	ldr	r2, =REG_BG2X
	ldr	r3, =0xffff9000
	mov	r1, #0x70
	neg	r1, r1
	str	r3, [r2]
	str	r1, [sp, #0x20]
	b	.Ld9bf4
.Ld9bec:
	ldr	r2, =REG_BG2X
	mov	r3, #0
	str	r3, [r2]
	str	r3, [sp, #0x20]
.Ld9bf4:
	mov	r2, #0
	ldr	r5, =gBuffer
	mov	r9, r2
	mov	r7, #0xc0
.Ld9bfc:
	bl	Random
	ldr	r3, =0xffff
	mov	r6, r0
	and	r6, r3
	mov	r3, #0
	str	r3, [r5]
	ldr	r3, [sp, #0x34]
	cmp	r3, #0
	bne	.Ld9c42
	mov	r2, #0x1f
	mov	r0, r9
	and	r2, r0
	cmp	r2, #0
	bge	.Ld9c1c
	add	r2, #3
.Ld9c1c:
	asr	r2, #2
	lsl	r3, r2, #1
	add	r3, r2
	ldr	r1, =0xfff60000
	lsl	r3, #17
	add	r3, r1
	str	r3, [r5, #4]
	mov	r3, r9
	cmp	r3, #0
	bge	.Ld9c32
	add	r3, #3
.Ld9c32:
	asr	r3, #2
	lsl	r3, #2
	mov	r2, r9
	sub	r3, r2, r3
	ldr	r0, =0xfffe0000
	lsl	r3, #17
	add	r3, r0
	b	.Ld9c72
.Ld9c42:
	mov	r2, #0x1f
	mov	r1, r9
	and	r2, r1
	cmp	r2, #0
	bge	.Ld9c4e
	add	r2, #3
.Ld9c4e:
	asr	r2, #2
	lsl	r3, r2, #1
	add	r3, r2
	ldr	r2, =0xfff60000
	lsl	r3, #17
	add	r3, r2
	str	r3, [r5, #4]
	mov	r3, r9
	cmp	r3, #0
	bge	.Ld9c64
	add	r3, #3
.Ld9c64:
	asr	r3, #2
	lsl	r3, #2
	mov	r0, r9
	sub	r3, r0, r3
	ldr	r1, =0xfff00000
	lsl	r3, #19
	add	r3, r1
.Ld9c72:
	str	r3, [r5, #8]
	ldr	r2, [sp, #0x30]
	ldr	r0, =0x7828
	add	r3, r2, r0
	ldr	r3, [r3]
	ldr	r3, [r3, #4]
	cmp	r3, #1
	bne	.Ld9c88
	mov	r3, #0x80
	lsl	r3, #10
	b	.Ld9c8a
.Ld9c88:
	ldr	r3, =0xfffe0000
.Ld9c8a:
	str	r3, [r5, #0xc]
	mov	r0, r6
	bl	cos
	mov	r3, r7
	mul	r3, r0
	mov	r1, #0x80
	lsl	r1, #9
	asr	r3, #6
	add	r3, r1
	str	r3, [r5, #0x10]
	mov	r0, r6
	bl	sin
	mov	r3, r7
	mul	r3, r0
	asr	r3, #6
	str	r3, [r5, #0x14]
	bl	Random
	mov	r3, #0xff
	and	r3, r0
	str	r3, [r5, #0x18]
	mov	r2, #1
	mov	r3, #0x80
	add	r9, r2
	lsl	r3, #2
	add	r5, #0x1c
	cmp	r9, r3
	bne	.Ld9bfc
	ldr	r0, [sp, #0x30]
	ldr	r1, =0x7828
	add	r5, r0, r1
	ldr	r3, [r5]
	mov	r2, sp
	add	r2, #0x38
	ldr	r0, [r3, #4]
	mov	r1, r2
	str	r2, [sp, #0x1c]
	bl	BuildDraw2DFuncs
	mov	r0, #0xef
	ldr	r3, [sp, #0x30]
	lsl	r0, #7
	add	r2, r3, r0
	mov	r3, #2
	str	r3, [r2]
	ldr	r1, [sp, #0x30]
	ldr	r3, =0x7784
	add	r2, r1, r3
	mov	r3, #0x32
	mov	r1, #0x90
	str	r3, [r2]
	lsl	r1, #3
	ldr	r0, =Task_BlitAnim
	bl	StartTask
	mov	r0, #0
	str	r0, [sp, #0x28]
	ldr	r3, [r5]
	ldr	r3, [r3, #0x14]
	mov	r1, #0x40
	lsl	r3, #2
	neg	r1, r1
	cmp	r3, r1
	bne	.Ld9d10
	b	.Ld9f82
.Ld9d10:
	ldr	r3, =iwram_3001e80
	ldr	r2, [sp, #0x28]
	ldr	r3, [r3]
	str	r3, [sp, #0x18]
	cmp	r2, #0x48
	bne	.Ld9d22
	mov	r0, #0
	bl	_Func_80bd7dc
.Ld9d22:
	mov	r3, #0
	str	r3, [sp, #0x24]
	ldr	r2, =0x7828
	ldr	r5, [sp, #0x30]
	ldr	r3, [r5, r2]
	ldr	r3, [r3, #0x14]
	cmp	r3, #0
	bne	.Ld9d34
	b	.Ld9f58
.Ld9d34:
	ldr	r0, [sp, #0x28]
	ldr	r3, [sp, #0x18]
	sub	r0, #0x18
	ldr	r1, [sp, #0x28]
	add	r3, #0xc
	mov	r5, #0
	str	r0, [sp, #0xc]
	str	r3, [sp, #0x14]
	str	r5, [sp, #8]
	mov	r8, r1
.Ld9d48:
	ldr	r0, [sp, #0x30]
	ldr	r1, [sp, #0x24]
	ldr	r2, [r0, r2]
	lsl	r3, r1, #1
	add	r3, #0x24
	ldrsh	r0, [r2, r3]
	bl	_GetBattleActor
	ldr	r6, [r0]
	mov	r0, r8
	cmp	r0, #0
	bgt	.Ld9d62
	b	.Ld9f2c
.Ld9d62:
	bl	InitMatrixStack
	ldr	r0, [sp, #0x18]
	ldr	r1, [sp, #0x14]
	bl	MatrixSetLook
	ldr	r3, [r6, #8]
	add	r5, sp, #0x40
	str	r3, [r5]
	mov	r3, #0xa0
	lsl	r3, #13
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #8]
	bl	InitMatrixStack
	ldr	r0, [sp, #0x18]
	ldr	r1, [sp, #0x14]
	bl	MatrixSetLook
	mov	r0, r5
	bl	MatrixTranslatev
	mov	r3, #0
	add	r0, sp, #0x58
	add	r5, sp, #0x4c
	str	r3, [r0]
	str	r3, [r0, #4]
	str	r3, [r0, #8]
	mov	r1, r5
	bl	Func_80e3944
	ldr	r3, [r5]
	ldr	r1, [sp, #0x20]
	ldr	r2, [r5, #4]
	add	r6, r3, r1
	ldr	r3, [sp, #0x34]
	mov	r10, r2
	cmp	r3, #0
	bne	.Ld9e2c
	mov	r0, r8
	cmp	r0, #0x1a
	bgt	.Ld9e68
	cmp	r0, #0
	bge	.Ld9dbe
	add	r0, #3
.Ld9dbe:
	mov	r1, #7
	asr	r0, #2
	bl	__modsi3
	lsl	r1, r0, #4
	sub	r1, r0
	ldr	r2, [sp, #0x30]
	mov	r0, #0x18
	lsl	r1, #6
	add	r1, r2, r1
	mov	r3, r10
	mov	r2, r6
	str	r0, [sp]
	mov	r0, #0x28
	str	r0, [sp, #4]
	sub	r2, #0xc
	sub	r3, #0x14
	ldr	r4, [sp, #0x38]
	ldr	r0, [sp, #0x2c]
	bl	_call_via_r4
	b	.Ld9e68

	.pool_aligned

.Ld9e2c:
	mov	r3, r8
	cmp	r3, #0x17
	bgt	.Ld9e68
	mov	r0, r8
	cmp	r3, #0
	bge	.Ld9e3a
	add	r0, #3
.Ld9e3a:
	mov	r1, #6
	asr	r0, #2
	bl	__modsi3
	lsl	r1, r0, #1
	add	r1, r0
	lsl	r1, #3
	add	r1, r0
	ldr	r0, [sp, #0x30]
	lsl	r1, #6
	add	r1, r0, r1
	mov	r0, #0x28
	str	r0, [sp]
	str	r0, [sp, #4]
	ldr	r0, [sp, #0x1c]
	mov	r2, r6
	mov	r3, r10
	ldr	r4, [r0, #4]
	sub	r2, #0x14
	sub	r3, #0x14
	ldr	r0, [sp, #0x2c]
	bl	_call_via_r4
.Ld9e68:
	mov	r1, r8
	cmp	r1, #0x18
	bne	.Ld9e74
	mov	r0, #0x8f
	bl	_PlaySound
.Ld9e74:
	ldr	r2, [sp, #0xc]
	cmp	r2, #0x24
	bhi	.Ld9f2c
	mov	r3, r8
	mov	r1, #0
	cmp	r3, #0x1c
	ble	.Ld9e94
	mov	r3, r2
	cmp	r2, #0
	bge	.Ld9e8c
	mov	r3, r8
	sub	r3, #0x15
.Ld9e8c:
	asr	r1, r3, #2
	cmp	r1, #7
	ble	.Ld9e94
	mov	r1, #7
.Ld9e94:
	mov	r3, #0xae
	lsl	r3, #2
	mov	r2, r1
	mul	r2, r3
	mov	r11, r5
	ldr	r3, [sp, #8]
	ldr	r5, =gBuffer
	mov	r0, #0
	str	r2, [sp, #0x10]
	mov	r9, r0
	add	r7, r3, r5
.Ld9eaa:
	mov	r3, r9
	cmp	r3, #0
	bge	.Ld9eb2
	add	r3, #3
.Ld9eb2:
	asr	r3, #2
	mov	r0, r9
	lsl	r3, #2
	sub	r3, r0, r3
	lsl	r2, r3, #1
	add	r6, r2, r3
	ldr	r3, [r7, #0x18]
	mov	r1, r8
	add	r0, r3, r1
	cmp	r0, #0
	bge	.Ld9eca
	add	r0, #7
.Ld9eca:
	mov	r1, #3
	asr	r0, #3
	bl	__modsi3
	mov	r1, r11
	mov	r5, r0
	mov	r0, r7
	bl	Func_80e3944
	mov	r2, r11
	ldr	r3, [r2]
	ldr	r0, [sp, #0x20]
	ldr	r1, [r2, #4]
	add	r5, r6, r5
	ldr	r2, =.Leea08
	add	r6, r3, r0
	lsl	r3, r5, #1
	mov	r10, r1
	ldrh	r1, [r2, r3]
	ldr	r2, [sp, #0x10]
	ldr	r3, [sp, #0x30]
	add	r1, r2, r1
	add	r1, r3, r1
	ldr	r3, =.Leea20
	ldrb	r3, [r3, r5]
	str	r3, [sp]
	ldr	r3, =.Leea2c
	mov	r0, #0x96
	ldrb	r3, [r3, r5]
	lsl	r0, #6
	add	r1, r0
	str	r3, [sp, #4]
	ldr	r4, [sp, #0x38]
	ldr	r0, [sp, #0x2c]
	mov	r2, r6
	mov	r3, r10
	bl	_call_via_r4
	mov	r0, r7
	mov	r1, #0x3c
	mov	r2, #0
	bl	Func_80e38b8
	mov	r1, #1
	add	r9, r1
	mov	r2, r9
	add	r7, #0x1c
	cmp	r2, #0x18
	bne	.Ld9eaa
.Ld9f2c:
	ldr	r3, [sp, #0xc]
	ldr	r0, [sp, #8]
	ldr	r2, [sp, #0x24]
	mov	r1, #0xe0
	mov	r5, #4
	lsl	r1, #2
	sub	r3, #4
	neg	r5, r5
	add	r0, r1
	add	r2, #1
	str	r2, [sp, #0x24]
	str	r0, [sp, #8]
	str	r3, [sp, #0xc]
	add	r8, r5
	ldr	r2, =0x7828
	ldr	r5, [sp, #0x30]
	ldr	r3, [r5, r2]
	ldr	r0, [sp, #0x24]
	ldr	r3, [r3, #0x14]
	cmp	r0, r3
	beq	.Ld9f58
	b	.Ld9d48
.Ld9f58:
	ldr	r1, [sp, #0x30]
	ldr	r3, =0x7824
	add	r2, r1, r3
	mov	r3, #1
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	ldr	r5, [sp, #0x28]
	ldr	r0, [sp, #0x30]
	add	r5, #1
	ldr	r1, =0x7828
	str	r5, [sp, #0x28]
	add	r3, r0, r1
	ldr	r3, [r3]
	ldr	r3, [r3, #0x14]
	lsl	r3, #2
	add	r3, #0x40
	cmp	r5, r3
	beq	.Ld9f82
	b	.Ld9d10
.Ld9f82:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x64
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end BaseAnim_StatDown

@ Sub_d9fc8
@ Battle animation routine, 273 instructions.
@ State: iwram_1eec.
@ Calls out to: _Func_bd7dc, _Func_f9080.
@ Touches: REG_BLDCNT.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Flare  @ 0x080d9fc8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r6, =iwram_3001eec
	mov	r3, r6
	ldmia	r3!, {r1}
	ldr	r5, =0x7828
	mov	r8, r1
	ldr	r3, [r3]
	sub	sp, #0x14
	add	r5, r8
	str	r3, [sp, #0x10]
	str	r0, [r5]
	mov	r0, #0
	bl	AnimStart
	ldr	r2, =REG_BLDCNT
	ldr	r3, .Lda028	@ 0x3f46
	strh	r3, [r2]
	ldr	r3, .Lda02c	@ 0x100e
	add	r2, #2
	strh	r3, [r2]
	ldr	r0, =_FILE_b4
	mov	r1, r8
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	mov	r3, #3
	mov	r2, #7
	mov	r0, #0x2e
	mov	r1, #7
	str	r3, [sp]
	bl	BuildDraw2DFuncEx
	ldr	r6, [r6, #0x1c]
	str	r6, [sp, #8]
	ldr	r3, [r5]
	mov	r2, #0x24
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x7f
	ble	.Lda04a
	b	.Lda040

	.align	2, 0
.Lda028:
	.word	0x3f46
.Lda02c:
	.word	0x100e
	.pool

.Lda040:
	mov	r3, #0
	mov	r4, #1
	mov	r11, r3
	mov	r10, r4
	b	.Lda054
.Lda04a:
	mov	r1, #1
	mov	r0, #0x40
	neg	r1, r1
	mov	r11, r0
	mov	r10, r1
.Lda054:
	mov	r5, #0xe1
	mov	r2, #0
	lsl	r5, #7
	ldr	r6, =0xffffc000
	mov	r9, r2
	mov	r7, #0
	add	r5, r8
.Lda062:
	mov	r0, r6
	bl	sin
	lsl	r0, #5
	asr	r0, #16
	mov	r3, r10
	mul	r3, r0
	add	r3, r11
	add	r3, #0x14
	str	r3, [r5]
	mov	r0, r6
	bl	cos
	lsl	r0, #4
	asr	r0, #16
	mov	r4, #1
	add	r0, #0x28
	mov	r3, #0x80
	add	r9, r4
	str	r0, [r5, #4]
	lsl	r3, #5
	mov	r0, r9
	str	r7, [r5, #0x18]
	add	r6, r3
	sub	r7, #4
	add	r5, #0x1c
	cmp	r0, #9
	bne	.Lda062
	mov	r2, #0xef
	lsl	r2, #7
	mov	r3, #2
	add	r2, r8
	str	r3, [r2]
	ldr	r3, =0x7828
	add	r3, r8
	ldr	r3, [r3]
	ldr	r3, [r3, #0x18]
	cmp	r3, #2
	bne	.Lda0b6
	ldr	r2, =0x7784
	mov	r3, #0x4b
	b	.Lda0ba
.Lda0b6:
	ldr	r2, =0x7784
	mov	r3, #0x32
.Lda0ba:
	add	r2, r8
	str	r3, [r2]
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Task_BlitAnim
	bl	StartTask
	mov	r0, #0x88
	bl	_PlaySound
	mov	r1, #0
	str	r1, [sp, #0xc]
.Lda0d2:
	ldr	r2, [sp, #0xc]
	cmp	r2, #0x18
	bne	.Lda0de
	mov	r0, #0x85
	bl	_Func_80bd7dc
.Lda0de:
	mov	r6, #0xe1
	mov	r3, #0
	lsl	r6, #7
	mov	r9, r3
	add	r6, r8
.Lda0e8:
	ldr	r3, [r6, #0x18]
	cmp	r3, #0x17
	bhi	.Lda194
	mov	r2, r3
	cmp	r3, #0
	bge	.Lda0f6
	add	r2, r3, #3
.Lda0f6:
	asr	r5, r2, #2
	ldr	r0, =Data_edeb2
	ldr	r2, =Data_ede9f
	lsl	r4, r5, #1
	ldrh	r1, [r0, r4]
	mov	r10, r4
	ldrb	r4, [r2, r5]
	ldr	r2, [r6]
	lsr	r3, r4, #1
	sub	r2, r3
	ldr	r3, =Data_edeab
	ldrb	r0, [r3, r5]
	mov	r11, r3
	ldr	r3, [r6, #4]
	str	r4, [sp]
	ldr	r4, =Data_edea5
	add	r3, r0
	ldr	r7, =0x7828
	ldrb	r0, [r4, r5]
	add	r1, r8
	str	r0, [sp, #4]
	ldr	r4, [sp, #8]
	ldr	r0, [sp, #0x10]
	add	r7, r8
	bl	_call_via_r4
	ldr	r2, [r7]
	ldr	r3, [r2, #0x18]
	cmp	r3, #0
	beq	.Lda160
	ldr	r3, =Data_ede9f
	ldr	r0, =Data_edeb2
	ldrb	r4, [r3, r5]
	mov	r2, r10
	ldrh	r1, [r0, r2]
	ldr	r2, [r6]
	lsr	r3, r4, #1
	sub	r2, r3
	mov	r3, r11
	ldrb	r0, [r3, r5]
	ldr	r3, [r6, #4]
	str	r4, [sp]
	ldr	r4, =Data_edea5
	add	r3, r0
	ldrb	r0, [r4, r5]
	add	r1, r8
	str	r0, [sp, #4]
	sub	r3, #0x10
	ldr	r0, [sp, #0x10]
	ldr	r4, [sp, #8]
	bl	_call_via_r4
	ldr	r2, [r7]
.Lda160:
	ldr	r3, [r2, #0x18]
	cmp	r3, #2
	bne	.Lda192
	ldr	r3, =Data_ede9f
	ldr	r0, =Data_edeb2
	ldrb	r4, [r3, r5]
	mov	r2, r10
	ldrh	r1, [r0, r2]
	ldr	r2, [r6]
	lsr	r3, r4, #1
	sub	r2, r3
	mov	r3, r11
	ldrb	r0, [r3, r5]
	ldr	r3, [r6, #4]
	str	r4, [sp]
	ldr	r4, =Data_edea5
	add	r3, r0
	ldrb	r0, [r4, r5]
	sub	r3, #0x20
	str	r0, [sp, #4]
	add	r1, r8
	ldr	r0, [sp, #0x10]
	ldr	r4, [sp, #8]
	bl	_call_via_r4
.Lda192:
	ldr	r3, [r6, #0x18]
.Lda194:
	mov	r0, #1
	add	r9, r0
	add	r3, #1
	mov	r1, r9
	str	r3, [r6, #0x18]
	add	r6, #0x1c
	cmp	r1, #9
	bne	.Lda0e8
	mov	r2, #0
	mov	r9, r2
	ldr	r2, =0x7828
	mov	r4, r8
	ldr	r3, [r4, r2]
	ldr	r3, [r3, #0x14]
	cmp	r3, #0
	beq	.Lda1e6
	mov	r6, #0x24
	mov	r5, #0x10
.Lda1b8:
	ldr	r0, [sp, #0xc]
	cmp	r0, r5
	bne	.Lda1d2
	mov	r1, r8
	ldr	r3, [r1, r2]
	ldrsh	r0, [r3, r6]
	mov	r3, #0xc
	str	r3, [sp]
	mov	r1, #0xa
	mov	r2, #5
	mov	r3, r9
	bl	Func_80d6888
.Lda1d2:
	ldr	r2, =0x7828
	mov	r3, #1
	mov	r4, r8
	add	r9, r3
	ldr	r3, [r4, r2]
	ldr	r3, [r3, #0x14]
	add	r6, #2
	add	r5, #8
	cmp	r9, r3
	bne	.Lda1b8
.Lda1e6:
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r8
	mov	r0, #1
	str	r3, [r2]
	bl	WaitFrames
	ldr	r0, [sp, #0xc]
	add	r0, #1
	str	r0, [sp, #0xc]
	cmp	r0, #0x50
	beq	.Lda204
	b	.Lda0d2
.Lda204:
	mov	r0, #0x2e
	bl	gfree
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	bl	AnimEnd
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Flare

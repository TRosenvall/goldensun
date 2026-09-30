	.include "macros.inc"
	.include "gba.inc"

@ Sub_dab74
@ Battle animation routine, 782 instructions.
@ State: iwram_1eec, ewram_10000.
@ Calls out to: _Func_bd7dc, _Func_f9080.
@ Touches: REG_BG2PA, REG_BG2X.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Frost  @ 0x080dab74
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r2, =iwram_3001eec
	mov	r3, r2
	ldmia	r3!, {r1}
	sub	sp, #0x60
	str	r1, [sp, #0x30]
	ldr	r3, [r3]
	str	r3, [sp, #0x2c]
	ldr	r2, [r2, #8]
	str	r2, [sp, #0x24]
	ldr	r2, =0x7828
	add	r3, r1, r2
	str	r0, [r3]
	mov	r0, #1
	bl	AnimStart
	ldr	r2, =REG_BG2PA
	ldr	r3, .Ldabdc	@ 0x100
	strh	r3, [r2]
	ldr	r1, [sp, #0x30]
	ldr	r0, =_FILE_b8
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	ldr	r0, =_FILE_ba
	ldr	r1, [sp, #0x24]
	mov	r2, #0
	mov	r3, #0
	bl	LoadVFXFile
	mov	r3, sp
	add	r3, #0x34
	mov	r0, #0
	mov	r1, r3
	str	r3, [sp, #0x20]
	bl	BuildDraw2DFuncs
	mov	r1, #0xe1
	ldr	r0, [sp, #0x30]
	mov	r6, #0
	lsl	r1, #7
	mov	r10, r6
	add	r5, r0, r1
	b	.Ldabf4

	.align	2, 0
.Ldabdc:
	.word	0x100
	.pool

.Ldabf4:
	bl	Random
	ldr	r3, =0xffff
	and	r3, r0
	str	r3, [r5]
	bl	Random
	mov	r3, #0x3f
	and	r3, r0
	add	r3, #0x38
	str	r3, [r5, #8]
	bl	Random
	mov	r3, #0x1f
	and	r3, r0
	sub	r3, #0x40
	mov	r2, #1
	lsl	r3, #16
	add	r10, r2
	str	r3, [r5, #4]
	mov	r3, r10
	add	r5, #0x1c
	cmp	r3, #0x40
	bne	.Ldabf4
	ldr	r6, [sp, #0x30]
	mov	r0, #0xef
	lsl	r0, #7
	ldr	r1, =0x7784
	add	r2, r6, r0
	mov	r3, #2
	str	r3, [r2]
	add	r2, r6, r1
	mov	r3, #0x32
	mov	r1, #0x90
	str	r3, [r2]
	lsl	r1, #3
	ldr	r0, =Task_BlitAnim
	bl	StartTask
	ldr	r2, =0x7828
	add	r3, r6, r2
	ldr	r1, [r3]
	ldr	r3, [r1, #4]
	cmp	r3, #1
	bne	.Ldac54
	ldr	r2, =REG_BG2X
	ldr	r3, =0xffff9000
	str	r3, [r2]
.Ldac54:
	mov	r3, #0
	str	r3, [sp, #0x28]
	ldr	r3, [r1, #0x18]
	lsl	r2, r3, #1
	add	r2, r3
	ldr	r0, =.Leea88
	add	r2, #2
	mov	r6, #0x4b
	ldrb	r3, [r0, r2]
	neg	r6, r6
	cmp	r3, r6
	bne	.Ldac6e
	b	.Ldb214
.Ldac6e:
	ldr	r3, =0x7828
	ldr	r2, [sp, #0x30]
	mov	r1, sp
	add	r1, #0x3c
	add	r3, r2, r3
	str	r1, [sp, #8]
	str	r3, [sp, #0x10]
.Ldac7c:
	mov	r6, #0xf0
	lsl	r6, #15
	mov	r1, #0
	str	r6, [sp, #0x18]
	str	r1, [sp, #0x14]
	ldr	r2, [sp, #0x10]
	ldr	r3, [r2]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	ldrb	r3, [r0, r3]
	ldr	r6, [sp, #0x28]
	add	r3, #0xb
	cmp	r6, r3
	bne	.Ldaca2
	mov	r0, #0x84
	bl	_Func_80bd7dc
.Ldaca2:
	ldr	r0, [sp, #0x14]
	ldr	r1, [sp, #8]
	mov	r3, #0x80
	lsl	r3, #18
	str	r0, [r1]
	str	r0, [r1, #4]
	str	r3, [r1, #8]
	bl	InitMatrixStack
	ldr	r0, [sp, #8]
	bl	MatrixTranslatev
	ldr	r2, [sp, #0x28]
	sub	r2, #0x24
	str	r2, [sp, #0xc]
	cmp	r2, #0x1b
	bhi	.Ldacd4
	ldr	r6, [sp, #0x28]
	mov	r3, #3
	and	r3, r6
	cmp	r3, #0
	bne	.Ldacd4
	mov	r0, #0x73
	bl	_PlaySound
.Ldacd4:
	ldr	r0, [sp, #0x28]
	cmp	r0, #0x55
	bne	.Ldace0
	mov	r0, #0x88
	bl	_PlaySound
.Ldace0:
	ldr	r2, =0x7828
	ldr	r6, [sp, #0x30]
	ldr	r3, [r6, r2]
	ldr	r3, [r3, #0x14]
	mov	r1, #0
	mov	r10, r1
	cmp	r3, #0
	beq	.Ldad22
	mov	r6, #0x24
	mov	r5, #0x28
.Ldacf4:
	ldr	r0, [sp, #0x28]
	cmp	r0, r5
	bne	.Ldad0e
	ldr	r1, [sp, #0x30]
	ldr	r3, [r1, r2]
	ldrsh	r0, [r3, r6]
	mov	r3, #0
	str	r3, [sp]
	mov	r1, #9
	mov	r2, #5
	sub	r3, #1
	bl	Func_80d6888
.Ldad0e:
	ldr	r2, =0x7828
	ldr	r0, [sp, #0x30]
	mov	r3, #1
	add	r10, r3
	ldr	r3, [r0, r2]
	ldr	r3, [r3, #0x14]
	add	r6, #2
	add	r5, #4
	cmp	r10, r3
	bne	.Ldacf4
.Ldad22:
	ldr	r2, [sp, #0x10]
	ldr	r3, [r2]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	ldr	r1, =.Leea88
	add	r2, r3, r2
	add	r3, r2, #2
	ldrb	r3, [r1, r3]
	ldr	r0, [sp, #0x28]
	mov	r6, #0x10
	mov	r9, r1
	str	r6, [sp, #0x1c]
	cmp	r0, r3
	bge	.Ldad42
	ldrb	r2, [r1, r2]
	str	r2, [sp, #0x1c]
.Ldad42:
	ldr	r2, [sp, #0x10]
	ldr	r3, [r2]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	ldrb	r3, [r1, r3]
	ldr	r6, [sp, #0x28]
	add	r3, #0x23
	cmp	r6, r3
	blt	.Ldad5a
	b	.Ldae90
.Ldad5a:
	ldr	r1, [sp, #0x1c]
	mov	r0, #0
	mov	r10, r0
	cmp	r1, #0
	bne	.Ldad66
	b	.Ldae90
.Ldad66:
	ldr	r6, [sp, #0x30]
	mov	r0, #0xe1
	mov	r2, #0x54
	mov	r3, #0x48
	lsl	r0, #7
	add	r2, sp
	add	r3, sp
	add	r6, r0
	mov	r11, r2
	mov	r9, r3
	mov	r8, r6
.Ldad7c:
	ldr	r1, [sp, #0x28]
	cmp	r1, r10
	ble	.Ldae7c
	mov	r1, r10
	cmp	r1, #0
	bge	.Ldad8a
	add	r1, #7
.Ldad8a:
	asr	r7, r1, #3
	mov	r2, r10
	lsl	r3, r7, #3
	sub	r7, r2, r3
	lsr	r3, r2, #31
	add	r3, r10
	asr	r3, #1
	mov	r6, r8
	mov	r2, #0x30
	sub	r2, r3
	ldr	r3, [r6, #4]
	lsl	r2, #16
	cmp	r3, r2
	bge	.Ldae10
	ldr	r0, =0xffd00000
	cmp	r3, r0
	ble	.Ldae10
	ldr	r0, [r6]
	bl	sin
	ldr	r3, [r6, #8]
	mul	r3, r0
	mov	r1, r11
	str	r3, [r1]
	ldr	r3, [r6, #4]
	str	r3, [r1, #4]
	ldr	r0, [r6]
	bl	cos
	ldr	r3, [r6, #8]
	mul	r3, r0
	mov	r2, r11
	str	r3, [r2, #8]
	mov	r1, r9
	mov	r0, r11
	bl	Func_80e3944
	mov	r3, r9
	ldr	r2, [r3]
	asr	r2, #17
	add	r2, #0x40
	str	r2, [r3]
	mov	r0, #6
	ldrsh	r3, [r3, r0]
	mov	r1, r9
	add	r3, #0x3c
	ldr	r0, =.Leeaa2
	str	r3, [r1, #4]
	lsl	r1, r7, #1
	ldrh	r1, [r0, r1]
	ldr	r0, [sp, #0x30]
	add	r1, r0, r1
	ldr	r0, =.Leea91
	ldrb	r5, [r0, r7]
	lsr	r0, r5, #1
	sub	r2, r0
	ldr	r0, =.Leea99
	ldrb	r4, [r0, r7]
	lsr	r0, r4, #1
	sub	r3, r0
	str	r5, [sp]
	ldr	r0, [sp, #0x20]
	str	r4, [sp, #4]
	ldr	r4, [r0, #4]
	ldr	r0, [sp, #0x2c]
	bl	_call_via_r4
.Ldae10:
	ldr	r1, [sp, #0x10]
	ldr	r3, [r1]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	ldr	r2, =.Leea88
	add	r3, #2
	ldrb	r3, [r2, r3]
	ldr	r0, [sp, #0x28]
	cmp	r0, r3
	bge	.Ldae54
	mov	r3, r10
	add	r3, #0x10
	cmp	r0, r3
	ble	.Ldae7c
	ldr	r3, [r6, #8]
	cmp	r3, #4
	ble	.Ldae38
	sub	r3, #2
	str	r3, [r6, #8]
.Ldae38:
	ldr	r3, [r6, #4]
	ldr	r1, =0x2fffff
	cmp	r3, r1
	bgt	.Ldae48
	mov	r2, #0xa0
	lsl	r2, #11
	add	r3, r2
	str	r3, [r6, #4]
.Ldae48:
	ldr	r3, [r6]
	mov	r0, #0x80
	lsl	r0, #2
	add	r3, r0
	str	r3, [r6]
	b	.Ldae7c
.Ldae54:
	ldr	r3, [r6, #8]
	add	r3, #8
	str	r3, [r6, #8]
	mov	r1, #5
	mov	r0, r10
	bl	__modsi3
	ldr	r3, [r6, #4]
	add	r0, #2
	lsl	r0, #16
	sub	r3, r0
	str	r3, [r6, #4]
	ldr	r1, [sp, #0x18]
	cmp	r1, r3
	ble	.Ldae74
	str	r3, [sp, #0x18]
.Ldae74:
	ldr	r2, [sp, #0x14]
	cmp	r2, r3
	bge	.Ldae7c
	str	r3, [sp, #0x14]
.Ldae7c:
	mov	r6, #1
	ldr	r0, [sp, #0x1c]
	mov	r3, #0x1c
	add	r10, r6
	add	r8, r3
	cmp	r10, r0
	beq	.Ldae8c
	b	.Ldad7c
.Ldae8c:
	ldr	r1, =.Leea88
	mov	r9, r1
.Ldae90:
	ldr	r2, [sp, #0x18]
	ldr	r6, [sp, #0x14]
	mov	r3, #0x80
	lsl	r3, #15
	add	r2, r3
	add	r6, r3
	str	r6, [sp, #0x14]
	str	r2, [sp, #0x18]
	ldr	r0, [sp, #0x10]
	ldr	r3, [r0]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r2, r3, r2
	add	r3, r2, #2
	mov	r1, r9
	ldrb	r3, [r1, r3]
	ldr	r6, [sp, #0x28]
	cmp	r6, r3
	bge	.Ldafa0
	add	r3, r2, #1
	ldrb	r3, [r1, r3]
	mov	r0, #0
	mov	r10, r0
	cmp	r3, #0
	beq	.Ldafa0
	ldr	r5, =.Leea62
	ldr	r7, =.Leeab2
	mov	r8, r5
	mov	r6, #0
.Ldaeca:
	ldr	r0, [sp, #0xc]
	mov	r1, #3
	bl	__divsi3
	cmp	r10, r0
	bge	.Ldaf84
	mov	r1, #3
	mov	r0, r10
	bl	__modsi3
	ldr	r1, [sp, #0x10]
	ldr	r3, [r1]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	mov	r2, r9
	ldrb	r3, [r2, r3]
	mov	r4, r0
	ldr	r0, [sp, #0x28]
	sub	r3, #7
	cmp	r0, r3
	blt	.Ldaf60
	ldr	r0, =.Leeab8
	lsl	r3, r4, #1
	ldrh	r1, [r7, r3]
	ldrb	r4, [r0, r4]
	ldr	r2, [sp, #0x30]
	mov	r3, r8
	mov	r0, #0x20
	add	r1, r2, r1
	ldrb	r2, [r6, r3]
	ldrb	r3, [r5, #1]
	str	r0, [sp]
	str	r4, [sp, #4]
	ldr	r0, [sp, #0x20]
	sub	r3, r4
	ldr	r4, [r0, #4]
	ldr	r0, [sp, #0x2c]
	bl	_call_via_r4
	ldr	r1, =.Leea88
	mov	r9, r1
	b	.Ldaf84

	.pool_aligned

.Ldaf60:
	ldr	r0, =.Leeab8
	lsl	r3, r4, #1
	ldrh	r1, [r7, r3]
	ldrb	r4, [r0, r4]
	ldrb	r3, [r5, #1]
	ldr	r2, [sp, #0x30]
	mov	r0, #0x20
	sub	r3, r4
	add	r1, r2, r1
	ldrb	r2, [r5]
	str	r0, [sp]
	str	r4, [sp, #4]
	ldr	r0, [sp, #0x2c]
	ldr	r4, [sp, #0x34]
	bl	_call_via_r4
	ldr	r3, =.Leea88
	mov	r9, r3
.Ldaf84:
	ldr	r1, [sp, #0x10]
	ldr	r3, [r1]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #1
	mov	r2, r9
	mov	r0, #1
	ldrb	r3, [r2, r3]
	add	r10, r0
	add	r5, #2
	add	r6, #2
	cmp	r10, r3
	bne	.Ldaeca
.Ldafa0:
	ldr	r6, [sp, #0x10]
	ldr	r3, [r6]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	mov	r0, r9
	ldrb	r3, [r0, r3]
	ldr	r1, [sp, #0x28]
	cmp	r1, r3
	bne	.Ldb00c
	mov	r2, #0
	ldr	r5, =gBuffer
	mov	r10, r2
	mov	r6, #0xf
.Ldafbe:
	bl	Random
	mov	r3, #0x7f
	and	r3, r0
	lsl	r3, #16
	str	r3, [r5]
	bl	Random
	and	r0, r6
	add	r0, #0x50
	lsl	r0, #16
	str	r0, [r5, #4]
	bl	Random
	mov	r3, #0x3f
	and	r3, r0
	sub	r3, #0x20
	lsl	r3, #12
	str	r3, [r5, #8]
	bl	Random
	neg	r0, r0
	and	r0, r6
	sub	r0, #0x10
	lsl	r0, #13
	str	r0, [r5, #0x10]
	bl	Random
	mov	r3, #1
	and	r0, r6
	add	r0, #0x10
	add	r10, r3
	str	r0, [r5, #0x18]
	mov	r0, r10
	add	r5, #0x1c
	cmp	r0, #0x20
	bne	.Ldafbe
	ldr	r1, =.Leea88
	mov	r9, r1
.Ldb00c:
	ldr	r2, [sp, #0x10]
	ldr	r3, [r2]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	mov	r6, r9
	ldrb	r3, [r6, r3]
	ldr	r0, [sp, #0x28]
	cmp	r0, r3
	blt	.Ldb09e
	mov	r1, #0
	ldr	r5, =gBuffer
	mov	r10, r1
.Ldb028:
	ldr	r3, [r5, #0x18]
	cmp	r3, #0
	blt	.Ldb07c
	mov	r3, r10
	cmp	r3, #0
	bge	.Ldb036
	add	r3, #7
.Ldb036:
	asr	r4, r3, #3
	lsl	r3, r4, #3
	mov	r2, r10
	sub	r4, r2, r3
	ldr	r2, =.Leeacc
	lsl	r3, r4, #1
	ldrh	r1, [r2, r3]
	ldr	r3, [sp, #0x30]
	add	r1, r3, r1
	mov	r0, #6
	ldrsh	r3, [r5, r0]
	ldr	r0, =.Leeabb
	ldrb	r0, [r0, r4]
	mov	r6, #2
	ldrsh	r2, [r5, r6]
	str	r0, [sp]
	ldr	r0, =.Leeac3
	ldrb	r0, [r0, r4]
	ldr	r6, [sp, #0x20]
	str	r0, [sp, #4]
	ldr	r0, [sp, #0x2c]
	ldr	r4, [r6, #4]
	bl	_call_via_r4
	ldr	r3, [r5]
	ldr	r2, [r5, #0xc]
	add	r3, r2
	str	r3, [r5]
	ldr	r2, [r5, #0x10]
	ldr	r3, [r5, #4]
	add	r3, r2
	str	r3, [r5, #4]
	ldr	r3, [r5, #0x18]
	sub	r3, #1
	str	r3, [r5, #0x18]
.Ldb07c:
	ldr	r3, [r5, #4]
	ldr	r0, [sp, #0x18]
	cmp	r0, r3
	ble	.Ldb086
	str	r3, [sp, #0x18]
.Ldb086:
	ldr	r1, [sp, #0x14]
	cmp	r1, r3
	bge	.Ldb08e
	str	r3, [sp, #0x14]
.Ldb08e:
	mov	r2, #1
	add	r10, r2
	mov	r3, r10
	add	r5, #0x1c
	cmp	r3, #0x18
	bne	.Ldb028
	ldr	r6, =.Leea88
	mov	r9, r6
.Ldb09e:
	ldr	r0, [sp, #0x18]
	ldr	r1, [sp, #0x14]
	asr	r0, #16
	asr	r1, #16
	str	r0, [sp, #0x18]
	str	r1, [sp, #0x14]
	cmp	r1, r0
	bgt	.Ldb0b2
	add	r0, #1
	str	r0, [sp, #0x14]
.Ldb0b2:
	ldr	r2, [sp, #0x10]
	ldr	r3, [r2]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	mov	r6, r9
	ldrb	r3, [r6, r3]
	ldr	r0, [sp, #0x28]
	cmp	r0, r3
	bne	.Ldb120
	ldr	r2, [sp, #0x30]
	mov	r3, #0xe1
	mov	r1, #0
	lsl	r3, #7
	mov	r10, r1
	add	r5, r2, r3
.Ldb0d4:
	bl	Random
	mov	r3, #0x7f
	and	r3, r0
	lsl	r3, #16
	str	r3, [r5, #0xc]
	ldr	r6, [sp, #0x14]
	ldr	r0, [sp, #0x18]
	cmp	r6, r0
	bne	.Ldb0ee
	lsl	r3, r0, #16
	str	r3, [r5, #0x10]
	b	.Ldb104
.Ldb0ee:
	bl	Random
	ldr	r2, [sp, #0x14]
	ldr	r3, [sp, #0x18]
	sub	r1, r2, r3
	bl	__umodsi3
	ldr	r6, [sp, #0x18]
	add	r0, r6
	lsl	r0, #16
	str	r0, [r5, #0x10]
.Ldb104:
	bl	Random
	mov	r3, #0xf
	and	r3, r0
	mov	r0, #1
	add	r10, r0
	add	r3, #0x14
	mov	r1, r10
	str	r3, [r5, #0x18]
	add	r5, #0x1c
	cmp	r1, #0x20
	bne	.Ldb0d4
	ldr	r2, =.Leea88
	mov	r9, r2
.Ldb120:
	ldr	r6, [sp, #0x10]
	ldr	r3, [r6]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	mov	r0, r9
	ldrb	r3, [r0, r3]
	ldr	r1, [sp, #0x28]
	cmp	r1, r3
	blt	.Ldb1e4
	sub	r3, r1, r3
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	mov	r8, r3
	mov	r6, #0xe1
	ldr	r3, [sp, #0x30]
	mov	r2, #0
	lsl	r6, #7
	ldr	r7, =.Leea88
	mov	r10, r2
	add	r5, r3, r6
.Ldb14e:
	ldr	r3, [r5, #0x18]
	cmp	r3, #0x11
	bhi	.Ldb18e
	mov	r0, #0x11
	sub	r0, r3
	lsr	r3, r0, #31
	add	r0, r3
	asr	r0, #1
	ldr	r2, =Data_ede84
	lsl	r3, r0, #1
	ldrh	r1, [r2, r3]
	ldr	r2, [sp, #0x24]
	add	r1, r2, r1
	mov	r3, #0xe
	ldrsh	r2, [r5, r3]
	ldr	r3, =Data_ede96
	ldrb	r4, [r3, r0]
	mov	r6, #0x12
	ldrsh	r3, [r5, r6]
	lsr	r0, r4, #1
	str	r4, [sp]
	str	r4, [sp, #4]
	ldr	r6, [sp, #0x20]
	sub	r2, r0
	sub	r3, r0
	mov	r0, r8
	sub	r3, r0
	ldr	r4, [r6, #4]
	ldr	r0, [sp, #0x2c]
	bl	_call_via_r4
	ldr	r3, [r5, #0x18]
.Ldb18e:
	mov	r0, #1
	sub	r3, #1
	neg	r0, r0
	str	r3, [r5, #0x18]
	cmp	r3, r0
	beq	.Ldb19e
	cmp	r3, #0x11
	bne	.Ldb1d8
.Ldb19e:
	ldr	r1, [sp, #0x10]
	ldr	r3, [r1]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	add	r3, #2
	ldrb	r3, [r7, r3]
	ldr	r2, [sp, #0x28]
	add	r3, #0x23
	cmp	r2, r3
	bge	.Ldb1d8
	mov	r3, #0x11
	str	r3, [r5, #0x18]
	bl	Random
	mov	r3, #0x7f
	and	r3, r0
	lsl	r3, #16
	str	r3, [r5, #0xc]
	bl	Random
	ldr	r6, [sp, #0x18]
	ldr	r3, [sp, #0x14]
	sub	r1, r3, r6
	bl	__umodsi3
	add	r0, r6
	lsl	r0, #16
	str	r0, [r5, #0x10]
.Ldb1d8:
	mov	r0, #1
	add	r10, r0
	mov	r1, r10
	add	r5, #0x1c
	cmp	r1, #0x20
	bne	.Ldb14e
.Ldb1e4:
	ldr	r3, [sp, #0x30]
	ldr	r6, =0x7824
	add	r2, r3, r6
	mov	r3, #1
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	ldr	r0, [sp, #0x28]
	add	r0, #1
	str	r0, [sp, #0x28]
	ldr	r1, [sp, #0x10]
	ldr	r3, [r1]
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r2
	ldr	r0, =.Leea88
	add	r3, #2
	ldrb	r3, [r0, r3]
	ldr	r2, [sp, #0x28]
	add	r3, #0x4b
	cmp	r2, r3
	beq	.Ldb214
	b	.Ldac7c
.Ldb214:
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	bl	AnimEnd
	add	sp, #0x60
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Frost

@ Sub_db264
@ Battle animation routine, 464 instructions.
@ State: iwram_1eec, iwram_1e50, ewram_10000.
@ Calls out to: _Func_bd7dc, _Func_f9080.
@ Touches: REG_BG2PA, REG_BG2X.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Ray  @ 0x080db264
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r2, =iwram_3001eec
	mov	r3, r2
	ldmia	r3!, {r1}
	ldr	r3, [r3]
	sub	sp, #0x2c
	str	r3, [sp, #0x20]
	ldr	r5, =0x7828
	mov	r10, r1
	ldr	r2, [r2, #8]
	add	r5, r10
	str	r2, [sp, #0x18]
	str	r0, [r5]
	mov	r0, #1
	bl	Func_80cdb24
	ldr	r3, [r5]
	ldr	r3, [r3, #0x18]
	cmp	r3, #2
	bne	.Ldb2b0
	ldr	r2, =REG_BG2PA
	ldr	r3, .Ldb2a0	@ 0x80
	b	.Ldb2b4

	.align	2, 0
.Ldb2a0:
	.word	0x80
	.pool

.Ldb2b0:
	ldr	r2, =REG_BG2PA
	ldr	r3, .Ldb2ec	@ 0x100
.Ldb2b4:
	strh	r3, [r2]
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #7
	mov	r3, #3
	mov	r0, #0x2e
	bl	BuildDraw2DFuncEx
	ldr	r5, =gPtrs
	mov	r3, r5
	add	r3, #0xb8
	ldr	r3, [r3]
	str	r3, [sp, #0x24]
	mov	r3, #3
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #7
	mov	r3, #7
	mov	r0, #0x2f
	bl	BuildDraw2DFuncEx
	add	r5, #0xbc
	ldr	r3, [r5]
	mov	r2, sp
	ldr	r1, =0x60e
	add	r2, #0x24
	b	.Ldb2fc

	.align	2, 0
.Ldb2ec:
	.word	0x100
	.pool

.Ldb2fc:
	str	r2, [sp, #0xc]
	ldr	r0, =_FILE_c4
	str	r3, [r2, #4]
	add	r1, r10
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	mov	r2, #0
	mov	r3, #0
	ldr	r0, =_FILE_73
	ldr	r1, [sp, #0x18]
	bl	LoadVFXFile
	ldr	r3, =0x7828
	add	r3, r10
	ldr	r2, [r3]
	ldr	r3, [r2, #0x18]
	cmp	r3, #2
	bne	.Ldb338
	ldr	r3, [r2, #4]
	cmp	r3, #1
	bne	.Ldb330
	ldr	r2, =REG_BG2X
	ldr	r3, =0xfffff000
	b	.Ldb342
.Ldb330:
	ldr	r2, =REG_BG2X
	mov	r3, #0x80
	lsl	r3, #5
	b	.Ldb342
.Ldb338:
	ldr	r3, [r2, #4]
	cmp	r3, #1
	bne	.Ldb344
	ldr	r2, =REG_BG2X
	ldr	r3, =0xffff8000
.Ldb342:
	str	r3, [r2]
.Ldb344:
	mov	r3, #0
	ldr	r7, =gBuffer
	mov	r8, r3
.Ldb34a:
	bl	Random
	ldr	r6, =0x3ff
	mov	r1, #0x80
	lsl	r1, #1
	and	r6, r0
	add	r6, r1
	bl	Random
	ldr	r5, =0x7fff
	mov	r3, #0x80
	ldr	r2, =0xffffc000
	lsl	r3, #7
	str	r3, [r7]
	and	r5, r0
	mov	r3, #0xe0
	add	r5, r2
	lsl	r3, #7
	str	r3, [r7, #4]
	mov	r0, r5
	bl	sin
	mov	r3, r6
	mul	r3, r0
	asr	r3, #16
	str	r3, [r7, #8]
	mov	r0, r5
	bl	cos
	mov	r3, r6
	mul	r3, r0
	lsl	r3, #1
	neg	r3, r3
	asr	r3, #16
	str	r3, [r7, #0x14]
	mov	r3, #0
	str	r3, [r7, #0x18]
	mov	r1, #0x80
	mov	r3, #1
	add	r8, r3
	lsl	r1, #3
	add	r7, #0x1c
	cmp	r8, r1
	bne	.Ldb34a
	mov	r2, #0xef
	lsl	r2, #7
	add	r2, r10
	mov	r3, #2
	str	r3, [r2]
	ldr	r2, =0x7784
	mov	r3, #0x4b
	add	r2, r10
	str	r3, [r2]
	add	r1, #0x80
	ldr	r0, =Task_BlitAnim
	bl	StartTask
	mov	r0, #0x8a
	bl	_PlaySound
	mov	r2, #0
	str	r2, [sp, #0x1c]
.Ldb3c6:
	ldr	r3, [sp, #0x1c]
	cmp	r3, #0x14
	bne	.Ldb3d2
	mov	r0, #0x85
	bl	_Func_80bd7dc
.Ldb3d2:
	ldr	r1, [sp, #0x1c]
	cmp	r1, #0xf
	ble	.Ldb3da
	b	.Ldb55a
.Ldb3da:
	mov	r0, r1
	mov	r1, #5
	bl	__modsi3
	cmp	r0, #2
	bne	.Ldb3f4
	mov	r1, #0x80
	ldr	r3, =Func_80008d8
	ldr	r0, [sp, #0x20]
	lsl	r1, #7
	ldr	r2, =0x10101010
	bl	_call_via_r3
.Ldb3f4:
	ldr	r3, [sp, #0x1c]
	mov	r2, #0
	lsl	r3, #11
	str	r2, [sp, #0x14]
	str	r3, [sp, #8]
.Ldb3fe:
	mov	r1, #0
	ldr	r2, [sp, #8]
	str	r1, [sp, #0x10]
	mov	r1, #0x80
	lsl	r1, #7
	add	r3, r2, r1
	ldr	r2, [sp, #0x14]
	mov	r5, r2
	mul	r5, r3
	mov	r0, r5
	bl	sin
	ldr	r1, [sp, #0x1c]
	mov	r3, #0x20
	sub	r3, r1
	mul	r3, r0
	asr	r3, #16
	add	r3, #0x40
	mov	r0, r5
	mov	r9, r3
	bl	cos
	ldr	r3, =0x7828
	add	r3, r10
	lsl	r0, #3
	ldr	r3, [r3]
	asr	r0, #16
	neg	r0, r0
	ldr	r3, [r3, #0x18]
	mov	r6, r0
	sub	r6, #8
	cmp	r3, #0
	bne	.Ldb47a
	bl	Random
	mov	r3, #3
	and	r0, r3
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r5, r3, #4
	sub	r5, r3
	ldr	r2, =0x60e
	lsl	r5, #6
	add	r5, r10
	add	r5, r2
	bl	Random
	mov	r2, #7
	and	r2, r0
	mov	r3, #0x18
	add	r2, r9
	str	r3, [sp]
	mov	r3, #0x78
	str	r3, [sp, #4]
	sub	r2, #0x10
	ldr	r4, [sp, #0x24]
	ldr	r0, [sp, #0x20]
	mov	r1, r5
	mov	r3, r6
	bl	_call_via_r4
	b	.Ldb4bc
.Ldb47a:
	bl	Random
	mov	r3, #3
	and	r0, r3
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r5, r3, #4
	sub	r5, r3
	lsl	r5, #6
	ldr	r3, =0x60e
	add	r5, r10
	add	r5, r3
	bl	Random
	ldr	r3, [sp, #0x14]
	mov	r1, #1
	and	r1, r3
	mov	r3, #0x18
	str	r3, [sp]
	mov	r2, #7
	mov	r3, #0x78
	and	r2, r0
	str	r3, [sp, #4]
	ldr	r3, [sp, #0xc]
	lsl	r1, #2
	add	r2, r9
	ldr	r4, [r1, r3]
	sub	r2, #0x10
	ldr	r0, [sp, #0x20]
	mov	r1, r5
	mov	r3, r6
	bl	_call_via_r4
.Ldb4bc:
	mov	r3, r6
	add	r3, #0x70
	mov	r1, #0
	lsl	r3, #16
	ldr	r7, =gBuffer
	mov	r8, r1
	mov	r11, r3
.Ldb4ca:
	ldr	r3, [r7, #0x18]
	cmp	r3, #0
	bne	.Ldb538
	bl	Random
	ldr	r6, =0x1ff
	and	r6, r0
	bl	Random
	ldr	r5, =0x7fff
	ldr	r2, =0xffffc000
	mov	r1, r9
	and	r5, r0
	lsl	r3, r1, #16
	add	r5, r2
	mov	r2, r11
	str	r2, [r7, #4]
	str	r3, [r7]
	mov	r0, r5
	bl	sin
	add	r6, #0x80
	mov	r3, r6
	mul	r3, r0
	asr	r3, #9
	str	r3, [r7, #0xc]
	mov	r0, r5
	bl	cos
	mov	r3, r6
	mul	r3, r0
	lsl	r3, #1
	neg	r3, r3
	asr	r3, #7
	str	r3, [r7, #0x10]
	bl	Random
	mov	r3, #7
	and	r3, r0
	add	r3, #0x20
	str	r3, [r7, #0x18]
	ldr	r3, [sp, #0x10]
	add	r3, #1
	str	r3, [sp, #0x10]
	ldr	r3, =0x7828
	add	r3, r10
	ldr	r3, [r3]
	ldr	r3, [r3, #0x18]
	ldr	r1, =.Leeadc
	lsl	r3, #1
	add	r3, #1
	ldrb	r3, [r1, r3]
	ldr	r2, [sp, #0x10]
	cmp	r2, r3
	beq	.Ldb546
.Ldb538:
	mov	r3, #1
	mov	r1, #0x80
	add	r8, r3
	lsl	r1, #3
	add	r7, #0x1c
	cmp	r8, r1
	bne	.Ldb4ca
.Ldb546:
	ldr	r2, [sp, #0x14]
	add	r2, #1
	str	r2, [sp, #0x14]
	cmp	r2, #4
	beq	.Ldb552
	b	.Ldb3fe
.Ldb552:
	ldr	r2, =0x77a8
	mov	r3, #1
	add	r2, r10
	str	r3, [r2]
.Ldb55a:
	mov	r3, #0
	ldr	r5, =gBuffer
	mov	r8, r3
.Ldb560:
	ldr	r3, [r5, #0x18]
	cmp	r3, #0
	ble	.Ldb5ca
	sub	r3, #1
	str	r3, [r5, #0x18]
	mov	r1, #0x3c
	mov	r0, r5
	ldr	r2, =0xfffff800
	bl	Func_80e3908
	mov	r1, #0xf0
	ldr	r3, [r5, #4]
	lsl	r1, #15
	cmp	r3, r1
	ble	.Ldb58c
	ldr	r3, [r5, #0x10]
	neg	r3, r3
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	str	r3, [r5, #0x10]
	b	.Ldb5ca
.Ldb58c:
	ldr	r2, [r5]
	ldr	r1, =0x7effff
	cmp	r2, r1
	bhi	.Ldb5ca
	cmp	r3, #0
	blt	.Ldb5ca
	ldr	r0, [r5, #0x18]
	asr	r6, r2, #16
	asr	r7, r3, #16
	cmp	r0, #0
	bge	.Ldb5a4
	add	r0, #7
.Ldb5a4:
	asr	r0, #3
	add	r0, #1
	lsl	r4, r0, #1
	ldr	r2, =Data_ede48
	sub	r3, r4, #2
	ldrh	r1, [r2, r3]
	ldr	r2, [sp, #0x18]
	add	r1, r2, r1
	lsr	r2, r0, #31
	add	r2, r0, r2
	asr	r2, #1
	sub	r3, r7, r0
	str	r0, [sp]
	str	r4, [sp, #4]
	sub	r2, r6, r2
	ldr	r4, [sp, #0x24]
	ldr	r0, [sp, #0x20]
	bl	_call_via_r4
.Ldb5ca:
	mov	r3, #1
	mov	r1, #0x80
	add	r8, r3
	lsl	r1, #3
	add	r5, #0x1c
	cmp	r8, r1
	bne	.Ldb560
	ldr	r3, [sp, #0x1c]
	sub	r3, #4
	cmp	r3, #0x5b
	bhi	.Ldb622
	mov	r2, #0
	mov	r8, r2
	ldr	r2, =0x7828
	mov	r1, r10
	ldr	r3, [r1, r2]
	ldr	r3, [r3, #0x14]
	cmp	r3, #0
	beq	.Ldb622
	mov	r6, #0x24
	mov	r5, #4
.Ldb5f4:
	ldr	r3, [sp, #0x1c]
	cmp	r3, r5
	bne	.Ldb60e
	mov	r1, r10
	ldr	r3, [r1, r2]
	ldrsh	r0, [r3, r6]
	mov	r3, #0xa
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #5
	mov	r3, r8
	bl	Func_80d6888
.Ldb60e:
	ldr	r2, =0x7828
	mov	r3, #1
	mov	r1, r10
	add	r8, r3
	ldr	r3, [r1, r2]
	ldr	r3, [r3, #0x14]
	add	r6, #2
	add	r5, #4
	cmp	r8, r3
	bne	.Ldb5f4
.Ldb622:
	mov	r0, #2
	mov	r1, #4
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r10
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	ldr	r2, [sp, #0x1c]
	add	r2, #1
	str	r2, [sp, #0x1c]
	cmp	r2, #0x40
	beq	.Ldb648
	b	.Ldb3c6
.Ldb648:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x2c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Ray

	.section .rodata

	.global .Leea08
.Leea08:
	.incrom 0xeea08, 0xeea20
	.global .Leea20
.Leea20:
	.incrom 0xeea20, 0xeea2c
	.global .Leea2c
.Leea2c:
	.incrom 0xeea2c, 0xeea38
	.global .Leea38
.Leea38:
	.incrom 0xeea38, 0xeea41
	.global .Leea41
.Leea41:
	.incrom 0xeea41, 0xeea44
	.global .Leea44
.Leea44:
	.incrom 0xeea44, 0xeea4a
	.global .Leea4a
.Leea4a:
	.incrom 0xeea4a, 0xeea50
	.global .Leea50
.Leea50:
	.incrom 0xeea50, 0xeea56
	.global .Leea56
.Leea56:
	.incrom 0xeea56, 0xeea62
.Leea62:
	.incrom 0xeea62, 0xeea88
.Leea88:
	.incrom 0xeea88, 0xeea91
.Leea91:
	.incrom 0xeea91, 0xeea99
.Leea99:
	.incrom 0xeea99, 0xeeaa2
.Leeaa2:
	.incrom 0xeeaa2, 0xeeab2
.Leeab2:
	.incrom 0xeeab2, 0xeeab8
.Leeab8:
	.incrom 0xeeab8, 0xeeabb
.Leeabb:
	.incrom 0xeeabb, 0xeeac3
.Leeac3:
	.incrom 0xeeac3, 0xeeacc
.Leeacc:
	.incrom 0xeeacc, 0xeeadc
.Leeadc:
	.incrom 0xeeadc, 0xeeae2

	.include "macros.inc"
	.include "gba.inc"

@ Sub_dd9c0
@ Battle animation routine, 446 instructions.
@ State: iwram_1eec, ewram_10000.
@ Calls out to: _Func_bd7dc, _Func_f9080.
@ Touches: REG_BG2PA, REG_BG2X, REG_BLDALPHA, REG_BLDCNT.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Thorn  @ 0x080dd9c0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r7, =iwram_3001eec
	mov	r3, r7
	ldmia	r3!, {r1}
	ldr	r6, =0x7828
	mov	r9, r1
	ldr	r3, [r3]
	sub	sp, #0x34
	add	r6, r9
	str	r3, [sp, #0x30]
	str	r0, [r6]
	mov	r0, #1
	bl	AnimStart
	ldr	r2, =REG_BG2PA
	ldr	r3, .Ldda18	@ 0x100
	strh	r3, [r2]
	ldr	r3, .Ldda1c	@ 0
	add	r2, #0x30
	strh	r3, [r2]
	ldr	r3, .Ldda20	@ 0x1010
	add	r2, #2
	strh	r3, [r2]
	ldr	r0, =_FILE_7e
	mov	r3, #1
	mov	r1, r9
	mov	r2, #1
	bl	LoadVFXFile
	ldr	r3, [r6]
	ldr	r3, [r3, #4]
	cmp	r3, #1
	bne	.Ldda3c
	ldr	r2, =REG_BG2X
	ldr	r3, =0xffff9000
	str	r3, [r2]
	b	.Ldda3c

	.align	2, 0
.Ldda18:
	.word	0x100
.Ldda1c:
	.word	0
.Ldda20:
	.word	0x1010
	.pool

.Ldda3c:
	mov	r5, #1
	mov	r1, #7
	mov	r2, #7
	mov	r3, #3
	mov	r0, #0x2e
	str	r5, [sp]
	bl	BuildDraw2DFuncEx
	ldr	r2, [r7, #0x1c]
	mov	r1, #7
	str	r2, [sp, #0x24]
	mov	r3, #7
	mov	r2, #7
	mov	r0, #0x2f
	str	r5, [sp]
	bl	BuildDraw2DFuncEx
	mov	r3, #0xef
	lsl	r3, #7
	ldr	r2, =0x7784
	add	r3, r9
	str	r5, [r3]
	add	r2, r9
	mov	r3, #0
	ldr	r7, [r7, #0x20]
	mov	r1, #0x90
	str	r3, [r2]
	lsl	r1, #3
	ldr	r0, =Task_BlitAnim
	str	r7, [sp, #0x28]
	bl	StartTask
	ldr	r3, [r6]
	ldr	r2, =.Leebb6
	ldr	r3, [r3, #0x18]
	ldrb	r3, [r2, r3]
	lsl	r3, #3
	add	r3, #0x38
	str	r3, [sp, #0x20]
	mov	r3, #0
	mov	r11, r3
	mov	r1, #1
	mov	r2, #0x80
	ldr	r3, =ewram_2010018
	neg	r1, r1
	lsl	r2, #3
.Ldda98:
	mov	r4, #1
	add	r11, r4
	str	r1, [r3]
	add	r3, #0x1c
	cmp	r11, r2
	bne	.Ldda98
	ldr	r0, [sp, #0x20]
	mov	r6, #0
	str	r6, [sp, #0x2c]
	cmp	r0, #0
	bne	.Lddab0
	b	.Lddd90
.Lddab0:
	ldr	r1, [sp, #0x20]
	sub	r0, #0x40
	sub	r1, #0x10
	str	r0, [sp, #0x1c]
	str	r1, [sp, #0x18]
.Lddaba:
	ldr	r2, [sp, #0x2c]
	ldr	r3, [sp, #0x1c]
	cmp	r2, r3
	bne	.Lddac8
	mov	r0, #0x84
	bl	_Func_80bd7dc
.Lddac8:
	ldr	r4, [sp, #0x2c]
	ldr	r6, [sp, #0x18]
	cmp	r4, r6
	blt	.Lddae4
	ldr	r2, =REG_BLDCNT
	ldr	r3, .Lddb0c	@ 0x3f44
	strh	r3, [r2]
	ldr	r0, [sp, #0x20]
	ldr	r2, .Lddb10	@ 0x1000
	sub	r3, r0, r4
	ldr	r1, =REG_BLDALPHA
	sub	r3, #1
	orr	r3, r2
	strh	r3, [r1]
.Lddae4:
	ldr	r3, =0x7828
	add	r3, r9
	ldr	r3, [r3]
	ldr	r2, =.Leebb6
	ldr	r3, [r3, #0x18]
	ldrb	r3, [r2, r3]
	mov	r1, #0
	mov	r11, r1
	cmp	r3, #0
	bne	.Lddafa
	b	.Lddcda
.Lddafa:
	ldr	r4, =.Leebae
	ldr	r6, [sp, #0x2c]
	mov	r3, #0xc
	mov	r0, #8
	sub	r6, #8
	str	r3, [sp, #0x14]
	str	r4, [sp, #0x10]
	str	r0, [sp, #0xc]
	b	.Lddb34

	.align	2, 0
.Lddb0c:
	.word	0x3f44
.Lddb10:
	.word	0x1000
	.pool

.Lddb34:
	mov	r8, r6
.Lddb36:
	ldr	r1, [sp, #0x2c]
	ldr	r2, [sp, #0xc]
	cmp	r1, r2
	bgt	.Lddb40
	b	.Lddc6c
.Lddb40:
	ldr	r3, =.Leeba6
	mov	r4, r11
	ldrb	r2, [r3, r4]
	mov	r3, r2
	cmp	r3, #1
	bhi	.Lddba6
	mov	r6, r8
	lsl	r3, r6, #1
	add	r3, r8
	lsl	r0, r6, #4
	lsl	r1, r3, #1
	cmp	r0, #0x50
	ble	.Lddb5c
	mov	r0, #0x50
.Lddb5c:
	cmp	r1, #0x1e
	ble	.Lddb62
	mov	r1, #0x1e
.Lddb62:
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Lddb88
	ldr	r3, [sp, #0x10]
	mov	r2, #0
	ldrsb	r2, [r3, r2]
	mov	r4, #0x6c
	sub	r2, r1
	mov	r1, #0x30
	sub	r3, r4, r0
	str	r1, [sp]
	str	r0, [sp, #4]
	mov	r1, r9
	ldr	r0, [sp, #0x30]
	ldr	r6, [sp, #0x28]
	bl	_call_via_r6
	b	.Lddc02
.Lddb88:
	ldr	r3, [sp, #0x10]
	mov	r2, #0
	ldrsb	r2, [r3, r2]
	mov	r4, #0x6c
	add	r2, r1
	mov	r1, #0x30
	sub	r3, r4, r0
	str	r1, [sp]
	str	r0, [sp, #4]
	mov	r1, r9
	ldr	r0, [sp, #0x30]
	ldr	r6, [sp, #0x24]
	bl	_call_via_r6
	b	.Lddc02
.Lddba6:
	mov	r1, r8
	lsl	r0, r1, #3
	cmp	r0, #0x40
	ble	.Lddbb0
	mov	r0, #0x40
.Lddbb0:
	mov	r3, r8
	cmp	r3, #8
	ble	.Lddbb8
	mov	r1, #8
.Lddbb8:
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Lddbe2
	ldr	r4, [sp, #0x10]
	mov	r2, #0
	ldrsb	r2, [r4, r2]
	sub	r2, r1
	mov	r1, #0x20
	str	r1, [sp]
	mov	r1, #0xf0
	mov	r6, #0x6c
	lsl	r1, #4
	sub	r3, r6, r0
	str	r0, [sp, #4]
	add	r1, r9
	ldr	r0, [sp, #0x30]
	ldr	r4, [sp, #0x28]
	bl	_call_via_r4
	b	.Lddc02
.Lddbe2:
	ldr	r6, [sp, #0x10]
	mov	r2, #0
	ldrsb	r2, [r6, r2]
	add	r2, r1
	mov	r1, #0x6c
	sub	r3, r1, r0
	mov	r1, #0x20
	str	r1, [sp]
	mov	r1, #0xf0
	lsl	r1, #4
	str	r0, [sp, #4]
	add	r1, r9
	ldr	r0, [sp, #0x30]
	ldr	r4, [sp, #0x24]
	bl	_call_via_r4
.Lddc02:
	ldr	r3, [sp, #0xc]
	ldr	r6, [sp, #0x2c]
	add	r3, #1
	cmp	r6, r3
	bne	.Lddc14
	ldr	r2, =0x77a8
	mov	r3, #3
	add	r2, r9
	str	r3, [r2]
.Lddc14:
	ldr	r3, [sp, #0xc]
	ldr	r0, [sp, #0x2c]
	add	r3, #3
	cmp	r0, r3
	bge	.Lddc6c
	bl	Random
	mov	r3, #0x1f
	and	r3, r0
	mov	r1, r3
	ldr	r7, =gBuffer
	add	r1, #0x48
	mov	r6, #0
	b	.Lddc36
.Lddc30:
	mov	r7, r5
	add	r7, #0x1c
	add	r6, #1
.Lddc36:
	cmp	r6, #0x40
	beq	.Lddc6c
	mov	r5, r7
	mov	r2, #1
	ldr	r3, [r5, #0x18]
	neg	r2, r2
	cmp	r3, r2
	bne	.Lddc30
	str	r1, [sp, #8]
	bl	Random
	ldr	r3, [sp, #0x10]
	mov	r2, #0
	ldrsb	r2, [r3, r2]
	mov	r3, #0x1f
	and	r3, r0
	add	r2, r3
	add	r2, #0x20
	str	r2, [r5]
	ldr	r1, [sp, #8]
	cmp	r2, #0x60
	ble	.Lddc66
	mov	r3, #0x60
	str	r3, [r5]
.Lddc66:
	mov	r3, #0
	str	r1, [r7, #4]
	str	r3, [r7, #0x18]
.Lddc6c:
	ldr	r2, =0x7828
	add	r2, r9
	ldr	r3, [r2]
	ldr	r3, [r3, #0x14]
	mov	r6, #0
	cmp	r3, #0
	beq	.Lddcac
	ldr	r4, [sp, #0x14]
	mov	r5, r2
	mov	r10, r4
	mov	r7, #0x24
.Lddc82:
	ldr	r0, [sp, #0x2c]
	cmp	r0, r10
	bne	.Lddca0
	mov	r0, #0x84
	bl	_PlaySound
	ldr	r3, [r5]
	ldrsh	r0, [r3, r7]
	mov	r3, #3
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #5
	mov	r3, r6
	bl	Func_80d6888
.Lddca0:
	ldr	r3, [r5]
	ldr	r3, [r3, #0x14]
	add	r6, #1
	add	r7, #2
	cmp	r6, r3
	bne	.Lddc82
.Lddcac:
	ldr	r3, [sp, #0x10]
	ldr	r2, [sp, #0x14]
	ldr	r6, [sp, #0xc]
	add	r3, #1
	str	r3, [sp, #0x10]
	add	r2, #8
	add	r6, #8
	ldr	r3, =0x7828
	str	r2, [sp, #0x14]
	str	r6, [sp, #0xc]
	add	r3, r9
	ldr	r3, [r3]
	ldr	r1, =.Leebb6
	ldr	r3, [r3, #0x18]
	mov	r4, #8
	mov	r0, #1
	ldrb	r3, [r1, r3]
	neg	r4, r4
	add	r11, r0
	add	r8, r4
	cmp	r11, r3
	beq	.Lddcda
	b	.Lddb36
.Lddcda:
	ldr	r3, =.Leebc0
	mov	r2, #0
	ldr	r7, =gBuffer
	mov	r11, r2
	mov	r10, r3
.Lddce4:
	ldr	r3, [r7, #0x18]
	cmp	r3, #0
	blt	.Lddd5c
	lsr	r5, r3, #31
	add	r5, r3, r5
	asr	r5, #1
	ldr	r6, =.Leebc8
	ldr	r0, =.Leebb9
	lsl	r4, r5, #1
	mov	r3, r10
	ldrh	r1, [r6, r4]
	ldrsb	r6, [r0, r5]
	ldrb	r0, [r3, r5]
	lsl	r0, #24
	mov	r8, r4
	asr	r4, r0, #24
	lsr	r0, #31
	ldr	r2, [r7]
	ldr	r3, [r7, #4]
	add	r0, r4, r0
	asr	r0, #1
	sub	r2, r6
	sub	r3, r0
	str	r6, [sp]
	str	r4, [sp, #4]
	add	r1, r9
	ldr	r4, [sp, #0x24]
	ldr	r0, [sp, #0x30]
	bl	_call_via_r4
	ldr	r6, =.Leebc8
	mov	r0, r8
	mov	r3, r10
	ldrh	r1, [r6, r0]
	ldrb	r0, [r3, r5]
	lsl	r0, #24
	asr	r4, r0, #24
	lsr	r0, #31
	ldr	r3, [r7, #4]
	add	r0, r4, r0
	ldr	r6, =.Leebb9
	asr	r0, #1
	sub	r3, r0
	ldrsb	r0, [r6, r5]
	ldr	r2, [r7]
	add	r1, r9
	str	r0, [sp]
	str	r4, [sp, #4]
	ldr	r0, [sp, #0x30]
	ldr	r4, [sp, #0x28]
	bl	_call_via_r4
	ldr	r3, [r7, #0x18]
	add	r3, #1
	str	r3, [r7, #0x18]
	cmp	r3, #0xe
	bne	.Lddd5c
	mov	r3, #1
	neg	r3, r3
	str	r3, [r7, #0x18]
.Lddd5c:
	mov	r6, #1
	add	r11, r6
	mov	r0, r11
	add	r7, #0x1c
	cmp	r0, #0x40
	bne	.Lddce4
	mov	r1, #8
	mov	r0, #8
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r9
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	ldr	r1, [sp, #0x2c]
	ldr	r2, [sp, #0x20]
	add	r1, #1
	str	r1, [sp, #0x2c]
	cmp	r1, r2
	beq	.Lddd90
	b	.Lddaba
.Lddd90:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x34
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Thorn

@ Sub_ddde0
@ Battle animation routine, 555 instructions.
@ State: iwram_1eec, iwram_1f0c, ewram_10000.
@ Calls out to: _Func_b8228, _Func_bd7dc, _Func_f9080.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Bolt  @ 0x080ddde0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r5, =iwram_3001eec
	mov	r3, r5
	ldmia	r3!, {r1}
	ldr	r3, [r3]
	sub	sp, #0x40
	str	r3, [sp, #0x30]
	ldr	r3, =0x7828
	mov	r11, r1
	ldr	r2, [r5, #8]
	add	r3, r11
	str	r2, [sp, #0x1c]
	str	r0, [r3]
	mov	r0, #1
	bl	AnimStart
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #7
	mov	r3, #3
	mov	r0, #0x2e
	bl	BuildDraw2DFuncEx
	ldr	r5, [r5, #0x1c]
	ldr	r0, =_FILE_ce
	mov	r1, r11
	mov	r2, #1
	mov	r3, #0
	str	r5, [sp, #0x20]
	bl	LoadVFXFile
	ldr	r1, =0xc56
	ldr	r0, =_FILE_c4
	add	r1, r11
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	mov	r2, #0
	ldr	r1, [sp, #0x1c]
	mov	r3, #0
	ldr	r0, =_FILE_73
	bl	LoadVFXFile
	mov	r3, #0
	mov	r10, r3
	mov	r2, #0x80
	ldr	r3, =ewram_2010018
	mov	r1, #0
	lsl	r2, #3
.Ldde52:
	mov	r4, #1
	add	r10, r4
	str	r1, [r3]
	add	r3, #0x1c
	cmp	r10, r2
	bne	.Ldde52
	ldr	r3, =0x7098
	mov	r0, #0
	mov	r2, #1
	mov	r10, r0
	neg	r2, r2
	add	r3, r11
.Ldde6a:
	mov	r1, #1
	add	r10, r1
	mov	r4, r10
	str	r2, [r3]
	add	r3, #0x1c
	cmp	r4, #0x40
	bne	.Ldde6a
	mov	r2, #0xef
	lsl	r2, #7
	add	r2, r11
	mov	r3, #2
	str	r3, [r2]
	ldr	r2, =0x7784
	mov	r3, #0x4b
	add	r2, r11
	mov	r1, #0x90
	str	r3, [r2]
	lsl	r1, #3
	ldr	r0, =Task_BlitAnim
	bl	StartTask
	mov	r0, #0x8a
	bl	_PlaySound
	mov	r0, #0
	ldr	r3, =0x7828
	str	r0, [sp, #0x28]
	add	r3, r11
	ldr	r3, [r3]
	ldr	r3, [r3, #0x14]
	mov	r1, #0x28
	lsl	r3, #3
	neg	r1, r1
	cmp	r3, r1
	bne	.Lddeb2
	b	.Lde2a0
.Lddeb2:
	ldr	r2, =0x7828
	add	r2, r11
	str	r2, [sp, #0x14]
.Lddeb8:
	ldr	r3, [sp, #0x28]
	cmp	r3, #0x18
	bne	.Lddec4
	mov	r0, #0x85
	bl	_Func_80bd7dc
.Lddec4:
	mov	r4, #0
	str	r4, [sp, #0x2c]
	ldr	r0, [sp, #0x14]
	ldr	r3, [r0]
	ldr	r3, [r3, #0x14]
	cmp	r3, #0
	beq	.Lddefc
	ldr	r5, =Func_80008d8
.Ldded4:
	ldr	r1, [sp, #0x2c]
	ldr	r2, [sp, #0x28]
	lsl	r3, r1, #3
	cmp	r2, r3
	bne	.Lddeea
	mov	r1, #0x80
	ldr	r0, [sp, #0x30]
	lsl	r1, #7
	ldr	r2, =0x10101010
	bl	_call_via_r5
.Lddeea:
	ldr	r3, [sp, #0x2c]
	add	r3, #1
	str	r3, [sp, #0x2c]
	ldr	r4, [sp, #0x14]
	ldr	r3, [r4]
	ldr	r0, [sp, #0x2c]
	ldr	r3, [r3, #0x14]
	cmp	r0, r3
	bne	.Ldded4
.Lddefc:
	mov	r1, #0
	str	r1, [sp, #0x2c]
	ldr	r2, =0x7828
	mov	r4, r11
	ldr	r3, [r4, r2]
	ldr	r3, [r3, #0x14]
	cmp	r3, #0
	bne	.Lddf0e
	b	.Lde1f0
.Lddf0e:
	mov	r0, sp
	add	r0, #0x34
	mov	r1, #0x24
	mov	r3, #0
	str	r0, [sp, #0x18]
	str	r1, [sp, #0x10]
	str	r3, [sp, #0xc]
.Lddf1c:
	mov	r0, r11
	add	r5, r0, r2
	ldr	r3, [r5]
	ldr	r1, [sp, #0x10]
	ldr	r4, [sp, #0x2c]
	ldrsh	r0, [r3, r1]
	lsl	r4, #3
	ldr	r1, [sp, #0x18]
	mov	r8, r4
	bl	GetBattleActorPos3
	ldr	r4, [sp, #0x18]
	ldr	r3, [r4]
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	str	r3, [r4]
	ldr	r0, [sp, #0x28]
	mov	r3, r8
	add	r3, #1
	cmp	r0, r3
	bne	.Lddf50
	ldr	r2, =0x77a8
	mov	r3, #4
	add	r2, r11
	str	r3, [r2]
.Lddf50:
	mov	r3, r8
	ldr	r1, [sp, #0x28]
	add	r3, #4
	cmp	r1, r3
	bne	.Lddf7a
	ldr	r3, [r5]
	ldr	r2, [sp, #0x10]
	ldrsh	r0, [r3, r2]
	mov	r3, #6
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #5
	ldr	r3, [sp, #0x2c]
	bl	Func_80d6888
	ldr	r3, [r5]
	ldr	r1, [sp, #0x10]
	ldrsh	r0, [r3, r1]
	mov	r1, #6
	bl	_SetBattleActorKnockback
.Lddf7a:
	mov	r3, #2
	ldr	r4, [sp, #0x28]
	add	r3, r8
	mov	r9, r3
	cmp	r4, r8
	bge	.Lddf88
	b	.Lde0da
.Lddf88:
	mov	r3, r8
	add	r3, #0x10
	cmp	r4, r3
	blt	.Lddf92
	b	.Lde0d4
.Lddf92:
	mov	r0, r8
	sub	r3, r4, r0
	lsl	r6, r3, #6
	cmp	r6, #0x68
	ble	.Lddf9e
	mov	r6, #0x68
.Lddf9e:
	ldr	r3, [r5]
	ldr	r3, [r3, #0x18]
	ldr	r7, =.Leebd6
	lsl	r3, #2
	add	r3, #3
	mov	r2, r7
	ldrb	r3, [r2, r3]
	mov	r1, #0
	mov	r10, r1
	cmp	r3, #0
	beq	.Lde004
	ldr	r3, [sp, #0x2c]
	ldr	r4, [sp, #0x28]
	mov	r9, r7
	add	r5, r3, r4
.Lddfbc:
	mov	r0, r10
	add	r3, r5, r0
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	mov	r2, #3
	and	r3, r2
	lsl	r2, r3, #1
	add	r2, r3
	lsl	r1, r2, #4
	sub	r1, r2
	lsl	r1, #6
	ldr	r2, =0xc56
	add	r1, r11
	add	r1, r2
	ldr	r2, [sp, #0x34]
	mov	r3, #0x18
	sub	r2, #0xc
	str	r3, [sp]
	str	r6, [sp, #4]
	mov	r3, #0
	ldr	r0, [sp, #0x30]
	ldr	r4, [sp, #0x20]
	bl	_call_via_r4
	ldr	r1, [sp, #0x14]
	ldr	r3, [r1]
	ldr	r3, [r3, #0x18]
	lsl	r3, #2
	add	r3, #3
	mov	r2, r9
	mov	r0, #1
	ldrb	r3, [r2, r3]
	add	r10, r0
	cmp	r10, r3
	bne	.Lddfbc
.Lde004:
	mov	r3, #2
	add	r3, r8
	ldr	r4, [sp, #0x28]
	mov	r9, r3
	cmp	r4, r9
	bne	.Lde0da
	ldr	r1, [sp, #0x14]
	ldr	r3, [r1]
	ldr	r3, [r3, #0x18]
	lsl	r3, #2
	ldrb	r3, [r7, r3]
	mov	r0, #0
	mov	r10, r0
	cmp	r3, #0
	beq	.Lde0da
	ldr	r2, [sp, #0xc]
	ldr	r3, =gBuffer
	add	r7, r2, r3
.Lde028:
	bl	Random
	ldr	r6, =0x1ff
	and	r6, r0
	bl	Random
	ldr	r3, [sp, #0x34]
	ldr	r5, =0x7fff
	ldr	r4, =0xffffc000
	lsl	r3, #16
	str	r3, [r7]
	and	r5, r0
	mov	r3, #0xd0
	add	r5, r4
	lsl	r3, #15
	str	r3, [r7, #4]
	mov	r0, r5
	bl	sin
	add	r6, #0x40
	mov	r3, r6
	mul	r3, r0
	asr	r3, #5
	str	r3, [r7, #0xc]
	mov	r0, r5
	bl	cos
	mov	r3, r6
	mul	r3, r0
	neg	r3, r3
	asr	r3, #6
	str	r3, [r7, #0x10]
	bl	Random
	mov	r3, #7
	and	r3, r0
	add	r3, #0x20
	str	r3, [r7, #0x18]
	ldr	r1, [sp, #0x14]
	ldr	r3, [r1]
	ldr	r3, [r3, #0x18]
	ldr	r2, =.Leebd6
	lsl	r3, #2
	mov	r0, #1
	ldrb	r3, [r2, r3]
	add	r10, r0
	add	r7, #0x1c
	cmp	r10, r3
	bne	.Lde028
	b	.Lde0da

	.pool_aligned

.Lde0d4:
	mov	r3, #2
	add	r3, r8
	mov	r9, r3
.Lde0da:
	ldr	r4, [sp, #0x28]
	cmp	r4, r9
	blt	.Lde1ca
	mov	r3, r8
	add	r3, #0x18
	cmp	r4, r3
	bge	.Lde1ca
	ldr	r1, [sp, #0x14]
	ldr	r3, [r1]
	ldr	r3, [r3, #0x18]
	ldr	r2, =.Leebd6
	lsl	r3, #2
	add	r3, #1
	ldrb	r3, [r2, r3]
	mov	r0, #0
	mov	r10, r0
	cmp	r3, #0
	beq	.Lde1ca
	ldr	r7, =0x7828
	mov	r3, #3
	mov	r9, r3
	add	r7, r11
.Lde106:
	mov	r4, r10
	mov	r0, r9
	and	r4, r0
	str	r4, [sp, #8]
	bl	Random
	ldr	r3, [r7]
	ldr	r3, [r3, #0x18]
	ldr	r1, =.Leebd6
	lsl	r3, #2
	add	r3, #2
	ldrb	r5, [r1, r3]
	mov	r1, r5
	bl	__umodsi3
	ldr	r2, [sp, #0x18]
	ldr	r2, [r2, #4]
	mov	r8, r2
	mov	r3, r8
	ldr	r4, [sp, #8]
	sub	r3, r0
	sub	r5, r0
	ldr	r0, =Data_eded0
	mov	r8, r3
	ldrb	r3, [r0, r4]
	mov	r1, r8
	lsr	r3, #1
	sub	r1, r3
	mov	r2, #8
	mov	r8, r1
	add	r8, r2
	bl	Random
	add	r5, #1
	ldr	r3, [sp, #0x18]
	mov	r1, r5
	ldr	r6, [r3]
	bl	__umodsi3
	ldr	r4, [sp, #8]
	add	r6, r0
	ldr	r0, =Data_edeca
	lsr	r3, r5, #31
	add	r5, r3
	ldrb	r3, [r0, r4]
	asr	r5, #1
	lsr	r3, #1
	sub	r6, r5
	sub	r6, r3
	bl	Random
	ldr	r3, =.Leebe2
	mov	r1, r9
	and	r0, r1
	ldrb	r2, [r3, r0]
	mov	r3, r9
	orr	r3, r2
	ldr	r2, [r7]
	ldr	r1, =.Leebe6
	ldr	r2, [r2, #0x18]
	ldrb	r2, [r1, r2]
	mov	r0, #0x2f
	str	r2, [sp]
	mov	r1, #7
	mov	r2, #7
	bl	BuildDraw2DFuncEx
	ldr	r4, [sp, #8]
	ldr	r2, =Data_edebe
	lsl	r3, r4, #1
	ldrh	r1, [r2, r3]
	ldr	r2, =Data_edeca
	ldrb	r3, [r2, r4]
	ldr	r0, =Data_eded0
	str	r3, [sp]
	ldrb	r3, [r0, r4]
	ldr	r2, =iwram_3001f0c
	str	r3, [sp, #4]
	add	r1, r11
	ldr	r4, [r2]
	mov	r3, r8
	ldr	r0, [sp, #0x30]
	mov	r2, r6
	bl	_call_via_r4
	mov	r0, #0x2f
	bl	gfree
	mov	r3, #1
	add	r10, r3
	ldr	r3, [r7]
	ldr	r3, [r3, #0x18]
	ldr	r4, =.Leebd6
	lsl	r3, #2
	add	r3, #1
	ldrb	r3, [r4, r3]
	cmp	r10, r3
	bne	.Lde106
.Lde1ca:
	ldr	r0, [sp, #0x10]
	ldr	r1, [sp, #0xc]
	ldr	r3, [sp, #0x2c]
	mov	r2, #0xe0
	lsl	r2, #4
	add	r0, #2
	add	r1, r2
	add	r3, #1
	str	r0, [sp, #0x10]
	str	r3, [sp, #0x2c]
	str	r1, [sp, #0xc]
	ldr	r2, =0x7828
	mov	r4, r11
	ldr	r3, [r4, r2]
	ldr	r0, [sp, #0x2c]
	ldr	r3, [r3, #0x14]
	cmp	r0, r3
	beq	.Lde1f0
	b	.Lddf1c
.Lde1f0:
	mov	r1, #0
	ldr	r6, =gBuffer
	mov	r10, r1
.Lde1f6:
	ldr	r3, [r6, #0x18]
	cmp	r3, #0
	ble	.Lde262
	sub	r3, #1
	mov	r2, #0x80
	str	r3, [r6, #0x18]
	lsl	r2, #5
	mov	r0, r6
	mov	r1, #0x3c
	bl	Func_80e3908
	mov	r2, #0xd0
	ldr	r3, [r6, #4]
	lsl	r2, #15
	cmp	r3, r2
	ble	.Lde224
	ldr	r3, [r6, #0x10]
	neg	r3, r3
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	str	r3, [r6, #0x10]
	b	.Lde262
.Lde224:
	ldr	r2, [r6]
	ldr	r4, =0x7effff
	cmp	r2, r4
	bhi	.Lde262
	cmp	r3, #0
	blt	.Lde262
	ldr	r4, [r6, #0x18]
	cmp	r4, #0
	bge	.Lde238
	add	r4, #0xf
.Lde238:
	asr	r4, #4
	add	r4, #1
	lsl	r5, r4, #1
	ldr	r0, =Data_ede48
	sub	r1, r5, #2
	ldrh	r1, [r0, r1]
	ldr	r0, [sp, #0x1c]
	add	r1, r0, r1
	lsr	r0, r4, #31
	add	r0, r4, r0
	asr	r0, #1
	asr	r2, #16
	asr	r3, #16
	sub	r2, r0
	sub	r3, r4
	str	r4, [sp]
	str	r5, [sp, #4]
	ldr	r0, [sp, #0x30]
	ldr	r4, [sp, #0x20]
	bl	_call_via_r4
.Lde262:
	mov	r0, #1
	mov	r1, #0x80
	add	r10, r0
	lsl	r1, #3
	add	r6, #0x1c
	cmp	r10, r1
	bne	.Lde1f6
	mov	r0, #2
	mov	r1, #8
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r11
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	ldr	r2, [sp, #0x28]
	add	r2, #1
	str	r2, [sp, #0x28]
	ldr	r4, [sp, #0x14]
	ldr	r3, [r4]
	ldr	r3, [r3, #0x14]
	lsl	r3, #3
	add	r3, #0x28
	cmp	r2, r3
	beq	.Lde2a0
	b	.Lddeb8
.Lde2a0:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x40
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Bolt

@ Sub_de2f8
@ Battle animation routine, 696 instructions.
@ State: iwram_1eec, iwram_1e80, iwram_1e50, ewram_10000.
@ Calls out to: _Func_b7dd0, _Func_f9080.
@ Touches: REG_BLDALPHA.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Djinni  @ 0x080de2f8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x6c
	mov	r6, r2
	ldr	r2, =iwram_3001eec
	str	r3, [sp, #0x2c]
	mov	r3, r2
	mov	r5, r1
	ldmia	r3!, {r1}
	str	r1, [sp, #0x28]
	ldr	r3, [r3]
	str	r3, [sp, #0x24]
	ldr	r2, [r2, #8]
	str	r2, [sp, #0x18]
	ldr	r2, =0x7828
	add	r3, r1, r2
	str	r0, [r3]
	mov	r0, #0
	bl	AnimStart
	ldr	r4, =0x77b4
	ldr	r3, [sp, #0x28]
	add	r2, r3, r4
	mov	r3, #0x18
	str	r3, [r2]
	ldr	r7, [sp, #0x28]
	ldr	r1, =0x77b8
	mov	r3, #0
	add	r2, r7, r1
	str	r3, [r2]
	cmp	r5, #3
	ble	.Lde34a
	mov	r2, #0x54
	sub	r5, #4
	str	r2, [sp, #0x14]
	b	.Lde34e
.Lde34a:
	mov	r3, #0x40
	str	r3, [sp, #0x14]
.Lde34e:
	cmp	r5, #1
	beq	.Lde366
	cmp	r5, #1
	bgt	.Lde35c
	cmp	r5, #0
	beq	.Lde362
	b	.Lde36e
.Lde35c:
	cmp	r5, #2
	beq	.Lde36a
	b	.Lde36e
.Lde362:
	ldr	r0, =_FILE_94
	b	.Lde370
.Lde366:
	ldr	r0, =_FILE_92
	b	.Lde370
.Lde36a:
	ldr	r0, =_FILE_8e
	b	.Lde370
.Lde36e:
	ldr	r0, =_FILE_90
.Lde370:
	bl	GetFile
	mov	r5, r0
	mov	r0, #0xa0
	ldr	r3, =Func_8001af8
	mov	r1, r5
	mov	r2, #0x80
	lsl	r0, #19
	bl	_call_via_r3
	add	r5, #0x80
	ldr	r1, [sp, #0x28]
	mov	r0, r5
	bl	DecompressLZ
	ldr	r0, =_FILE_73
	ldr	r1, [sp, #0x18]
	mov	r2, #0
	mov	r3, #0
	bl	LoadVFXFile
	cmp	r6, #1
	bne	.Lde3c0
	mov	r3, #3
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #7
	mov	r3, #7
	mov	r0, #0x2e
	bl	BuildDraw2DFuncEx
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x2f
	mov	r1, #7
	mov	r2, #7
	mov	r3, #7
	bl	BuildDraw2DFuncEx
	b	.Lde3de
.Lde3c0:
	mov	r3, #3
	mov	r1, #7
	mov	r2, #7
	mov	r0, #0x2e
	str	r3, [sp]
	bl	BuildDraw2DFuncEx
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x2f
	mov	r1, #7
	mov	r2, #7
	mov	r3, #3
	bl	BuildDraw2DFuncEx
.Lde3de:
	ldr	r3, =gPtrs
	ldr	r4, [sp, #0x28]
	mov	r2, r3
	ldr	r7, =0x7828
	add	r3, #0xbc
	ldr	r3, [r3]
	add	r5, r4, r7
	str	r3, [sp, #0x20]
	add	r2, #0xb8
	ldr	r3, [r5]
	ldr	r2, [r2]
	ldr	r0, [r3, #8]
	str	r2, [sp, #0x1c]
	bl	_GetBattleActor
	ldr	r3, [r5]
	ldr	r0, [r0]
	mov	r9, r0
	mov	r1, #0x24
	ldrsh	r0, [r3, r1]
	bl	_GetBattleActor
	ldr	r0, [r0]
	mov	r2, #0
	str	r0, [sp, #0x10]
	ldr	r7, =gBuffer
	mov	r8, r2
	mov	r10, r2
.Lde416:
	bl	Random
	ldr	r3, =0xffff
	mov	r6, r0
	and	r6, r3
	bl	Random
	mov	r3, r10
	str	r3, [r7]
	mov	r5, #0xff
	and	r5, r0
	bl	Random
	mov	r3, #0x1f
	and	r3, r0
	add	r3, #0x14
	mov	r4, r10
	lsl	r3, #16
	str	r4, [r7, #8]
	str	r3, [r7, #4]
	mov	r0, r6
	bl	sin
	add	r5, #0x80
	mov	r3, r5
	mul	r3, r0
	mov	r1, r10
	asr	r3, #5
	str	r3, [r7, #0xc]
	str	r1, [r7, #0x10]
	mov	r0, r6
	bl	cos
	mov	r3, r5
	mul	r3, r0
	asr	r3, #5
	str	r3, [r7, #0x14]
	mov	r3, #1
	add	r8, r3
	mov	r2, r10
	mov	r4, r8
	str	r2, [r7, #0x18]
	add	r7, #0x1c
	cmp	r4, #0x40
	bne	.Lde416
	ldr	r7, [sp, #0x28]
	mov	r1, #0xef
	lsl	r1, #7
	add	r2, r7, r1
	mov	r3, #2
	str	r3, [r2]
	ldr	r3, =0x7784
	mov	r1, #0x90
	add	r2, r7, r3
	mov	r3, #0x4b
	str	r3, [r2]
	lsl	r1, #3
	ldr	r0, =Task_BlitAnim
	bl	StartTask
	mov	r7, r9
	ldr	r3, [r7, #8]
	add	r4, sp, #0x60
	str	r3, [r4]
	mov	r3, #0
	str	r3, [r4, #4]
	ldr	r3, [r7, #0x10]
	str	r3, [r4, #8]
	ldr	r1, [sp, #0x2c]
	mov	r11, r4
	cmp	r1, #4
	bhi	.Lde558
	ldr	r2, =.Lde4b0
	lsl	r3, r1, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.Lde4b0:
	.word	.Lde4c4
	.word	.Lde4d8
	.word	.Lde4de
	.word	.Lde4f2
	.word	.Lde506

.Lde4c4:
	ldr	r2, [sp, #0x10]
	ldr	r3, [r2, #8]
	add	r5, sp, #0x54
	str	r3, [r5]
	mov	r3, #0xf0
	lsl	r3, #14
	str	r3, [r5, #4]
	ldr	r3, [r2, #0x10]
	str	r3, [r5, #8]
	b	.Lde55a
.Lde4d8:
	ldr	r4, [sp, #0x10]
	ldr	r3, [r4, #8]
	b	.Lde4f6
.Lde4de:
	mov	r7, r9
	ldr	r3, [r7, #8]
	add	r5, sp, #0x54
	str	r3, [r5]
	mov	r3, #0xf0
	lsl	r3, #14
	str	r3, [r5, #4]
	ldr	r3, [r7, #0x10]
	str	r3, [r5, #8]
	b	.Lde55a
.Lde4f2:
	mov	r1, r9
	ldr	r3, [r1, #8]
.Lde4f6:
	add	r5, sp, #0x54
	str	r3, [r5]
	mov	r3, #0xf0
	lsl	r3, #14
	str	r3, [r5, #4]
	mov	r3, #0
	str	r3, [r5, #8]
	b	.Lde55a
.Lde506:
	mov	r3, #0xf0
	add	r5, sp, #0x54
	mov	r2, #0
	lsl	r3, #14
	str	r2, [r5]
	str	r3, [r5, #4]
	str	r2, [r5, #8]
	b	.Lde55a

	.pool_aligned

.Lde558:
	add	r5, sp, #0x54
.Lde55a:
	mov	r2, sp
	add	r2, #0x48
	str	r2, [sp, #0xc]
	mov	r4, r11
	ldr	r3, [r4]
	ldr	r0, [r5]
	mov	r1, #0x28
	sub	r0, r3
	bl	__divsi3
	ldr	r7, [sp, #0xc]
	str	r0, [r7]
	mov	r1, r11
	ldr	r3, [r1, #4]
	ldr	r0, [r5, #4]
	mov	r1, #0x28
	sub	r0, r3
	bl	__divsi3
	str	r0, [r7, #4]
	mov	r2, r11
	ldr	r3, [r2, #8]
	ldr	r0, [r5, #8]
	mov	r1, #0x28
	sub	r0, r3
	bl	__divsi3
	str	r0, [r7, #8]
	ldr	r4, [sp, #0x14]
	mov	r3, #0
	mov	r10, r3
	cmp	r4, #0
	bne	.Lde59e
	b	.Lde8f4
.Lde59e:
	ldr	r3, =iwram_3001e80
	mov	r7, r10
	ldr	r5, [r3]
	cmp	r7, #0x4b
	ble	.Lde5b6
	ldr	r3, .Lde5c4	@ 0xa8
	lsl	r2, r7, #1
	sub	r3, r2
	ldr	r2, .Lde5c8	@ 0x1000
	ldr	r1, =REG_BLDALPHA
	orr	r3, r2
	strh	r3, [r1]
.Lde5b6:
	mov	r1, r10
	cmp	r1, #8
	bne	.Lde5d4
	mov	r0, #0xd4
	bl	_PlaySound
	b	.Lde5d4

	.align	2, 0
.Lde5c4:
	.word	0xa8
.Lde5c8:
	.word	0x1000
	.pool

.Lde5d4:
	bl	InitMatrixStack
	mov	r1, r5
	add	r1, #0xc
	mov	r0, r5
	bl	MatrixSetLook
	mov	r3, r10
	sub	r3, #6
	cmp	r3, #0x27
	bhi	.Lde608
	ldr	r4, [sp, #0xc]
	mov	r2, r11
	ldr	r3, [r2]
	ldr	r2, [r4]
	mov	r7, r11
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r7, #4]
	ldr	r2, [r4, #4]
	add	r3, r2
	str	r3, [r7, #4]
	ldr	r3, [r7, #8]
	ldr	r2, [r4, #8]
	add	r3, r2
	str	r3, [r7, #8]
.Lde608:
	mov	r0, r11
	bl	MatrixTranslatev
	mov	r1, r10
	cmp	r1, #0
	bne	.Lde62e
	ldr	r2, [sp, #0x28]
	ldr	r4, =0x7828
	add	r3, r2, r4
	ldr	r3, [r3]
	mov	r2, #1
	ldr	r0, [r3, #8]
	mov	r3, #1
	str	r1, [sp]
	neg	r2, r2
	mov	r1, #7
	neg	r3, r3
	bl	Func_80d6888
.Lde62e:
	mov	r7, r10
	cmp	r7, #0x18
	bne	.Lde64e
	ldr	r1, [sp, #0x28]
	ldr	r2, =0x7828
	add	r3, r1, r2
	ldr	r3, [r3]
	mov	r2, #1
	ldr	r0, [r3, #8]
	mov	r3, #0
	str	r3, [sp]
	mov	r1, #0
	neg	r2, r2
	sub	r3, #1
	bl	Func_80d6888
.Lde64e:
	mov	r4, r10
	neg	r4, r4
	mov	r1, r10
	mov	r3, #0
	lsl	r1, #8
	str	r4, [sp, #8]
	ldr	r6, =gBuffer
	mov	r8, r3
	lsl	r7, r4, #8
	mov	r9, r1
.Lde662:
	mov	r3, r8
	cmp	r3, #0
	bge	.Lde66a
	add	r3, #7
.Lde66a:
	asr	r3, #3
	cmp	r10, r3
	bge	.Lde672
	b	.Lde7a4
.Lde672:
	ldr	r3, [r6, #0x18]
	cmp	r3, #0
	beq	.Lde67a
	b	.Lde7a4
.Lde67a:
	bl	MatrixPush
	mov	r3, #3
	mov	r2, r8
	and	r3, r2
	cmp	r3, #1
	beq	.Lde6a4
	cmp	r3, #1
	bgt	.Lde692
	cmp	r3, #0
	beq	.Lde69c
	b	.Lde6c0
.Lde692:
	cmp	r3, #2
	beq	.Lde6ac
	cmp	r3, #3
	beq	.Lde6b4
	b	.Lde6c0
.Lde69c:
	mov	r0, r9
	bl	MatrixYaw
	b	.Lde6c0
.Lde6a4:
	mov	r0, r7
	bl	MatrixPitch
	b	.Lde6c0
.Lde6ac:
	mov	r0, r7
	bl	MatrixRoll
	b	.Lde6c0
.Lde6b4:
	mov	r0, r7
	bl	MatrixPitch
	mov	r0, r7
	bl	MatrixRoll
.Lde6c0:
	add	r5, sp, #0x30
	mov	r1, r5
	mov	r0, r6
	bl	Func_80e3944
	ldr	r3, [r5]
	asr	r3, #1
	str	r3, [r5]
	bl	MatrixPop
	ldr	r2, [r5, #8]
	cmp	r2, #0xf9
	bgt	.Lde6e0
	mov	r3, #0xfa
	str	r3, [r5, #8]
	mov	r2, #0xfa
.Lde6e0:
	ldr	r3, =0x27a
	cmp	r2, r3
	ble	.Lde6ea
	str	r3, [r5, #8]
	mov	r2, r3
.Lde6ea:
	mov	r3, r2
	sub	r3, #0xfa
	cmp	r3, #0
	bge	.Lde6f4
	add	r3, #0x3f
.Lde6f4:
	asr	r3, #6
	mov	r0, #8
	sub	r0, r3
	lsl	r4, r0, #1
	ldr	r2, =Data_ede48
	sub	r3, r4, #2
	ldrh	r1, [r2, r3]
	ldr	r3, [sp, #0x18]
	add	r1, r3, r1
	lsr	r3, r0, #31
	ldr	r2, [r5]
	add	r3, r0, r3
	asr	r3, #1
	sub	r2, r3
	ldr	r3, [r5, #4]
	str	r0, [sp]
	sub	r3, r0
	str	r4, [sp, #4]
	ldr	r0, [sp, #0x24]
	ldr	r4, [sp, #0x20]
	bl	_call_via_r4
	mov	r0, r6
	mov	r1, #0x3c
	mov	r2, #0
	bl	Func_80e38b8
	mov	r3, r8
	cmp	r3, #0
	bge	.Lde732
	add	r3, #7
.Lde732:
	asr	r3, #3
	add	r3, #0x18
	cmp	r10, r3
	blt	.Lde7a4
	ldr	r3, [r6]
	neg	r3, r3
	asr	r5, r3, #7
	ldr	r3, [r6, #8]
	ldr	r2, [r6, #4]
	neg	r3, r3
	asr	r4, r3, #7
	neg	r2, r2
	ldr	r3, [r6, #0x10]
	ldr	r1, [r6, #0xc]
	asr	r2, #7
	add	r2, r3, r2
	ldr	r3, [r6, #0x14]
	add	r1, r5
	add	r0, r3, r4
	lsl	r3, r1, #5
	sub	r3, r1
	lsl	r3, #1
	str	r1, [r6, #0xc]
	str	r2, [r6, #0x10]
	str	r0, [r6, #0x14]
	cmp	r3, #0
	bge	.Lde76a
	add	r3, #0x3f
.Lde76a:
	asr	r3, #6
	str	r3, [r6, #0xc]
	lsl	r3, r2, #5
	sub	r3, r2
	lsl	r3, #1
	cmp	r3, #0
	bge	.Lde77a
	add	r3, #0x3f
.Lde77a:
	asr	r3, #6
	str	r3, [r6, #0x10]
	lsl	r3, r0, #5
	sub	r3, r0
	lsl	r3, #1
	cmp	r3, #0
	bge	.Lde78a
	add	r3, #0x3f
.Lde78a:
	ldr	r1, =0x7ff
	asr	r3, #6
	ldr	r2, =0xffe
	str	r3, [r6, #0x14]
	add	r3, r5, r1
	cmp	r3, r2
	bhi	.Lde7a4
	add	r3, r4, r1
	cmp	r3, r2
	bhi	.Lde7a4
	mov	r2, #1
	neg	r2, r2
	str	r2, [r6, #0x18]
.Lde7a4:
	ldr	r4, [sp, #8]
	mov	r1, r10
	lsl	r3, r4, #5
	mov	r2, #1
	add	r7, r3
	add	r8, r2
	lsl	r3, r1, #5
	add	r9, r3
	mov	r3, r8
	add	r6, #0x1c
	cmp	r3, #0x20
	beq	.Lde7be
	b	.Lde662
.Lde7be:
	mov	r3, r10
	sub	r3, #0x36
	cmp	r3, #0xf
	bhi	.Lde80c
	lsl	r0, r1, #10
	bl	sin
	mov	r3, #0
	add	r5, sp, #0x30
	add	r2, sp, #0x3c
	lsl	r0, #2
	str	r0, [r2]
	str	r3, [r2, #4]
	str	r3, [r2, #8]
	mov	r0, r2
	mov	r1, r5
	bl	Func_80e3944
	ldr	r3, [r5]
	ldr	r4, [sp, #0x8c]
	str	r3, [r4]
	ldr	r7, [sp, #0x90]
	ldr	r3, [r5, #4]
	str	r3, [r7]
	ldr	r2, [r5]
	ldr	r3, [r5, #4]
	asr	r2, #1
	mov	r1, #0x14
	str	r2, [r5]
	str	r1, [sp]
	mov	r1, #0x28
	str	r1, [sp, #4]
	sub	r2, #0xa
	sub	r3, #0x14
	ldr	r0, [sp, #0x24]
	ldr	r1, [sp, #0x28]
	ldr	r4, [sp, #0x1c]
	bl	_call_via_r4
.Lde80c:
	mov	r7, r10
	cmp	r7, #0x40
	bne	.Lde876
	ldr	r2, [sp, #0x28]
	mov	r3, #0xe1
	mov	r1, #0
	lsl	r3, #7
	mov	r8, r1
	add	r7, r2, r3
.Lde81e:
	bl	Random
	ldr	r3, =0xffff
	mov	r6, r0
	and	r6, r3
	bl	Random
	ldr	r4, [sp, #0x8c]
	ldr	r3, [r4]
	lsl	r3, #15
	str	r3, [r7]
	ldr	r1, [sp, #0x90]
	ldr	r3, [r1]
	mov	r5, #0xff
	lsl	r3, #16
	str	r3, [r7, #4]
	and	r5, r0
	mov	r0, r6
	bl	sin
	add	r5, #0x80
	mov	r3, r5
	mul	r3, r0
	asr	r3, #6
	str	r3, [r7, #0xc]
	mov	r0, r6
	bl	cos
	mov	r3, r5
	mul	r3, r0
	asr	r3, #5
	str	r3, [r7, #0x10]
	bl	Random
	mov	r3, #0xf
	and	r3, r0
	mov	r2, #1
	add	r3, #8
	add	r8, r2
	str	r3, [r7, #0x18]
	mov	r3, r8
	add	r7, #0x1c
	cmp	r3, #0x40
	bne	.Lde81e
.Lde876:
	mov	r4, r10
	cmp	r4, #0x3f
	ble	.Lde8d8
	ldr	r1, [sp, #0x28]
	mov	r2, #0xe1
	mov	r7, #0
	lsl	r2, #7
	ldr	r6, =Data_ede48
	mov	r8, r7
	add	r5, r1, r2
.Lde88a:
	ldr	r0, [r5, #0x18]
	cmp	r0, #0
	blt	.Lde8cc
	asr	r0, #3
	add	r0, #2
	lsl	r4, r0, #1
	sub	r3, r4, #2
	ldrh	r1, [r6, r3]
	ldr	r3, [sp, #0x18]
	add	r1, r3, r1
	lsr	r3, r0, #31
	mov	r7, #2
	ldrsh	r2, [r5, r7]
	add	r3, r0, r3
	asr	r3, #1
	sub	r2, r3
	mov	r7, #6
	ldrsh	r3, [r5, r7]
	str	r0, [sp]
	sub	r3, r0
	str	r4, [sp, #4]
	ldr	r0, [sp, #0x24]
	ldr	r4, [sp, #0x1c]
	bl	_call_via_r4
	mov	r0, r5
	mov	r1, #0x3c
	mov	r2, #0
	bl	Func_80e3908
	ldr	r3, [r5, #0x18]
	sub	r3, #1
	str	r3, [r5, #0x18]
.Lde8cc:
	mov	r7, #1
	add	r8, r7
	mov	r1, r8
	add	r5, #0x1c
	cmp	r1, #0x40
	bne	.Lde88a
.Lde8d8:
	ldr	r3, [sp, #0x28]
	ldr	r4, =0x7824
	add	r2, r3, r4
	mov	r3, #1
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	mov	r7, #1
	ldr	r1, [sp, #0x14]
	add	r10, r7
	cmp	r10, r1
	beq	.Lde8f4
	b	.Lde59e
.Lde8f4:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	ldr	r0, =Func_80cd4b4
	bl	StopTask
	mov	r1, #0x80
	ldr	r5, =Func_80008d4
	lsl	r1, #7
	ldr	r0, =0x6004000
	bl	_call_via_r5
	mov	r1, #0x80
	ldr	r0, [sp, #0x24]
	lsl	r1, #7
	bl	_call_via_r5
	ldr	r2, =REG_BLDALPHA
	ldr	r3, .Lde93c	@ 0x1010
	add	sp, #0x6c
	strh	r3, [r2]
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0

	.align	2, 0
.Lde93c:
	.word	0x1010
.func_end Anim_Djinni

	.section .rodata
	.global .Leeb96
	.global .Leeb48
	.global .Leeb4b
	.global .Leeb4e
	.global .Leeb54
	.global .Leeb58
	.global .Leeb5e
	.global .Leeb61
	.global .Leeb71
	.global .Leeb79
	.global .Leeb80
	.global .Leeb88

.Leeb48:
	.incrom 0xeeb48, 0xeeb4b
.Leeb4b:
	.incrom 0xeeb4b, 0xeeb4e
.Leeb4e:
	.incrom 0xeeb4e, 0xeeb54
.Leeb54:
	.incrom 0xeeb54, 0xeeb58
.Leeb58:
	.incrom 0xeeb58, 0xeeb5e
.Leeb5e:
	.incrom 0xeeb5e, 0xeeb61
.Leeb61:
	.incrom 0xeeb61, 0xeeb71
.Leeb71:
	.incrom 0xeeb71, 0xeeb79
.Leeb79:
	.incrom 0xeeb79, 0xeeb80
.Leeb80:
	.incrom 0xeeb80, 0xeeb88
.Leeb88:
	.incrom 0xeeb88, 0xeeb96
.Leeb96:
	.incrom 0xeeb96, 0xeeba6
.Leeba6:
	.incrom 0xeeba6, 0xeebae
.Leebae:
	.incrom 0xeebae, 0xeebb6
.Leebb6:
	.incrom 0xeebb6, 0xeebb9
.Leebb9:
	.incrom 0xeebb9, 0xeebc0
.Leebc0:
	.incrom 0xeebc0, 0xeebc8
.Leebc8:
	.incrom 0xeebc8, 0xeebd6
.Leebd6:
	.incrom 0xeebd6, 0xeebe2
.Leebe2:
	.incrom 0xeebe2, 0xeebe6
.Leebe6:
	.incrom 0xeebe6, 0xeebe9

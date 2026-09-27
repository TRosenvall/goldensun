	.include "macros.inc"
	.include "gba.inc"

@ Sub_e0564
@ Battle animation routine, 362 instructions.
@ State: iwram_1eec, ewram_10000.
@ Calls out to: _Func_b8228, _Func_bd7dc, _Func_f9080.
@ Touches: REG_BLDALPHA.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Venus  @ 0x080e0564
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
	sub	sp, #0x28
	str	r3, [sp, #0x24]
	ldr	r3, =0x7828
	mov	r9, r1
	ldr	r2, [r5, #8]
	add	r3, r9
	str	r2, [sp, #0x14]
	str	r0, [r3]
	mov	r0, #0
	bl	AnimStart
	ldr	r2, =REG_BLDALPHA
	ldr	r3, .Le05d0	@ 0x1010
	mov	r6, #2
	strh	r3, [r2]
	mov	r1, #7
	mov	r2, #7
	mov	r3, #0xb
	mov	r0, #0x2e
	str	r6, [sp]
	bl	BuildDraw2DFuncEx
	mov	r3, #3
	mov	r1, #7
	mov	r2, #7
	mov	r0, #0x2f
	str	r3, [sp]
	bl	BuildDraw2DFuncEx
	ldr	r3, [r5, #0x1c]
	ldr	r5, [r5, #0x20]
	str	r3, [sp, #0x18]
	ldr	r1, [sp, #0x14]
	ldr	r0, =_FILE_73
	mov	r2, #0
	mov	r3, #0
	str	r5, [sp, #0x1c]
	bl	LoadVFXFile
	ldr	r0, =_FILE_94
	mov	r1, r9
	mov	r2, #1
	b	.Le05e8

	.align	2, 0
.Le05d0:
	.word	0x1010
	.pool

.Le05e8:
	mov	r3, #1
	bl	LoadVFXFile
	mov	r1, #0xbe
	lsl	r1, #2
	ldr	r0, =_FILE_6f
	add	r1, r9
	mov	r2, #1
	mov	r3, #0
	bl	LoadVFXFile
	mov	r3, #0xef
	lsl	r3, #7
	ldr	r2, =0x7784
	add	r3, r9
	str	r6, [r3]
	add	r2, r9
	mov	r3, #0x4b
	mov	r1, #0x90
	str	r3, [r2]
	ldr	r0, =Task_BlitAnim
	lsl	r1, #3
	mov	r5, #0xe1
	bl	StartTask
	lsl	r5, #7
	mov	r4, #0
	mov	r10, r4
	mov	r7, #0x3f
	add	r5, r9
	mov	r6, #0x68
.Le0626:
	bl	Random
	and	r0, r7
	str	r0, [r5]
	mov	r0, #1
	add	r10, r0
	mov	r1, r10
	str	r6, [r5, #4]
	add	r5, #0x1c
	cmp	r1, #0x20
	bne	.Le0626
	mov	r2, #0
	mov	r10, r2
	mov	r1, #1
	mov	r2, #0x80
	ldr	r3, =ewram_2010018
	neg	r1, r1
	lsl	r2, #2
.Le064a:
	mov	r4, #1
	add	r10, r4
	str	r1, [r3]
	add	r3, #0x1c
	cmp	r10, r2
	bne	.Le064a
	mov	r0, #0x8d
	bl	_PlaySound
	mov	r0, #0x80
	mov	r7, #0
	lsl	r0, #8
	str	r7, [sp, #0x20]
	str	r0, [sp, #0x10]
.Le0666:
	ldr	r1, [sp, #0x20]
	cmp	r1, #0x4f
	bgt	.Le06a4
	ldr	r0, [sp, #0x10]
	bl	sin
	lsl	r5, r0, #1
	add	r5, r0
	ldr	r0, [sp, #0x10]
	bl	cos
	ldr	r3, [sp, #0x20]
	lsl	r2, r3, #1
	mov	r3, #0x40
	sub	r3, r2
	mul	r3, r0
	lsl	r5, #3
	mov	r2, #0x14
	asr	r5, #16
	asr	r3, #16
	add	r5, #0x16
	str	r2, [sp]
	mov	r2, #0x26
	str	r2, [sp, #4]
	add	r3, #0x1d
	ldr	r0, [sp, #0x24]
	mov	r1, r9
	mov	r2, r5
	ldr	r4, [sp, #0x1c]
	bl	_call_via_r4
.Le06a4:
	ldr	r7, [sp, #0x20]
	cmp	r7, #0x38
	bne	.Le06b0
	mov	r0, #0x85
	bl	_Func_80bd7dc
.Le06b0:
	mov	r2, #0xe1
	mov	r0, #0
	lsl	r2, #7
	mov	r1, #0x10
	add	r2, r9
	str	r0, [sp, #0xc]
	mov	r10, r0
	mov	r11, r1
	mov	r8, r2
.Le06c2:
	ldr	r3, [sp, #0x20]
	cmp	r3, r11
	blt	.Le07b6
	mov	r4, r8
	mov	r1, #0x22
	ldr	r2, [r4]
	ldr	r3, [r4, #4]
	str	r1, [sp]
	mov	r1, #0x41
	str	r1, [sp, #4]
	mov	r1, #0x9e
	lsl	r1, #4
	sub	r2, #0x11
	sub	r3, #0x20
	ldr	r0, [sp, #0x24]
	add	r1, r9
	ldr	r7, [sp, #0x18]
	bl	_call_via_r7
	ldr	r0, [sp, #0x20]
	cmp	r0, r11
	bne	.Le07ae
	ldr	r1, [sp, #0xc]
	ldr	r2, =gBuffer
	mov	r4, #0
	add	r7, r1, r2
.Le06f6:
	str	r4, [sp, #8]
	bl	Random
	ldr	r6, =0x7fff
	mov	r3, #0x80
	lsl	r3, #7
	and	r6, r0
	add	r6, r3
	bl	Random
	mov	r1, r8
	ldr	r3, [r1]
	lsl	r3, #16
	str	r3, [r7]
	ldr	r5, =0x1ff
	ldr	r3, [r1, #4]
	and	r5, r0
	add	r3, #0x10
	mov	r0, #0x80
	lsl	r3, #16
	lsl	r0, #1
	add	r5, r0
	str	r3, [r7, #4]
	mov	r0, r6
	bl	sin
	mov	r3, r5
	mul	r3, r0
	asr	r3, #7
	str	r3, [r7, #0xc]
	mov	r0, r6
	bl	cos
	mov	r3, r5
	mul	r3, r0
	asr	r3, #6
	str	r3, [r7, #0x10]
	bl	Random
	mov	r3, #0xf
	ldr	r4, [sp, #8]
	and	r3, r0
	add	r3, #0x20
	add	r4, #1
	str	r3, [r7, #0x18]
	add	r7, #0x1c
	cmp	r4, #0x10
	bne	.Le06f6
	mov	r3, #1
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.Le0766
	mov	r0, #0x85
	bl	_PlaySound
.Le0766:
	ldr	r2, =0x77a8
	mov	r3, #4
	add	r2, r9
	str	r3, [r2]
	ldr	r3, =0x7828
	mov	r7, r9
	ldr	r3, [r7, r3]
	ldr	r3, [r3, #0x14]
	mov	r4, #0
	cmp	r3, #0
	beq	.Le07ae
	ldr	r5, =0x7828
	mov	r6, #0x24
	add	r5, r9
.Le0782:
	ldr	r3, [r5]
	ldrsh	r0, [r3, r6]
	mov	r3, #6
	str	r3, [sp]
	mov	r2, #5
	mov	r3, r4
	mov	r1, #7
	str	r4, [sp, #8]
	bl	Func_80d6888
	ldr	r3, [r5]
	mov	r1, #6
	ldrsh	r0, [r3, r6]
	bl	_SetBattleActorKnockback
	ldr	r3, [r5]
	ldr	r4, [sp, #8]
	ldr	r3, [r3, #0x14]
	add	r4, #1
	add	r6, #2
	cmp	r4, r3
	bne	.Le0782
.Le07ae:
	mov	r4, r8
	ldr	r3, [r4, #4]
	sub	r3, #0xc
	str	r3, [r4, #4]
.Le07b6:
	ldr	r1, [sp, #0xc]
	mov	r2, #0xe0
	mov	r3, #1
	lsl	r2, #2
	add	r10, r3
	mov	r7, #4
	mov	r0, #0x1c
	add	r1, r2
	mov	r4, r10
	add	r11, r7
	add	r8, r0
	str	r1, [sp, #0xc]
	cmp	r4, #0xa
	beq	.Le07d4
	b	.Le06c2
.Le07d4:
	mov	r7, #0
	ldr	r5, =gBuffer
	ldr	r6, =Data_ede48
	mov	r10, r7
.Le07dc:
	mov	r1, #1
	ldr	r0, [r5, #0x18]
	neg	r1, r1
	cmp	r0, r1
	beq	.Le082a
	cmp	r0, #0
	bge	.Le07ec
	add	r0, #0xf
.Le07ec:
	asr	r0, #4
	add	r0, #2
	lsl	r4, r0, #1
	sub	r3, r4, #2
	ldrh	r1, [r6, r3]
	ldr	r2, [sp, #0x14]
	add	r1, r2, r1
	mov	r3, #2
	ldrsh	r2, [r5, r3]
	lsr	r3, r0, #31
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
	mov	r2, #0x80
	mov	r0, r5
	mov	r1, #0x3e
	lsl	r2, #6
	bl	Func_80e3908
	ldr	r3, [r5, #0x18]
	sub	r3, #1
	str	r3, [r5, #0x18]
.Le082a:
	mov	r7, #1
	mov	r0, #0x80
	add	r10, r7
	lsl	r0, #2
	add	r5, #0x1c
	cmp	r10, r0
	bne	.Le07dc
	mov	r1, #4
	mov	r0, #4
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r9
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	ldr	r2, =0xfffff800
	ldr	r1, [sp, #0x10]
	ldr	r3, [sp, #0x20]
	add	r1, r2
	add	r3, #1
	str	r1, [sp, #0x10]
	str	r3, [sp, #0x20]
	cmp	r3, #0x60
	beq	.Le0866
	b	.Le0666
.Le0866:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x28
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Venus

@ Sub_e08c0
@ Battle animation routine, 406 instructions.
@ State: iwram_1eec, ewram_10000.
@ Calls out to: _Func_b8228, _Func_bd7dc, _Func_f9080.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Mars  @ 0x080e08c0
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
	sub	sp, #0x20
	str	r3, [sp, #0x14]
	ldr	r3, =0x7828
	mov	r10, r1
	ldr	r2, [r2, #8]
	add	r3, r10
	str	r2, [sp, #0x10]
	str	r0, [r3]
	mov	r0, #0
	bl	AnimStart
	mov	r2, sp
	add	r2, #0x18
	mov	r1, r2
	mov	r0, #0
	str	r2, [sp, #0xc]
	bl	BuildDraw2DFuncs
	ldr	r0, =_FILE_73
	ldr	r1, [sp, #0x10]
	mov	r2, #0
	mov	r3, #0
	bl	LoadVFXFile
	ldr	r0, =_FILE_8e
	mov	r1, r10
	mov	r2, #1
	mov	r3, #0
	bl	LoadVFXFile
	mov	r1, #0xc8
	lsl	r1, #2
	ldr	r0, =_FILE_b7
	add	r1, r10
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	mov	r2, #0xef
	lsl	r2, #7
	add	r2, r10
	mov	r3, #2
	str	r3, [r2]
	ldr	r2, =0x7784
	mov	r3, #0x4b
	add	r2, r10
	mov	r1, #0x90
	str	r3, [r2]
	ldr	r0, =Task_BlitAnim
	lsl	r1, #3
	bl	StartTask
	mov	r6, #0xe1
	mov	r3, #0
	lsl	r6, #7
	str	r3, [sp, #8]
	mov	r7, #0
	add	r6, r10
.Le094a:
	lsl	r5, r7, #11
	mov	r0, r5
	bl	sin
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #3
	asr	r3, #16
	str	r3, [r6]
	mov	r0, r5
	bl	cos
	lsl	r0, #2
	asr	r0, #16
	mov	r3, #1
	add	r0, #0x34
	and	r3, r7
	str	r0, [r6, #4]
	cmp	r3, #0
	beq	.Le097a
	ldr	r2, [r6]
	mov	r3, #0x20
	sub	r3, r2
	b	.Le097e
.Le097a:
	ldr	r3, [r6]
	add	r3, #0x20
.Le097e:
	str	r3, [r6]
	lsl	r3, r7, #1
	neg	r3, r3
	str	r3, [r6, #0x18]
	ldr	r1, =0xffff
	ldr	r2, [sp, #8]
	ldr	r3, =gBuffer
	mov	r4, #0
	mov	r0, #0x7f
	mov	r8, r4
	mov	r11, r0
	mov	r9, r1
	add	r5, r2, r3
.Le0998:
	bl	Random
	ldr	r2, [r6]
	mov	r3, #0xf
	and	r3, r0
	add	r3, r2
	sub	r3, #8
	lsl	r3, #16
	str	r3, [r5]
	bl	Random
	mov	r3, #7
	and	r3, r0
	add	r3, #0x60
	lsl	r3, #16
	str	r3, [r5, #4]
	bl	Random
	mov	r4, r11
	and	r0, r4
	sub	r0, #0x40
	lsl	r0, #11
	str	r0, [r5, #0xc]
	bl	Random
	mov	r1, r11
	and	r0, r1
	sub	r0, #0x40
	lsl	r0, #10
	str	r0, [r5, #0x10]
	bl	Random
	mov	r2, r9
	and	r0, r2
	str	r0, [r5, #8]
	bl	Random
	mov	r3, r9
	mov	r4, #1
	and	r0, r3
	add	r8, r4
	str	r0, [r5, #0x14]
	mov	r0, r8
	add	r5, #0x1c
	cmp	r0, #0x10
	bne	.Le0998
	ldr	r1, [sp, #8]
	mov	r2, #0xe0
	lsl	r2, #1
	add	r1, r2
	add	r7, #1
	add	r6, #0x1c
	str	r1, [sp, #8]
	cmp	r7, #9
	bne	.Le094a
	mov	r0, #0x88
	bl	_PlaySound
	mov	r4, #0xac
	mov	r3, #0
	neg	r4, r4
	mov	r8, r3
	mov	r9, r4
.Le0a16:
	mov	r0, r8
	cmp	r0, #0x38
	bne	.Le0a22
	mov	r0, #0x85
	bl	_Func_80bd7dc
.Le0a22:
	mov	r1, r8
	cmp	r1, #0x17
	bgt	.Le0a56
	mov	r3, r8
	cmp	r1, #0
	bge	.Le0a30
	add	r3, #3
.Le0a30:
	asr	r3, #2
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #3
	add	r1, r3
	lsl	r1, #6
	mov	r2, #0xc8
	mov	r3, #0x28
	lsl	r2, #2
	add	r1, r10
	add	r1, r2
	str	r3, [sp]
	str	r3, [sp, #4]
	ldr	r4, [sp, #0x18]
	ldr	r0, [sp, #0x14]
	mov	r2, #0x28
	mov	r3, #0x14
	bl	_call_via_r4
.Le0a56:
	mov	r3, r8
	cmp	r3, #0x14
	bne	.Le0a70
	ldr	r0, =_FILE_8e
	bl	GetFile
	mov	r1, r0
	mov	r0, #0xa0
	ldr	r3, =Func_8001af8
	lsl	r0, #19
	mov	r2, #0x80
	bl	_call_via_r3
.Le0a70:
	mov	r3, r8
	sub	r3, #0x14
	cmp	r3, #0xb
	bhi	.Le0ab2
	mov	r4, r8
	cmp	r4, #0x17
	ble	.Le0a9c
	lsl	r3, r4, #2
	mov	r2, #0x92
	sub	r2, r3
	mov	r3, #0x14
	str	r3, [sp]
	mov	r3, #0x28
	str	r3, [sp, #4]
	ldr	r0, [sp, #0xc]
	mov	r1, r10
	ldr	r4, [r0, #4]
	mov	r3, r9
	ldr	r0, [sp, #0x14]
	bl	_call_via_r4
	b	.Le0ab2
.Le0a9c:
	mov	r3, #0x14
	str	r3, [sp]
	mov	r3, #0x28
	str	r3, [sp, #4]
	ldr	r4, [sp, #0x18]
	ldr	r0, [sp, #0x14]
	mov	r1, r10
	mov	r2, #0x32
	mov	r3, #0x14
	bl	_call_via_r4
.Le0ab2:
	mov	r1, r8
	cmp	r1, #0x20
	bne	.Le0ad2
	mov	r0, #0x91
	bl	_PlaySound
	ldr	r2, =0x77a8
	mov	r3, #8
	add	r2, r10
	str	r3, [r2]
	ldr	r0, =_FILE_b4
	mov	r1, r10
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
.Le0ad2:
	mov	r2, r8
	cmp	r2, #0x1f
	ble	.Le0b28
	mov	r6, #0xe1
	lsl	r6, #7
	mov	r7, #0
	add	r6, r10
.Le0ae0:
	ldr	r3, [r6, #0x18]
	cmp	r3, #0x2f
	bhi	.Le0b1c
	mov	r5, r3
	cmp	r3, #0
	bge	.Le0aee
	add	r5, r3, #7
.Le0aee:
	asr	r5, #3
	ldr	r2, =Data_edeb2
	lsl	r3, r5, #1
	ldrh	r1, [r2, r3]
	ldr	r3, =Data_ede9f
	ldrb	r4, [r3, r5]
	ldr	r2, [r6]
	lsr	r3, r4, #1
	sub	r2, r3
	ldr	r3, =Data_edeab
	ldrb	r0, [r3, r5]
	ldr	r3, [r6, #4]
	str	r4, [sp]
	add	r3, r0
	ldr	r0, =Data_edea5
	ldrb	r0, [r0, r5]
	add	r1, r10
	str	r0, [sp, #4]
	ldr	r4, [sp, #0x18]
	ldr	r0, [sp, #0x14]
	bl	_call_via_r4
	ldr	r3, [r6, #0x18]
.Le0b1c:
	add	r3, #1
	add	r7, #1
	str	r3, [r6, #0x18]
	add	r6, #0x1c
	cmp	r7, #9
	bne	.Le0ae0
.Le0b28:
	ldr	r6, =gBuffer
	mov	r7, #0
.Le0b2c:
	mov	r3, r7
	cmp	r7, #0
	bge	.Le0b34
	add	r3, #0xf
.Le0b34:
	asr	r3, #4
	lsl	r3, #1
	add	r3, #0x28
	cmp	r8, r3
	blt	.Le0b98
	ldr	r0, [r6, #8]
	bl	sin
	mov	r5, #1
	mov	r3, #2
	ldrsh	r2, [r6, r3]
	and	r5, r7
	lsl	r0, #2
	add	r5, #3
	asr	r0, #16
	add	r2, r0
	ldr	r1, =Data_ede48
	lsl	r0, r5, #1
	sub	r3, r0, #2
	ldrh	r1, [r1, r3]
	ldr	r4, [sp, #0x10]
	lsr	r3, r5, #1
	add	r1, r4, r1
	sub	r2, r3
	mov	r4, #6
	ldrsh	r3, [r6, r4]
	str	r0, [sp, #4]
	str	r5, [sp]
	ldr	r0, [sp, #0xc]
	sub	r3, r5
	ldr	r4, [r0, #4]
	ldr	r0, [sp, #0x14]
	bl	_call_via_r4
	mov	r1, #0x40
	ldr	r2, =0xffffe000
	mov	r0, r6
	bl	Func_80e3908
	ldr	r2, [r6, #8]
	mov	r1, #0x80
	lsl	r1, #4
	ldr	r4, =0xffff
	add	r3, r2, r1
	str	r3, [r6, #8]
	cmp	r3, r4
	ble	.Le0b98
	ldr	r0, =0xffff0801
	add	r3, r2, r0
	str	r3, [r6, #8]
.Le0b98:
	add	r7, #1
	add	r6, #0x1c
	cmp	r7, #0x90
	bne	.Le0b2c
	mov	r1, r8
	cmp	r1, #0x26
	bne	.Le0be2
	ldr	r3, =0x7828
	mov	r2, r10
	ldr	r3, [r2, r3]
	ldr	r3, [r3, #0x14]
	mov	r7, #0
	cmp	r3, #0
	beq	.Le0be2
	ldr	r5, =0x7828
	mov	r6, #0x24
	add	r5, r10
.Le0bba:
	ldr	r3, [r5]
	ldrsh	r0, [r3, r6]
	mov	r3, #0x10
	str	r3, [sp]
	mov	r1, #7
	mov	r3, r7
	mov	r2, #5
	bl	Func_80d6888
	ldr	r3, [r5]
	ldrsh	r0, [r3, r6]
	mov	r1, #6
	bl	_SetBattleActorKnockback
	ldr	r3, [r5]
	ldr	r3, [r3, #0x14]
	add	r7, #1
	add	r6, #2
	cmp	r7, r3
	bne	.Le0bba
.Le0be2:
	mov	r0, #8
	mov	r1, #8
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r10
	str	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	mov	r3, #1
	add	r8, r3
	mov	r2, #8
	mov	r4, r8
	add	r9, r2
	cmp	r4, #0x70
	beq	.Le0c0c
	b	.Le0a16
.Le0c0c:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x20
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Mars

@ Sub_e0c84
@ Battle animation routine, 398 instructions.
@ State: iwram_1eec, ewram_10000.
@ Calls out to: _Func_b8228, _Func_bd7dc.
@ Touches: REG_BLDALPHA.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Hail  @ 0x080e0c84
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001eec
	mov	r6, r0
	ldmia	r3!, {r0}
	ldr	r5, =0x7828
	mov	r11, r0
	ldr	r3, [r3]
	sub	sp, #0x30
	add	r5, r11
	str	r3, [sp, #0x10]
	mov	r0, #0
	str	r6, [r5]
	bl	AnimStart
	ldr	r3, [r5]
	ldr	r2, [r3, #4]
	add	r3, sp, #0x20
	str	r3, [sp]
	add	r3, sp, #0x1c
	str	r3, [sp, #4]
	mov	r0, r6
	mov	r1, #1
	mov	r3, #2
	bl	Anim_Djinni
	ldr	r3, [r5]
	mov	r1, sp
	ldr	r0, [r3, #4]
	add	r1, #0x14
	str	r1, [sp, #0xc]
	bl	BuildDraw2DFuncs
	ldr	r0, =_FILE_6e
	mov	r1, r11
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
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
	ldr	r3, [r5]
	add	r5, sp, #0x24
	mov	r2, #0x24
	ldrsh	r0, [r3, r2]
	mov	r1, r5
	bl	GetBattleActorPos3
	ldr	r4, =gBuffer
	mov	r3, #0xf
	mov	r0, #0x7f
	mov	r7, #0
	mov	r9, r3
	mov	r8, r4
	mov	r10, r0
.Le0d14:
	bl	Random
	ldr	r6, =0x7fff
	mov	r1, #0x80
	lsl	r1, #7
	and	r6, r0
	add	r6, r1
	bl	Random
	ldr	r5, =0x1ff
	and	r5, r0
	bl	Random
	ldr	r3, [sp, #0x24]
	lsr	r2, r3, #31
	add	r3, r2
	mov	r2, r9
	and	r0, r2
	asr	r3, #1
	add	r3, r0
	sub	r3, #8
	mov	r4, r8
	lsl	r3, #16
	str	r3, [r4]
	ldr	r3, [sp, #0x28]
	add	r3, #8
	lsl	r3, #16
	str	r3, [r4, #4]
	mov	r0, r6
	bl	sin
	add	r5, #0x80
	mov	r3, r5
	mul	r3, r0
	mov	r0, r8
	asr	r3, #9
	str	r3, [r0, #0xc]
	mov	r0, r6
	bl	cos
	mov	r3, r5
	mul	r3, r0
	mov	r1, r8
	asr	r3, #6
	str	r3, [r1, #0x10]
	bl	Random
	mov	r2, r10
	mov	r3, r8
	and	r0, r2
	str	r0, [r3, #8]
	bl	Random
	mov	r4, r10
	and	r0, r4
	mov	r1, r8
	str	r0, [r1, #0x14]
	bl	Random
	mov	r2, r9
	and	r0, r2
	mov	r3, r8
	add	r0, #0x20
	mov	r4, #0x1c
	add	r7, #1
	str	r0, [r3, #0x18]
	add	r8, r4
	cmp	r7, #0x40
	bne	.Le0d14
	ldr	r1, =0x7828
	add	r1, r11
	mov	r0, #0
	str	r1, [sp, #8]
	mov	r10, r0
.Le0da8:
	mov	r2, r10
	cmp	r2, #0x2f
	ble	.Le0dbc
	ldr	r2, .Le0de4	@ 0x40
	mov	r4, r10
	ldr	r1, .Le0de8	@ 0x1000
	ldr	r3, =REG_BLDALPHA
	sub	r2, r4
	orr	r2, r1
	strh	r2, [r3]
.Le0dbc:
	mov	r0, r10
	cmp	r0, #1
	bne	.Le0e1c
	mov	r1, #0x80
	lsl	r1, #3
	ldr	r0, =_FILE_b8
	add	r1, r11
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	ldr	r1, =0x65c0
	ldr	r0, =_FILE_92
	add	r1, r11
	mov	r2, #1
	mov	r3, #0
	bl	LoadVFXFile
	b	.Le0e1c

	.align	2, 0
.Le0de4:
	.word	0x40
.Le0de8:
	.word	0x1000
	.pool

.Le0e1c:
	ldr	r1, [sp, #8]
	ldr	r3, [r1]
	ldr	r3, [r3, #0x1c]
	cmp	r3, #1
	bne	.Le0e9e
	mov	r2, r10
	lsl	r5, r2, #11
	mov	r0, r5
	bl	sin
	ldr	r3, [sp, #0x20]
	neg	r0, r0
	lsr	r2, r3, #31
	add	r3, r2
	lsl	r0, #2
	asr	r3, #1
	asr	r0, #16
	add	r0, r3
	sub	r0, #0xa
	mov	r9, r0
	mov	r0, r5
	bl	cos
	ldr	r3, [sp, #0x1c]
	lsl	r0, #1
	asr	r0, #16
	add	r0, r3
	mov	r5, r0
	mov	r3, r10
	sub	r5, #0x16
	cmp	r3, #0x45
	ble	.Le0e64
	lsl	r3, #1
	sub	r3, r5, r3
	mov	r5, r3
	add	r5, #0x8a
.Le0e64:
	mov	r4, #0x14
	ldr	r6, =0x65c0
	mov	r7, #0x28
	ldr	r0, [sp, #0xc]
	str	r4, [sp]
	str	r7, [sp, #4]
	add	r6, r11
	mov	r1, r6
	mov	r8, r4
	mov	r2, r9
	ldr	r4, [r0, #4]
	mov	r3, r5
	ldr	r0, [sp, #0x10]
	bl	_call_via_r4
	mov	r1, r10
	cmp	r1, #3
	bgt	.Le0e9e
	mov	r2, r8
	str	r2, [sp]
	ldr	r3, [sp, #0xc]
	str	r7, [sp, #4]
	ldr	r0, [sp, #0x10]
	ldr	r4, [r3, #4]
	mov	r1, r6
	mov	r2, r9
	mov	r3, r5
	bl	_call_via_r4
.Le0e9e:
	ldr	r4, =gBuffer
	mov	r7, #0
	mov	r8, r4
.Le0ea4:
	mov	r3, r7
	cmp	r7, #0
	bge	.Le0eac
	add	r3, r7, #3
.Le0eac:
	asr	r3, #2
	add	r3, #4
	cmp	r10, r3
	blt	.Le0f10
	mov	r0, r8
	ldr	r4, [r0, #8]
	cmp	r4, #0
	bge	.Le0ebe
	add	r4, #0x7f
.Le0ebe:
	mov	r3, #3
	asr	r4, #7
	and	r4, r3
	ldr	r2, =.Leec68
	lsl	r3, r4, #1
	ldrh	r1, [r2, r3]
	mov	r2, #0x80
	lsl	r2, #3
	mov	r0, r8
	add	r1, r11
	add	r1, r2
	mov	r3, #2
	ldrsh	r2, [r0, r3]
	ldr	r3, =.Leec5f
	ldrb	r6, [r3, r4]
	lsr	r3, r6, #1
	sub	r2, r3
	mov	r3, #6
	ldrsh	r0, [r0, r3]
	mov	r12, r0
	ldr	r0, =.Leec63
	ldrb	r4, [r0, r4]
	mov	r3, r12
	lsr	r0, r4, #1
	mov	r5, #1
	sub	r3, r0
	str	r6, [sp]
	ldr	r0, [sp, #0xc]
	str	r4, [sp, #4]
	and	r5, r7
	lsl	r5, #2
	ldr	r4, [r5, r0]
	ldr	r0, [sp, #0x10]
	bl	_call_via_r4
	mov	r2, #0x80
	mov	r0, r8
	mov	r1, #0x3f
	lsl	r2, #5
	bl	Func_80e38b8
.Le0f10:
	mov	r1, #0x1c
	add	r7, #1
	add	r8, r1
	cmp	r7, #0x40
	bne	.Le0ea4
	mov	r2, r10
	cmp	r2, #8
	bne	.Le0f50
	ldr	r3, =0x77a8
	add	r3, r11
	str	r2, [r3]
	mov	r0, #0x86
	bl	_Func_80bd7dc
	ldr	r4, [sp, #8]
	ldr	r3, [r4]
	mov	r1, #0x24
	ldrsh	r0, [r3, r1]
	mov	r3, #0x10
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #5
	mov	r3, #0
	bl	Func_80d6888
	ldr	r2, [sp, #8]
	ldr	r3, [r2]
	mov	r1, #3
	mov	r4, #0x24
	ldrsh	r0, [r3, r4]
	bl	_SetBattleActorKnockback
.Le0f50:
	mov	r0, r10
	lsl	r5, r0, #2
	cmp	r5, #0x20
	ble	.Le0f5a
	mov	r5, #0x20
.Le0f5a:
	ldr	r1, [sp, #8]
	ldr	r3, [r1]
	ldr	r3, [r3, #4]
	cmp	r3, #0
	bne	.Le0f9a
	mov	r2, #0x78
	mov	r7, #0
	mov	r6, #0x20
	mov	r8, r2
.Le0f6c:
	mov	r2, r10
	lsl	r1, r7, #5
	cmp	r2, #0
	bge	.Le0f76
	add	r2, #3
.Le0f76:
	mov	r3, #0x1f
	asr	r2, #2
	and	r2, r3
	ldr	r3, [sp, #0x14]
	mov	r4, r8
	sub	r2, r1, r2
	mov	r12, r3
	str	r6, [sp]
	str	r6, [sp, #4]
	ldr	r0, [sp, #0x10]
	mov	r1, r11
	sub	r3, r4, r5
	add	r7, #1
	bl	_call_via_r12
	cmp	r7, #5
	bne	.Le0f6c
	b	.Le0fd0
.Le0f9a:
	mov	r0, #0x78
	mov	r7, #0
	mov	r6, #0x20
	mov	r8, r0
.Le0fa2:
	mov	r2, r10
	lsl	r1, r7, #5
	cmp	r2, #0
	bge	.Le0fac
	add	r2, #3
.Le0fac:
	mov	r3, #0x1f
	asr	r2, #2
	and	r2, r3
	add	r2, r1, r2
	ldr	r1, [sp, #0x14]
	mov	r4, r8
	mov	r12, r1
	sub	r2, #0x20
	str	r6, [sp]
	str	r6, [sp, #4]
	ldr	r0, [sp, #0x10]
	mov	r1, r11
	sub	r3, r4, r5
	add	r7, #1
	bl	_call_via_r12
	cmp	r7, #5
	bne	.Le0fa2
.Le0fd0:
	mov	r1, #8
	mov	r0, #4
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r11
	mov	r0, #1
	str	r3, [r2]
	bl	WaitFrames
	mov	r0, #1
	add	r10, r0
	mov	r1, r10
	cmp	r1, #0x40
	beq	.Le0ff6
	b	.Le0da8
.Le0ff6:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x30
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Hail

@ Sub_e1040
@ Battle animation routine, 561 instructions.
@ State: iwram_1eec, iwram_1f0c, iwram_1e50.
@ Calls out to: _Func_b7dd0, _Func_b8228, _Func_f9080.
@ Touches: REG_BLDALPHA.
@ Plays sound effects via _Func_f9080.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Anim_Ground  @ 0x080e1040
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r2, =iwram_3001eec
	mov	r3, r2
	mov	r6, r0
	ldmia	r3!, {r0}
	ldr	r3, [r3]
	sub	sp, #0x48
	str	r3, [sp, #0x30]
	sub	r2, #0x6c
	ldr	r5, =0x7828
	ldr	r2, [r2]
	mov	r9, r0
	add	r5, r9
	str	r2, [sp, #0x1c]
	mov	r0, #0
	str	r6, [r5]
	bl	AnimStart
	ldr	r3, [r5]
	ldr	r2, [r3, #4]
	add	r3, sp, #0x38
	str	r3, [sp]
	add	r3, sp, #0x34
	str	r3, [sp, #4]
	mov	r0, r6
	mov	r3, #2
	mov	r1, #0
	bl	Anim_Djinni
	ldr	r3, .Le10a8	@ 0x1010
	ldr	r2, =REG_BLDALPHA
	strh	r3, [r2]
	ldr	r3, [r5]
	ldr	r3, [r3, #4]
	cmp	r3, #1
	bne	.Le10b8
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x2e
	mov	r1, #7
	mov	r2, #7
	mov	r3, #7
	bl	BuildDraw2DFuncEx
	b	.Le10c8

	.align	2, 0
.Le10a8:
	.word	0x1010
	.pool

.Le10b8:
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x2e
	mov	r1, #7
	mov	r2, #7
	mov	r3, #3
	bl	BuildDraw2DFuncEx
.Le10c8:
	ldr	r3, =gPtrs
	add	r3, #0xb8
	ldr	r3, [r3]
	ldr	r0, =_FILE_a7
	str	r3, [sp, #0x20]
	mov	r1, r9
	mov	r2, #1
	mov	r3, #0
	bl	LoadVFXFile
	ldr	r1, =0x65c0
	ldr	r0, =_FILE_94
	add	r1, r9
	mov	r2, #1
	mov	r3, #1
	bl	LoadVFXFile
	mov	r2, #0xef
	lsl	r2, #7
	add	r2, r9
	mov	r3, #2
	str	r3, [r2]
	ldr	r2, =0x7784
	ldr	r5, =0x7828
	add	r2, r9
	mov	r3, #0x4b
	mov	r1, #0x90
	str	r3, [r2]
	lsl	r1, #3
	add	r5, r9
	ldr	r0, =Task_BlitAnim
	bl	StartTask
	ldr	r3, [r5]
	ldr	r0, [r3, #8]
	bl	_GetBattleActor
	ldr	r3, [r5]
	ldr	r6, [r0]
	mov	r1, #0x24
	ldrsh	r0, [r3, r1]
	bl	_GetBattleActor
	ldr	r0, [r0]
	mov	r2, #0
	mov	r5, #0xe1
	lsl	r5, #7
	str	r0, [sp, #0x18]
	str	r2, [sp, #0x2c]
	mov	r7, #0
	add	r5, r9
.Le112e:
	ldr	r3, [r6, #8]
	str	r3, [r5]
	mov	r3, #0x84
	lsl	r3, #15
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #8]
	asr	r3, r7, #5
	str	r3, [r5, #0xc]
	bl	Random
	mov	r3, #0x7f
	and	r3, r0
	sub	r3, #0x40
	lsl	r3, #16
	asr	r3, #6
	str	r3, [r5, #0x10]
	bl	Random
	mov	r3, #0xff
	and	r3, r0
	sub	r3, #0x7f
	lsl	r3, #16
	asr	r3, #5
	str	r3, [r5, #0x14]
	ldr	r3, [r5]
	cmp	r3, #0
	ble	.Le116c
	ldr	r3, [r5, #0xc]
	neg	r3, r3
	str	r3, [r5, #0xc]
.Le116c:
	mov	r3, #1
	str	r3, [r5, #0x18]
	ldr	r4, [sp, #0x2c]
	mov	r3, #0xa0
	lsl	r3, #15
	add	r4, #1
	add	r7, r3
	add	r5, #0x1c
	str	r4, [sp, #0x2c]
	cmp	r4, #8
	bne	.Le112e
	ldr	r0, [sp, #0x1c]
	mov	r5, #0
	add	r0, #0xc
	str	r5, [sp, #0x28]
	str	r0, [sp, #0x10]
.Le118c:
	ldr	r1, [sp, #0x28]
	cmp	r1, #0x10
	ble	.Le1198
	ldr	r0, =_FILE_a7
	bl	Func_80e46f0
.Le1198:
	ldr	r6, =0x7828
	add	r6, r9
	ldr	r3, [r6]
	ldr	r3, [r3, #0x1c]
	cmp	r3, #1
	bne	.Le1248
	ldr	r2, [sp, #0x28]
	lsl	r5, r2, #11
	mov	r0, r5
	bl	sin
	ldr	r3, [sp, #0x38]
	neg	r0, r0
	lsr	r2, r3, #31
	add	r3, r2
	lsl	r0, #2
	asr	r3, #1
	asr	r0, #16
	add	r0, r3
	mov	r7, r0
	mov	r0, r5
	bl	cos
	ldr	r3, [sp, #0x34]
	lsl	r0, #1
	asr	r0, #16
	add	r0, r3
	ldr	r3, [sp, #0x28]
	mov	r5, r0
	sub	r7, #0xa
	sub	r5, #0x16
	cmp	r3, #0x10
	ble	.Le11e2
	lsl	r3, #1
	sub	r3, r5, r3
	mov	r5, r3
	add	r5, #0x20
.Le11e2:
	ldr	r3, [r6]
	ldr	r3, [r3, #4]
	cmp	r3, #1
	bne	.Le11fc
	mov	r3, #3
	str	r3, [sp]
	mov	r0, #0x2f
	mov	r1, #7
	mov	r2, #7
	mov	r3, #7
	bl	BuildDraw2DFuncEx
	b	.Le120a
.Le11fc:
	mov	r3, #3
	mov	r0, #0x2f
	mov	r1, #7
	mov	r2, #7
	str	r3, [sp]
	bl	BuildDraw2DFuncEx
.Le120a:
	ldr	r4, [sp, #0x28]
	cmp	r4, #3
	bgt	.Le122a
	mov	r3, #0x14
	str	r3, [sp]
	mov	r3, #0x28
	str	r3, [sp, #4]
	ldr	r0, =iwram_3001f0c
	ldr	r1, =0x65c0
	ldr	r4, [r0]
	add	r1, r9
	ldr	r0, [sp, #0x30]
	mov	r2, r7
	mov	r3, r5
	bl	_call_via_r4
.Le122a:
	mov	r0, #0x2f
	bl	gfree
	mov	r3, #0x14
	ldr	r1, =0x65c0
	str	r3, [sp]
	mov	r3, #0x28
	str	r3, [sp, #4]
	ldr	r0, [sp, #0x30]
	add	r1, r9
	mov	r2, r7
	mov	r3, r5
	ldr	r4, [sp, #0x20]
	bl	_call_via_r4
.Le1248:
	ldr	r5, [sp, #0x28]
	mov	r3, #1
	and	r3, r5
	cmp	r3, #0
	bne	.Le1284
	mov	r0, #0
	mov	r5, #0xe8
	lsl	r5, #7
	str	r0, [sp, #0x2c]
	ldr	r6, =.Leec70
	add	r5, r9
.Le125e:
	bl	Random
	mov	r1, #6
	bl	__umodsi3
	add	r0, #3
	str	r0, [r5, #0xc]
	bl	Random
	mov	r3, #3
	and	r3, r0
	ldrb	r3, [r6, r3]
	str	r3, [r5, #0x10]
	ldr	r1, [sp, #0x2c]
	add	r1, #1
	add	r5, #0x1c
	str	r1, [sp, #0x2c]
	cmp	r1, #0x20
	bne	.Le125e
.Le1284:
	bl	InitMatrixStack
	ldr	r0, [sp, #0x1c]
	ldr	r1, [sp, #0x10]
	bl	MatrixSetLook
	mov	r6, #0xe1
	mov	r2, #0
	mov	r3, r9
	lsl	r6, #7
	str	r2, [sp, #0x2c]
	str	r2, [sp, #0xc]
	str	r3, [sp, #8]
	add	r6, r9
.Le12a0:
	ldr	r3, [r6, #0x18]
	cmp	r3, #1
	beq	.Le12a8
	b	.Le14da
.Le12a8:
	ldr	r4, [sp, #0xc]
	ldr	r5, [sp, #0x28]
	str	r4, [sp, #0x14]
	cmp	r5, r4
	bgt	.Le12b4
	b	.Le142a
.Le12b4:
	add	r5, sp, #0x3c
	mov	r1, r5
	mov	r0, r6
	bl	Func_80e3944
	ldr	r3, [r5]
	asr	r3, #1
	str	r3, [r5]
	sub	r3, #0xc
	mov	r10, r3
	ldr	r3, [r5, #4]
	sub	r3, #0x18
	mov	r8, r3
	mov	r3, #0x18
	str	r3, [sp]
	mov	r3, #0x30
	str	r3, [sp, #4]
	ldr	r5, [sp, #0x20]
	mov	r3, r8
	ldr	r0, [sp, #0x30]
	mov	r1, r9
	mov	r2, r10
	bl	_call_via_r5
	ldr	r0, [sp, #0x28]
	mov	r3, #3
	and	r3, r0
	cmp	r3, #1
	bgt	.Le1314
	ldr	r3, =.Leec86
	ldr	r4, =.Leeca1
	ldrh	r1, [r3, #2]
	ldr	r3, =.Leec98
	ldrb	r2, [r3, #1]
	ldrb	r3, [r4, #1]
	ldr	r4, =.Leec74
	ldrb	r0, [r4, #1]
	str	r0, [sp]
	ldr	r0, =.Leec7d
	ldrb	r0, [r0, #1]
	add	r1, r9
	str	r0, [sp, #4]
	add	r2, r10
	add	r3, r8
	ldr	r0, [sp, #0x30]
	bl	_call_via_r5
	b	.Le1338
.Le1314:
	ldr	r3, =.Leec86
	ldr	r0, =.Leeca1
	ldrh	r1, [r3, #4]
	ldr	r4, =.Leec74
	ldr	r3, =.Leec98
	ldrb	r2, [r3, #2]
	ldrb	r3, [r0, #2]
	ldrb	r0, [r4, #2]
	str	r0, [sp]
	ldr	r0, =.Leec7d
	ldrb	r0, [r0, #2]
	add	r1, r9
	str	r0, [sp, #4]
	add	r2, r10
	add	r3, r8
	ldr	r0, [sp, #0x30]
	bl	_call_via_r5
.Le1338:
	ldr	r0, [sp, #8]
	mov	r1, #0xe8
	mov	r5, #0
	lsl	r1, #7
	mov	r11, r5
	add	r7, r0, r1
.Le1344:
	mov	r2, #2
	ldr	r3, [r7, #0x10]
	mov	r1, #7
	str	r2, [sp]
	mov	r0, #0x2f
	mov	r2, #7
	bl	BuildDraw2DFuncEx
	ldr	r2, =iwram_3001f0c
	ldr	r1, [r7, #0x10]
	ldr	r2, [r2]
	mov	r3, #4
	and	r3, r1
	str	r2, [sp, #0x24]
	cmp	r3, #0
	beq	.Le1378
	ldr	r0, [r7, #0xc]
	ldr	r4, =.Leec74
	ldrb	r3, [r4, r0]
	mov	r5, r10
	sub	r3, r5, r3
	ldr	r5, =.Leec98
	ldrb	r2, [r5, r0]
	sub	r3, r2
	add	r3, #0x18
	b	.Le1382
.Le1378:
	ldr	r0, [r7, #0xc]
	ldr	r2, =.Leec98
	ldrb	r3, [r2, r0]
	ldr	r4, =.Leec74
	add	r3, r10
.Le1382:
	mov	r12, r3
	mov	r3, #8
	and	r3, r1
	cmp	r3, #0
	beq	.Le13d8
	ldr	r5, =.Leec7d
	ldrb	r3, [r5, r0]
	ldr	r5, =.Leeca1
	mov	r1, r8
	ldrb	r2, [r5, r0]
	sub	r3, r1, r3
	sub	r3, r2
	mov	r5, r3
	add	r5, #0x30
	b	.Le13e0

	.pool_aligned

.Le13d8:
	ldr	r1, =.Leeca1
	ldrb	r3, [r1, r0]
	mov	r2, r8
	add	r5, r2, r3
.Le13e0:
	ldr	r2, =.Leec86
	lsl	r3, r0, #1
	ldrh	r1, [r2, r3]
	ldrb	r3, [r4, r0]
	str	r3, [sp]
	ldr	r3, [r7, #0xc]
	ldr	r4, =.Leec7d
	ldrb	r3, [r4, r3]
	add	r1, r9
	str	r3, [sp, #4]
	ldr	r0, [sp, #0x30]
	mov	r3, r5
	mov	r2, r12
	ldr	r5, [sp, #0x24]
	bl	_call_via_r5
	mov	r0, #0x2f
	bl	gfree
	mov	r0, #1
	add	r11, r0
	mov	r1, r11
	add	r7, #0x1c
	cmp	r1, #4
	bne	.Le1344
	ldr	r3, [r6]
	ldr	r2, [r6, #0xc]
	add	r3, r2
	str	r3, [r6]
	ldr	r2, [r6, #0x10]
	ldr	r3, [r6, #4]
	add	r3, r2
	str	r3, [r6, #4]
	ldr	r2, [r6, #0x14]
	ldr	r3, [r6, #8]
	add	r3, r2
	str	r3, [r6, #8]
.Le142a:
	ldr	r3, [sp, #0xc]
	ldr	r2, [sp, #0x28]
	add	r3, #0x10
	cmp	r2, r3
	ble	.Le14da
	ldr	r4, [sp, #0x18]
	ldr	r2, [r6]
	ldr	r3, [r4, #8]
	sub	r3, r2
	ldr	r2, [r6, #0xc]
	asr	r3, #8
	add	r1, r2, r3
	ldr	r2, [r6, #4]
	mov	r3, #0xa0
	lsl	r3, #13
	sub	r3, r2
	ldr	r2, [r6, #0x10]
	asr	r3, #8
	add	r0, r2, r3
	str	r1, [r6, #0xc]
	str	r0, [r6, #0x10]
	ldr	r2, [r6, #8]
	ldr	r3, [r4, #0x10]
	sub	r3, r2
	ldr	r2, [r6, #0x14]
	asr	r3, #8
	add	r4, r2, r3
	str	r4, [r6, #0x14]
	ldr	r3, [sp, #0x14]
	ldr	r5, [sp, #0x28]
	add	r3, #0x55
	cmp	r5, r3
	bge	.Le149c
	lsl	r3, r1, #4
	sub	r3, r1
	lsl	r2, r3, #2
	cmp	r2, #0
	bge	.Le1478
	add	r2, #0x3f
.Le1478:
	asr	r3, r2, #6
	str	r3, [r6, #0xc]
	lsl	r3, r0, #4
	sub	r3, r0
	lsl	r2, r3, #2
	cmp	r2, #0
	bge	.Le1488
	add	r2, #0x3f
.Le1488:
	asr	r3, r2, #6
	str	r3, [r6, #0x10]
	lsl	r3, r4, #4
	sub	r3, r4
	lsl	r2, r3, #2
	cmp	r2, #0
	bge	.Le1498
	add	r2, #0x3f
.Le1498:
	asr	r3, r2, #6
	str	r3, [r6, #0x14]
.Le149c:
	ldr	r3, [r6, #4]
	ldr	r0, =0x13ffff
	cmp	r3, r0
	bgt	.Le14da
	ldr	r2, =0x77a8
	mov	r3, #8
	add	r2, r9
	str	r3, [r2]
	mov	r3, #0
	str	r3, [r6, #0x18]
	mov	r0, #0x86
	bl	_PlaySound
	ldr	r5, =0x7828
	add	r5, r9
	ldr	r3, [r5]
	mov	r1, #0x24
	ldrsh	r0, [r3, r1]
	mov	r3, #4
	str	r3, [sp]
	mov	r1, #7
	mov	r2, #5
	mov	r3, #0
	bl	Func_80d6888
	ldr	r3, [r5]
	mov	r1, #4
	mov	r2, #0x24
	ldrsh	r0, [r3, r2]
	bl	_SetBattleActorKnockback
.Le14da:
	ldr	r3, [sp, #0xc]
	ldr	r4, [sp, #8]
	ldr	r5, [sp, #0x2c]
	add	r3, #2
	add	r4, #0x70
	add	r5, #1
	str	r3, [sp, #0xc]
	add	r6, #0x1c
	str	r4, [sp, #8]
	str	r5, [sp, #0x2c]
	cmp	r5, #6
	beq	.Le14f4
	b	.Le12a0
.Le14f4:
	mov	r0, #0x10
	mov	r1, #0x10
	bl	UpdateScreenShake
	bl	Func_80cd52c
	ldr	r2, =0x7824
	mov	r3, #1
	add	r2, r9
	mov	r0, #1
	str	r3, [r2]
	bl	WaitFrames
	ldr	r0, [sp, #0x28]
	add	r0, #1
	str	r0, [sp, #0x28]
	cmp	r0, #0x60
	beq	.Le151a
	b	.Le118c
.Le151a:
	ldr	r0, =Task_BlitAnim
	bl	StopTask
	mov	r0, #0x2e
	bl	gfree
	bl	AnimEnd
	add	sp, #0x48
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Anim_Ground

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

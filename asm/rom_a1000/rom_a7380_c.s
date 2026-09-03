	.include "macros.inc"
	.include "gba.inc"

@ OpenStatusBody
@ r0 = page. Opens the (0, 5, 0x1E, 0xF) body window into state+0x24 if it is
@ not already up, populates its menu entries with _Func_1ec6c (storing the node
@ at state+0x17C with sort order 0xF0), creates the list sprites when
@ state+0x220 is 3, and hands off to Func_a8604 -- with flag 0x100 on a fresh
@ window and 0 on a reused one, which is how a redraw avoids re-creating what is
@ already there.
.thumb_func_start Func_80a8088  @ 0x080a8088
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001f2c
	ldr	r6, [r3]
	ldr	r5, [r6, #0x24]
	mov	r7, r0
	sub	sp, #8
	mov	r0, #0
	cmp	r5, #0
	bne	.La80b4
	mov	r3, #0xf
	mov	r5, r6
	add	r5, #0x24
	str	r3, [sp]
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r0, r5
	mov	r1, #0
	mov	r2, #5
	mov	r3, #0x1e
	bl	Func_80a10d0
	ldr	r5, [r5]
.La80b4:
	cmp	r0, #0
	beq	.La80fc
	mov	r3, #0
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r2, #0
	mov	r3, r5
	mov	r0, r7
	mov	r1, #0
	bl	_Func_801ec6c
	mov	r2, #0xbe
	lsl	r2, #1
	add	r3, r6, r2
	str	r0, [r3]
	add	r2, #0xa4
	mov	r3, #0xf0
	strb	r3, [r0, #0xf]
	add	r3, r6, r2
	ldrh	r3, [r3]
	cmp	r3, #3
	bne	.La80e8
	mov	r0, r6
	mov	r1, r5
	bl	Func_80a33d4
.La80e8:
	mov	r0, r5
	bl	Func_80a9cf8
	mov	r2, #0x80
	lsl	r2, #1
	mov	r0, r5
	mov	r1, r7
	bl	Func_80a8604
	b	.La8106
.La80fc:
	mov	r0, r5
	mov	r1, r7
	mov	r2, #0
	bl	Func_80a8604
.La8106:
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a8088

@ RunStatusPage1
@ Takes no arguments. The main stats page: Func_a8604 draws the numbers,
@ Func_a847c tints the selected row, Func_a8508 prints the elemental affinities
@ and Func_a8578 the class or level-up line. Registers a per-frame task, loops on
@ the d-pad with Left and Right changing character and A opening the sub-page,
@ and prints label 0xB06. Watches save bits 0x150 and 0x242.
@ 410 lines; traced structurally.
.thumb_func_start Func_80a8114  @ 0x080a8114
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	sub	sp, #0x28
	mov	r0, #0
	ldr	r7, [r3]
	mov	r10, r0
	str	r0, [sp, #0x1c]
	mov	r8, r0
	sub	r0, #1
	bl	_GetNumDjinn
	neg	r3, r0
	orr	r3, r0
	lsr	r3, #31
	str	r3, [sp, #0xc]
	mov	r3, #5
	str	r3, [sp]
	mov	r0, r7
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0x1e
	add	r0, #0x2c
	bl	Func_80a10d0
	ldr	r0, =Func_80a19a0
	bl	StopTask
	ldr	r0, =0x242
	ldr	r1, .La8184	@ 0x68
	mov	r2, #3
	add	r3, r7, r0
.La8162:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La8162
	mov	r1, #0
	mov	r0, #0xa
	str	r1, [sp, #0x10]
	neg	r0, r0
	mov	r1, #0x58
	bl	Func_80a1ac0
	ldr	r2, =0x21a
	add	r2, r7, r2
	str	r2, [sp, #8]
	b	.La8406

	.align	2, 0
.La8184:
	.word	0x68
	.pool

.La8198:
	ldr	r3, [sp, #8]
	ldrb	r0, [r3]
	bl	_GetUnit
	ldr	r2, [sp, #8]
	ldr	r0, [r7, #0x24]
	ldrb	r1, [r2]
	mov	r2, #1
	bl	Func_80a8604
	mov	r3, #0x20
	ldr	r0, [sp, #8]
	add	r3, sp
	mov	r11, r3
	ldrb	r2, [r0]
	mov	r1, #1
	mov	r0, r11
	bl	Func_80a8b10
	lsl	r0, #24
	mov	r1, #0
	lsr	r2, r0, #24
	str	r1, [sp, #0x14]
	str	r2, [sp, #0x18]
	cmp	r0, #0
	bne	.La81f2
	mov	r3, #1
	str	r3, [sp, #0x18]
	b	.La81f6
.La81d2:
	mov	r0, #0x70
	bl	_PlaySound
	mov	r0, #1
	str	r0, [sp, #0x10]
	str	r0, [sp, #0x1c]
	b	.La8406
.La81e0:
	mov	r0, #0x71
	bl	_PlaySound
	mov	r2, #1
	mov	r1, #1
	neg	r2, r2
	str	r1, [sp, #0x10]
	str	r2, [sp, #0x1c]
	b	.La8406
.La81f2:
	mov	r3, #1
	str	r3, [sp, #0x14]
.La81f6:
	mov	r0, #1
	mov	r9, r0
	b	.La83f8
.La81fc:
	mov	r1, r9
	cmp	r1, #0
	beq	.La82ba
	mov	r2, #0
	ldr	r0, [sp, #0x18]
	mov	r9, r2
	mov	r2, r10
	add	r2, #2
	lsl	r3, r0, #24
	asr	r1, r3, #24
	lsr	r3, r2, #31
	add	r3, r2, r3
	asr	r3, #1
	lsl	r3, #1
	sub	r2, r3
	mov	r10, r2
	cmp	r2, #0
	bne	.La8256
	mov	r2, r8
	add	r0, r2, r1
	bl	__modsi3
	mov	r8, r0
	ldr	r0, [r7, #0x2c]
	bl	_Func_8016498
	ldr	r3, [sp, #0x14]
	cmp	r3, #0
	bne	.La8286
	ldr	r5, =0xb06
	mov	r6, #0x18
	neg	r6, r6
	ldr	r1, [r7, #0x24]
	mov	r0, r5
	mov	r2, #0x50
	mov	r3, r6
	bl	_Func_801e7c0
	ldr	r1, [r7, #0x24]
	add	r0, r5, #1
	mov	r2, #0
	mov	r3, r6
	bl	_Func_801e7c0
	b	.La8286
.La8256:
	ldr	r0, [r7, #0x2c]
	bl	_Func_8016498
	ldr	r0, [sp, #0xc]
	cmp	r0, #0
	beq	.La827a
	mov	r2, r8
	add	r2, #8
	mov	r3, r2
	cmp	r2, #0
	bge	.La8270
	mov	r3, r8
	add	r3, #0xf
.La8270:
	asr	r3, #3
	lsl	r3, #3
	sub	r2, r3
	mov	r8, r2
	b	.La8286
.La827a:
	mov	r0, r8
	add	r0, #7
	mov	r1, #7
	bl	__modsi3
	mov	r8, r0
.La8286:
	mov	r1, r8
	mov	r2, r11
	mov	r3, #0
	mov	r0, r10
	bl	Func_80a847c
	ldr	r0, [r7, #0x2c]
	bl	_Func_80164ac
	mov	r0, #1
	bl	WaitFrames
	mov	r1, r10
	cmp	r1, #0
	bne	.La82b0
	ldr	r0, [r7, #0x2c]
	mov	r1, r8
	mov	r2, r11
	bl	Func_80a8508
	b	.La82ba
.La82b0:
	ldr	r0, [r7, #0x2c]
	mov	r1, r8
	ldr	r2, [sp, #0xc]
	bl	Func_80a8578
.La82ba:
	ldr	r2, [r7, #0x14]
	mov	r3, #1
	strb	r3, [r2, #5]
	mov	r2, r10
	cmp	r2, #0
	bne	.La82d6
	mov	r3, r8
	lsl	r1, r3, #4
	mov	r0, #0xa
	add	r1, #0x58
	neg	r0, r0
	bl	Func_80a1a40
	b	.La82f4
.La82d6:
	mov	r0, r8
	cmp	r0, #3
	bgt	.La82e8
	lsl	r1, r0, #3
	add	r1, #0x30
	mov	r0, #0x18
	bl	Func_80a1a40
	b	.La82f4
.La82e8:
	mov	r2, r8
	lsl	r1, r2, #3
	add	r1, #0x50
	mov	r0, #0x30
	bl	Func_80a1a40
.La82f4:
	mov	r0, #1
	bl	WaitFrames
	ldr	r5, =gKeyRepeat
	ldr	r2, [r5]
	mov	r3, #0xf0
	and	r2, r3
	cmp	r2, #0
	beq	.La8312
	mov	r0, r10
	mov	r1, r8
	mov	r2, r11
	mov	r3, #1
	bl	Func_80a847c
.La8312:
	ldr	r1, =gKeyPress
	ldr	r2, [r1]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	beq	.La8320
	b	.La81d2
.La8320:
	ldr	r2, [r1]
	mov	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La832c
	b	.La81e0
.La832c:
	ldr	r2, [r5]
	mov	r3, #0x40
	and	r2, r3
	cmp	r2, #0
	beq	.La8346
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r0, #1
	mov	r3, #1
	neg	r0, r0
	mov	r9, r3
	add	r8, r0
.La8346:
	ldr	r2, [r5]
	mov	r3, #0x80
	and	r2, r3
	cmp	r2, #0
	beq	.La835c
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r1, #1
	mov	r9, r1
	add	r8, r1
.La835c:
	ldr	r2, [r5]
	mov	r3, #0x10
	and	r2, r3
	cmp	r2, #0
	beq	.La8372
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r2, #1
	mov	r9, r2
	add	r10, r2
.La8372:
	ldr	r2, [r5]
	mov	r3, #0x20
	and	r2, r3
	cmp	r2, #0
	beq	.La838c
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r0, #1
	mov	r3, #1
	neg	r0, r0
	mov	r9, r3
	add	r10, r0
.La838c:
	ldr	r3, [r5]
	mov	r6, #0x80
	lsl	r6, #1
	and	r3, r6
	cmp	r3, #0
	bne	.La83a4
	ldr	r2, [r5]
	mov	r3, #0x80
	lsl	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La83f8
.La83a4:
	mov	r0, #0x6f
	bl	_PlaySound
	ldr	r3, [r5]
	and	r3, r6
	mov	r0, #0x1c
	ldrsb	r0, [r7, r0]
	cmp	r3, #0
	beq	.La83ba
	add	r0, #1
	b	.La83bc
.La83ba:
	sub	r0, #1
.La83bc:
	ldr	r1, =0x219
	add	r3, r7, r1
	ldrb	r1, [r3]
	add	r0, r1
	bl	__modsi3
	mov	r3, #0x82
	lsl	r2, r0, #1
	lsl	r3, #2
	add	r2, r3
	ldrh	r3, [r7, r2]
	str	r3, [r7, #8]
	ldr	r1, [sp, #8]
	ldrh	r3, [r7, r2]
	strb	r3, [r1]
	strb	r0, [r7, #0x1c]
	mov	r0, r7
	ldrh	r1, [r7, r2]
	bl	Func_80a1804
	b	.La8406

	.pool_aligned

.La83f8:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	bne	.La8406
	b	.La81fc
.La8406:
	ldr	r2, [sp, #0x10]
	cmp	r2, #0
	bne	.La841a
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	bne	.La841a
	b	.La8198
.La841a:
	ldr	r0, [r7, #0x2c]
	bl	_Func_80164ac
	ldr	r0, [r7, #0x2c]
	bl	_Func_8016498
	mov	r3, #0x60
	ldr	r0, [r7, #0x24]
	mov	r2, #0x38
	str	r3, [sp]
	mov	r1, #0x40
	mov	r3, #0xe0
	bl	_Func_80164d4
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Func_80a19a0
	bl	StartTask
	ldr	r1, =0x242
	ldr	r3, .La844c	@ 0x80
	mov	r2, #3
	add	r0, r7, r1
	b	.La8458

	.align	2, 0
.La844c:
	.word	0x80
	.pool

.La8458:
	sub	r2, #1
	strh	r3, [r0]
	sub	r0, #2
	cmp	r2, #0
	bge	.La8458
	bl	Func_80a9d84
	ldr	r0, [sp, #0x1c]
	add	sp, #0x28
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a8114

@ TintStatusRow
@ r0 = layout, r1 = index, r2 = five affinity flags, r3 = 1 for the selected
@ row. Tints one row of the status page through Func_a2268 with palette 0x0E
@ when r3 is 1 and 0x0F otherwise.
@
@ Layout 0 walks the flag bytes and takes the row width from .Laf2fc, so only
@ the affinities the character actually has get a row. Layout 1 uses a fixed
@ geometry that changes at index 4 -- rows 0..3 sit at x 5 width 0x0D and rows
@ 4 upward at x 8 width 0x14.
.thumb_func_start Func_80a847c  @ 0x080a847c
	push	{r5, r6, r7, lr}
	mov	r12, r3
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	sub	sp, #8
	mov	r14, r3
	cmp	r0, #0
	bne	.La84c0
	lsl	r3, r1, #1
	add	r5, r3, #5
	ldrb	r3, [r2]
	mov	r7, #0
	mov	r6, #5
	mov	r4, #0
	mov	r0, #0
	cmp	r3, #0
	beq	.La84aa
	cmp	r1, #0
	bne	.La84a8
	ldr	r3, =.Laf2fc
	ldrb	r6, [r3]
	b	.La84d2
.La84a8:
	add	r4, #1
.La84aa:
	add	r0, #1
	cmp	r0, #4
	bgt	.La84d2
	ldrb	r3, [r2, r0]
	cmp	r3, #0
	beq	.La84aa
	cmp	r1, r4
	bne	.La84a8
	ldr	r3, =.Laf2fc
	ldrb	r6, [r3, r0]
	b	.La84d2
.La84c0:
	cmp	r1, #3
	bgt	.La84cc
	mov	r5, r1
	mov	r7, #5
	mov	r6, #0xd
	b	.La84d2
.La84cc:
	add	r5, r1, #4
	mov	r7, #8
	mov	r6, #0x14
.La84d2:
	mov	r1, #1
	mov	r2, r12
	mov	r3, r1
	eor	r3, r2
	neg	r2, r3
	orr	r2, r3
	lsr	r2, #31
	mov	r3, #0xf
	sub	r3, r2
	mov	r2, r14
	ldr	r0, [r2, #0x24]
	str	r1, [sp]
	str	r3, [sp, #4]
	mov	r1, r7
	mov	r2, r5
	mov	r3, r6
	bl	Func_80a2268
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a847c

@ DrawAffinityLabels
@ r0 = window, r1 = which, r2 = five flag bytes. Walks the flags and, for the
@ r1-th one that is set, prints the pair 0xBDC + i*2 and 0xBDD + i*2 -- a label
@ and its value. When no flag is set at all it prints 0xBDA instead.
.thumb_func_start Func_80a8508  @ 0x080a8508
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r8, r0
	mov	r9, r1
	mov	r10, r2
	mov	r7, #0
	mov	r6, #0
.La851c:
	mov	r2, r10
	ldrb	r3, [r2, r6]
	cmp	r3, #0
	beq	.La854c
	cmp	r9, r7
	bne	.La854a
	ldr	r3, =0xbdc
	lsl	r5, r6, #1
	add	r5, r3
	mov	r3, #1
	mov	r0, r5
	mov	r1, r8
	mov	r2, #0
	neg	r3, r3
	add	r5, #1
	bl	_DrawSmallText
	mov	r0, r5
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0xf
	bl	_DrawSmallText
.La854a:
	add	r7, #1
.La854c:
	add	r6, #1
	cmp	r6, #4
	ble	.La851c
	cmp	r7, #0
	bne	.La8562
	ldr	r0, =0xbda
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0
	bl	_DrawSmallText
.La8562:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a8508

@ DrawLevelLine
@ r0 = window, r1 = line, r2 = 0 to skip past line 3.
@ Line 1 is the experience-to-next-level readout: the character's level is the
@ byte at record+0x0F, and _Func_79008(id, level + 1) gives the threshold, from
@ which the current total at record+0x124 is subtracted and the difference
@ registered with _Func_19908.
@
@ A level of 0x63 -- NINETY-NINE, the cap -- switches to line 8 instead, which
@ is the "maximum" text. Whichever line is chosen renders as string 0xBE6 + line
@ into a 0x100 scratch and out through _Func_17aa4.
.thumb_func_start Func_80a8578  @ 0x080a8578
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	mov	r8, r0
	mov	r7, r1
	ldr	r3, [r3]
	cmp	r2, #0
	bne	.La8590
	cmp	r7, #3
	ble	.La8590
	add	r7, #1
.La8590:
	cmp	r7, #1
	bne	.La85c4
	ldr	r2, =0x21a
	add	r6, r3, r2
	ldrb	r0, [r6]
	bl	_GetUnit
	mov	r5, r0
	ldrb	r3, [r5, #0xf]
	cmp	r3, #0x63
	bne	.La85aa
	mov	r7, #8
	b	.La85c4
.La85aa:
	ldrb	r1, [r5, #0xf]
	ldrb	r0, [r6]
	add	r1, #1
	bl	_Func_8079008
	mov	r2, #0x92
	lsl	r2, #1
	add	r3, r5, r2
	ldr	r3, [r3]
	mov	r1, #5
	sub	r0, r3
	bl	_Func_8019908
.La85c4:
	mov	r0, #0x80
	lsl	r0, #1
	bl	Func_8004938
	mov	r5, r0
	ldr	r0, =0xbe6
	mov	r1, r5
	add	r0, r7, r0
	mov	r2, #0x80
	bl	_Func_801965c
	mov	r3, #1
	mov	r0, r5
	neg	r3, r3
	mov	r1, r8
	mov	r2, #0
	bl	_Func_8017aa4
	mov	r0, r5
	bl	free
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a8578

	.section .rodata
	.global .Laf2fc

.Laf2fc:
	.incrom 0xaf2fc, 0xaf304

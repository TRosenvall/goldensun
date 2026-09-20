	.include "macros.inc"
	.include "gba.inc"

@ RunSellFlow
@ r0.. = parameters. 200 lines. The sell counterpart to Func_b17e4. Traced
@ structurally.
.thumb_func_start Func_80b1a14  @ 0x080b1a14
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	mov	r0, #1
	mov	r2, #0
	sub	sp, #4
	ldr	r6, [r3]
	mov	r5, #2
	mov	r1, #9
	mov	r3, #4
	mov	r9, r0
	mov	r10, r2
	mov	r0, #0
	mov	r2, #0xc
	str	r5, [sp]
	bl	_CreateUIBox
	str	r0, [r6, #0xc]
	bl	Func_80b10cc
	mov	r1, #0xc
	mov	r2, #0xe
	mov	r3, #8
	mov	r0, #0x10
	str	r5, [sp]
	bl	_CreateUIBox
	mov	r1, #0xe
	str	r0, [r6, #0x20]
	mov	r2, #0xd
	mov	r3, #3
	mov	r0, #0
	str	r5, [sp]
	bl	_CreateUIBox
	mov	r4, #0xe0
	lsl	r4, #2
	add	r3, r6, r4
	ldr	r2, [r3]
	mov	r8, r0
	mov	r0, #0xea
	mov	r3, #4
	lsl	r0, #2
	strb	r3, [r2, #5]
	add	r2, r6, r0
	mov	r3, #0xc
	strb	r3, [r2]
	mov	r7, #0
	mov	r2, #0
	mov	r0, r8
	mov	r1, #2
	mov	r3, #8
	str	r7, [sp]
	bl	_Func_80a1870
	mov	r2, #0xea
	lsl	r2, #2
	add	r2, r6
	mov	r11, r2
.Lb1a94:
	mov	r3, r9
	cmp	r3, #0
	beq	.Lb1ae2
	ldr	r0, =0x3a7
	add	r3, r6, r0
	mov	r1, #0
	ldrsb	r1, [r3, r1]
	mov	r4, #0
	add	r0, r7, r1
	mov	r9, r4
	bl	__modsi3
	mov	r3, #0xdb
	mov	r7, r0
	lsl	r1, r7, #1
	lsl	r3, #2
	add	r2, r1, r3
	add	r3, r6, #2
	add	r1, r7
	ldrsh	r4, [r3, r2]
	lsl	r1, #3
	sub	r1, #0xc
	mov	r0, r8
	mov	r2, #0
	mov	r10, r4
	bl	Func_80b0a6c
	mov	r3, #3
	mov	r2, r11
	strb	r3, [r2]
	mov	r0, r8
	mov	r1, r7
	mov	r2, #0
	bl	Func_80b11c4
	ldr	r0, [r6, #0x20]
	mov	r1, r10
	bl	Func_80b1dec
.Lb1ae2:
	ldr	r1, =gKeyPress
	ldr	r2, [r1]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	beq	.Lb1b40
	mov	r0, #1
	bl	WaitFrames
	mov	r0, r10
	bl	_FindEmptyInventorySlot
	cmp	r0, #0
	bne	.Lb1b06
	mov	r0, #0x71
	bl	_PlaySound
	b	.Lb1a94
.Lb1b06:
	mov	r0, #0x70
	bl	_PlaySound
	ldr	r4, =0x3aa
	add	r3, r6, r4
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #1
	bne	.Lb1b22
	mov	r0, r10
	bl	Func_80b1bd0
	b	.Lb1b28
.Lb1b22:
	mov	r0, r10
	bl	Func_80b211c
.Lb1b28:
	mov	r0, #0xe0
	lsl	r0, #2
	add	r3, r6, r0
	ldr	r2, [r3]
	mov	r3, #4
	strb	r3, [r2, #5]
	mov	r3, #0xc
	mov	r2, r11
	strb	r3, [r2]
	mov	r3, #1
	mov	r9, r3
	b	.Lb1a94
.Lb1b40:
	ldr	r3, [r1]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1b86
	mov	r0, #0x71
	bl	_PlaySound
	bl	_Func_80a195c
	mov	r0, r8
	mov	r1, #2
	bl	_CloseUIBox
	ldr	r0, [r6, #0x20]
	mov	r1, #2
	bl	_CloseUIBox
	ldr	r0, [r6, #0xc]
	mov	r1, #2
	bl	_CloseUIBox
	mov	r0, #1
	bl	WaitFrames
	mov	r0, #0
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.Lb1b86:
	ldr	r5, =gKeyRepeat
	ldr	r3, [r5]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1b9e
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r4, #1
	sub	r7, #1
	mov	r9, r4
.Lb1b9e:
	ldr	r3, [r5]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1bb4
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r0, #1
	add	r7, #1
	mov	r9, r0
.Lb1bb4:
	mov	r0, #1
	bl	WaitFrames
	b	.Lb1a94
.func_end Func_80b1a14

@ RunEquipFlow
@ r0.. = parameters. 262 lines. The equip-from-shop interaction. Traced
@ structurally.
.thumb_func_start Func_80b1bd0  @ 0x080b1bd0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x18
	str	r0, [sp, #0x14]
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	mov	r11, r3
	bl	_GetUnit
	mov	r2, #1
	mov	r3, #2
	str	r0, [sp, #8]
	str	r3, [sp]
	mov	r9, r2
	mov	r0, #0xf
	mov	r1, #8
	mov	r2, #0xf
	mov	r3, #4
	bl	_CreateUIBox
	str	r0, [sp, #0xc]
	mov	r7, #0
.Lb1c06:
	mov	r3, #2
	str	r3, [sp]
	mov	r2, #0x1e
	mov	r3, #3
	mov	r0, #0
	mov	r1, #5
	bl	_CreateUIBox
	mov	r3, #0xe0
	str	r0, [sp, #0x10]
	lsl	r3, #2
	add	r3, r11
	ldr	r2, [r3]
	mov	r3, #0x12
	strb	r3, [r2, #5]
	mov	r2, #0xea
	lsl	r2, #2
	mov	r3, #0xc
	add	r2, r11
	strb	r3, [r2]
	mov	r3, #1
	mov	r10, r3
.Lb1c32:
	mov	r2, r10
	cmp	r2, #0
	beq	.Lb1cba
	mov	r3, #0
	ldr	r0, [sp, #0x14]
	mov	r10, r3
	bl	_FindEmptyInventorySlot
	mov	r9, r0
	mov	r3, r9
	sub	r3, #1
	cmp	r7, r3
	ble	.Lb1c4e
	mov	r7, r3
.Lb1c4e:
	ldr	r2, [sp, #8]
	lsl	r4, r7, #1
	add	r4, #0xd8
	ldrh	r3, [r2, r4]
	ldr	r6, .Lb1c94	@ 0x1ff
	and	r6, r3
	mov	r3, r11
	ldr	r3, [r3, #0x20]
	mov	r1, #5
	mov	r0, r7
	str	r4, [sp, #4]
	mov	r8, r3
	bl	__modsi3
	mov	r1, #5
	mov	r5, r0
	mov	r0, r7
	bl	__divsi3
	mov	r2, r0
	lsl	r5, #4
	lsl	r2, #4
	mov	r1, r5
	add	r2, #8
	mov	r0, r8
	bl	Func_80b0a6c
	mov	r2, #0xea
	lsl	r2, #2
	mov	r3, #3
	add	r2, r11
	strb	r3, [r2]
	ldr	r2, [sp, #8]
	ldr	r4, [sp, #4]
	b	.Lb1c9c

	.align	2, 0
.Lb1c94:
	.word	0x1ff
	.pool

.Lb1c9c:
	ldrh	r0, [r2, r4]
	bl	Func_80b19cc
	mov	r1, r6
	mov	r2, r0
	mov	r3, #1
	ldr	r0, [sp, #0xc]
	bl	Func_80b110c
	ldr	r3, =0x75
	add	r6, r3
	ldr	r0, [sp, #0x10]
	mov	r1, r6
	bl	Func_80b11a4
.Lb1cba:
	ldr	r1, =gKeyPress
	ldr	r3, [r1]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1cd0
	mov	r0, #0x70
	bl	_PlaySound
	mov	r5, #0
	b	.Lb1d82
.Lb1cd0:
	ldr	r3, [r1]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1ce6
	mov	r0, #0x71
	mov	r5, #1
	bl	_PlaySound
	neg	r5, r5

	b	.Lb1d82
.Lb1ce6:
	ldr	r5, =gKeyRepeat
	ldr	r3, [r5]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1d0a
	mov	r0, #0x6f
	bl	_PlaySound
	sub	r7, #1
	mov	r3, r9
	add	r0, r7, r3
	mov	r1, r9
	bl	__modsi3
	mov	r2, #1
	mov	r7, r0
	mov	r10, r2
.Lb1d0a:
	ldr	r3, [r5]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1d2c
	mov	r0, #0x6f
	bl	_PlaySound
	add	r7, #1
	mov	r3, r9
	add	r0, r7, r3
	mov	r1, r9
	bl	__modsi3
	mov	r2, #1
	mov	r7, r0
	mov	r10, r2
.Lb1d2c:
	ldr	r3, [r5]
	mov	r2, #0x40
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1d52
	sub	r7, #5
	cmp	r7, #0
	bge	.Lb1d3e
	add	r7, #0xf
.Lb1d3e:
	cmp	r7, r9
	blt	.Lb1d48
.Lb1d42:
	sub	r7, #5
	cmp	r7, r9
	bge	.Lb1d42
.Lb1d48:
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r3, #1
	mov	r10, r3
.Lb1d52:
	ldr	r3, =gKeyRepeat
	ldr	r3, [r3]
	mov	r2, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1d7a
	add	r7, #5
	cmp	r7, r9
	blt	.Lb1d66
	sub	r7, #0xf
.Lb1d66:
	cmp	r7, #0
	bge	.Lb1d70
.Lb1d6a:
	add	r7, #5
	cmp	r7, #0
	blt	.Lb1d6a
.Lb1d70:
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r2, #1
	mov	r10, r2
.Lb1d7a:
	mov	r0, #1
	bl	WaitFrames
	b	.Lb1c32
.Lb1d82:
	ldr	r0, [sp, #0x10]
	mov	r1, #2
	bl	_CloseUIBox
	mov	r0, #1
	bl	WaitFrames
	cmp	r5, #0
	bne	.Lb1dc0
	ldr	r0, [sp, #0x14]
	mov	r1, r7
	bl	Func_80b1e80
	mov	r3, #1
	mov	r2, r0
	neg	r3, r3
	cmp	r2, r3
	beq	.Lb1dae
	ldr	r0, [sp, #0x14]
	mov	r1, r7
	bl	Func_80b1f4c
.Lb1dae:
	ldr	r0, =0xcaa
	bl	Func_80b04dc
	ldr	r0, [sp, #0x14]
	bl	_FindEmptyInventorySlot
	cmp	r0, #0
	beq	.Lb1dc0
	b	.Lb1c06
.Lb1dc0:
	ldr	r0, [sp, #0xc]
	mov	r1, #2
	bl	_CloseUIBox
	mov	r0, r5
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80b1bd0

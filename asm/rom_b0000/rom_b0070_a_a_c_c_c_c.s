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

@ DrawItemDetail
@ r0.. = parameters. Draws the detail panel for one item, counting the owner's
@ inventory with _Func_784d8.
.thumb_func_start Func_80b1dec  @ 0x080b1dec
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r5, r1
	mov	r9, r0
	mov	r0, r5
	sub	sp, #4
	bl	_GetUnit
	mov	r2, #0
	mov	r3, r9
	mov	r10, r0
	mov	r7, #8
	mov	r8, r2
	cmp	r3, #0
	beq	.Lb1e6c
	mov	r0, r9
	bl	_Func_8016478
	mov	r0, r5
	bl	_FindEmptyInventorySlot
	cmp	r0, #0
	bne	.Lb1e2e
	ldr	r0, =0xc91
	mov	r1, r9
	mov	r2, #8
	mov	r3, #0x14
	bl	_DrawSmallText
	b	.Lb1e6c
.Lb1e2e:
	mov	r5, #0
	mov	r6, #0xd8
.Lb1e32:
	mov	r2, r10
	ldrh	r3, [r6, r2]
	cmp	r3, #0
	beq	.Lb1e4e
	mov	r0, r3
	mov	r3, r8
	str	r3, [sp]
	mov	r1, #0x1b
	mov	r3, r7
	mov	r2, r9
	bl	_Func_801eb90
	mov	r3, #0xfc
	strb	r3, [r0, #0xf]
.Lb1e4e:
	add	r7, #0x10
	cmp	r5, #4
	bne	.Lb1e5a
	mov	r2, #0x10
	mov	r7, #8
	add	r8, r2
.Lb1e5a:
	cmp	r5, #9
	bne	.Lb1e64
	mov	r3, #0x10
	mov	r7, #8
	add	r8, r3
.Lb1e64:
	add	r5, #1
	add	r6, #2
	cmp	r5, #0xe
	ble	.Lb1e32
.Lb1e6c:
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80b1dec

@ RunDetailView
@ r0.. = parameters. Shows the detail panel and waits for dismissal.
.thumb_func_start Func_80b1e80  @ 0x080b1e80
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	sub	sp, #4
	ldr	r7, [r3]
	mov	r10, r0
	mov	r9, r1
	bl	_GetUnit
	mov	r1, r9
	lsl	r5, r1, #1
	mov	r6, r0
	add	r5, #0xd8
	ldrh	r0, [r6, r5]
	bl	_GetItemInfo
	mov	r2, #1
	str	r2, [sp]
	mov	r8, r0
	ldrh	r0, [r6, r5]
	bl	Func_80b19cc
	mov	r1, r9
	mov	r11, r0
	mov	r0, r10
	bl	_GetInventoryItem
	mov	r3, r8
	ldrb	r2, [r3, #3]
	mov	r3, #0x10
	and	r3, r2
	mov	r10, r0
	cmp	r3, #0
	beq	.Lb1f2c
	cmp	r0, #1
	ble	.Lb1f2c
	ldr	r0, =0xcad
	bl	Func_80b04dc
	mov	r1, #0xe2
	lsl	r1, #2
	add	r3, r7, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	mov	r8, r2
	ldr	r2, =0x38a
	add	r3, r7, r2
	sub	r2, #0xa
	add	r5, r7, r2
	ldr	r2, [r5]
	mov	r1, #0
	ldrsh	r6, [r3, r1]
	mov	r3, #4
	strb	r3, [r2, #5]
	mov	r3, #0xea
	lsl	r3, #2
	add	r2, r7, r3
	mov	r3, #0xc
	strb	r3, [r2]
	mov	r0, #0
	mov	r1, #0x80
	mov	r2, #0x30
	bl	Func_80b0a6c
	mov	r1, r10
	mov	r2, r11
	mov	r0, #0
	bl	Func_80b1614
	str	r0, [sp]
	mov	r0, #1
	bl	WaitFrames
	ldr	r0, [r5]
	bl	_Func_80a17c4
	mov	r0, #0
	mov	r1, r8
	mov	r2, r6
	bl	Func_80b0a6c
.Lb1f2c:
	ldr	r0, [sp]
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80b1e80

@ ApplyTransaction
@ r0.. = parameters. 182 lines. Commits a buy or sell: adjusts the inventory and
@ registers the result values with _Func_19908. Traced structurally.
.thumb_func_start Func_80b1f4c  @ 0x080b1f4c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x14
	str	r0, [sp, #0x10]
	str	r1, [sp, #0xc]
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	mov	r7, r2
	str	r3, [sp, #8]
	bl	_GetUnit
	ldr	r2, [sp, #0xc]
	lsl	r2, #1
	str	r2, [sp, #4]
	mov	r5, r2
	mov	r6, r0
	add	r5, #0xd8
	ldrh	r3, [r6, r5]
	ldr	r2, =0x1ff
	and	r2, r3
	mov	r10, r2
	mov	r0, r10
	bl	_GetItemInfo
	ldrb	r2, [r0, #3]
	mov	r3, #4
	and	r3, r2
	lsl	r3, #24
	lsr	r3, #24
	mov	r2, #1
	mov	r11, r3
	neg	r2, r2
	mov	r3, #0
	mov	r9, r0
	str	r3, [sp]
	cmp	r7, r2
	bne	.Lb1fa6
	mov	r3, #1
	str	r3, [sp]
	mov	r7, #1
.Lb1fa6:
	ldrh	r0, [r6, r5]
	bl	Func_80b19cc
	mov	r2, r7
	mul	r2, r0
	mov	r8, r2
	cmp	r2, #0
	bne	.Lb1fc6
	mov	r0, r10
	mov	r1, #2
	bl	_Func_8019908
	ldr	r0, =0xcac
	bl	Func_80b0574
	b	.Lb20a0
.Lb1fc6:
	ldrh	r2, [r6, r5]
	mov	r3, #0x80
	lsl	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1fee
	mov	r3, r9
	ldrb	r2, [r3, #3]
	mov	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.Lb1fee
	mov	r0, r10
	mov	r1, #2
	bl	_Func_8019908
	ldr	r0, =0xcab
	bl	Func_80b0574
	b	.Lb20a0
.Lb1fee:
	ldr	r2, [sp]
	cmp	r2, #0
	beq	.Lb1ff8
	ldr	r5, =0xcb2
	b	.Lb2020
.Lb1ff8:
	ldr	r3, [sp, #4]
	add	r3, #0xd8
	ldrh	r2, [r6, r3]
	mov	r3, #0x80
	lsl	r3, #3
	and	r3, r2
	cmp	r3, #0
	beq	.Lb200c
	ldr	r5, =0xcb1
	b	.Lb2020
.Lb200c:
	cmp	r7, #1
	ble	.Lb2014
	ldr	r5, =0xcb0
	b	.Lb2020
.Lb2014:
	mov	r3, r11
	cmp	r3, #0
	beq	.Lb201e
	ldr	r5, =0xcaf
	b	.Lb2020
.Lb201e:
	ldr	r5, =0xcae
.Lb2020:
	mov	r0, r10
	mov	r1, #2
	bl	_Func_8019908
	mov	r0, r8
	mov	r1, #5
	bl	_Func_8019908
	mov	r0, r5
	bl	Func_80b0574
	mov	r0, #0
	bl	Func_80b0634
	cmp	r0, #0
	beq	.Lb205a
	mov	r2, r11
	cmp	r2, #0
	bne	.Lb204c
	ldr	r3, [sp]
	cmp	r3, #0
	beq	.Lb2050
.Lb204c:
	ldr	r5, =0xcb6
	b	.Lb2052
.Lb2050:
	ldr	r5, =0xcb4
.Lb2052:
	mov	r0, r5
	bl	Func_80b0574
	b	.Lb20a0
.Lb205a:
	mov	r0, #0x66
	bl	_PlaySound
	cmp	r7, #0
	ble	.Lb2074
	mov	r5, r7
.Lb2066:
	ldr	r0, [sp, #0x10]
	ldr	r1, [sp, #0xc]
	sub	r5, #1
	bl	_Func_8078948
	cmp	r5, #0
	bne	.Lb2066
.Lb2074:
	mov	r0, r8
	bl	_AddCoins
	bl	Func_80b10cc
	ldr	r2, [sp, #8]
	ldr	r1, [sp, #0x10]
	ldr	r0, [r2, #0x20]
	bl	Func_80b1dec
	mov	r3, r11
	cmp	r3, #0
	bne	.Lb2094
	ldr	r2, [sp]
	cmp	r2, #0
	beq	.Lb2098
.Lb2094:
	ldr	r5, =0xcb5
	b	.Lb209a
.Lb2098:
	ldr	r5, =0xcb3
.Lb209a:
	mov	r0, r5
	bl	Func_80b0574
.Lb20a0:
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80b1f4c

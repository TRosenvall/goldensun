	.include "macros.inc"
	.include "gba.inc"

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

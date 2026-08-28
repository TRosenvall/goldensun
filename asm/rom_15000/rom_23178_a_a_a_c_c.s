	.include "macros.inc"
	.include "gba.inc"

@ DrawSubScreenLabel
@ r0.. = parameters. Draws with DrawSmallText and releases with .gcc2_compiled..
.thumb_func_start Func_8028b80  @ 0x08028b80
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001f38
	ldr	r6, [r3]
	mov	r0, r6
	mov	r5, r6
	add	r5, #0x8c
	add	r0, #0x96
	mov	r3, #0
	ldrsh	r2, [r0, r3]
	mov	r4, #0
	ldrsh	r3, [r5, r4]
	ldrh	r1, [r5]
	cmp	r2, r3
	beq	.L28bf2
	strh	r1, [r0]
	ldr	r0, [r6, #0x7c]
	bl	Func_8016478
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	bne	.L28bca
	ldr	r5, =0xc71
	ldr	r1, [r6, #0x7c]
	mov	r0, r5
	mov	r2, #0x10
	mov	r3, #4
	add	r5, #1
	bl	DrawSmallText
	ldr	r1, [r6, #0x7c]
	mov	r0, r5
	mov	r2, #0x10
	mov	r3, #0x10
	bl	DrawSmallText
	b	.L28bf2
.L28bca:
	ldr	r5, =0xc73
	ldr	r1, [r6, #0x7c]
	mov	r0, r5
	mov	r2, #0
	mov	r3, #4
	bl	DrawSmallText
	add	r0, r5, #1
	ldr	r1, [r6, #0x7c]
	mov	r2, #0
	mov	r3, #0x10
	add	r5, #2
	bl	DrawSmallText
	ldr	r1, [r6, #0x7c]
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0x1c
	bl	DrawSmallText
.L28bf2:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8028b80

@ RunPromptSubScreen
@ r0.. = parameters. A sub-screen that also opens its own window and label.
.thumb_func_start DataTransferMenu  @ 0x08028c04
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r10, r0
	sub	sp, #4
	bl	Func_80284dc
	ldr	r3, =iwram_3001f38
	ldr	r7, [r3]
	mov	r3, r10
	cmp	r3, #0
	bne	.L28c2c
	mov	r0, #0x2c
	bl	AddMenuBarOption
	mov	r0, #0x2d
	bl	AddMenuBarOption
	b	.L28c3e
.L28c2c:
	mov	r0, #0x2e
	bl	AddMenuBarOption
	mov	r0, #0x2f
	bl	AddMenuBarOption
	mov	r0, #0x30
	bl	AddMenuBarOption
.L28c3e:
	mov	r0, #0x11
	mov	r1, #7
	mov	r2, #0
	bl	Func_8028808
	mov	r3, r10
	cmp	r3, #0
	beq	.L28cbc
	ldr	r1, =0xc76
	ldr	r0, =Func_8028aa8
	bl	StartTask
	ldr	r3, =0xffff
	mov	r2, r7
	add	r2, #0x96
	strh	r3, [r2]
	mov	r6, #2
	mov	r1, #0
	mov	r2, #0x11
	mov	r3, #4
	mov	r0, #7
	str	r6, [sp]
	bl	CreateUIBox
	ldr	r5, =0xc77
	mov	r3, #0x80
	mov	r1, r0
	add	r3, r7
	str	r1, [r3]
	mov	r0, r5
	mov	r2, #0
	mov	r8, r3
	mov	r3, #4
	bl	DrawSmallText
	mov	r1, #4
	mov	r2, #0x19
	mov	r3, #0xc
	mov	r0, #3
	str	r6, [sp]
	bl	CreateUIBox
	mov	r1, r0
	str	r1, [r7, #0x7c]
	add	r0, r5, #1
	mov	r2, #8
	mov	r3, #0
	bl	DrawSmallText
	add	r0, r5, #2
	ldr	r1, [r7, #0x7c]
	mov	r2, #8
	mov	r3, #0xb
	add	r5, #3
	bl	DrawSmallText
	ldr	r1, [r7, #0x7c]
	mov	r0, r5
	mov	r2, #8
	mov	r3, #0x16
	bl	DrawSmallText
	b	.L28d00
.L28cbc:
	ldr	r1, =0xc76
	ldr	r0, =Func_8028b80
	bl	StartTask
	ldr	r3, =0xffff
	mov	r2, r7
	add	r2, #0x96
	strh	r3, [r2]
	mov	r5, #2
	mov	r1, #0
	mov	r2, #0x12
	mov	r3, #4
	mov	r0, #6
	str	r5, [sp]
	bl	CreateUIBox
	mov	r3, #0x80
	mov	r1, r0
	add	r3, r7
	str	r1, [r3]
	ldr	r0, .L28d70	@ 0xc76
	mov	r2, #2
	mov	r8, r3
	mov	r3, #4
	bl	DrawSmallText
	mov	r0, #1
	mov	r1, #5
	mov	r2, #0x1c
	mov	r3, #7
	str	r5, [sp]
	bl	CreateUIBox
	str	r0, [r7, #0x7c]
.L28d00:
	mov	r0, #0
	bl	Func_8028574
	mov	r3, r10
	mov	r5, r0
	cmp	r3, #0
	beq	.L28d16
	ldr	r0, =Func_8028aa8
	bl	StopTask
	b	.L28d1c
.L28d16:
	ldr	r0, =Func_8028b80
	bl	StopTask
.L28d1c:
	mov	r3, r8
	ldr	r0, [r3]
	bl	Func_8016478
	ldr	r0, [r7, #0x7c]
	bl	Func_8016478
	mov	r3, r8
	ldr	r0, [r3]
	mov	r1, #2
	bl	CloseUIBox
	mov	r1, #2
	ldr	r0, [r7, #0x7c]
	bl	CloseUIBox
	mov	r0, #1
	bl	WaitFrames
	bl	Func_802851c
	mov	r0, r5
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1

	.pool_aligned
.L28d70:
	.word	0xc76
.func_end DataTransferMenu

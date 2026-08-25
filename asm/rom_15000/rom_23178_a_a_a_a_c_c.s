	.include "macros.inc"
	.include "gba.inc"

@ CloseSubScreen
@ Takes no arguments. Unregisters with StopTask, closes the window with
@ CloseUIBox, releases tiles with Func_3f3c, frees with Func_2dd8, and gives a
@ frame with WaitFrames.
.thumb_func_start Func_802851c  @ 0x0802851c
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001f38
	ldr	r5, [r3]
	ldr	r0, =Func_8028194
	bl	StopTask
	ldr	r0, [r5, #0x78]
	cmp	r0, #0
	beq	.L28534
	mov	r1, #2
	bl	CloseUIBox
.L28534:
	mov	r2, r5
	add	r2, #0x8e
	mov	r1, #0
	ldrsh	r3, [r2, r1]
	mov	r6, #0
	cmp	r6, r3
	bge	.L28558
	mov	r7, r2
	add	r5, #0x12
.L28546:
	ldrh	r0, [r5]
	bl	Func_8003f3c
	mov	r2, #0
	ldrsh	r3, [r7, r2]
	add	r6, #1
	add	r5, #0x14
	cmp	r6, r3
	blt	.L28546
.L28558:
	mov	r0, #0x3a
	bl	gfree
	mov	r0, #1
	bl	WaitFrames
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_802851c

@ RunSubScreenLoop
@ r0.. = parameters. Drives a sub-screen a frame at a time, drawing with
@ Func_1e7c0, releasing with .gcc2_compiled., and playing sounds via _Func_f9080.
.thumb_func_start Func_8028574  @ 0x08028574
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f38
	ldr	r3, [r3]
	mov	r1, #0x8c
	mov	r8, r3
	add	r1, r8
	mov	r10, r1
	mov	r2, r10
	mov	r3, #0x92
	add	r3, r8
	strh	r0, [r2]
	mov	r11, r3
.L28598:
	mov	r1, r8
	ldr	r0, [r1, #0x78]
	bl	Func_8016478
	mov	r3, r11
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	cmp	r0, #0
	beq	.L285c0
	mov	r2, r10
	mov	r1, #0
	ldrsh	r3, [r2, r1]
	add	r0, r3
	b	.L285d0
.L285b4:
	mov	r0, #0x71
	bl	_PlaySound
	mov	r0, #1
	neg	r0, r0
	b	.L2867e
.L285c0:
	mov	r0, r10
	mov	r2, #0
	ldrsh	r3, [r0, r2]
	mov	r1, r8
	add	r3, #0x84
	ldrb	r2, [r1, r3]
	ldr	r3, =0x1f
	add	r0, r2, r3
.L285d0:
	mov	r2, r8
	ldr	r1, [r2, #0x78]
	mov	r3, #0
	mov	r2, #0
	bl	Func_801e7c0
	mov	r3, #0x8e
	add	r3, r8
	ldr	r7, =gKeyPress
	mov	r6, r10
	mov	r9, r3
.L285e6:
	mov	r0, #1
	bl	WaitFrames
	ldr	r2, [r7]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	bne	.L28672
	ldr	r2, [r7]
	mov	r3, #2
	and	r2, r3
	cmp	r2, #0
	bne	.L285b4
	ldr	r2, [r7]
	mov	r3, #8
	and	r2, r3
	cmp	r2, #0
	bne	.L285b4
	ldr	r1, =gKeyRepeat
	ldr	r2, [r1]
	mov	r3, #0x20
	and	r2, r3
	cmp	r2, #0
	bne	.L28620
	ldr	r5, [r1]
	mov	r3, #0x40
	and	r5, r3
	cmp	r5, #0
	beq	.L2863e
.L28620:
	mov	r0, #0x6f
	bl	_PlaySound
	ldrh	r3, [r6]
	sub	r3, #1
	strh	r3, [r6]
	lsl	r3, #16
	cmp	r3, #0
	bge	.L28598
	mov	r0, r9
	ldrh	r3, [r0]
	mov	r1, r10
	sub	r3, #1
	strh	r3, [r1]
	b	.L28598
.L2863e:
	ldr	r2, [r1]
	mov	r3, #0x10
	and	r2, r3
	cmp	r2, #0
	bne	.L28652
	ldr	r2, [r1]
	mov	r3, #0x80
	and	r2, r3
	cmp	r2, #0
	beq	.L285e6
.L28652:
	mov	r0, #0x6f
	bl	_PlaySound
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	mov	r1, r9
	lsl	r3, #16
	mov	r0, #0
	ldrsh	r2, [r1, r0]
	asr	r3, #16
	cmp	r3, r2
	blt	.L28598
	mov	r2, r10
	strh	r5, [r2]
	b	.L28598
.L28672:
	mov	r0, #0x70
	bl	_PlaySound
	mov	r1, r10
	mov	r3, #0
	ldrsh	r0, [r1, r3]
.L2867e:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8028574

@ RunSubScreenLoopShort
@ r0.. = parameters. The abbreviated form of Func_28574.
.thumb_func_start Func_80286a0  @ 0x080286a0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f38
	ldr	r6, [r3]
	mov	r5, r6
	mov	r2, #1
	mov	r3, #0xc
	add	r5, #0x8c
	mov	r7, r1
	mov	r9, r2
	mov	r10, r3
	strh	r0, [r5]
	cmp	r7, r0
	bge	.L286ca
	sub	r2, #2
	mov	r9, r2
.L286ca:
	mov	r8, r0
	mov	r3, #0x92
	add	r3, r6
	mov	r11, r3
	b	.L286e6
.L286d4:
	ldrh	r3, [r5]
	add	r3, r9
	strh	r3, [r5]
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r2, #0
	mov	r10, r2
	add	r8, r9
.L286e6:
	ldr	r0, [r6, #0x78]
	bl	Func_8016478
	mov	r2, r11
	mov	r3, #0
	ldrsh	r0, [r2, r3]
	cmp	r0, #0
	beq	.L286fe
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	add	r0, r3
	b	.L2870a
.L286fe:
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	add	r3, #0x84
	ldrb	r2, [r6, r3]
	ldr	r3, =0x1f
	add	r0, r2, r3
.L2870a:
	mov	r3, #0
	ldr	r1, [r6, #0x78]
	mov	r2, #0
	bl	Func_801e7c0
	mov	r3, #0
	ldrsh	r1, [r5, r3]
	sub	r3, r1, r7
	ldr	r0, =.L373ef
	mov	r2, r3
	cmp	r3, #0
	bge	.L28724
	sub	r2, r7, r1
.L28724:
	ldrb	r0, [r0, r2]
	add	r0, r10
	bl	WaitFrames
	cmp	r8, r7
	bne	.L286d4
	mov	r0, #0x30
	bl	WaitFrames
	mov	r0, #0x70
	bl	_PlaySound
	mov	r0, r7
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80286a0

@ LoadSubScreenGraphics
@ r0 = asset id. Fetches with GetFile, stages through a Func_4938 scratch,
@ unpacks with DecompressLZ1, reserves with UploadSpriteGFX, releases with free.
.thumb_func_start LoadUIIcon  @ 0x0802875c
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r3, #0x80
	lsl	r3, #3
	mov	r8, r3
	mov	r10, r0
	mov	r0, r8
	mov	r5, r1
	bl	Func_8004938
	mov	r6, r0
	ldr	r0, =_FILE_f1
	bl	GetFile
	lsl	r5, #1
	ldrh	r3, [r5, r0]
	mov	r1, r6
	add	r0, r3
	bl	DecompressLZ1
	mov	r0, r10
	mov	r1, r8
	mov	r2, r6
	bl	UploadSpriteGFX
	mov	r0, r6
	bl	free
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end LoadUIIcon

@ AttachSubScreenGraphics
@ r0 = asset id. LoadUIIcon then AllocSpriteSlot to reserve the OBJ tiles.
.thumb_func_start AddMenuBarOption  @ 0x080287a8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f38
	ldr	r3, [r3]
	mov	r8, r3
	mov	r2, r8
	add	r2, #0x8e
	mov	r1, #0
	ldrsh	r7, [r2, r1]
	mov	r10, r0
	ldrh	r3, [r2]
	cmp	r7, #5
	bgt	.L287f8
	add	r3, #1
	strh	r3, [r2]
	bl	AllocSpriteSlot
	mov	r1, r10
	mov	r6, r0
	lsl	r5, r7, #2
	bl	LoadUIIcon
	lsl	r3, r7, #1
	add	r5, r7
	add	r3, r7
	lsl	r5, #2
	lsl	r3, #3
	add	r5, r8
	add	r3, #0x20
	strh	r3, [r5, #0xc]
	mov	r3, #0x88
	strh	r3, [r5, #0xe]
	mov	r3, r7
	add	r3, #0x84
	mov	r1, r10
	mov	r2, r8
	strh	r6, [r5, #0x12]
	strb	r1, [r2, r3]
.L287f8:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end AddMenuBarOption

@ OpenSubScreenWindow
@ r0.. = parameters. Opens the sub-screen's window with CreateUIBox, sizing it
@ with Func_af0.
.thumb_func_start Func_8028808  @ 0x08028808
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_3001f38
	ldr	r7, [r3]
	mov	r3, #0x90
	add	r3, r7
	mov	r8, r3
	mov	r3, r7
	mov	r10, r0
	add	r1, #2
	mov	r5, r8
	add	r3, #0x92
	strh	r1, [r5]
	mov	r6, r10
	strh	r2, [r3]
	add	r3, #2
	strh	r6, [r3]
	mov	r1, #0x8e
	add	r1, r7
	mov	r2, #0
	ldrsh	r6, [r1, r2]
	mov	r9, r1
	mov	r1, r8
	mov	r3, #0
	ldrsh	r0, [r1, r3]
	mov	r1, #3
	lsl	r0, #1
	sub	sp, #4
	bl	__divsi3
	lsl	r5, r6, #1
	add	r5, r6
	add	r5, r0
	lsr	r3, r5, #31
	add	r5, r3
	asr	r5, #1
	mov	r3, #0xf
	mov	r1, #0
	sub	r0, r3, r5
	cmp	r1, r6
	bge	.L2887e
	mov	r2, r10
	lsl	r4, r2, #3
	mov	r12, r9
	mov	r2, r7
.L28868:
	lsl	r3, r0, #3
	strh	r3, [r2, #0xc]
	strh	r4, [r2, #0xe]
	mov	r6, r12
	mov	r5, #0
	ldrsh	r3, [r6, r5]
	add	r1, #1
	add	r0, #3
	add	r2, #0x14
	cmp	r1, r3
	blt	.L28868
.L2887e:
	mov	r3, r8
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	mov	r3, #2
	str	r3, [sp]
	mov	r1, r10
	mov	r3, #3
	bl	CreateUIBox
	str	r0, [r7, #0x78]
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8028808

@ OpenSubScreenWindowAlt
@ r0.. = parameters. As Func_28808 with fixed geometry.
.thumb_func_start Func_80288a8  @ 0x080288a8
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r1
	ldr	r1, =iwram_3001f38
	ldr	r5, [r1]
	mov	r1, #0x90
	add	r1, r5
	mov	r14, r1
	add	r2, #2
	mov	r4, r14
	strh	r2, [r4]
	mov	r2, r5
	add	r2, #0x92
	strh	r3, [r2]
	mov	r3, r5
	add	r3, #0x94
	strh	r6, [r3]
	mov	r7, #0x8e
	add	r7, r5
	mov	r2, #0
	ldrsh	r3, [r7, r2]
	mov	r1, #0
	sub	sp, #4
	mov	r12, r7
	cmp	r1, r3
	bge	.L288fc
	lsl	r3, r6, #3
	mov	r8, r3
	mov	r2, r5
.L288e4:
	lsl	r3, r0, #3
	mov	r4, r8
	strh	r4, [r2, #0xe]
	strh	r3, [r2, #0xc]
	mov	r4, r12
	mov	r7, #0
	ldrsh	r3, [r4, r7]
	add	r1, #1
	add	r0, #3
	add	r2, #0x14
	cmp	r1, r3
	blt	.L288e4
.L288fc:
	mov	r1, r14
	mov	r3, #2
	mov	r7, #0
	ldrsh	r2, [r1, r7]
	str	r3, [sp]
	mov	r1, r6
	mov	r3, #3
	bl	CreateUIBox
	str	r0, [r5, #0x78]
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80288a8

@ RunFieldMenu
@ r0 = the previously chosen entry. Draws the field menu and returns which
@ entry the player picked, or a negative value when they backed out.
@
@ _Func_7a5bc(-1) decides how many entries there are: when the party summary
@ comes back empty the extra panel 0x0F is left out, so the menu is three rows
@ instead of four. The two byte tables .L37403 and .L373f7 translate between
@ the row on screen and the caller's entry number in each of those two shapes,
@ indexed by `previous + 6 * short`.
@
@ The panels are appended in order -- 1, then 0x0F when the party is present,
@ then 2 and 7 -- with Func_28808(0x11, 7, 0) placing the box, Func_28574
@ running the cursor and Func_2851c tearing it down.
@
@ Func_1c244 is the caller, and it dispatches the result 0..4 into
@ _Func_8ce74, _Func_a5b94, _Func_aa56c, _Func_a24d0 and _Func_a7478.
.thumb_func_start Func_8028920  @ 0x08028920
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r0, #1
	neg	r0, r0
	mov	r6, #0
	bl	_GetNumDjinn
	cmp	r0, #0
	bne	.L28934
	mov	r6, #1
.L28934:
	lsl	r3, r6, #1
	add	r3, r6
	ldr	r2, =.L37403
	lsl	r7, r3, #1
	add	r3, r5, r7
	ldrsb	r3, [r2, r3]
	sub	r5, r3, #1
	cmp	r5, #0
	bge	.L28948
	mov	r5, #0
.L28948:
	bl	Func_80284dc
	mov	r0, #1
	bl	AddMenuBarOption
	cmp	r6, #0
	bne	.L2895c
	mov	r0, #0xf
	bl	AddMenuBarOption
.L2895c:
	mov	r0, #2
	bl	AddMenuBarOption
	mov	r0, #7
	bl	AddMenuBarOption
	mov	r0, #0x11
	mov	r1, #7
	mov	r2, #0
	bl	Func_8028808
	mov	r0, r5
	bl	Func_8028574
	mov	r5, r0
	bl	Func_802851c
	cmp	r5, #0
	blt	.L2898a
	add	r3, r5, r7
	ldr	r2, =.L373f7
	add	r3, #1
	ldrsb	r5, [r2, r3]
.L2898a:
	mov	r0, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8028920

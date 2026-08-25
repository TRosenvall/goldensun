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

	.include "macros.inc"
	.include "gba.inc"

@ OpenPartyScreen
@ Takes no arguments. Func_1a66c allocates the block, Func_1a778 resets it, then
@ a frame passes.
.thumb_func_start Func_1c2f0
	push	{lr}
	bl	Func_1a66c
	bl	Func_1a778
	mov	r0, #1
	bl	Func_30f8
	pop	{r0}
	bx	r0
.func_end Func_1c2f0

@ RunPartyScreen
@ Takes no arguments. Builds the screen with Func_1a7f4, starts its task with
@ Func_1a968, opens the panel with Func_1b010, runs input with Func_1b424, and
@ tears down with Func_1b148.
.thumb_func_start Func_1c304
	push	{r5, lr}
	ldr	r3, =iwram_1e98
	ldr	r1, =0x39e
	ldr	r3, [r3]
	add	r2, r3, r1
	strh	r0, [r2]
	mov	r2, #0xee
	lsl	r2, #2
	add	r3, r2
	mov	r2, #1
	strh	r2, [r3]
	bl	Func_1a7f4
	bl	Func_1b228
	mov	r1, #5
	mov	r0, #0
	bl	Func_1b010
	bl	Func_1a968
	mov	r0, #1
	bl	Func_1b424
	mov	r5, r0
	bl	Func_1b148
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_1c304

@ OpenStatusOverlay
@ r0.. = parameters. Opens a window with Func_162d4, renders text with
@ Func_187ac, registers a task with Func_41d8, and reads the save-data
@ preferences at ewram_240. State lives in iwram_1ebc.
.thumb_func_start Func_1c34c
	push	{r5, r6, lr}
	ldr	r3, =iwram_1ebc
	sub	sp, #0x14
	ldr	r6, [r3]
	ldr	r2, =ewram_240
	mov	r3, #8
	mov	r1, #0xe0
	str	r3, [sp, #0x10]
	str	r3, [sp, #0xc]
	lsl	r1, #1
	add	r3, r2, r1
	mov	r1, #0
	ldrsh	r0, [r3, r1]
	mov	r1, #0xe1
	lsl	r1, #1
	add	r3, r2, r1
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	bl	_Func_8b158
	ldr	r3, =0x99b
	mov	r5, r0
	add	r5, r3
	add	r0, sp, #4
	add	r1, sp, #0x10
	add	r2, sp, #0xc
	add	r3, sp, #8
	str	r0, [sp]
	mov	r0, r5
	bl	Func_187ac
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0xa
	sub	r0, r2
	sub	r1, r3
	mov	r4, #2
	asr	r1, #1
	asr	r0, #1
	str	r1, [sp, #0xc]
	str	r4, [sp]
	str	r0, [sp, #0x10]
	bl	Func_162d4
	mov	r2, #0x8c
	lsl	r2, #2
	mov	r1, r0
	add	r3, r6, r2
	str	r1, [r3]
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0
	bl	Func_1e74c
	mov	r3, #0x8d
	lsl	r3, #2
	add	r2, r6, r3
	mov	r1, #0xc8
	mov	r3, #0x5a
	strh	r3, [r2]
	lsl	r1, #4
	ldr	r0, =Func_1c3e8
	bl	Func_41d8
	add	sp, #0x14
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_1c34c

@ CloseStatusOverlay
@ Takes no arguments. Unregisters the task with Func_4278 and closes the window
@ with Func_16418.
.thumb_func_start Func_1c3e8
	push	{lr}
	ldr	r3, =iwram_1ebc
	mov	r0, #0x8d
	ldr	r1, [r3]
	lsl	r0, #2
	add	r2, r1, r0
	ldrh	r3, [r2]
	ldr	r0, =0xffff
	add	r3, r0
	strh	r3, [r2]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L1c416
	mov	r2, #0x8c
	lsl	r2, #2
	add	r3, r1, r2
	ldr	r0, [r3]
	mov	r1, #2
	bl	Func_16418
	ldr	r0, =Func_1c3e8
	bl	Func_4278
.L1c416:
	pop	{r0}
	bx	r0
.func_end Func_1c3e8

@ CloseStatusOverlayAlt
@ Takes no arguments. As Func_1c3e8 with different cleanup ordering.
.thumb_func_start Func_1c428
	push	{lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0x8c
	ldr	r3, [r3]
	lsl	r2, #2
	add	r3, r2
	ldr	r0, [r3]
	cmp	r0, #0
	beq	.L1c44c
	ldrh	r3, [r0, #0x16]
	cmp	r3, #0
	beq	.L1c44c
	mov	r1, #2
	bl	Func_16418
	ldr	r0, =Func_1c3e8
	bl	Func_4278
.L1c44c:
	pop	{r0}
	bx	r0
.func_end Func_1c428

@ ForwardToParty
@ r0, r1 = parameters. Calls _Func_789dc with r1 and always returns 0 -- the
@ callee's result is discarded.
.thumb_func_start Func_1c458
	push	{lr}
	mov	r0, r1
	bl	_Func_789dc
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end Func_1c458

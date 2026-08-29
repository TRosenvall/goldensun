	.include "macros.inc"
	.include "gba.inc"

@ OpenTextBox
@ r0 = string id. Opens the standard message box and queues the string.
@ Sets the mode byte at [iwram_1e8c]+0xEA5 to 2 while measuring, calls
@ BufferString to lay the string out, and if the resulting entry in the halfword
@ table at +0xEB0 is non-zero, opens a window with
@ CreateUIBox(0, 0xF, 0x1E, 6, 0xA) -- full width, six rows, at row 15 -- caching
@ it in the slot's first word.
@ Returns without opening anything when the string measures empty.
.thumb_func_start PrintBattleText  @ 0x080174f8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_3001e8c
	ldr	r6, [r3]
	ldr	r7, [r3, #0x58]
	ldr	r3, =0xea5
	add	r5, r6, r3
	mov	r3, #2
	mov	r2, #0
	strb	r3, [r5]
	mov	r1, #1
	mov	r8, r2
	sub	sp, #4
	bl	BufferString
	mov	r2, #1
	mov	r9, r2
	mov	r3, r9
	mov	r2, #0xeb
	strb	r3, [r5]
	lsl	r2, #4
	lsl	r3, r0, #1
	add	r3, r2
	ldrh	r3, [r6, r3]
	mov	r10, r0
	cmp	r3, #0
	beq	.L17588
	ldr	r0, [r7]
	cmp	r0, #0
	bne	.L17564
	mov	r3, #0xa
	str	r3, [sp]
	mov	r1, #0xf
	mov	r2, #0x1e
	mov	r3, #6
	mov	r0, #0
	bl	CreateUIBox
	mov	r3, r9
	mov	r5, r0
	str	r5, [r7]
	mov	r2, #0x1e
	str	r3, [sp]
	mov	r0, #0
	mov	r1, #0xf
	mov	r3, #6
	bl	Func_8017248
	mov	r2, r8
	str	r2, [r7, #8]
	b	.L17566
.L17564:
	mov	r5, r0
.L17566:
	cmp	r5, #0
	beq	.L17588
	ldr	r2, [r7, #8]
	mov	r0, r5
	mov	r1, r10
	bl	Func_8016670
	mov	r3, #0
	mov	r8, r0
	str	r0, [r7, #4]
	str	r3, [r7, #8]
	cmp	r0, #0
	bne	.L17588
	mov	r0, r5
	mov	r1, #1
	bl	CloseUIBox
.L17588:
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end PrintBattleText


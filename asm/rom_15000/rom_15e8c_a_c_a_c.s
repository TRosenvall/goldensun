	.include "macros.inc"
	.include "gba.inc"

@ OpenWindow
@ r0 = x column, r1 = y row, r2 = width, r3 = height, arg5 on the stack = flags.
@ All geometry in TILES. Returns the window record, or 0 when all eight slots
@ are taken.
@ Scans the pool at [iwram_1e8c]+0x500 (8 records, stride 0x24) for a slot that
@ is free by .gcc2_compiled.'s test -- bit 0 of +0x16 clear AND the signed halfword
@ at +0x1A zero -- then fills it in:
@     +0x0C,+0x0E = x, y     +0x08,+0x0A = width, height
@     +0x00,+0x04 cleared    +0x10 = 1     +0x14 = 0     +0x16 = 1 | flags
@ Func_173ac resets the global text style so the new window starts clean, then
@ selected flag bits are copied into +0x16 and Func_16230 paints it.
@ The standard message box is CreateUIBox(0, 0xF, 0x1E, 6, ...).
.thumb_func_start CreateUIBox  @ 0x080162d4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r14, r3
	ldr	r3, =iwram_3001e8c
	mov	r12, r2
	ldr	r3, [r3]
	mov	r2, #0xa0
	lsl	r2, #3
	add	r4, r3, r2
	ldrh	r2, [r4, #0x16]
	mov	r3, #1
	and	r3, r2
	mov	r7, r1
	ldr	r6, [sp, #0x14]
	mov	r5, #0
	mov	r1, #0
	b	.L1630a

	.pool_aligned

.L162fc:
	add	r1, #1
	add	r4, #0x24
	cmp	r1, #8
	beq	.L16318
	ldrh	r2, [r4, #0x16]
	mov	r3, #1
	and	r3, r2
.L1630a:
	cmp	r3, #0
	bne	.L162fc
	mov	r2, #0x1a
	ldrsh	r3, [r4, r2]
	cmp	r3, #0
	bne	.L162fc
	mov	r5, r4
.L16318:
	cmp	r5, #0
	beq	.L163e0
	mov	r3, #0
	mov	r8, r3
	mov	r2, r12
	mov	r3, r14
	strh	r7, [r5, #0xe]
	strh	r2, [r5, #8]
	strh	r3, [r5, #0xa]
	mov	r2, r8
	mov	r3, r8
	mov	r7, #1
	strh	r0, [r5, #0xc]
	strh	r3, [r5, #0x14]
	str	r2, [r5]
	str	r4, [r5, #4]
	strh	r7, [r5, #0x10]
	strh	r7, [r5, #0x16]
	bl	Func_80173ac
	mov	r0, #8
	mov	r3, r6
	and	r3, r0
	cmp	r3, #0
	beq	.L16352
	ldrh	r3, [r5, #0x16]
	ldr	r2, =8
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L16352:
	mov	r3, #0x20
	and	r3, r6
	cmp	r3, #0
	beq	.L16362
	ldrh	r3, [r5, #0x16]
	ldr	r2, =0x20
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L16362:
	mov	r3, #0x40
	and	r3, r6
	cmp	r3, #0
	beq	.L16380
	ldrh	r3, [r5, #0x16]
	ldr	r2, =0x40
	orr	r3, r2
	strh	r3, [r5, #0x16]
	b	.L16380

	.pool_aligned

.L16380:
	mov	r3, #0x80
	and	r3, r6
	cmp	r3, #0
	beq	.L16390
	ldrh	r3, [r5, #0x16]
	ldr	r2, =0x80
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L16390:
	mov	r1, #0x80
	lsl	r1, #1
	mov	r3, r6
	and	r3, r1
	cmp	r3, #0
	beq	.L163a4
	ldrh	r2, [r5, #0x16]
	mov	r3, r1
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L163a4:
	mov	r3, #2
	and	r3, r6
	cmp	r3, #0
	beq	.L163ce
	ldrh	r3, [r5, #0x16]
	ldr	r2, =2
	orr	r3, r2
	mov	r2, r8
	strh	r3, [r5, #0x16]
	strh	r2, [r5, #0x18]
	b	.L163c4

	.pool_aligned

.L163c4:
	strh	r7, [r5, #0x1a]
	mov	r0, r5
	bl	Func_8016230
	b	.L163e0
.L163ce:
	mov	r3, #7
	strh	r0, [r5, #0x1a]
	strh	r3, [r5, #0x18]
	mov	r0, r5
	bl	UIBox_WaitAnim
	mov	r0, #1
	bl	WaitFrames
.L163e0:
	mov	r0, r5
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end CreateUIBox

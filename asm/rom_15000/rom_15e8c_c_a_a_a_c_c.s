	.include "macros.inc"
	.include "gba.inc"

@ FillTilemapRun
@ r0 = destination, r1 = tile entry, r2 = count. DMA3-fills `count` halfwords
@ with the same tile entry and returns the advanced destination pointer, so
@ callers can chain runs. A count of 0 or less writes nothing and returns the
@ pointer unchanged. The source is a stack halfword with the DMA source-fixed
@ bit set (control 0x81000000).
.thumb_func_start Func_80170c4  @ 0x080170c4
	push	{r5, lr}
	mov	r4, r2
	sub	sp, #4
	mov	r5, r0
	cmp	r4, #0
	ble	.L170e8
	mov	r0, sp
	mov	r2, #0x81
	add	r0, #2
	lsl	r2, #24
	strh	r1, [r0]
	ldr	r3, =REG_DMA3SAD
	mov	r1, r5
	orr	r2, r4
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	lsl	r3, r4, #1
	add	r5, r3
.L170e8:
	mov	r0, r5
	add	sp, #4
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_80170c4

@ DrawWindowFrame
@ r0 = x column, r1 = y row, r2 = width, r3 = height, in tiles.
@ Paints a window's border and interior into the tilemap at
@ [iwram_1e8c] + (row*32 + column)*2, emitting each span with Func_170c4.
@ Widths or heights of 1 or less take an early exit, so degenerate windows draw
@ nothing rather than corrupting the map.
@ Body traced structurally; the individual corner and edge tile indices are not
@ yet documented.
.thumb_func_start Func_80170f8  @ 0x080170f8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r7, r3
	ldr	r3, =iwram_3001e8c
	ldr	r3, [r3]
	mov	r11, r3
	lsl	r3, r1, #5
	add	r3, r0
	mov	r8, r2
	lsl	r3, #1
	mov	r2, r11
	add	r5, r3, r2
	mov	r3, r8
	cmp	r3, #1
	bhi	.L17122
	b	.L17230
.L17122:
	cmp	r7, #1
	bhi	.L17128
	b	.L17230
.L17128:
	cmp	r3, #0x1e
	bls	.L1712e
	b	.L17230
.L1712e:
	cmp	r7, #0x1e
	bls	.L17134
	b	.L17230
.L17134:
	mov	r3, r7
	mov	r2, r8
	bl	Func_801e260
	ldr	r3, =0xea4
	add	r3, r11
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L1714a
	ldr	r3, .L17170	@ 0xf01c
	b	.L1714c
.L1714a:
	ldr	r3, .L17174	@ 0xf010
.L1714c:
	strh	r3, [r5]
	add	r5, #2
	mov	r2, #2
	neg	r2, r2
	add	r2, r8
	mov	r0, r5
	ldr	r1, =0xf011f011
	mov	r10, r2
	bl	Func_80170c4
	ldr	r3, =0xea4
	add	r3, r11
	ldrb	r3, [r3]
	mov	r5, r0
	cmp	r3, #0
	beq	.L17188
	ldr	r3, .L17178	@ 0xf41c
	b	.L1718a

	.align	2, 0
.L17170:
	.word	0xf01c
.L17174:
	.word	0xf010
.L17178:
	.word	0xf41c
	.pool

.L17188:
	ldr	r3, =0xf012
.L1718a:
	strh	r3, [r5]
	add	r5, #2
	mov	r3, #0x20
	mov	r2, r8
	sub	r3, r2
	lsl	r3, #1
	mov	r6, #1
	sub	r7, #1
	add	r5, r3
	cmp	r6, r7
	bcs	.L171d8
	mov	r9, r3
.L171a2:
	ldr	r3, =0xf016
	mov	r2, r8
	strh	r3, [r5]
	add	r5, #2
	cmp	r2, #2
	beq	.L171ca
	mov	r0, r5
	ldr	r1, =0xf020f020
	mov	r2, r10
	bl	Func_80170c4
	b	.L171c8

	.pool_aligned

.L171c8:
	mov	r5, r0
.L171ca:
	ldr	r3, .L171e8	@ 0xf017
	add	r6, #1
	strh	r3, [r5]
	add	r5, #2
	add	r5, r9
	cmp	r6, r7
	bcc	.L171a2
.L171d8:
	ldr	r3, =0xea4
	add	r3, r11
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L171f4
	ldr	r3, .L171ec	@ 0xf81c
	b	.L171f6

	.align	2, 0
.L171e8:
	.word	0xf017
.L171ec:
	.word	0xf81c
	.pool

.L171f4:
	ldr	r3, .L17214	@ 0xf013
.L171f6:
	strh	r3, [r5]
	add	r5, #2
	mov	r0, r5
	ldr	r1, =0xf014f014
	mov	r2, r10
	bl	Func_80170c4
	ldr	r3, =0xea4
	add	r3, r11
	ldrb	r3, [r3]
	mov	r5, r0
	cmp	r3, #0
	beq	.L17224
	ldr	r3, .L17218	@ 0xfc1c
	b	.L17226

	.align	2, 0
.L17214:
	.word	0xf013
.L17218:
	.word	0xfc1c
	.pool

.L17224:
	ldr	r3, =0xf015
.L17226:
	strh	r3, [r5]
	ldr	r2, =0xea3
	mov	r3, #1
	add	r2, r11
	strb	r3, [r2]
.L17230:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80170f8

@ SaveTilemapRect
@ r0 = x column, r1 = y row, r2 = width, r3 = height, arg5 = destination.
@ The counterpart to ClearUIRegion: copies the current tilemap contents of a
@ rectangle out to a buffer so the window that is about to cover it can restore
@ them later. Same (row*32 + column)*2 indexing, same early-out when width or
@ height is 1 or less.
.thumb_func_start Func_8017248  @ 0x08017248
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r14, r3
	ldr	r3, =iwram_3001e8c
	lsl	r1, #5
	ldr	r3, [r3]
	add	r1, r0
	sub	sp, #4
	lsl	r1, #1
	mov	r6, r2
	ldr	r4, [sp, #0x24]
	str	r3, [sp]
	add	r1, r3
	cmp	r6, #1
	bls	.L17350
	mov	r2, r14
	cmp	r2, #1
	bls	.L17350
	cmp	r6, #0x1e
	bhi	.L17350
	cmp	r2, #0x1e
	bhi	.L17350
	add	r1, #0x40
	cmp	r4, #0
	bne	.L172e8
	mov	r3, #1
	neg	r3, r3
	add	r3, r14
	mov	r5, #1
	mov	r8, r3
	cmp	r5, r8
	bcs	.L17334
	mov	r3, #0x20
	sub	r3, r6
	lsl	r3, #1
	sub	r2, r6, #1
	mov	r10, r3
	ldr	r3, =0x127
	mov	r12, r2
	mov	r9, r12
	mov	r11, r3
.L172a4:
	mov	r4, #1
	add	r1, #2
	cmp	r4, r9
	bcs	.L172cc
	mov	r3, r14
	sub	r3, #2
	mov	r2, r3
	ldr	r7, .L172d8	@ 0xfff
	mov	r3, r11
	ldr	r6, .L172dc	@ 0xf000
	add	r0, r3, r5
.L172ba:
	mov	r3, r0
	and	r3, r7
	orr	r3, r6
	add	r4, #1
	strh	r3, [r1]
	add	r0, r2
	add	r1, #2
	cmp	r4, r12
	bcc	.L172ba
.L172cc:
	add	r1, #2
	add	r5, #1
	add	r1, r10
	cmp	r5, r8
	bcc	.L172a4
	b	.L17334

	.align	2, 0
.L172d8:
	.word	0xfff
.L172dc:
	.word	0xf000
	.pool

.L172e8:
	mov	r0, r14
	mov	r5, #1
	sub	r0, #1
	cmp	r5, r0
	bcs	.L17334
	mov	r3, #0x20
	sub	r3, r6
	ldr	r2, =0x127
	lsl	r3, #1
	mov	r10, r3
	mov	r9, r2
	mov	r8, r0
.L17300:
	mov	r4, #0
	cmp	r4, r6
	bcs	.L1732c
	mov	r3, #2
	neg	r3, r3
	add	r3, r14
	ldr	r2, =0xfff
	mov	r11, r3
	ldr	r7, .L17340	@ 0xf000
	mov	r3, r9
	mov	r12, r2
	add	r0, r5, r3
.L17318:
	mov	r3, r0
	mov	r2, r12
	and	r3, r2
	orr	r3, r7
	add	r4, #1
	strh	r3, [r1]
	add	r0, r11
	add	r1, #2
	cmp	r4, r6
	bcc	.L17318
.L1732c:
	add	r5, #1
	add	r1, r10
	cmp	r5, r8
	bcc	.L17300
.L17334:
	ldr	r3, [sp]
	ldr	r1, =0xea3
	add	r2, r3, r1
	mov	r3, #1
	strb	r3, [r2]
	b	.L17350

	.align	2, 0
.L17340:
	.word	0xf000
	.pool

.L17350:
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8017248

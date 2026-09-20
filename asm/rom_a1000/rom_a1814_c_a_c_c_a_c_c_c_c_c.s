	.include "macros.inc"
	.include "gba.inc"

@ BuildPartyRow
@ r0..r3 = ignored. Builds the standard party strip: Func_a1814 makes the header
@ window and cursors, Func_a1870 spawns the actors with spacing 8 at offset
@ (2, 2). Sets the y of members 1..3 (state+0x146, +0x148, +0x14A) to 0x1E,
@ clears +0x20, +0x24, +0x28, +0x110 and +0x111, opens the bottom window
@ (0, 0x11, 0x1E, 3) into state+0x2C, and seeds +0x112 = 8 and +0x113 = 2 --
@ eight columns, two visible rows.
.thumb_func_start Func_80a3354  @ 0x080a3354
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001f2c
	ldr	r7, [r3]
	mov	r0, r7
	sub	sp, #4
	bl	Func_80a1814
	mov	r3, #0
	str	r3, [sp]
	mov	r1, #2
	mov	r2, #2
	mov	r3, #8
	bl	Func_80a1870
	mov	r0, #0xa5
	lsl	r0, #1
	ldr	r1, .La33b0	@ 0x1e
	mov	r2, #3
	add	r3, r7, r0
.La337a:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La337a
	mov	r5, #0
	str	r5, [r7, #0x28]
	str	r5, [r7, #0x24]
	mov	r6, #2
	mov	r1, #0x11
	mov	r2, #0x1e
	mov	r3, #3
	mov	r0, #0
	str	r6, [sp]
	bl	_CreateUIBox
	mov	r2, #0x88
	str	r0, [r7, #0x2c]
	lsl	r2, #1
	ldr	r0, =0x111
	add	r3, r7, r2
	str	r5, [r7, #0x20]
	strb	r5, [r3]
	add	r3, r7, r0
	strb	r5, [r3]
	b	.La33bc

	.align	2, 0
.La33b0:
	.word	0x1e
	.pool

.La33bc:
	mov	r3, #0x89
	lsl	r3, #1
	add	r2, r7, r3
	add	r0, #2
	mov	r3, #8
	strb	r3, [r2]
	add	r3, r7, r0
	strb	r6, [r3]
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a3354

@ CreateListSprites
@ r0 = state block, r1 = y. Fills the 32 node slots at state+0x48 with panel
@ sprites from _Func_1eb64 at priority 0xA8: the first eight take tile source
@ 0xF8, the remaining twenty-four take 0x100. Three separate loops rather than
@ one because the tile source changes at index 8.
.thumb_func_start Func_80a33d4  @ 0x080a33d4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r8, r0
	mov	r3, #0xa8
	mov	r6, r8
	sub	sp, #4
	mov	r7, r1
	mov	r5, #0
	mov	r10, r3
	add	r6, #0x48
.La33ec:
	mov	r3, r10
	str	r3, [sp]
	mov	r1, r5
	mov	r0, #2
	mov	r2, r7
	mov	r3, #0xf8
	bl	_Func_801eb64
	add	r5, #1
	stmia	r6!, {r0}
	cmp	r5, #7
	ble	.La33ec
	mov	r3, #0xa8
	mov	r6, r8
	mov	r5, #8
	mov	r10, r3
	add	r6, #0x68
.La340e:
	mov	r3, r10
	str	r3, [sp]
	mov	r3, #0x80
	mov	r1, r5
	mov	r0, #2
	mov	r2, r7
	lsl	r3, #1
	bl	_Func_801eb64
	add	r5, #1
	stmia	r6!, {r0}
	cmp	r5, #0xf
	ble	.La340e
	mov	r3, #0xa8
	mov	r6, r8
	mov	r5, #0x10
	mov	r10, r3
	add	r6, #0x88
.La3432:
	mov	r3, r10
	str	r3, [sp]
	mov	r3, #0x80
	mov	r1, r5
	mov	r0, #2
	mov	r2, r7
	lsl	r3, #1
	bl	_Func_801eb64
	add	r5, #1
	stmia	r6!, {r0}
	cmp	r5, #0x1f
	ble	.La3432
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a33d4

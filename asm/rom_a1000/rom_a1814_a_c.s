	.include "macros.inc"
	.include "gba.inc"

@ PlaceCursor
@ r0 = x offset in pixels, r1 = y offset. Positions the cursor sprite at
@ state+0x14 inside the window at state+0x10: the window's tile column and row
@ (+0x0C and +0x0E) are scaled by 8, the caller's offset added, and 8 more for
@ the border. A small per-axis wobble comes from the byte tables .Laf294 and
@ .Laf29d indexed by (iwram_1e40 >> 1) & 7 -- iwram_1e40 is the free-running
@ frame counter, so the cursor bobs on an eight-frame cycle.
@ The x also goes into the low 9 bits of +0x16, which is the OBJ attribute the
@ hardware reads.
.thumb_func_start Func_80a1a40  @ 0x080a1a40
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001f2c
	ldr	r5, [r3]
	ldr	r3, =iwram_3001e40
	mov	r14, r3
	ldr	r3, [r3]
	mov	r6, #7
	lsr	r3, #1
	mov	r12, r6
	and	r3, r6
	ldr	r2, =.Laf294
	ldr	r6, [r5, #0x10]
	ldrb	r2, [r2, r3]
	ldrh	r3, [r6, #0xc]
	add	r2, r0
	lsl	r3, #3
	ldr	r4, [r5, #0x14]
	add	r2, r3
	ldr	r5, .La1a9c	@ 0xffff
	add	r2, #8
	ldr	r3, .La1aa0	@ 0x1ff
	strh	r2, [r4, #6]
	and	r2, r5
	ldrh	r0, [r4, #0x16]
	and	r2, r3
	ldr	r3, =0xfffffe00
	and	r3, r0
	orr	r3, r2
	mov	r0, r14
	strh	r3, [r4, #0x16]
	ldr	r3, [r0]
	ldr	r2, =.Laf29d
	mov	r0, r12
	lsr	r3, #1
	and	r3, r0
	ldrb	r3, [r2, r3]
	ldrh	r2, [r6, #0xe]
	add	r3, r1
	lsl	r2, #3
	add	r3, r2
	add	r3, #8
	strh	r3, [r4, #8]
	and	r3, r5
	strb	r3, [r4, #0x14]
	b	.La1ab8

	.align	2, 0
.La1a9c:
	.word	0xffff
.La1aa0:
	.word	0x1ff
	.pool

.La1ab8:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80a1a40

@ GlideCursorTo
@ r0 = target x, r1 = target y, both in the same offsets Func_a1a40 uses.
@ Moves the cursor there over two frames rather than snapping: the per-frame
@ step is (target*16 - current*16 + 1) / 2 in 1/16-pixel units, applied twice
@ with a WaitFrames(1) between. The 0x40 biases and the 0x38 subtraction cancel
@ out; they exist so the intermediate arithmetic stays positive.
@
@ When state+0x222 is non-zero it clears that flag and returns immediately --
@ that is the "snap, do not glide" request a screen raises when it has just
@ redrawn everything and an animation would look wrong.
.thumb_func_start Func_80a1ac0  @ 0x080a1ac0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	ldr	r2, =0x222
	mov	r10, r3
	add	r2, r10
	ldrh	r3, [r2]
	mov	r8, r1
	mov	r1, #2
	sub	sp, #4
	mov	r9, r1
	cmp	r3, #0
	beq	.La1af4
	mov	r3, #0
	strh	r3, [r2]
	b	.La1bb6

	.pool_aligned

.La1af4:
	mov	r2, r10
	ldr	r7, [r2, #0x14]
	ldrh	r3, [r7, #0x16]
	lsl	r3, #23
	lsr	r3, #23
	add	r3, #0x40
	ldrb	r2, [r7, #0x14]
	strh	r3, [r7, #6]
	add	r2, #0x40
	strh	r2, [r7, #8]
	ldrh	r2, [r7, #6]
	mov	r3, #0x40
	add	r8, r3
	mov	r3, r2
	sub	r3, #8
	add	r0, #0x40
	cmp	r3, #0
	ble	.La1b1e
	ldr	r1, =0xfff8
	add	r3, r2, r1
	strh	r3, [r7, #6]
.La1b1e:
	ldrh	r6, [r7, #8]
	mov	r3, r6
	sub	r3, #8
	cmp	r3, #0
	ble	.La1b30
	ldr	r2, =0xfff8
	add	r3, r6, r2
	strh	r3, [r7, #8]
	ldrh	r6, [r7, #8]
.La1b30:
	ldrh	r5, [r7, #6]
	lsl	r0, #4
	lsl	r5, #4
	sub	r0, r5
	mov	r1, #2
	add	r0, #1
	bl	__divsi3
	mov	r3, r8
	mov	r11, r0
	lsl	r6, #4
	lsl	r0, r3, #4
	sub	r0, r6
	add	r0, #1
	mov	r1, #2
	bl	__divsi3
	ldr	r4, .La1b88	@ 0xffff
	mov	r8, r0
.La1b56:
	mov	r2, r10
	ldr	r0, [r2, #0x10]
	ldrh	r3, [r0, #0xc]
	add	r5, r11
	lsl	r3, #3
	asr	r1, r5, #4
	add	r1, r3
	sub	r1, #0x38
	ldr	r3, .La1b8c	@ 0x1ff
	strh	r1, [r7, #6]
	and	r1, r4
	and	r1, r3
	ldr	r2, .La1b90	@ 0xfffffe00
	ldrh	r3, [r7, #0x16]
	and	r3, r2
	orr	r3, r1
	strh	r3, [r7, #0x16]
	ldrh	r3, [r0, #0xe]
	add	r6, r8
	lsl	r3, #3
	asr	r2, r6, #4
	add	r2, r3
	mov	r3, #1
	b	.La1b98

	.align	2, 0
.La1b88:
	.word	0xffff
.La1b8c:
	.word	0x1ff
.La1b90:
	.word	0xfffffe00
	.pool

.La1b98:
	neg	r3, r3
	sub	r2, #0x38
	add	r9, r3
	strh	r2, [r7, #8]
	mov	r1, r9
	and	r2, r4
	strb	r2, [r7, #0x14]
	cmp	r1, #0
	beq	.La1bb6
	mov	r0, #1
	str	r4, [sp]
	bl	WaitFrames
	ldr	r4, [sp]
	b	.La1b56
.La1bb6:
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a1ac0

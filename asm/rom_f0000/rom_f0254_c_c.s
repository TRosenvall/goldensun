	.include "macros.inc"
	.include "gba.inc"

@ FeedNextCreditLine
@ The per-frame task Func_f0678 registers at sort key 0xC80. Watches the scroll
@ position and, whenever it crosses into a new 8-pixel row, renders the next
@ credit line into the row that has just scrolled off the top.
@
@ The line index is `(row + 0x10) & 0x1F`, so the text bitmap is a 32-row ring
@ buffer half a screen ahead of what is showing. The string comes from the
@ pointer table .Lf1220 and goes through Func_f07f0 with alignment 1 (centred).
@ Does nothing while ewram_4c04 is set.
.thumb_func_start Func_80f0614  @ 0x080f0614
	push	{r5, r6, lr}
	ldr	r5, =ewram_2004c04
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	bne	.Lf0662
	ldr	r3, =ewram_2004c00
	mov	r6, #0
	ldrsh	r2, [r3, r6]
	ldrh	r4, [r3]
	mov	r3, r2
	cmp	r2, #0
	bge	.Lf0630
	add	r3, r2, #7
.Lf0630:
	ldr	r0, =ewram_2004c08
	asr	r1, r3, #3
	mov	r6, #0
	ldrsh	r3, [r0, r6]
	cmp	r3, #0
	bge	.Lf063e
	add	r3, #7
.Lf063e:
	asr	r3, #3
	cmp	r1, r3
	beq	.Lf0662
	ldr	r2, =.Lf1220
	lsl	r3, r1, #2
	strh	r4, [r0]
	ldr	r0, [r2, r3]
	mov	r3, r1
	mov	r2, #0x1f
	add	r3, #0x10
	and	r3, r2
	lsl	r1, r3, #1
	add	r1, r3
	lsl	r1, #3
	mov	r2, #1
	bl	Func_80f07f0
	strh	r0, [r5]
.Lf0662:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80f0614

@ BuildCreditText
@ Takes no arguments. Sets the scrolling text up. Takes a 0x400-byte OAM buffer
@ at ewram_4c0c, blanks the text bitmap at 0x6010000 (0x1800 words) and sets its
@ palette entry at 0x6016000 to 0x11111111.
@
@ It then fills the OAM buffer with four groups of sprite pairs -- three groups
@ of eight at fixed attributes, then the 15 x 6 grid the text itself lives in,
@ then eight more -- zeroes the scroll state at ewram_4c00, ewram_4c04 and
@ ewram_4c08, and registers Func_f0538 at 0x480 and Func_f0614 at 0xC80.
@
@ Finally it pre-renders the first 32 lines from .Lf1220 through Func_f07f0, so
@ the roll starts with a full screen of text rather than scrolling in from
@ nothing.
.thumb_func_start Func_80f0678  @ 0x080f0678
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r0, #0x80
	lsl	r0, #3
	sub	sp, #4
	bl	Func_8004970
	ldr	r5, =ewram_2004c0c
	mov	r4, sp
	mov	r3, #0
	str	r0, [r5]
	str	r3, [r4]
	mov	r0, r4
	ldr	r3, =REG_DMA3SAD
	ldr	r1, =0x6010000
	ldr	r2, =0x85001800
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =0x11111111
	mov	r0, r4
	str	r3, [r4]
	ldr	r1, =0x6016000
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85000040
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r5, [r5]
	mov	r1, #0xc0
	ldr	r0, =0x80004000
	mov	r6, #0
	lsl	r1, #2
.Lf06ba:
	lsl	r2, r6, #21
	mov	r3, r5
	orr	r2, r0
	stmia	r3!, {r2}
	add	r6, #1
	str	r1, [r3]
	add	r5, #8
	cmp	r6, #7
	bls	.Lf06ba
	mov	r1, #0xc0
	ldr	r0, =0x80004088
	mov	r6, #0
	lsl	r1, #2
.Lf06d4:
	lsl	r2, r6, #21
	mov	r3, r5
	orr	r2, r0
	stmia	r3!, {r2}
	add	r6, #1
	str	r1, [r3]
	add	r5, #8
	cmp	r6, #7
	bls	.Lf06d4
	mov	r1, #0xc0
	ldr	r0, =0x40004098
	mov	r6, #0
	lsl	r1, #2
.Lf06ee:
	lsl	r2, r6, #21
	mov	r3, r5
	orr	r2, r0
	stmia	r3!, {r2}
	add	r6, #1
	str	r1, [r3]
	add	r5, #8
	cmp	r6, #7
	bls	.Lf06ee
	mov	r2, #0x10
	mov	r14, r2
	ldr	r3, =0x40004000
	mov	r2, #0x80
	lsl	r2, #14
	mov	r6, #0
	mov	r7, #0
	mov	r10, r3
	mov	r8, r2
.Lf0712:
	mov	r0, #0xc0
	add	r3, r7, r6
	mov	r4, #0
	mov	r12, r14
	lsl	r0, #13
	lsl	r1, r3, #3
.Lf071e:
	mov	r3, r12
	orr	r3, r0
	mov	r2, r10
	orr	r3, r2
	mov	r2, r5
	stmia	r2!, {r3}
	add	r4, #1
	str	r1, [r2]
	add	r5, #8
	add	r0, r8
	add	r1, #4
	cmp	r4, #5
	bls	.Lf071e
	mov	r3, #8
	add	r6, #1
	add	r14, r3
	add	r7, #2
	cmp	r6, #0xf
	bls	.Lf0712
	mov	r2, #0xc0
	ldr	r1, =0xc000c0
	mov	r6, #0
	lsl	r2, #2
.Lf074c:
	mov	r3, r5
	stmia	r3!, {r1}
	add	r6, #1
	str	r2, [r3]
	add	r5, #8
	cmp	r6, #7
	bls	.Lf074c
	ldr	r2, .Lf0784	@ 0
	ldr	r3, =ewram_2004c00
	strh	r2, [r3]
	ldr	r3, =ewram_2004c08
	strh	r2, [r3]
	ldr	r3, =ewram_2004c04
	mov	r1, #0x90
	strh	r2, [r3]
	lsl	r1, #3
	ldr	r0, =Func_80f0538
	bl	StartTask
	mov	r1, #0xc8
	ldr	r0, =Func_80f0614
	lsl	r1, #4
	bl	StartTask
	ldr	r7, =.Lf1220
	mov	r6, #0
	mov	r5, #0
	b	.Lf07d0

	.align	2, 0
.Lf0784:
	.word	0
	.pool

.Lf07d0:
	mov	r1, r5
	ldr	r0, [r7]
	mov	r2, #1
	add	r6, #1
	bl	Func_80f07f0
	add	r5, #0x18
	cmp	r6, #0x1f
	bls	.Lf07d0
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80f0678

@ RenderCreditLine
@ r0 = a NUL-terminated ASCII string, r1 = destination row, r2 = alignment
@ (1 centred, 2 right, anything else left). Returns 0, or -1 for a null string.
@
@ THE CREDITS HAVE THEIR OWN FONT. This does not touch rom_15000's Huffman text
@ system at all -- it rasterises 1bpp glyphs straight out of two tables:
@
@     .Lf1770   the glyph bitmaps, 8 bytes each, 8x8 and 1bpp
@     .Lf11bd   the advance width per glyph, one byte each
@
@ Both are indexed by `character - 0x20`, and characters below 0x20 are skipped
@ entirely, so this is plain ASCII from space upward. That is why the credits are
@ English-only.
@
@ It measures the string first to work out the left edge (0xC0 is the field
@ width), then draws each glyph into a 0x900-byte scratch TWICE: colour 1 at an
@ offset of 0x101 -- one pixel right and one row down -- and colour 0x0F at the
@ glyph position. That offset pair is a drop shadow.
@
@ The scratch is 8 bits per pixel while it is being drawn and gets packed down
@ to 4bpp in place (`hi << 4 | lo`) before eight 32-byte strips are copied to
@ 0x6010000 + row * 0x20.
@
@ Save bit 0x200 latches "the scratch has been used once": the first call zeroes
@ the whole buffer and sets the bit, and every call after that scrolls the
@ previous contents up with Func_1af8 and clears only the new row.
.thumb_func_start Func_80f07f0  @ 0x080f07f0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r6, #0x90
	lsl	r6, #4
	sub	sp, #0x2c
	mov	r10, r0
	mov	r0, r6
	str	r1, [sp, #8]
	mov	r7, r2
	bl	Func_8004970
	mov	r1, #0
	mov	r2, #0xc0
	mov	r3, r10
	str	r0, [sp, #4]
	str	r1, [sp]
	mov	r9, r2
	cmp	r3, #0
	bne	.Lf0826
	mov	r0, #1
	neg	r0, r0
	b	.Lf0a16
.Lf0826:
	mov	r5, #0x80
	lsl	r5, #2
	mov	r0, r5
	bl	_GetFlag
	cmp	r0, #0
	bne	.Lf0848
	ldr	r3, =Func_80008d8
	ldr	r0, [sp, #4]
	mov	r1, r6
	mov	r2, #0
	bl	_call_via_r3
	mov	r0, r5
	bl	_SetFlag
	b	.Lf086e
.Lf0848:
	ldr	r4, [sp, #4]
	mov	r5, #0x80
	lsl	r5, #4
	mov	r2, #0x80
	add	r1, r4, r5
	ldr	r3, =Func_8001af8
	lsl	r2, #1
	mov	r0, r4
	bl	_call_via_r3
	mov	r2, #0x80
	ldr	r1, [sp, #4]
	lsl	r2, #1
	add	r0, r1, r2
	ldr	r3, =Func_80008d8
	mov	r1, r5
	mov	r2, #0
	bl	_call_via_r3
.Lf086e:
	mov	r4, r10
	ldrb	r0, [r4]
	mov	r3, #0
	mov	r8, r3
	add	r4, #1
	cmp	r0, #0
	beq	.Lf0892
	ldr	r2, =.Lf11bd
.Lf087e:
	cmp	r0, #0x1f
	bls	.Lf088a
	mov	r3, r0
	sub	r3, #0x20
	ldrb	r3, [r2, r3]
	add	r8, r3
.Lf088a:
	ldrb	r0, [r4]
	add	r4, #1
	cmp	r0, #0
	bne	.Lf087e
.Lf0892:
	cmp	r7, #2
	bne	.Lf08a0
	mov	r4, r9
	mov	r1, r8
	sub	r4, r1
	str	r4, [sp]
	b	.Lf08b2
.Lf08a0:
	cmp	r7, #1
	bne	.Lf08b2
	mov	r2, r9
	mov	r4, r8
	sub	r3, r2, r4
	lsr	r2, r3, #31
	add	r3, r2
	asr	r3, #1
	str	r3, [sp]
.Lf08b2:
	mov	r4, r10
	ldrb	r0, [r4]
	mov	r1, #0
	add	r4, #1
	mov	r8, r1
	mov	r10, r4
	cmp	r0, #0
	beq	.Lf0938
.Lf08c2:
	cmp	r0, #0x1f
	bls	.Lf092c
	mov	r2, #0x20
	neg	r2, r2
	add	r2, r0
	ldr	r1, =.Lf1770
	lsl	r3, r2, #3
	add	r4, r1, r3
	mov	r14, r2
	ldr	r1, [sp]
	ldr	r2, [sp, #4]
	add	r3, r2, r1
	mov	r2, r8
	add	r1, r3, r2
	mov	r3, #0
	mov	r12, r3
	mov	r2, #1
	mov	r3, #0xf
	mov	r11, r2
	mov	r9, r3
.Lf08ea:
	ldr	r3, =0x101
	ldrb	r7, [r4]
	mov	r6, #0x80
	add	r4, #1
	mov	r5, #7
	add	r2, r1, r3
.Lf08f6:
	mov	r3, r7
	and	r3, r6
	cmp	r3, #0
	beq	.Lf0906
	mov	r3, r11
	strb	r3, [r2]
	mov	r3, r9
	strb	r3, [r1]
.Lf0906:
	sub	r5, #1
	add	r2, #1
	add	r1, #1
	lsr	r6, #1
	cmp	r5, #0
	bge	.Lf08f6
	mov	r2, #1
	add	r12, r2
	mov	r3, r12
	add	r1, #0xf8
	cmp	r3, #7
	ble	.Lf08ea
	mov	r3, #1
	cmp	r0, #0x1f
	bls	.Lf092a
	ldr	r4, =.Lf11bd
	mov	r1, r14
	ldrb	r3, [r4, r1]
.Lf092a:
	add	r8, r3
.Lf092c:
	mov	r2, r10
	ldrb	r0, [r2]
	mov	r3, #1
	add	r10, r3
	cmp	r0, #0
	bne	.Lf08c2
.Lf0938:
	mov	r4, #0x18
	mov	r2, #0x60
	mov	r10, r4
	ldr	r4, [sp, #4]
	mov	r8, r2
	mov	r6, #0x80
	mov	r3, #7
	mov	r2, #0xc0
	mov	r1, r4
	mov	r7, #0x60
	lsl	r6, #1
	mov	r12, r3
	mov	r14, r2
.Lf0952:
	cmp	r7, #0
	beq	.Lf0970
	mov	r5, r8
	mov	r2, r4
.Lf095a:
	ldrb	r3, [r2, #1]
	ldrb	r0, [r2]
	lsl	r3, #4
	orr	r0, r3
	sub	r5, #1
	strb	r0, [r1]
	add	r2, #2
	add	r4, #2
	add	r1, #1
	cmp	r5, #0
	bne	.Lf095a
.Lf0970:
	sub	r3, r1, r7
	mov	r2, r14
	add	r1, r3, r6
	sub	r3, r4, r2
	add	r4, r3, r6
	mov	r3, #1
	neg	r3, r3
	add	r12, r3
	mov	r2, r12
	cmp	r2, #0
	bge	.Lf0952
	mov	r3, r10
	cmp	r3, #0
	beq	.Lf0a0e
	ldr	r4, [sp, #8]
	ldr	r0, [sp, #4]
	lsl	r1, r4, #5
	mov	r12, r10
.Lf0994:
	ldr	r3, =0x6010000
	ldr	r4, =0x6010004
	add	r2, r1, r3
	ldr	r3, [r0]
	str	r3, [r2]
	add	r2, r1, r4
	mov	r4, #0x80
	lsl	r4, #1
	add	r3, r0, r4
	ldr	r3, [r3]
	mov	r4, #0x80
	str	r3, [r2]
	ldr	r3, =0x6010008
	lsl	r4, #2
	add	r2, r1, r3
	add	r3, r0, r4
	ldr	r3, [r3]
	mov	r4, #0xc0
	str	r3, [r2]
	ldr	r3, =0x601000c
	lsl	r4, #2
	add	r2, r1, r3
	add	r3, r0, r4
	ldr	r3, [r3]
	mov	r4, #0x80
	str	r3, [r2]
	ldr	r3, =0x6010010
	lsl	r4, #3
	add	r2, r1, r3
	add	r3, r0, r4
	ldr	r3, [r3]
	mov	r4, #0xa0
	str	r3, [r2]
	ldr	r3, =0x6010014
	lsl	r4, #3
	add	r2, r1, r3
	add	r3, r0, r4
	ldr	r3, [r3]
	mov	r4, #0xc0
	str	r3, [r2]
	ldr	r3, =0x6010018
	lsl	r4, #3
	add	r2, r1, r3
	add	r3, r0, r4
	ldr	r3, [r3]
	mov	r4, #0xe0
	str	r3, [r2]
	ldr	r3, =0x601001c
	lsl	r4, #3
	add	r2, r1, r3
	add	r3, r0, r4
	ldr	r3, [r3]
	str	r3, [r2]
	mov	r2, #1
	neg	r2, r2
	add	r12, r2
	mov	r3, r12
	add	r1, #0x20
	add	r0, #4
	cmp	r3, #0
	bne	.Lf0994
.Lf0a0e:
	ldr	r0, [sp, #4]
	bl	free
	mov	r0, #0
.Lf0a16:
	add	sp, #0x2c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80f07f0

	.section .rodata

	.global .Lf0a5c
.Lf0a5c:
	.incrom 0xf0a5c, 0xf11bd
.Lf11bd:
	.incrom 0xf11bd, 0xf1220
.Lf1220:
	.incrom 0xf1220, 0xf1770
.Lf1770:
	.incrom 0xf1770, 0xf1a64

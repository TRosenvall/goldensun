	.include "macros.inc"

@ ============================================================================
@ Text, windows and menus.
@
@ rom_15000 is the game's UI layer: it decodes compressed strings, rasterises
@ glyphs, and runs the window and menu system that every other module calls
@ into. 135 functions are exported -- more than any other module -- and the
@ hardware footprint is almost nil (31 DMA3 setups, two BG control writes), so
@ this is a service layer rather than a renderer.
@
@ TEXT is stored Huffman-compressed with an ORDER-1 CONTEXT MODEL: the tree used
@ to decode a character is selected by the character before it. Func_15430 is
@ the decoder and Func_15570 seeks it to a string by id. Both are ARM, not
@ Thumb, because they are bit-twiddling inner loops.
@
@ The decoder state is three words, passed by pointer:
@     +0x00  the previous character -- doubles as the tree selector
@     +0x04  byte pointer into the compressed stream
@     +0x08  bit buffer, refilled a word at a time
@
@ WINDOWS live in a pool of 8 records of 0x24 bytes at [iwram_1e8c] + 0x500,
@ allocated by Func_162d4. Immediately after them, at + 0x620, sit 3 message-box
@ slots of 0x28 bytes; Func_17364 reports "all idle" by checking the halfword at
@ +0x14 of each. Func_175a0(textId) is the common "show a message and block
@ until it is dismissed" entry point, and it is what rom_b5000 calls with ids
@ like 0x816 and 0x847.
@
@ Window record, 0x24 bytes. Func_162d4(x, y, w, h, flags) fills it:
@     +0x00 node list head        +0x04 node list tail cache
@     +0x08 width  in tiles       +0x0A height in tiles
@     +0x0C x column              +0x0E y row
@     +0x10 = 1                   +0x14 busy/state halfword
@     +0x16 flags, bit 0 = slot in use
@     +0x1A pending item count, signed -- Func_163ec waits on this
@     +0x1C..+0x22 geometry saved at close, in the order x, y, w, h
@ Geometry is in TILES on a 30x20 grid, not pixels. The standard message box is
@ Func_162d4(0, 0xF, 0x1E, 6, ...) -- full width, six rows, at row 15.
@ A slot is free when +0x16 == 0 AND the signed halfword at +0x1A is zero
@ (Func_17394 is that predicate).
@
@ Input comes from the globals rom_c0 maintains: iwram_1ae8 keys held,
@ iwram_1c94 keys newly pressed this frame, iwram_1b04 keys with auto-repeat.
@ Menus poll these directly. Sound is played through _Func_f9080 (103 sites).
@ Party records are reached through _Func_77394.
@ ============================================================================

@ DecodeNextCharacter
@ r0 = decoder state (3 words, see the file header). Returns the character and
@ writes the advanced state back through r0.
@ The tree is chosen by the PREVIOUS character, which is state[0] on entry:
@     HuffmanTreePointers[prev >> 8] gives {base, offsets}
@     base + offsets[prev & 0xFF] is where this character's tree starts
@ so the model is order-1 -- 'u' after 'q' costs almost nothing.
@ From there it is a standard bit walk: shift the buffer right, reload a word
@ when the sentinel falls out, branch on carry until a leaf. The leaf's value is
@ 12 bits packed nibble-aligned across two bytes read BACKWARDS from the tree
@ base, which is why the tail does two ldrb at negative offsets.
.arm_func_start Func_15430
	push	{r5, r6}
	ldm	r0, {r1, r2, r3}
	ldr	r12, =HuffmanTreePointers
	lsr	r4, r1, #8
	add	r12, r4, lsl #3
	ldm	r12, {r4, r5}
	and	r12, r1, #0xff
	add	r12, r12
	ldrh	r5, [r5, r12]
	add	r4, r5
	mov	r5, r4
	mov	r12, #1
	ands	r6, r4, #3
	beq	.L15480
	rsbs	r6, r12, r6, lsl #3
	bic	r4, #3
	ldr	r12, [r4], #4
	rrx	r12, r12
	lsr	r12, r6
	mov	r6, #0
.L15480:
	lsrs	r12, #1
	ldreq	r12, [r4], #4
	rrxeqs	r12, r12
	bcs	.L15538
	lsrs	r3, #1
	bcc	.L15480
	ldreq	r3, [r2], #4
	rrxeqs	r3, r3
	bcc	.L15480
	mov	r1, #0
.L154a8:
	lsrs	r12, #1
	bcs	.L15510
	lsrs	r12, #1
	bcs	.L154d4
	lsrs	r12, #1
	bcs	.L154d0
	lsrs	r12, #1
	bcs	.L154f0
	add	r1, #4
	b	.L154a8
.L154d0:
	add	r1, #1
.L154d4:
	addne	r6, #1
	bne	.L154a8
	ldr	r12, [r4], #4
	rrxs	r12, r12
	addcc	r1, #2
	addcs	r6, #1
	b	.L154a8
.L154f0:
	add	r1, #2
	addne	r6, #1
	bne	.L154a8
	ldr	r12, [r4], #4
	rrxs	r12, r12
	addcc	r1, #2
	addcs	r6, #1
	b	.L154a8
.L15510:
	beq	.L15524
.L15514:
	add	r6, #1
	subs	r1, #1
	bge	.L154a8
	b	.L15480
.L15524:
	ldr	r12, [r4], #4
	rrxs	r12, r12
	bcs	.L15514
	add	r1, #1
	b	.L154a8
.L15538:
	lsrs	r1, r6, #1
	add	r6, r1
	sub	r6, r5, r6
	ldrb	r5, [r6, #-1]
	ldrb	r6, [r6, #-2]
	andcs	r1, r5, #0xf
	orrcs	r1, r6, r1, lsl #8
	lslcc	r1, r5, #4
	orrcc	r1, r6, lsr #4
	stm	r0, {r1, r2, r3}
	movs	r0, r1
	pop	{r5, r6}
	bx	lr
.func_end Func_15430

@ SeekToString
@ r0 = decoder state, r1 = string id. Points the decoder at a string and primes
@ its bit buffer; the caller then pulls characters with Func_15430.
@ StringPointers[id >> 8] gives {base, lengths}. The low byte of the id is a
@ COUNT OF STRINGS TO SKIP, not an offset: it walks that many entries of a byte
@ length table, adding each to base, with 0xFF meaning "this entry continues
@ into the next byte" so strings longer than 254 bytes still encode.
@ Byte offsets are then rounded down to a word and the bit buffer is pre-shifted
@ by the discarded bits, so decoding can start mid-word.
@ state[0] is left 0, so the first character decodes under the null context.
.arm_func_start Func_15570
	lsr	r3, r1, #8
	ldr	r12, =StringPointers
	add	r12, r3, lsl #3
	ldm	r12, {r2, r4}
	ands	r1, #0xff
	beq	.L155a0
.L15588:
	ldrb	r3, [r4], #1
	add	r2, r3
	cmp	r3, #0xff
	beq	.L15588
	subs	r1, #1
	bne	.L15588
.L155a0:
	mov	r3, #1
	ands	r4, r2, #3
	beq	.L155c0
	rsbs	r4, r3, r4, lsl #3
	bic	r2, #3
	ldr	r3, [r2], #4
	rrx	r3, r3
	lsr	r3, r4
.L155c0:
	mov	r1, #0
	stm	r0, {r1, r2, r3}
	bx	lr
.func_end Func_15570

@ DrawGlyph
@ r0 = window/target record, r1 = character, r2 = x in pixels, r3 = y in
@ pixels, arg5 on the stack = the glyph source. Returns non-zero when it
@ declines to draw.
@ Characters below 0x20 or above 0x8F are rejected outright; codes above 0xFF
@ are only accepted as 0xDE and 0xDF, which are the two-byte lead-ins.
@ Width comes from Data_370d4 indexed by (character - 0x20), so that table is
@ the font's advance-width array.
@ Pixels are plotted 4bpp into VRAM at 0x6002500 + (row * stride) with the
@ nibble selected by the low bits of x -- the read-modify-write with a 0xF mask
@ shifted by (x & 3) * 4 is what makes glyphs land on arbitrary pixel columns
@ rather than tile boundaries. The 0x1100/0x3300 constants are the two ink
@ colours it alternates between for the shadow pass.
.arm_func_start Func_155d0
	push	{r5, r6, r7, r8, r9, r10, lr}
	mov	r7, r3
	sub	sp, #0x2c
	ldr	r3, [sp, #0x48]
	mov	r9, r2
	str	r0, [sp, #0x28]
	sub	r12, r1, #0x20
	ldrh	r0, [r0, #0xa]
	cmp	r12, #0x6f
	str	r0, [sp, #0x1c]
	movhi	r0, #1
	bhi	.L158dc
	cmp	r1, #0xff
	bls	.L1561c
	cmp	r3, #0
	sub	r14, r1, #0xde
	beq	.L15750
	cmp	r14, #1
	bhi	.L15750
.L1561c:
	cmp	r3, #0
	beq	.L1572c
	add	r1, r3, r1, lsl #5
	str	r1, [sp, #0x18]
	mov	r1, #0x1100
	add	r1, #0x11
	mov	r2, #0x3300
	str	r1, [sp, #0x14]
	add	r2, #0x33
	str	r2, [sp, #0x10]
	ldr	r3, =Data_370d4
	mov	r2, #0
	ldrb	r3, [r3, r12]
	cmp	r7, #0x1f
	str	r3, [sp, #0xc]
	bhi	.L15724
	mov	r10, #0x2500
	add	r10, #0x6000000
.L15664:
	mov	r14, #0
	add	r4, r2, #1
	add	r3, r7, r2
	lsr	r8, r3, #3
	and	r3, #7
	lsl	r6, r3, #1
.L1567c:
	tst	r14, #1
	lsr	r5, #4
	ldreq	r3, [sp, #0x18]
	ldreqb	r5, [r3], #1
	streq	r3, [sp, #0x18]
	and	r3, r5, #0xd
	cmp	r3, #1
	bne	.L15700
	and	r3, r5, #0xf
	cmp	r3, #1
	ldr	r12, [sp, #0x10]
	add	r1, r9, r14
	ldr	r0, [sp, #0x14]
	lsr	r2, r1, #3
	moveq	r12, r0
	cmp	r2, #0x1d
	bhi	.L15700
	ldr	r0, [sp, #0x1c]
	sub	r3, r0, #2
	mla	r0, r3, r2, r8
	and	r3, r1, #7
	orr	r3, r6, r3, lsr #2
	and	r1, #3
	lsl	r1, #2
	add	r0, r10, r0, lsl #5
	orr	r0, r3, lsl #1
	mov	r3, #0xf
	ldrh	r2, [r0]
	lsl	r3, r1
	bic	r2, r3
	and	r3, r12
	orr	r2, r3
	strh	r2, [r0]
.L15700:
	add	r14, #1
	cmp	r14, #7
	bls	.L1567c
	mov	r2, r4
	cmp	r4, #7
	bhi	.L15724
	add	r3, r7, r4
	cmp	r3, #0x1f
	bls	.L15664
.L15724:
	ldr	r0, [sp, #0xc]
	b	.L158dc
.L1572c:
	sub	r14, r1, #0xde
	add	r3, r12, r12, lsl #1
	add	r3, r12, r3, lsl #2
	ldr	r2, =Data_32224
	lsl	r3, #1
	add	r1, r3, r2
	add	r1, #2
	str	r1, [sp, #0x24]
	b	.L15768
.L15750:
	sub	r3, r1, #0x20
	ldr	r2, =Data_32224
	lsl	r3, #5
	add	r0, r3, r2
	add	r0, #8
	str	r0, [sp, #0x24]
.L15768:
	ldrh	r0, [r3, r2]
	cmp	r14, #1
	bhi	.L15788
	sub	r9, #3
	sub	r7, #2
	mov	r1, #0
	str	r1, [sp, #0x20]
	b	.L1578c
.L15788:
	str	r0, [sp, #0x20]
.L1578c:
	ldr	r2, [sp, #0x24]
	mov	r1, #0
	add	r2, #2
	str	r2, [sp, #4]
.L1579c:
	cmp	r1, #3
	moveq	r5, #0x1100
	addeq	r5, #0x11
	movne	r5, #0x5500
	addne	r5, #0x55
	ldr	r0, [sp, #0x28]
	ldrh	r3, [r0, #0x16]
	tst	r3, #0x40
	beq	.L157f8
	cmp	r1, #1
	bhi	.L157d8
	addeq	r9, #2
	beq	.L157f8
	sub	r9, #1
	b	.L157f8
.L157d8:
	cmp	r1, #2
	bne	.L157f4
	sub	r9, #1
	add	r7, #1
	b	.L157f8

	.pool

.L157f4:
	sub	r7, #1
.L157f8:
	ldr	r3, [sp, #4]
	ldr	r0, [sp, #0x24]
	mov	r2, #0
	str	r3, [sp, #8]
	cmp	r7, #0x1f
	ldrh	r12, [r0]
	add	r1, #1
	str	r1, [sp]
	lsl	r12, #16
	bhi	.L158cc
	mov	r6, #0x2500
	add	r6, #0x6000000
.L15828:
	mov	r14, #0
	add	r4, r2, #1
	add	r3, r7, r2
	lsr	r10, r3, #3
	and	r3, #7
	lsl	r8, r3, #1
.L15840:
	cmp	r12, #0
	bge	.L15890
	add	r1, r9, r14
	ldr	r0, [sp, #0x1c]
	lsr	r2, r1, #3
	sub	r3, r0, #2
	mla	r0, r3, r2, r10
	and	r3, r1, #7
	orr	r3, r8, r3, lsr #2
	and	r1, #3
	lsl	r1, #2
	add	r0, r6, r0, lsl #5
	orr	r0, r3, lsl #1
	mov	r3, #0xf
	ldrh	r2, [r0]
	lsl	r3, r1
	bic	r2, r3
	and	r3, r5
	orr	r2, r3
	strh	r2, [r0]
.L15890:
	lsls	r12, #1
	beq	.L158a4
	add	r14, #1
	cmp	r14, #0xd
	bls	.L15840
.L158a4:
	mov	r2, r4
	cmp	r4, #0xb
	bhi	.L158cc
	ldr	r1, [sp, #8]
	add	r3, r7, r4
	ldrh	r12, [r1], #2
	cmp	r3, #0x1f
	lsl	r12, #16
	str	r1, [sp, #8]
	bls	.L15828
.L158cc:
	ldr	r1, [sp]
	cmp	r1, #3
	bls	.L1579c
	ldr	r0, [sp, #0x20]
.L158dc:
	add	sp, #0x2c
	pop	{r5, r6, r7, r8, r9, r10, lr}
	bx	lr
.func_end Func_155d0

@ BuildBorderRow
@ r0 = destination halfword array, r1 = source halfword array.
@ Walks a halfword-terminated source, dispatching codes 0xF007..0xF01A through
@ two jump tables -- one used before a state flag is set and one after -- and
@ emits four identical halfwords per run into the destination.
@ Each dispatch arm sets up the same six values from (index * 8) with small
@ +1/+7/+8 variations, which is the signature of picking corner, edge and fill
@ tiles out of one tile bank: the arms differ only in WHICH corner is rounded.
@ The 0x1D bound on the inner counter is 29 columns, one short of a 30-column
@ screen row, so this builds one row of a window border at a time.
@ NOTE the two jump tables are 20 and 21 entries and mostly point at the same
@ default arm; only codes 0xF010, 0xF013, 0xF016 and 0xF01A do anything
@ distinct. Body traced structurally rather than per-instruction.
.arm_func_start Func_158e8
	push	{r5, r6, r7, r8, r9, r10, lr}
	mov	r10, r1
	mov	r1, #0
.L158f4:
	mov	r7, #0
	mov	r14, r7
	mov	r8, r7
	mov	r2, r7
	mov	r12, r2
	mov	r4, r2
	mov	r5, r2
	mov	r6, r2
	add	r0, #2
	add	r9, r1, #1
	mov	r1, r2
.L15920:
	ldrh	r3, [r10], #2
	cmp	r3, #0
	beq	.L15a8c
	cmp	r5, #0
	bne	.L159f8
	sub	r3, #0xf000
	sub	r3, #7
	cmp	r3, #0x13
	ldrls	pc, [pc, r3, lsl #2]
	b	.L159f4

	.word	.L159d8
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15998
	.word	.L15a8c
	.word	.L15a8c
	.word	.L159b8
	.word	.L15a8c
	.word	.L15a8c
	.word	.L159d8
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L159d8

.L15998:
	lsl	r14, r6, #3
	add	r12, r14, #8
	add	r8, r14, #1
	mov	r7, r14
	add	r4, r7, #7
	mov	r2, r12
	add	r5, #1
	b	.L15a8c
.L159b8:
	lsl	r14, r6, #3
	add	r12, r14, #8
	mov	r8, r14
	add	r7, r14, #1
	mov	r4, r12
	add	r2, r14, #7
	add	r5, #1
	b	.L15a8c
.L159d8:
	lsl	r14, r6, #3
	add	r12, r14, #8
	mov	r7, r14
	mov	r8, r7
	mov	r2, r12
	mov	r4, r2
	add	r5, #1
.L159f4:
	b	.L15a8c
.L159f8:
	sub	r3, #0xf000
	sub	r3, #7
	cmp	r3, #0x14
	ldrls	pc, [pc, r3, lsl #2]
	b	.L15a8c

	.word	.L15a80
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a60
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a70
	.word	.L15a8c
	.word	.L15a80
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a8c
	.word	.L15a80

.L15a60:
	add	r12, r1, #8
	add	r4, r1, #7
	mov	r2, r12
	b	.L15a8c
.L15a70:
	add	r12, r1, #8
	mov	r4, r12
	add	r2, r1, #7
	b	.L15a8c
.L15a80:
	add	r2, r1, #8
	mov	r12, r2
	mov	r4, r2
.L15a8c:
	add	r1, #8
	add	r6, #1
	cmp	r6, #0x1d
	bls	.L15920
	add	r3, r4, r8, lsl #8
	strh	r3, [r0], #2
	add	r3, r12, r14, lsl #8
	add	r0, #2
	strh	r3, [r0], #2
	add	r0, #2
	strh	r3, [r0], #2
	add	r0, #2
	strh	r3, [r0], #2
	add	r10, #4
	add	r0, #2
	strh	r3, [r0], #2
	mov	r1, r9
	add	r0, #2
	strh	r3, [r0], #2
	cmp	r1, #0x13
	add	r0, #2
	strh	r3, [r0], #2
	add	r3, r2, r7, lsl #8
	add	r0, #2
	strh	r3, [r0], #2
	bls	.L158f4
	pop	{r5, r6, r7, r8, r9, r10, lr}
	bx	lr
.func_end Func_158e8

@ DecompressNibbleStream
@ r0 = compressed source, r1 = destination byte array.
@ Decodes to one nibble per output BYTE (values 0..0xF), so the output is an
@ unpacked 4bpp image ready for Func_15d74 / Func_15e10 to pack.
@ The alphabet is a MOVE-TO-FRONT table held in two registers primed from
@ 0xFEDCBA98_76543210 -- sixteen 4-bit symbols in descending order. A decoded
@ symbol is rotated to the front (`orr r2, r4, r3, lsr #28` / `orr r3, r6, r3,
@ lsl #4`), so recently used values become the cheapest to encode next.
@ The bit reader is the same shift-and-refill idiom as Func_15430: `lsrs r12,
@ #1` with `ldreq r12, [r0], #4` and `rrxeqs` reloading a word and re-seeding
@ the sentinel. A run of set bits selects longer codes and the deeper arms
@ handle repeats, which is why one decoded symbol can emit two bytes.
.arm_func_start Func_15afc
	push	{r5, r6}
	adr	r5, .L15b30
	ldm	r5, {r2, r3}
	mov	r5, #0xf
	mov	r12, #1
	ands	r6, r0, #3
	beq	.L15b54
	rsbs	r6, r12, r6, lsl #3
	bic	r0, #3
	ldr	r12, [r0], #4
	rrx	r12, r12
	lsr	r12, r6
	b	.L15b54

.L15b30:
	.word	0xfedcba98
	.word	0x76543210

.L15b38:
	orr	r2, r4, r3, lsr #28
	orr	r3, r6, r3, lsl #4
.L15b40:
	and	r6, r3, r5
.L15b44:
	strb	r6, [r1], #1
	lsrs	r12, #1
	bcs	.L15b5c
	strb	r6, [r1], #1
.L15b54:
	lsrs	r12, #1
	bcc	.L15b40
.L15b5c:
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcc	.L15b40
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcc	.L15d10
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcc	.L15cd8
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15bac
	lsr	r4, r3, #28
	orr	r4, r3, lsl #8
	and	r6, r5, r3, lsr #24
	orr	r3, r6, r4, ror #4
	b	.L15b44
.L15bac:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15be4
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15bd8
	ror	r3, #28
	and	r6, r5, r3
	b	.L15b44
.L15bd8:
	and	r6, r5, r2
	bic	r4, r2, #0xf
	b	.L15b38
.L15be4:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15c08
	and	r6, r5, r2, lsr #4
	lsr	r4, r2, #8
	orr	r4, r2, lsl #28
	ror	r4, #24
	b	.L15b38
.L15c08:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15c50
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15c3c
	and	r6, r5, r2, lsr #8
	lsr	r4, r2, #12
	orr	r4, r2, lsl #24
	ror	r4, #20
	b	.L15b38
.L15c3c:
	and	r6, r5, r2, lsr #12
	lsr	r4, r2, #16
	orr	r4, r2, lsl #20
	ror	r4, #16
	b	.L15b38
.L15c50:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15c74
	and	r6, r5, r2, lsr #16
	lsr	r4, r2, #20
	orr	r4, r2, lsl #16
	ror	r4, #12
	b	.L15b38
.L15c74:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15cbc
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15ca8
	and	r6, r5, r2, lsr #20
	lsr	r4, r2, #24
	orr	r4, r2, lsl #12
	ror	r4, #8
	b	.L15b38
.L15ca8:
	and	r6, r5, r2, lsr #24
	lsr	r4, r2, #28
	orr	r4, r2, lsl #8
	ror	r4, #4
	b	.L15b38
.L15cbc:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15d6c
	and	r6, r5, r2, lsr #28
	lsl	r4, r2, #4
	b	.L15b38
.L15cd8:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcc	.L15cfc
	lsr	r4, r3, #24
	orr	r4, r3, lsl #12
	and	r6, r5, r3, lsr #20
	orr	r3, r6, r4, ror #8
	b	.L15b44
.L15cfc:
	lsr	r4, r3, #20
	orr	r4, r3, lsl #16
	and	r6, r5, r3, lsr #16
	orr	r3, r6, r4, ror #12
	b	.L15b44
.L15d10:
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcc	.L15d58
	lsrs	r12, #1
	ldreq	r12, [r0], #4
	rrxeqs	r12, r12
	bcs	.L15d44
	lsr	r4, r3, #12
	orr	r4, r3, lsl #24
	and	r6, r5, r3, lsr #8
	orr	r3, r6, r4, ror #20
	b	.L15b44
.L15d44:
	lsr	r4, r3, #16
	orr	r4, r3, lsl #20
	and	r6, r5, r3, lsr #12
	orr	r3, r6, r4, ror #16
	b	.L15b44
.L15d58:
	lsr	r4, r3, #8
	orr	r4, r3, lsl #28
	and	r6, r5, r3, lsr #4
	orr	r3, r6, r4, ror #24
	b	.L15b44
.L15d6c:
	pop	{r5, r6}
	bx	lr
.func_end Func_15afc

@ PackTilesMasked -- 8bpp source to 4bpp destination, transparent
@ r0 = source, r1 = destination, r2 = source stride in 8-byte units,
@ r3 = number of tile rows.
@ Reads 8 source BYTES and writes them as 8 nibbles in one word. The packing is
@ the standard gather: `orr x, x, lsr #4` then mask 0x00FF00FF, `orr x, x,
@ lsr #8` then mask 0x0000FFFF, leaving four nibbles per source word, then the
@ two halves are combined.
@ The transparency is the part worth reading slowly:
@     orr r5, r12, r12, lsr #1 ; orr r5, r5, lsr #2 ; and r5, #0x11111111
@ leaves bit 0 of each nibble set exactly where that nibble was non-zero, and
@     rsbs r5, r5, lsl #4
@ turns each such marker into a full 0xF. That is a per-pixel opacity mask, so
@ zero pixels leave the destination untouched. When the mask comes out all-ones
@ (`mvns r8, r5` setting Z) the whole word is opaque and the read-modify-write
@ is skipped entirely.
@ Func_15e10 below is the same routine without the mask -- use that one when the
@ source is known to be fully opaque.
.arm_func_start Func_15d74
	push	{r5, r6, r7, r8, r9, r10}
	adr	r10, .L15e04
	ldm	r10, {r4, r6, r7}
.L15d80:
	add	r10, r0, r2, lsl #3
.L15d84:
	add	r9, r1, #0x20
.L15d88:
	ldm	r0, {r5, r12}
	orr	r5, r5, lsr #4
	and	r5, r4
	orr	r5, r5, lsr #8
	and	r5, r6
	orr	r12, r12, lsr #4
	and	r12, r4
	orr	r12, r12, lsr #8
	orr	r12, r5, r12, lsl #16
	orr	r5, r12, r12, lsr #1
	orr	r5, r5, lsr #2
	and	r5, r7
	rsbs	r5, r5, lsl #4
	mvns	r8, r5
	ldrne	r8, [r1]
	bicne	r8, r5
	orrne	r12, r8
	str	r12, [r1], #4
	add	r0, r2, lsl #3
	cmp	r1, r9
	bne	.L15d88
	sub	r0, r2, lsl #6
	add	r0, #8
	cmp	r0, r10
	bne	.L15d84
	add	r0, r2, lsl #6
	sub	r0, r2, lsl #3
	subs	r3, #1
	bne	.L15d80
	pop	{r5, r6, r7, r8, r9, r10}
	bx	lr

.L15e04:
	.word	0xff00ff
	.word	0xffff
	.word	0x11111111
.func_end Func_15d74

@ PackTilesOpaque -- 8bpp source to 4bpp destination, no transparency
@ r0 = source, r1 = destination, r2 = source stride in 8-byte units,
@ r3 = number of tile rows.
@ Identical to Func_15d74 above -- same gather, same loop, same three constants
@ at the tail -- except that the packed word is stored directly instead of being
@ masked against the destination. Every pixel is written, including zeros.
@ Note the 0x11111111 constant is still loaded even though the mask is never
@ built, so the two routines were plainly edited from one source.
.arm_func_start Func_15e10
	push	{r5, r6, r7, r8, r9, r10}
	adr	r10, .L15e80
	ldm	r10, {r4, r6, r7}
.L15e1c:
	add	r10, r0, r2, lsl #3
.L15e20:
	add	r9, r1, #0x20
.L15e24:
	ldm	r0, {r5, r12}
	orr	r5, r5, lsr #4
	and	r5, r4
	orr	r5, r5, lsr #8
	and	r5, r6
	orr	r12, r12, lsr #4
	and	r12, r4
	orr	r12, r12, lsr #8
	orr	r12, r5, r12, lsl #16
	str	r12, [r1], #4
	add	r0, r2, lsl #3
	cmp	r1, r9
	bne	.L15e24
	sub	r0, r2, lsl #6
	add	r0, #8
	cmp	r0, r10
	bne	.L15e20
	add	r0, r2, lsl #6
	sub	r0, r2, lsl #3
	subs	r3, #1
	bne	.L15e1c
	pop	{r5, r6, r7, r8, r9, r10}
	bx	lr

.L15e80:
	.word	0xff00ff
	.word	0xffff
	.word	0x11111111
.func_end Func_15e10

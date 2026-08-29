	.include "macros.inc"

@ ============================================================================
@ The staff credit roll.
@
@ Eleven functions, entered through Func_f03f0 from overlay rom_779188. It is a
@ scrolling credit list drawn over a slideshow of 33 images that crossfade into
@ each other, and it shares almost nothing with the rest of the ROM.
@
@ THE CREDITS HAVE THEIR OWN FONT. Func_f07f0 rasterises 1bpp 8x8 glyphs out of
@ .Lf1770, advances by the widths in .Lf11bd, and indexes both by
@ `character - 0x20` -- plain ASCII. rom_15000's Huffman text system is not
@ involved at all, which is why the credits are English-only. It draws each
@ glyph twice, at an offset of 0x101 in colour 1 and at the glyph position in
@ colour 0x0F, for a drop shadow.
@
@ THE SLIDESHOW crossfades with BLDALPHA between two background pages -- one
@ showing while the next decompresses into the other. Func_f0254 clears a page,
@ Func_f02b0 loads one, and the per-word bias the decoder adds (0 or
@ 0x80808080) is what puts the two pages in different halves of the palette.
@
@ Func_f0024 IS THE SAME DECOMPRESSOR AS rom_b5000's Func_b5138, instruction for
@ instruction, differing only in that the bias is a parameter rather than a
@ constant. It relocates its own eight-entry jump table because Func_f02b0
@ DMA3-copies it into a scratch and runs it there. That makes four independent
@ copies of the run-decompressor-from-RAM idiom in the ROM.
@
@ THE SCROLL is sprite-based, not tilemap-based: Func_f0678 lays out a 15 x 6
@ grid of OBJ entries, Func_f0538 rebuilds the whole OAM every frame from the
@ scroll position at ewram_4c00, and Func_f0614 renders the next line into the
@ 32-row ring buffer at 0x6010000 as each row scrolls off. One row of 8 pixels
@ every 32 frames.
@ ============================================================================

@ DotProduct3
@ r0, r1, r2, r3 and two words on the stack: three pairs of 32-bit factors.
@ Returns `(r1*r0 + r2*r3 + sp[0]*sp[1]) >> 16` -- a three-term multiply-
@ accumulate kept at full 64-bit width in r12:r0 and only narrowed at the end,
@ so the intermediate never overflows. The 16.16 dot product the credit roll's
@ transform uses. ARM, not Thumb.
.arm_func_start Func_f0008
	smull	r12, r0, r1, r0
	smlal	r12, r0, r2, r3
	ldm	sp, {r2, r3}
	smlal	r12, r0, r2, r3
	lsl	r0, #16
	orr	r0, r12, lsr #16
	bx	lr
.func_end Func_f0008

@ DecompressToVram -- designed to be copied into RAM and run there
@ r0 = compressed source, r1 = destination, r2 = a per-word bias.
@
@ THIS IS THE SAME DECOMPRESSOR AS rom_b5000's Func_b5138, instruction for
@ instruction, with one difference: the bias added to every output word is a
@ parameter here where Func_b5138 hard-codes 0x60606060. Everything else matches
@ -- the eight-entry jump table relocated into .Lf0104 by adding the delta
@ between where it was assembled and where the code is running, the same
@ halfword-at-a-time bit stream, the same 15 x 8 x 32 block walk with the same
@ 0x800 / 0x40 destination strides.
@
@ The relocation is necessary because Func_f02b0 DMA3-copies this function into a
@ 0x230-byte scratch under tag 0x31 and calls it there. That makes four
@ independent copies of the run-decompressor-from-RAM idiom in the ROM:
@ rom_c0's Func_5340/Func_2544, rom_15000's Func_1a5a4/Func_15afc,
@ rom_b5000's Func_c08ec/Func_b5138, and this one.
.arm_func_start Func_f0024
	push	{r5, r6, r7, r8, r9, r10, r11, lr}
	mov	r10, r2
	ldr	r2, .Lf00f4	@ .Lf0124
	adr	r3, .Lf0124
	sub	r2, r3, r2
	adr	r4, .Lf0104
	mov	r5, #8
.Lf0040:
	ldr	r6, [r3], #4
	add	r6, r2
	str	r6, [r4], #4
	subs	r5, #1
	bne	.Lf0040
	mov	r11, #7
	ldrh	r3, [r0], #2
	mov	r2, #0
	mov	r5, #0
	mov	r9, #0xf
.Lf0068:
	mov	r8, #8
.Lf006c:
	mov	r7, #0x20
.Lf0070:
	bl	.Lf00f8
	lsr	r6, r5, #25
	bl	.Lf00f8
	orr	r6, r5, lsr #17
	bl	.Lf00f8
	orr	r6, r5, lsr #9
	bl	.Lf00f8
	orr	r6, r5, lsr #1
	add	r6, r10
	str	r6, [r1], #4
	bl	.Lf00f8
	lsr	r6, r5, #25
	bl	.Lf00f8
	orr	r6, r5, lsr #17
	bl	.Lf00f8
	orr	r6, r5, lsr #9
	bl	.Lf00f8
	orr	r6, r5, lsr #1
	add	r6, r10
	str	r6, [r1], #0x3c
	subs	r7, #1
	bne	.Lf0070
	sub	r1, #0x800
	add	r1, #8
	subs	r8, #1
	bne	.Lf006c
	sub	r1, #0x40
	add	r1, #0x800
	subs	r9, #1
	bne	.Lf0068
	pop	{r5, r6, r7, r8, r9, r10, r11, lr}
	bx	lr

	.word	0x80808080
.Lf00f4:
	.word	.Lf0124

.Lf00f8:
	and	r12, r3, #7
	ldr	pc, [pc, r12, lsl #2]
	nop

.Lf0104:
	.space	0x20

.Lf0124:
	.word	.Lf0144
	.word	.Lf0160
	.word	.Lf0200
	.word	.Lf01b0
	.word	.Lf0144
	.word	.Lf0188
	.word	.Lf0230
	.word	.Lf01d8

.Lf0144:
	lsr	r3, #2
	subs	r2, #2
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lf0160:
	and	r12, r3, #8
	lsrs	r3, #4
	add	r12, #8
	add	r5, r12, lsl #22
	subs	r2, #4
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lf0188:
	and	r12, r3, #8
	lsrs	r3, #4
	add	r12, #8
	sub	r5, r12, lsl #22
	subs	r2, #4
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lf01b0:
	and	r12, r11, r3, lsr #3
	lsr	r3, #6
	add	r12, #3
	add	r5, r12, lsl #25
	subs	r2, #6
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lf01d8:
	and	r12, r11, r3, lsr #3
	lsr	r3, #6
	add	r12, #3
	sub	r5, r12, lsl #25
	subs	r2, #6
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lf0200:
	mov	r12, #0xf
	ands	r12, r3, lsr #4
	add	r12, #0xb
	rsbcs	r12, #0
	add	r5, r12, lsl #25
	lsr	r3, #8
	subs	r2, #8
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lf0230:
	ror	r5, r3, #10
	and	r5, #0xfe000000
	lsr	r3, #10
	subs	r2, #0xa
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.func_end Func_f0024

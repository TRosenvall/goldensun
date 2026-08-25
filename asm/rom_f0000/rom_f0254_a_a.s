	.include "macros.inc"
	.include "gba.inc"


@ ClearBackgroundPage
@ r0 = 0 for the first page, non-zero for the second. DMA3-fills one of the two
@ image pages and its palette with a constant:
@
@     page 0   0x6000000, 0x1E00 words of 0x01010101, palette 0x5000000
@     page 1   0x6008000, 0x1E00 words of 0x81818181, palette 0x5000100
@
@ The 0x8500xxxx control words are fixed-source fills, so the single stack word
@ supplies the value for the whole transfer.
.thumb_func_start Func_80f0254  @ 0x080f0254
	push	{r5, lr}
	sub	sp, #4
	cmp	r0, #0
	bne	.Lf0268
	mov	r1, #0xc0
	mov	r5, #0xa0
	ldr	r3, =0x1010101
	lsl	r1, #19
	lsl	r5, #19
	b	.Lf026e
.Lf0268:
	ldr	r3, =0x81818181
	ldr	r1, =0x6008000
	ldr	r5, =0x5000100
.Lf026e:
	mov	r4, sp
	str	r3, [r4]
	mov	r0, r4
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85001e00
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, #0
	str	r3, [r4]
	mov	r0, r4
	ldr	r3, =REG_DMA3SAD
	mov	r1, r5
	ldr	r2, =0x85000040
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_80f0254

@ LoadBackgroundImage
@ r0 = 0 to just clear, otherwise the asset id; r1 = which page.
@ Resolves the asset with GetFile, DMA3-copies Func_f0024 into a 0x230-byte
@ scratch under tag 0x31, calls it there to decompress straight into the page's
@ VRAM, and rewinds the tag. The per-word bias handed to the decoder is 0 for
@ page 0 and 0x80808080 for page 1, which is what puts the two pages in
@ different halves of the palette.
@
@ The palette is not written directly. Instead, with interrupts masked, an entry
@ is appended to the transfer queue at ewram_2090 -- source, destination,
@ 0x84000040 -- capped at 32 pending entries, for the VBlank handler to run.
.thumb_func_start LoadGS1CreditsBG  @ 0x080f02b0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r1
	cmp	r0, #0
	bne	.Lf02c6
	mov	r0, r5
	bl	Func_80f0254
	b	.Lf0348
.Lf02c6:
	bl	GetFile
	mov	r6, r0
	cmp	r5, #0
	bne	.Lf02de
	mov	r3, #0xa0
	mov	r2, #0
	mov	r7, #0xc0
	lsl	r3, #19
	mov	r8, r2
	lsl	r7, #19
	b	.Lf02e6
.Lf02de:
	ldr	r2, =0x80808080
	ldr	r3, =0x5000100
	ldr	r7, =0x6008000
	mov	r8, r2
.Lf02e6:
	mov	r10, r3
	ldr	r5, =0x230
	mov	r0, #0x31
	mov	r1, r5
	bl	galloc_iwram
	mov	r2, #0x84
	lsr	r5, #2
	lsl	r2, #24
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_80f0024
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =gPtrs
	mov	r2, #0x80
	lsl	r2, #1
	add	r3, #0xc4
	mov	r1, r7
	add	r0, r6, r2
	ldr	r3, [r3]
	mov	r2, r8
	bl	_call_via_r3
	mov	r0, #0x31
	bl	gfree
	ldr	r1, =gDMATaskCount
	ldr	r0, =REG_IME
	ldrh	r3, [r0]
	mov	r4, r3
	strh	r0, [r0]
	ldrh	r2, [r1]
	cmp	r2, #0x1f
	bgt	.Lf0346
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r3, r1
	add	r3, #4
	add	r2, #1
	stmia	r3!, {r6}
	strh	r2, [r1]
	mov	r2, r10
	stmia	r3!, {r2}
	ldr	r2, =0x84000040
	str	r2, [r3]
.Lf0346:
	strh	r4, [r0]
.Lf0348:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end LoadGS1CreditsBG

	.include "macros.inc"
	.include "gba.inc"

@ InitSpriteSystem
@ r0=allocation mode; 3 uses the galloc_ewram allocator, anything else galloc_iwram.
@ Brings up the actor/part subsystem:
@   - allocates the actor pool (0xE00 bytes, tag 4) and the part pool
@     (0x600 bytes, tag 3), then zero-fills both by DMA3 (0x380 and 0x180 words)
@   - LoadSpritePalette initialises the dependent tables
@   - UploadSpriteGFX(0x5D, 0x80, .L12f20) reserves the shared OBJ tile range
@   - allocates 0x7C bytes (tag 0x35) and DMA-copies Func_a418 (the linear
@     bitmap -> 8x8 tile converter, rom_92b8.s) into it so the hot conversion
@     path runs from RAM rather than ROM
.thumb_func_start InitSprites  @ 0x0800bb20
	push	{r5, r6, r7, lr}
	sub	sp, #4
	cmp	r0, #3
	bne	.Lbb40
	mov	r1, #0xe0
	lsl	r1, #4
	mov	r0, #4
	bl	galloc_ewram
	mov	r1, #0xc0
	mov	r7, r0
	lsl	r1, #3
	mov	r0, #3
	bl	galloc_ewram
	b	.Lbb56
.Lbb40:
	mov	r1, #0xe0
	lsl	r1, #4
	mov	r0, #4
	bl	galloc_iwram
	mov	r1, #0xc0
	mov	r7, r0
	lsl	r1, #3
	mov	r0, #3
	bl	galloc_iwram
.Lbb56:
	mov	r6, r0
	bl	LoadSpritePalette
	mov	r5, #0
	mov	r4, sp
	str	r5, [r4]
	ldr	r3, =REG_DMA3SAD
	mov	r0, r4
	mov	r1, r7
	ldr	r2, =0x85000380
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r5, [r4]
	mov	r0, r4
	mov	r1, r6
	ldr	r2, =0x85000180
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r2, =.L12f20
	mov	r1, #0x80
	mov	r0, #0x5d
	bl	UploadSpriteGFX
	ldr	r5, =0x7c
	mov	r0, #0x35
	mov	r1, r5
	bl	galloc_iwram
	mov	r2, #0x84
	lsr	r5, #2
	lsl	r2, #24
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_800a418
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end InitSprites

@ AllocPart
@ r0=resource id. Claims the first free entry in the part pool at [iwram_1e5c]
@ (stride 0x18, up to 0x40 entries; free means the kind byte at +0x04 is 0) and
@ initialises it from the resource header: +0x00 = id, +0x08 = pixel data
@ (header+0x0C, or Func_b798 when null), +0x0C = animation table, +0x07 =
@ header[0x0A], +0x04 = header[4], +0x05 = 0, +0x10 = first animation script,
@ +0x14 = 0, +0x16 = 0xFF. Returns the part, or 0 when the pool is full or the
@ resource header is empty.
.thumb_func_start CreateSpriteLayer  @ 0x0800bbc0
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	sub	sp, #4
	mov	r4, #0
	str	r4, [sp]
	mov	r7, r0
	bl	_GetSpriteInfo
	ldr	r3, =iwram_3001e5c
	mov	r6, r0
	ldr	r2, [r3]
	ldrb	r3, [r6]
	mov	r5, #0
	ldr	r4, [sp]
	cmp	r3, #0
	beq	.Lbc38
	ldrb	r3, [r2, #4]
	mov	r1, #0
	b	.Lbbf6

	.pool_aligned

.Lbbec:
	add	r1, #1
	add	r2, #0x18
	cmp	r1, #0x3f
	bgt	.Lbbfc
	ldrb	r3, [r2, #4]
.Lbbf6:
	cmp	r3, #0
	bne	.Lbbec
	mov	r4, r2
.Lbbfc:
	cmp	r4, #0
	beq	.Lbc38
	ldr	r3, =0
	ldr	r0, [r6, #0xc]
	mov	r5, r4
	mov	r8, r3
	strh	r7, [r5]
	cmp	r0, #0
	bne	.Lbc14
	mov	r0, r7
	bl	GetCachedSpriteGFX
.Lbc14:
	ldr	r2, [r6, #0x10]
	str	r0, [r5, #8]
	str	r2, [r5, #0xc]
	ldrb	r3, [r6, #0xa]
	strb	r3, [r5, #7]
	mov	r3, #0xff
	strb	r3, [r5, #0x16]
	ldr	r3, [r2]
	str	r3, [r5, #0x10]
	mov	r3, r8
	strb	r3, [r5, #0x14]
	ldrb	r3, [r6, #4]
	strb	r3, [r5, #4]
	mov	r3, r8
	strb	r3, [r5, #5]
	b	.Lbc38

	.pool_aligned

.Lbc38:
	mov	r0, r5
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end CreateSpriteLayer

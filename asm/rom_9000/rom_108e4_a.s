	.include "macros.inc"
	.include "gba.inc"

@ LoadTilesetQuadrant
@ r0=layer (0 or 1), r1=quadrant x, r2=quadrant z, r3=tileset id,
@ [sp+0x1C]=force flag. Returns 1 if the tileset was loaded, 0 if the cache
@ already held it (only possible when force is 0).
@ The cache slot is the halfword at [iwram_1e70] + 0x338 +
@ (((layer * 2 + (z & 1)) * 2 + (x & 1)) * 2). On a miss the id is recorded,
@ a 0x400-byte buffer (tag 0x0E) is decompressed from the tileset archive at
@ [iwram_1e70]+0x110 with DecompressLZ1, and 16 blocks of 0x40 bytes are DMAd into
@ ewram_20000 at a 0x80 destination stride.
@ When force is set it additionally expands the block into the BG map at
@ 0x6004000, turning each of the 16x16 metatile indices into its 2x2 tile pair
@ via ewram_10000 / ewram_10002. The buffer is released with Func_2dd8.
.thumb_func_start Func_80108e4  @ 0x080108e4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r5, r3
	ldr	r3, =iwram_3001e70
	mov	r10, r1
	mov	r8, r2
	mov	r1, #0x88
	ldr	r2, [r3]
	lsl	r1, #1
	add	r3, r2, r1
	ldr	r6, [r3]
	mov	r1, r10
	mov	r3, #1
	and	r1, r3
	mov	r10, r1
	lsl	r0, #1
	mov	r1, r8
	and	r1, r3
	mov	r9, r0
	mov	r8, r1
	mov	r3, r9
	add	r3, r8
	lsl	r3, #1
	add	r3, r10
	lsl	r3, #1
	add	r2, r3
	ldr	r1, [sp, #0x1c]
	mov	r3, #0xce
	lsl	r3, #2
	add	r2, r3
	cmp	r1, #0
	bne	.L10932
	ldrh	r3, [r2]
	mov	r0, #0
	cmp	r5, r3
	beq	.L109be
.L10932:
	mov	r1, #0x80
	strh	r5, [r2]
	lsl	r1, #3
	mov	r0, #0xe
	bl	galloc_iwram
	lsl	r3, r5, #2
	mov	r7, r0
	ldr	r0, [r3, r6]
	mov	r1, r7
	add	r0, r6, r0
	bl	DecompressLZ1
	mov	r3, r9
	add	r3, r8
	lsl	r3, #5
	add	r3, r10
	ldr	r2, =ewram_2020000
	lsl	r3, #6
	mov	r4, r7
	add	r5, r3, r2
	mov	r6, #0
.L1095e:
	ldr	r3, =REG_DMA3SAD
	mov	r0, r4
	mov	r1, r5
	ldr	r2, =0x84000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r6, #1
	add	r4, #0x40
	add	r5, #0x80
	cmp	r6, #0xf
	bls	.L1095e
	ldr	r3, [sp, #0x1c]
	cmp	r3, #0
	beq	.L109b6
	mov	r3, r9
	add	r3, r8
	lsl	r3, #6
	add	r3, r10
	ldr	r2, =0x6004000
	lsl	r3, #5
	ldr	r5, =gBuffer
	add	r1, r3, r2
	mov	r4, r7
	mov	r6, #0
.L1098e:
	mov	r0, #0
.L10990:
	ldrh	r3, [r4]
	lsl	r3, #2
	ldrh	r2, [r3, r5]
	strh	r2, [r1]
	ldr	r2, =ewram_2010002
	add	r3, r2
	ldrh	r3, [r3]
	mov	r2, r1
	add	r2, #0x40
	add	r0, #1
	strh	r3, [r2]
	add	r1, #2
	add	r4, #4
	cmp	r0, #0xf
	bls	.L10990
	add	r6, #1
	add	r1, #0x60
	cmp	r6, #0xf
	bls	.L1098e
.L109b6:
	mov	r0, #0xe
	bl	gfree
	mov	r0, #1
.L109be:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80108e4

@ InitWorldMap
@ Takes no arguments. Brings up the world-map state block that iwram_1e70 points
@ at. Blanks BG1-BG3 in DISPCNT (mask 0xC1FF) and calls Func_3bb4(0), then
@ allocates 0x35C bytes under tag 8 and DMA zero-fills it.
@ Seeds the camera at +0xE4/+0xE8 to 0, the near bounds at +0xEC/+0xF0 to
@ 0x200000 and 0x400000, and the far bounds at +0xF4/+0xF8 to 0x1FE00000.
@ Loads resource 0xD4 as the tileset archive into +0x110 and resource 0xD6 into
@ ewram_2d000, handing the latter to Func_118d8 as tile animations. Finally sets
@ REG_BLDCNT to 0x3F9E, REG_BLDALPHA to 0x1010 and REG_BLDY to 0.
.thumb_func_start InitWorldMap  @ 0x080109e8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r1, #0x80
	lsl	r1, #19
	ldrh	r2, [r1]
	ldr	r3, =0xc1ff
	and	r3, r2
	mov	r2, #0x80
	lsl	r2, #19
	strh	r3, [r2]
	mov	r0, #0
	sub	sp, #0x20
	bl	Func_8003bb4
	mov	r1, #0xd7
	lsl	r1, #2
	mov	r0, #8
	bl	galloc_iwram
	mov	r6, #0
	mov	r7, r0
	add	r0, sp, #0x1c
	str	r6, [r0]
	ldr	r3, =REG_DMA3SAD
	mov	r1, r7
	ldr	r2, =0x850000d7
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, r7
	add	r3, #0xe4
	str	r6, [r3]
	add	r3, #4
	str	r6, [r3]
	mov	r2, r7
	mov	r3, #0x80
	add	r2, #0xec
	lsl	r3, #14
	str	r3, [r2]
	mov	r3, #0x80
	add	r2, #4
	lsl	r3, #15
	str	r3, [r2]
	mov	r3, r7
	mov	r2, #0xff
	lsl	r2, #21
	add	r3, #0xf4
	str	r2, [r3]
	add	r3, #4
	str	r2, [r3]
	str	r6, [r7, #0x10]
	ldr	r0, =_FILE_d4
	bl	GetFile
	mov	r1, #0x88
	lsl	r1, #1
	add	r3, r7, r1
	ldr	r5, =ewram_202d000
	str	r0, [r3]
	ldr	r0, =_FILE_d6
	bl	GetFile
	mov	r1, r5
	bl	DecompressLZ
	mov	r0, r5
	bl	Func_80118d8
	ldr	r2, =0x3f9e
	ldr	r3, =REG_BLDCNT
	strh	r2, [r3]
	ldr	r2, =0x1010
	add	r3, #2
	strh	r2, [r3]
	add	r3, #2
	strh	r6, [r3]
	ldr	r0, =_FILE_d5
	bl	GetFile
	ldr	r1, =gBuffer
	bl	DecompressLZ
	ldr	r0, =_FILE_d7
	bl	GetFile
	ldr	r1, =ewram_202c000
	bl	DecompressLZ
	mov	r3, #0xf8
	lsl	r3, #5
	strh	r3, [r7, #0x14]
	mov	r3, #0x80
	strb	r3, [r7, #0x16]
	ldr	r2, =0xa80a
	ldr	r3, =REG_BG3CNT
	strh	r2, [r3]
	ldr	r2, =0xaa0e
	sub	r3, #2
	strh	r2, [r3]
	ldr	r2, =0x501
	sub	r3, #2
	strh	r2, [r3]
	add	r3, #0x16
	mov	r2, #0x80
	lsl	r2, #1
	strh	r2, [r3]
	add	r3, #2
	strh	r6, [r3]
	add	r3, #2
	strh	r6, [r3]
	add	r3, #2
	mov	r1, #0x80
	lsl	r1, #1
	strh	r1, [r3]
	add	r3, #2
	str	r6, [r3]
	add	r3, #4
	str	r6, [r3]
	add	r3, #4
	strh	r2, [r3]
	add	r3, #2
	strh	r6, [r3]
	add	r3, #2
	strh	r6, [r3]
	add	r3, #2
	strh	r1, [r3]
	add	r3, #2
	str	r6, [r3]
	add	r3, #4
	str	r6, [r3]
	mov	r0, #0xc
	mov	r1, #0x4c
	bl	galloc_ewram
	ldr	r1, =0x3484
	mov	r9, r0
	mov	r0, #7
	bl	galloc_iwram
	mov	r2, #0xc
	add	r2, r9
	mov	r3, #0xc8
	mov	r8, r2
	lsl	r3, #4
	mov	r1, #0xd2
	mov	r2, #0xd3
	add	r3, r0, r3
	lsl	r1, #2
	mov	r5, #0xff
	lsl	r2, #2
	str	r3, [sp, #8]
	lsl	r5, #17
	add	r3, r7, r1
	add	r2, r7, r2
	str	r0, [sp, #0xc]
	add	r1, #0xc
	str	r5, [r3]
	str	r2, [sp, #4]
	str	r5, [r2]
	mov	r2, #0x80
	add	r3, r7, r1
	lsl	r2, #9
	str	r2, [r3]
	add	r1, #4
	mov	r2, r9
	add	r3, r7, r1
	str	r6, [r2, #0x18]
	str	r6, [r2, #0x1c]
	ldr	r2, =gPhysVec
	strh	r6, [r3]
	mov	r3, #0x78
	str	r3, [r2, #0xc]
	mov	r3, #0x60
	asr	r1, r5, #1
	mov	r0, r5
	str	r3, [r2, #0x10]
	lsl	r2, r5, #1
	bl	Func_8005258
	mov	r3, r8
	str	r6, [r3]
	str	r6, [r3, #4]
	str	r6, [r3, #8]
	bl	InitMatrixStack
	mov	r0, r8
	bl	MatrixTranslatev
	mov	r1, #0x8d
	lsl	r1, #1
	add	r1, r7, r1
	str	r1, [sp]
	ldrh	r0, [r1]
	bl	MatrixYaw
	mov	r2, #0x8c
	lsl	r2, #1
	add	r2, r7
	ldrh	r0, [r2]
	mov	r11, r2
	bl	MatrixPitch
	add	r3, sp, #0x10
	mov	r10, r3
	str	r6, [r3]
	str	r6, [r3, #4]
	str	r5, [r3, #8]
	mov	r1, r9
	ldr	r2, =Func_80009c0
	mov	r0, r10
	bl	_call_via_r2
	bl	InitMatrixStack
	mov	r0, r9
	mov	r1, r8
	bl	MatrixSetLook
	ldr	r5, =0x284
	mov	r0, #0x2e
	mov	r1, r5
	bl	galloc_iwram
	mov	r2, #0x84
	lsr	r5, #2
	lsl	r2, #24
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_800a0f8
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, r11
	ldrh	r0, [r3]
	bl	cos
	mov	r1, r11
	mov	r5, r0
	ldrh	r0, [r1]
	bl	sin
	ldr	r3, =Func_80008ac
	mov	r1, r0
	mov	r0, r5
	bl	_call_via_r3
	mov	r1, r8
	ldr	r2, [sp, #0xc]
	bl	Func_80123f4
	ldr	r3, =iwram_3001f60
	str	r6, [r3]
	mov	r1, r11
	ldrh	r3, [r1]
	ldr	r2, =iwram_3001af4
	str	r3, [r2]
	ldr	r3, =iwram_3001e40
	ldr	r2, [r3]
	mov	r3, #1
	and	r2, r3
	ldr	r1, =gPtrs
	lsl	r3, r2, #2
	add	r3, r2
	ldr	r2, [sp, #8]
	add	r1, #0xb8
	lsl	r3, #10
	ldr	r4, [r1]
	add	r3, r2, r3
	mov	r1, r8
	ldr	r2, [sp, #0xc]
	mov	r0, r9
	bl	_call_via_r4
	mov	r3, r8
	str	r6, [r3]
	str	r6, [r3, #4]
	str	r6, [r3, #8]
	bl	InitMatrixStack
	mov	r3, #0xe0
	mov	r1, r11
	lsl	r3, #8
	strh	r3, [r1]
	ldr	r2, [sp]
	strh	r6, [r2]
	bl	InitMatrixStack
	mov	r0, r8
	bl	MatrixTranslatev
	ldr	r3, [sp]
	ldrh	r0, [r3]
	bl	MatrixYaw
	mov	r1, r11
	ldrh	r0, [r1]
	bl	MatrixPitch
	mov	r2, r10
	str	r6, [r2]
	str	r6, [r2, #4]
	ldr	r1, [sp, #4]
	mov	r2, #0x80
	ldr	r3, [r1]
	lsl	r2, #9
	add	r3, r2
	mov	r1, r10
	str	r3, [r1, #8]
	mov	r0, r10
	mov	r1, r9
	ldr	r2, =Func_80009c0
	bl	_call_via_r2
	ldr	r3, =REG_MOSAIC
	strh	r6, [r3]
	mov	r3, #0x42
	mov	r1, #0x80
	lsl	r1, #19
	strh	r3, [r1]
	ldr	r3, =iwram_3001ad0
	mov	r2, #0x80
	lsl	r2, #1
	strh	r6, [r3, #4]
	strh	r6, [r3, #6]
	strh	r6, [r3, #8]
	strh	r6, [r3, #0xa]
	strh	r6, [r3, #0xc]
	strh	r6, [r3, #0xe]
	add	r3, r7, r2
	strh	r6, [r3]
	mov	r3, #0x81
	lsl	r3, #1
	add	r2, r7, r3
	mov	r3, #0x9f
	strh	r3, [r2]
	ldr	r1, =0xc85
	ldr	r0, =Func_80111b4
	bl	StartTask
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Func_8010ff0
	bl	StartTask
	ldr	r1, =0x336
	mov	r3, #0xff
	add	r0, r7, r1
.L10ca6:
	strh	r3, [r0]
	sub	r3, #1
	sub	r0, #2
	cmp	r3, #0
	bge	.L10ca6
	add	sp, #0x20
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end InitWorldMap

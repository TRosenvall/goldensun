	.include "macros.inc"

@ LoadScreenGraphics
@ r0.. = parameters. Loads the party screen's shared graphics: GetFile gets
@ the asset, galloc_iwram allocates, DecompressLZ1 unpacks, UploadSpriteGFX / AllocSpriteSlot
@ reserve the tiles, and Func_2dd8 frees the scratch. Func_1b36c supplies the
@ entry count so only the visible panels are loaded.
.thumb_func_start Func_801c188  @ 0x0801c188
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e98
	ldr	r5, [r3]
	mov	r0, r5
	bl	Func_801b36c
	mov	r6, r0
	ldrh	r3, [r6, #0xa]
	cmp	r3, #1
	beq	.L1c1a4
	cmp	r3, #6
	bne	.L1c206
.L1c1a4:
	mov	r1, #0xc1
	lsl	r1, #3
	mov	r0, #0x11
	bl	galloc_iwram
	mov	r3, #0xc3
	lsl	r3, #2
	add	r5, r3
	ldrh	r3, [r6, #8]
	mov	r7, r0
	ldr	r0, =_FILE_f1
	mov	r8, r3
	bl	GetFile
	ldr	r3, =0x604
	add	r2, r7, r3
	ldrh	r3, [r6, #8]
	lsl	r3, #1
	ldrh	r3, [r3, r0]
	add	r0, r3
	str	r0, [r2]
	mov	r1, r7
	bl	DecompressLZ1
	ldrh	r3, [r5, #0xa]
	cmp	r3, #0
	bne	.L1c1e0
	bl	AllocSpriteSlot
	strh	r0, [r5, #0xc]
.L1c1e0:
	mov	r1, #0x80
	ldrh	r0, [r5, #0xc]
	lsl	r1, #3
	mov	r2, r7
	bl	UploadSpriteGFX
	mov	r3, #1
	strh	r3, [r5, #0xa]
	mov	r3, r8
	strh	r3, [r5, #8]
	mov	r3, #0x28
	strh	r3, [r5, #0x22]
	strh	r3, [r5, #0x24]
	mov	r3, #0xf0
	strh	r0, [r5, #0xe]
	strh	r3, [r5, #0x26]
	mov	r0, #0x11
	bl	gfree
.L1c206:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_801c188

@ ReleasePanelTiles
@ r0 = panel. Releases the panel's OBJ tiles with Func_3f3c and clears its
@ handle in the block at iwram_1e98.
.thumb_func_start Func_801c21c  @ 0x0801c21c
	push	{r5, lr}
	ldr	r3, =iwram_3001e98
	mov	r2, #0xc3
	ldr	r3, [r3]
	lsl	r2, #2
	add	r5, r3, r2
	ldrh	r3, [r5, #0xa]
	cmp	r3, #0
	beq	.L1c238
	ldrh	r0, [r5, #0xc]
	bl	Func_8003f3c
	mov	r3, #0
	strh	r3, [r5, #0xa]
.L1c238:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_801c21c

@ RunFieldMenuLoop
@ Takes no arguments. The field menu, opened from rom_8a000. Func_28920 draws
@ the menu and returns the chosen entry; this dispatches it, five ways:
@
@     0  _Func_8ce74  NOT a screen -- reads the terrain the player is standing
@                     on and stores it, or 0xFF, at [iwram_1ebc]+0x17A, then
@                     leaves the menu
@     1  _Func_a5b94  Items      -- loops the menu again on -1
@     2  _Func_aa56c  Djinn      -- loops again on 0
@     3  _Func_a24d0  Psynergy   -- loops again on -1
@     4  _Func_a7478  Status     -- loops again on -1
@
@ Note the polarity differs: entry 2 loops on ZERO and the others on -1, which
@ is why Func_aa56c returns 1 where the rest return their selection.
@ .gcc2_compiled. and .gcc2_compiled. fade around each pass. iwram_1ebc is the block the
@ result is written into.
.thumb_func_start Func_801c244  @ 0x0801c244
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r6, [r3]
	mov	r5, #0
.L1c24c:
	bl	Func_801c2d0
	mov	r0, r5
	bl	Func_8028920
	mov	r5, r0
	bl	Func_801c2e4
	cmp	r5, #4
	bhi	.L1c2c2
	ldr	r2, =.L1c268
	lsl	r3, r5, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L1c268:
	.word	.L1c27c
	.word	.L1c290
	.word	.L1c29e
	.word	.L1c2a8
	.word	.L1c2b6
.L1c27c:
	bl	_Func_808ce74
	cmp	r0, #0
	bne	.L1c286
	mov	r0, #0xff
.L1c286:
	mov	r2, #0xbd
	lsl	r2, #1
	add	r3, r6, r2
	strh	r0, [r3]
	b	.L1c2c2
.L1c290:
	bl	_Func_80a5b94
	mov	r3, #1
	neg	r3, r3
	cmp	r0, r3
	bne	.L1c2c2
	b	.L1c24c
.L1c29e:
	bl	_Func_80aa56c
	cmp	r0, #0
	beq	.L1c2c2
	b	.L1c24c
.L1c2a8:
	bl	_Func_80a24d0
	mov	r2, #1
	neg	r2, r2
	cmp	r0, r2
	bne	.L1c2c2
	b	.L1c24c
.L1c2b6:
	bl	_Func_80a7478
	mov	r3, #1
	neg	r3, r3
	cmp	r0, r3
	beq	.L1c24c
.L1c2c2:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_801c244


	.include "macros.inc"
	.include "gba.inc"

@ LoadIconIndexed
@ r0 = index, r1.. = placement. As LoadItemIconID but bounds-checked against
@ Func_19ed0's count of 160 and reading the second pointer table.
.thumb_func_start LoadMoveIconID  @ 0x0801a404
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r1
	mov	r1, #0xc1
	mov	r6, r0
	lsl	r1, #3
	mov	r0, #0x11
	mov	r10, r2
	mov	r9, r3
	bl	galloc_iwram
	mov	r1, #0
	mov	r5, r0
	mov	r8, r1
	bl	NumMoveIcons
	cmp	r6, r0
	bcc	.L1a430
	mov	r6, #0
.L1a430:
	cmp	r7, #0
	beq	.L1a45a
	ldr	r3, =0x604
	add	r2, r5, r3
	ldr	r3, =.L29a10
	mov	r1, #0xc0
	ldr	r3, [r3, #8]
	lsl	r1, #3
	str	r3, [r2]
	mov	r2, #2
	add	r3, r5, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, #0
	bl	LoadIcon
	mov	r2, #1
	mov	r8, r2
.L1a45a:
	ldr	r3, =0x604
	ldr	r2, =.L2de88
	add	r1, r5, r3
	lsl	r3, r6, #2
	ldr	r3, [r2, r3]
	str	r3, [r1]
	mov	r1, #0xc0
	lsl	r1, #3
	mov	r2, #2
	add	r3, r5, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, r8
	bl	LoadIcon
	ldr	r2, [sp, #0x1c]
	cmp	r2, #0
	bne	.L1a48c
	bl	AllocSpriteSlot
	mov	r3, r10
	str	r0, [r3]
.L1a48c:
	mov	r3, #0x80
	mov	r1, r10
	lsl	r3, #3
	ldr	r0, [r1]
	add	r2, r5, r3
	mov	r1, #0x80
	bl	UploadSpriteGFX
	mov	r1, r9
	str	r0, [r1]
	mov	r0, #0x11
	bl	gfree
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end LoadMoveIconID

@ SelectIconPointer
@ r0 = index. Stores .L308a0[index] at [iwram_1e94]+0x604 and sets the two
@ halfwords at +0x600 and +0x602 to 2, marking the block dirty.
.thumb_func_start DecompressStatusIcon  @ 0x0801a4c0
	push	{lr}
	ldr	r3, =iwram_3001e94
	ldr	r1, [r3]
	ldr	r3, =0x604
	add	r2, r1, r3
	ldr	r3, =.L308a0
	lsl	r0, #2
	ldr	r3, [r3, r0]
	mov	r0, #0xc0
	lsl	r0, #3
	str	r3, [r2]
	add	r3, r1, r0
	mov	r2, #2
	add	r0, #2
	strh	r2, [r3]
	add	r3, r1, r0
	strh	r2, [r3]
	mov	r0, r1
	mov	r1, #0
	bl	LoadIcon
	pop	{r0}
	bx	r0
.func_end DecompressStatusIcon

@ LoadNamedGraphic
@ r0 = id, r1, r2, r3 = placement. Allocates the tag-0x11 block, fetches asset
@ 0xF0 with GetFile, decompresses with LoadIcon, DMA3s it into place and
@ reserves tiles with UploadSpriteGFX / AllocSpriteSlot.
.thumb_func_start LoadPortrait  @ 0x0801a4fc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r1, #0xc1
	mov	r5, r0
	lsl	r1, #3
	mov	r0, #0x11
	mov	r8, r3
	mov	r7, r2
	bl	galloc_iwram
	mov	r6, r0
	ldr	r0, =_FILE_f0
	bl	GetFile
	mov	r3, r5
	cmp	r5, #0x7f
	bls	.L1a522
	sub	r3, #0x70
.L1a522:
	lsl	r3, #1
	ldrh	r3, [r3, r0]
	ldr	r1, =0x604
	add	r5, r0, r3
	mov	r3, r5
	add	r2, r6, r1
	add	r3, #0x20
	str	r3, [r2]
	mov	r2, #0xc0
	lsl	r2, #3
	add	r3, r6, r2
	sub	r1, #2
	mov	r2, #4
	strh	r2, [r3]
	add	r3, r6, r1
	strh	r2, [r3]
	mov	r0, r6
	mov	r1, #0
	bl	LoadIcon
	ldr	r2, [sp, #0x18]
	cmp	r2, #0
	bne	.L1a556
	bl	AllocSpriteSlot
	str	r0, [r7]
.L1a556:
	mov	r3, #0x80
	lsl	r3, #3
	mov	r1, #0x80
	add	r2, r6, r3
	ldr	r0, [r7]
	lsl	r1, #2
	bl	UploadSpriteGFX
	mov	r1, r8
	str	r0, [r1]
	mov	r0, #0x11
	bl	gfree
	ldr	r1, [sp, #0x14]
	ldr	r2, =0x5000200
	lsl	r1, #5
	add	r1, r2
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end LoadPortrait

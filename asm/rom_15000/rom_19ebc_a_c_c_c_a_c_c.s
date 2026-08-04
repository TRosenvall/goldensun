	.include "macros.inc"
	.include "gba.inc"

@ SelectGraphicTable
@ r0 = table selector 1..3, r1, r2 = parameters. A four-way switch returning one
@ of three table base addresses; anything outside 1..3 takes the default arm.
@ UploadSpriteGFX reserves the tiles for whichever is chosen.
.thumb_func_start LoadUIBanner  @ 0x0801a32c
	push	{lr}
	mov	r3, r2
	cmp	r0, #1
	beq	.L1a342
	cmp	r0, #1
	bcc	.L1a34e
	cmp	r0, #2
	beq	.L1a346
	cmp	r0, #3
	beq	.L1a34a
	b	.L1a34e
.L1a342:
	ldr	r2, .L1a360
	b	.L1a350
.L1a346:
	ldr	r2, .L1a364
	b	.L1a350
.L1a34a:
	ldr	r2, .L1a368
	b	.L1a350
.L1a34e:
	ldr	r2, .L1a36c
.L1a350:
	mov	r0, r3
	mov	r1, #0x20
	bl	UploadSpriteGFX
	mov	r0, #1
	pop	{r1}
	bx	r1
	.align	2, 0
.L1a360:
	.word	Data_31864
.L1a364:
	.word	Data_31864
.L1a368:
	.word	Data_31864
.L1a36c:
	.word	Data_31864
.func_end LoadUIBanner

@ SetPortraitPointer
@ r0 = id masked to 0x1FF. Looks the character up with _Func_78414, indexes the
@ 255-entry table at .L29ee4 by the halfword at +6 of that record, and stores the
@ resulting pointer at [iwram_1e94]+0x604. An id of 0 clears it instead.
.thumb_func_start LoadItemIcon  @ 0x0801a370
	push	{r5, r6, lr}
	mov	r5, r0
	ldr	r0, =0x1ff
	ldr	r3, =iwram_3001e94
	and	r0, r5
	ldr	r6, [r3]
	bl	_GetItemInfo
	cmp	r5, #0
	beq	.L1a394
	ldr	r2, =0x604
	ldrh	r3, [r0, #6]
	add	r1, r6, r2
	ldr	r2, =.L29ee4
	lsl	r3, #2
	ldr	r3, [r2, r3]
	str	r3, [r1]
	b	.L1a39e
.L1a394:
	ldr	r2, =.L29ee4
	ldr	r1, =0x604
	ldr	r2, [r2]
	add	r3, r6, r1
	str	r2, [r3]
.L1a39e:
	mov	r2, #0xc0
	lsl	r2, #3
	ldr	r1, =0x602
	add	r3, r6, r2
	mov	r2, #2
	strh	r2, [r3]
	add	r3, r6, r1
	strh	r2, [r3]
	mov	r0, r6
	mov	r1, #0
	bl	LoadIcon
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end LoadItemIcon

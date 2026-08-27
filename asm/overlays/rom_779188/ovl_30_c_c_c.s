	.include "macros.inc"
	.include "gba.inc"

@ BuildTitleOam
@ r0.. = parameters. Writes OAM entries for the title sprite: the tile base comes
@ from the reserved slot's entry in iwram_1b10 shifted right by 5, the vertical
@ positions step 8 apart counting down from 0xE8, and the attribute words are
@ built from .L68c and .L6a0. iwram_1e40 -- the frame counter -- drives the
@ animation phase.
.thumb_func_start OvlFunc_879_2008238
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =.L650
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	ldr	r2, =gSpriteSlots
	lsl	r3, #2
	add	r3, r2
	ldr	r4, =.L6a0
	ldrh	r3, [r3, #2]
	mov	r2, #0x88
	sub	sp, #4
	mov	r6, r4
	lsr	r7, r3, #5
	mov	r5, #0
	mov	r8, r2
.L25a:
	mov	r2, #0x12
	sub	r2, r5
	lsl	r2, #3
	mov	r3, #0xe8
	sub	r3, r2
	mov	r2, #0
	stmia	r6!, {r2}
	mov	r1, r8
	lsl	r3, #16
	mov	r2, #0x84
	orr	r3, r1
	lsl	r2, #8
	orr	r3, r2
	stmia	r6!, {r3}
	mov	r3, #0xf0
	lsl	r3, #8
	orr	r3, r7
	stmia	r6!, {r3}
	ldr	r3, =.L68c
	ldrh	r3, [r3]
	lsl	r3, #16
	asr	r2, r3, #16
	lsr	r3, #31
	add	r2, r3
	asr	r2, #1
	sub	r1, r2, r5
	cmp	r1, #0
	bge	.L294
	mov	r1, #0
.L294:
	cmp	r1, #2
	bgt	.L2a6
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L2a6
	mov	r1, #0
.L2a6:
	cmp	r1, #0
	beq	.L2b8
	mov	r0, r4
	mov	r1, #0xff
	add	r4, #0xc
	str	r4, [sp]
	bl	__Func_8003dec
	ldr	r4, [sp]
.L2b8:
	add	r5, #1
	add	r7, #2
	cmp	r5, #0x11
	ble	.L25a
	ldr	r2, =.L68c
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_879_2008238

@ RunTitleFade
@ r0 = direction. Drives a full-screen fade using the blend hardware: BLDCNT,
@ BLDALPHA and BLDY are all queued through the ewram_2090 transfer list with
@ interrupts masked rather than written directly, so they land at VBlank.
@ [iwram_1ebc] supplies the scene state. 160 lines; traced structurally.
.thumb_func_start OvlFunc_879_20082e8
	push	{r5, r6, r7, lr}
	bl	OvlFunc_879_2008454
	mov	r0, #0x1e
	bl	__CutsceneWait
	ldr	r2, =.L68c
	ldr	r3, .L31c	@ 0
	mov	r0, #0
	strh	r3, [r2]
	bl	OvlFunc_879_20081c0
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_879_2008238
	bl	__StartTask
	ldr	r7, =gDMATaskCount
	ldr	r5, =REG_IME
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r2, [r7]
	cmp	r2, #0x1f
	bgt	.L350
	b	.L330

	.align	2, 0
.L31c:
	.word	0
	.pool

.L330:
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r7
	strh	r2, [r7]
	mov	r2, #0xaa
	add	r3, #4
	lsl	r2, #5
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #19
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L350:
	strh	r1, [r5]
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r2, [r7]
	cmp	r2, #0x1f
	bgt	.L37a
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r7
	strh	r2, [r7]
	ldr	r2, =0x2fce
	add	r3, #4
	stmia	r3!, {r2}
	ldr	r2, =REG_BLDCNT
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L37a:
	strh	r1, [r5]
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r2, [r7]
	cmp	r2, #0x1f
	bgt	.L3a4
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r7
	add	r3, #4
	strh	r2, [r7]
	mov	r2, #0x10
	stmia	r3!, {r2}
	ldr	r2, =REG_BLDY
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L3a4:
	strh	r1, [r5]
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r2, [r7]
	cmp	r2, #0x1f
	bgt	.L3ce
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r7
	strh	r2, [r7]
	ldr	r2, =0x1010
	add	r3, #4
	stmia	r3!, {r2}
	ldr	r2, =REG_BLDALPHA
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L3ce:
	strh	r1, [r5]
	mov	r0, #0x78
	bl	__CutsceneWait
	mov	r6, #0
.L3d8:
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r3, [r7]
	cmp	r3, #0x1f
	bgt	.L402
	lsl	r2, r3, #1
	add	r2, r3
	lsl	r2, #2
	add	r3, #1
	add	r2, r7, r2
	strh	r3, [r7]
	mov	r3, #0x10
	add	r2, #4
	sub	r3, r6
	stmia	r2!, {r3}
	ldr	r3, =REG_BLDY
	stmia	r2!, {r3}
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r2]
.L402:
	strh	r1, [r5]
	mov	r0, #3
	add	r6, #1
	bl	__WaitFrames
	cmp	r6, #0x10
	ble	.L3d8
	ldr	r6, =iwram_3001ebc
	mov	r3, #0xe0
	ldr	r1, [r6]
	lsl	r3, #1
	add	r2, r1, r3
	mov	r5, #0xe4
	mov	r3, #0
	str	r3, [r2]
	lsl	r5, #1
	mov	r3, #1
	str	r3, [r1, r5]
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	ldr	r2, [r6]
	mov	r3, #0x3c
	str	r3, [r2, r5]
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_879_20082e8

@ SetUpTitleBackground
@ r0.. = parameters. Programs BG2CNT, stages the decompressed image through
@ ewram_10000, and points an HDMA at BG0HOFS fed from iwram_1ad0 -- the same
@ per-scanline shear rom_f2000's splash screens use. Reads the map bounds at
@ [iwram_1e70]. 105 lines; traced structurally.
.thumb_func_start OvlFunc_879_2008454
	push	{r5, r6, lr}
	mov	r0, #0
	ldr	r5, =0x1a
	bl	__Func_8003b70
	ldr	r2, =REG_BG2CNT
	ldr	r3, .L498	@ 0x681
	strh	r3, [r2]
	ldr	r2, =iwram_3001ad0
	mov	r3, #0
	strh	r3, [r2, #0xa]
	mov	r0, r5
	bl	__GetFile
	mov	r1, #0xa0
	ldr	r6, =0x1ff
	mov	r4, r0
	ldr	r3, =REG_DMA3SAD
	lsl	r1, #19
	ldr	r2, =0x84000070
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, #0xe0
	lsl	r3, #1
	add	r4, r3
	mov	r0, r4
	ldr	r1, =gBuffer
	bl	__DecompressLZ
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =gBuffer
	ldr	r1, =0x6006800
	ldr	r2, =0x84002580
	b	.L4c0

	.align	2, 0
.L498:
	.word	0x681
	.pool

.L4c0:
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, #0xd0
	ldr	r1, =0x6003000
	lsl	r3, #1
	mov	r4, #0
.L4cc:
	mov	r0, #0
.L4ce:
	mov	r2, r3
	mov	r5, #0x80
	lsl	r3, r2, #16
	lsl	r5, #9
	add	r3, r5
	add	r0, #1
	strh	r2, [r1]
	asr	r3, #16
	add	r1, #2
	cmp	r0, #0x1d
	bls	.L4ce
	strh	r6, [r1]
	add	r4, #1
	add	r1, #2
	strh	r6, [r1]
	add	r1, #2
	cmp	r4, #0x13
	bls	.L4cc
	ldr	r3, =iwram_3001ad0
	mov	r4, #0
	mov	r2, #0
.L4f8:
	add	r4, #1
	strh	r2, [r3, #2]
	strh	r2, [r3]
	add	r3, #4
	cmp	r4, #3
	bls	.L4f8
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =iwram_3001ad0
	ldr	r1, =REG_BG0HOFS
	ldr	r2, =0x84000004
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =iwram_3001e70
	ldr	r2, [r3]
	mov	r3, #0xa0
	lsl	r3, #5
	strh	r3, [r2, #0x14]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_879_2008454

	.section .data
	.global gOvl_020085f8
	.global MapEntrance_ARRAY_879__020085f8
gOvl_020085f8:
MapEntrance_ARRAY_879__020085f8:
	.incbin "overlays/rom_779188/orig.bin", 0x5f8, (0x628-0x5f8)
	.global gOvl_02008628
gOvl_02008628:
	.incbin "overlays/rom_779188/orig.bin", 0x628, (0x62c-0x628)
	.global gOvl_0200862c
gOvl_0200862c:
	.incbin "overlays/rom_779188/orig.bin", 0x62c, (0x644-0x62c)
	.global gOvl_02008644
gOvl_02008644:
	.incbin "overlays/rom_779188/orig.bin", 0x644, (0x650-0x644)
	.global .L650
.L650:
	.incbin "overlays/rom_779188/orig.bin", 0x650

	.section .bss

	.lcomm	.Lunused_658, 0x34
	.lcomm	.L68c, 4
	.lcomm	.Lunused_690, 0x10
	.lcomm	.L6a0, 0xc

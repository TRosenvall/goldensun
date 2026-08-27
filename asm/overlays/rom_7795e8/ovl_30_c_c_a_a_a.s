	.include "macros.inc"
	.include "gba.inc"

@ 88 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   StartFadeOut, GetAsset, DecompressVariant, GetSlotEntityChecked
.thumb_func_start OvlFunc_880_2008054
	push	{r5, r6, lr}
	mov	r0, #0
	ldr	r5, =0x1a
	bl	__Func_8003b70
	ldr	r2, =REG_BG2CNT
	ldr	r3, .L98	@ 0x681
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
	mov	r2, #0xe0
	lsl	r2, #1
	add	r4, r2
	mov	r0, r4
	ldr	r1, =gBuffer
	bl	__DecompressLZ
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =gBuffer
	ldr	r1, =0x6006800
	ldr	r2, =0x84002580
	b	.Lc0

	.align	2, 0
.L98:
	.word	0x681
	.pool

.Lc0:
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, #0xd0
	ldr	r1, =0x6003000
	lsl	r3, #1
	mov	r4, #0
.Lcc:
	mov	r0, #0
.Lce:
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
	bls	.Lce
	strh	r6, [r1]
	add	r4, #1
	add	r1, #2
	strh	r6, [r1]
	add	r1, #2
	cmp	r4, #0x13
	bls	.Lcc
	ldr	r3, =iwram_3001ad0
	mov	r4, #0
	mov	r2, #0
.Lf8:
	add	r4, #1
	strh	r2, [r3, #2]
	strh	r2, [r3]
	add	r3, #4
	cmp	r4, #3
	bls	.Lf8
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
	ldr	r3, =gState
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	__MapActor_GetActor
	ldr	r5, .L134	@ 0
	add	r0, #0x55
	strb	r5, [r0]
	pop	{r5, r6}
	pop	{r0}
	bx	r0

	.align	2, 0
.L134:
	.word	0
.func_end OvlFunc_880_2008054

@ 68 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   UnregisterTask
.thumb_func_start OvlFunc_880_2008154
	push	{r5, r6, lr}
	ldr	r3, =.L16b0
	ldrh	r2, [r3]
	add	r2, #1
	strh	r2, [r3]
	ldr	r4, =gDMATaskCount
	lsl	r2, #16
	lsr	r5, r2, #17
	ldr	r0, =REG_IME
	ldrh	r3, [r0]
	mov	r1, r3
	strh	r0, [r0]
	ldrh	r2, [r4]
	cmp	r2, #0x1f
	bgt	.L18e
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r4
	strh	r2, [r4]
	ldr	r2, =0x2e51
	add	r3, #4
	stmia	r3!, {r2}
	ldr	r2, =REG_BLDCNT
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L18e:
	strh	r1, [r0]
	ldrh	r3, [r0]
	mov	r6, r3
	strh	r0, [r0]
	ldrh	r3, [r4]
	cmp	r3, #0x1f
	bgt	.L1c4
	lsl	r2, r3, #1
	add	r2, r3
	lsl	r0, r5, #16
	add	r3, #1
	lsr	r1, r0, #16
	strh	r3, [r4]
	mov	r3, #0x10
	lsl	r2, #2
	sub	r3, r1
	add	r2, r4
	lsl	r3, #8
	add	r2, #4
	orr	r3, r1
	stmia	r2!, {r3}
	ldr	r3, =REG_BLDALPHA
	stmia	r2!, {r3}
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r2]
	b	.L1c6
.L1c4:
	lsl	r0, r5, #16
.L1c6:
	ldr	r3, =REG_IME
	strh	r6, [r3]
	mov	r3, #0xf0
	lsl	r3, #12
	cmp	r0, r3
	bls	.L1d8
	ldr	r0, =OvlFunc_880_2008154
	bl	__StopTask
.L1d8:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_880_2008154

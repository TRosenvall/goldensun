	.include "macros.inc"
	.include "gba.inc"

@ 101 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectFull, CopyMapRectAttributes x2, TestSaveBit, OvlFunc_a90
@   OvlFunc_b3c, OvlFunc_194, SetSlotAnimation, GetSlotEntityChecked x2
@   TestSaveBit, OvlFunc_e64
@ reads save bits 0x109, 0x845.
.thumb_func_start OvlFunc_916_2008980
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =.L12c8
	ldr	r1, =.L12c4
	ldr	r2, =ewram_2001000
	mov	r8, r3
	ldr	r7, =.L12c0
	str	r2, [r1]
	add	r3, r2, #2
	mov	r10, r1
	add	r2, #4
	mov	r1, r8
	sub	sp, #8
	str	r3, [r1]
	str	r2, [r7]
	mov	r6, #0
	mov	r5, #0x40
	mov	r0, #0x20
	mov	r1, #0
	mov	r2, #0x40
	mov	r3, #0x20
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0x20
	mov	r3, #0x20
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x20
	mov	r0, #0x20
	mov	r1, #0
	mov	r2, #0x20
	str	r6, [sp]
	str	r3, [sp, #4]
	bl	__Func_8010704
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L9fa
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =.L111c
	ldr	r1, [r7]
	ldr	r2, =0x84000012
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r2, r10
	ldr	r3, [r2]
	strh	r6, [r3]
	mov	r3, r8
	ldr	r2, [r3]
	ldr	r3, .La30	@ 1
	strh	r3, [r2]
.L9fa:
	ldr	r0, [r7]
	bl	OvlFunc_916_2008a90
	mov	r1, #0xff
	ldr	r0, =.L111c
	bl	OvlFunc_916_2008b3c
	bl	OvlFunc_916_2008194
	mov	r1, #0
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x55
	strb	r6, [r0]
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r3, #8
	strh	r3, [r0, #0x20]
	mov	r3, #0xc0
	lsl	r3, #8
	str	r3, [r0, #0x18]
	b	.La54

	.align	2, 0
.La30:
	.word	1
	.pool

.La54:
	str	r3, [r0, #0x1c]
	ldr	r3, =iwram_3001ebc
	mov	r1, #0xe0
	ldr	r3, [r3]
	lsl	r1, #1
	mov	r2, #0x81
	add	r3, r1
	lsl	r2, #2
	str	r2, [r3]
	ldr	r0, =0x845
	bl	__GetFlag
	cmp	r0, #0
	bne	.La76
	mov	r0, #4
	bl	OvlFunc_916_2008e64
.La76:
	mov	r0, #0
	add	sp, #8
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_916_2008980

@ 82 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SpawnEntity, SetEntityAnimation, SetEntityActorOptions, GetTerrainHeight
.thumb_func_start OvlFunc_916_2008a90
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r6, r0
	mov	r2, #0
	ldrsh	r3, [r6, r2]
	mov	r2, #1
	neg	r2, r2
	ldrh	r0, [r6]
	cmp	r3, r2
	beq	.Lb32
	mov	r8, r2
.Laa8:
	mov	r2, #6
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	bne	.Lac8
	mov	r2, #2
	ldrsh	r3, [r6, r2]
	mov	r2, #0x80
	lsl	r2, #14
	lsl	r3, #20
	add	r1, r3, r2
	mov	r2, #4
	ldrsh	r3, [r6, r2]
	mov	r2, #0x80
	lsl	r3, #20
	lsl	r2, #12
	b	.Lade
.Lac8:
	mov	r2, #2
	ldrsh	r3, [r6, r2]
	mov	r2, #0x80
	lsl	r2, #12
	lsl	r3, #20
	add	r1, r3, r2
	mov	r2, #4
	ldrsh	r3, [r6, r2]
	mov	r2, #0x80
	lsl	r3, #20
	lsl	r2, #14
.Lade:
	add	r3, r2
	lsl	r0, #16
	asr	r0, #16
	mov	r2, #0
	bl	__CreateActor
	mov	r5, r0
	cmp	r5, #0
	beq	.Lb32
	mov	r1, #1
	str	r5, [r6, #8]
	bl	__Actor_SetAnim
	mov	r0, r5
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r2, r5
	add	r2, #0x59
	mov	r3, #0
	strb	r3, [r2]
	mov	r3, #0x20
	strh	r3, [r5, #0x20]
	mov	r0, #0
	mov	r3, #0xa
	ldrsh	r1, [r5, r3]
	mov	r3, #0x12
	ldrsh	r2, [r5, r3]
	bl	__Func_8011f54
	ldr	r3, [r5, #0xc]
	lsl	r0, #16
	add	r3, r0
	add	r6, #0xc
	str	r3, [r5, #0xc]
	ldrh	r3, [r6]
	str	r0, [r5, #0x14]
	mov	r0, r3
	lsl	r3, r0, #16
	asr	r3, #16
	cmp	r3, r8
	bne	.Laa8
.Lb32:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_916_2008a90

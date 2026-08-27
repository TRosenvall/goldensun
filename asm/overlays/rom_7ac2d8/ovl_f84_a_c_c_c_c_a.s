	.include "macros.inc"

@ 66 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, BeginCutscene, GetSlotEntityChecked x3, CopyMapRectIndicesU
@   PlaySound, PlayMapRectAnimation, CopyMapRectAttributes, SetSaveBit
@   EndCutscene
@ sets 0x874.
.thumb_func_start OvlFunc_924_2009420
	push	{r5, r6, lr}
	mov	r0, #0xb
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	cmp	r3, #0
	bge	.L1434
	ldr	r2, =0xfffff
	add	r3, r2
.L1434:
	mov	r0, #0xb
	asr	r6, r3, #20
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	cmp	r3, #0
	bge	.L1446
	ldr	r2, =0xfffff
	add	r3, r2
.L1446:
	asr	r5, r3, #20
	bl	__CutsceneStart
	cmp	r6, #5
	bne	.L14b0
	cmp	r5, #0xd
	bne	.L14b0
	mov	r0, #0xb
	bl	__MapActor_GetActor
	ldr	r2, =0xfffe0000
	ldr	r3, [r0, #0xc]
	add	r3, r2
	str	r3, [r0, #0xc]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0xb
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0xc]
	str	r3, [r5, #0x3c]
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r1, #2
	mov	r3, #0xb
	mov	r2, #5
	mov	r0, #5
	bl	__CopyMapTiles
	mov	r0, #0xd9
	bl	__PlaySound
	ldr	r0, =.L6010
	mov	r1, #9
	mov	r2, #7
	bl	__Func_8010560
	mov	r3, #9
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #9
	mov	r1, #5
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	ldr	r0, =0x874
	bl	__SetFlag
.L14b0:
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_2009420

@ 58 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, GetSlotEntityChecked x2, BeginCutscene, SetSaveBit
@   DialogueWait, GetSlotEntityChecked x3, CopyMapRectIndicesU, PlaySound
@   PlayMapRectAnimation, EndCutscene
@ reads save bit 0x256; sets 0x256.
.thumb_func_start OvlFunc_924_20094cc
	push	{r5, lr}
	ldr	r0, =0x256
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1552
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0xa
	ldrsh	r5, [r0, r2]
	mov	r0, #0
	bl	__MapActor_GetActor
	sub	r5, #0x54
	mov	r2, #0x12
	ldrsh	r3, [r0, r2]
	cmp	r5, #7
	bhi	.L1552
	cmp	r3, #0xd3
	ble	.L1552
	cmp	r3, #0xdb
	bgt	.L1552
	bl	__CutsceneStart
	ldr	r0, =0x256
	bl	__SetFlag
	mov	r0, #5
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r2, =0xfffe0000
	ldr	r3, [r0, #0xc]
	add	r3, r2
	str	r3, [r0, #0xc]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0xc]
	str	r3, [r5, #0x3c]
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r1, #2
	mov	r2, #5
	mov	r3, #0xb
	mov	r0, #5
	bl	__CopyMapTiles
	mov	r0, #0xd9
	bl	__PlaySound
	ldr	r0, =.L6010
	mov	r1, #9
	mov	r2, #7
	bl	__Func_8010560
	bl	__CutsceneEnd
.L1552:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_20094cc

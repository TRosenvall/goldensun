	.include "macros.inc"

@ 61 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, GetSlotEntityChecked x2, BeginCutscene, SetSaveBit
@   DialogueWait, GetSlotEntityChecked x3, CopyMapRectIndicesU, PlaySound
@   PlayMapRectAnimation, EndCutscene
@ reads save bit 0x256; sets 0x256.
.thumb_func_start OvlFunc_924_2009c9c
	push	{r5, lr}
	ldr	r0, =0x256
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1d28
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0xa
	ldrsh	r5, [r0, r2]
	mov	r0, #0
	bl	__MapActor_GetActor
	sub	r5, #0xa4
	mov	r2, #0x12
	ldrsh	r3, [r0, r2]
	cmp	r5, #7
	bhi	.L1d28
	mov	r2, #0xba
	lsl	r2, #1
	cmp	r3, r2
	blt	.L1d28
	add	r2, #8
	cmp	r3, r2
	bge	.L1d28
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
	mov	r1, #0x1d
	mov	r2, #0xa
	mov	r3, #0x17
	mov	r0, #6
	bl	__CopyMapTiles
	mov	r0, #0xd9
	bl	__PlaySound
	ldr	r0, =.L6064
	mov	r1, #0xa
	mov	r2, #0x12
	bl	__Func_8010560
	bl	__CutsceneEnd
.L1d28:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_2009c9c

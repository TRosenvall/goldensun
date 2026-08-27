	.include "macros.inc"

@ 44 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, BeginCutscene, ClearSaveBit, GetSlotEntityChecked x3
@   DialogueWait, CopyMapRectIndicesU, PlaySound, PlayMapRectAnimation
@   EndCutscene
@ reads save bit 0x256; clears 0x256.
.thumb_func_start OvlFunc_924_2009568
	push	{r5, lr}
	ldr	r0, =0x256
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	beq	.L15ce
	bl	__CutsceneStart
	ldr	r0, =0x256
	bl	__ClearFlag
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0x80
	ldr	r3, [r0, #0xc]
	lsl	r2, #10
	add	r3, r2
	str	r3, [r0, #0xc]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0xc]
	mov	r0, #5
	str	r3, [r5, #0x3c]
	bl	__CutsceneWait
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r1, #2
	mov	r2, #5
	mov	r3, #0xb
	mov	r0, #7
	bl	__CopyMapTiles
	mov	r0, #0xd9
	bl	__PlaySound
	ldr	r0, =.L603a
	mov	r1, #9
	mov	r2, #7
	bl	__Func_8010560
	bl	__CutsceneEnd
.L15ce:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_924_2009568

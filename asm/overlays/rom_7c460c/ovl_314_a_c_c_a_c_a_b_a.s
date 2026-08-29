	.include "macros.inc"

@ 32 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotAnimation, SetActiveMessageId, ShowMessageAndWait
@   PlayInteractionEffect, SetSlotAnimation, MoveSlotBy, WaitForSlotArrival
@   SetSlotAnimation, ClearSaveBit, EndCutscene
@ message id 0x24cf; clears 0x243.
.thumb_func_start OvlFunc_939_2008b0c
	push	{lr}
	bl	__CutsceneStart
	mov	r1, #1
	mov	r0, #0
	bl	__MapActor_SetAnim
	ldr	r0, =0x24cf
	bl	__MessageID
	mov	r0, #1
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x81
	mov	r2, #0x64
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r2, #0xc
	mov	r1, #0
	mov	r0, #0
	bl	__Func_809228c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r1, #1
	mov	r0, #0
	bl	__MapActor_SetAnim
	ldr	r0, =0x243
	bl	__ClearFlag
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_939_2008b0c

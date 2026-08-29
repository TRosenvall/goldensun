	.include "macros.inc"

@ 27 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetSaveBit, SetActiveMessageId, ShowMessageAndWait, SetSlotEntitySpeed
@   WalkSlotByAndWait, TurnSlotToAngle, DialogueWait
@ message id 0x28b8; sets 0x9bb.
.thumb_func_start OvlFunc_966_200810c
	push	{lr}
	ldr	r0, =0x9bb
	bl	__SetFlag
	ldr	r0, =0x28b8
	bl	__MessageID
	mov	r0, #0x12
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x12
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x10
	mov	r0, #0x12
	neg	r1, r1
	mov	r2, #0
	bl	__Func_8092304
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0xa
	bl	__CutsceneWait
	pop	{r0}
	bx	r0
.func_end OvlFunc_966_200810c

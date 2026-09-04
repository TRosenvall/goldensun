	.include "macros.inc"

@ 34 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotEntitySpeed, ShowScreenOverlay, SetSlotAnimation
@   MoveSlotToAndWait x2, MoveSlotTo, HideScreenOverlay, WaitSceneDelay
@   SetPendingMessageId
.thumb_func_start OvlFunc_953_200a904
	push	{lr}
	bl	__CutsceneStart
	ldr	r2, =0xcccc
	mov	r0, #0
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	bl	__MapTransitionIn
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r1, #0xc8
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0xaf
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0x96
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__MapActor_TravelTo
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x16
	bl	__Func_8091e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a904

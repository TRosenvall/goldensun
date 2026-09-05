	.include "macros.inc"

@ 41 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotEntitySpeed, ShowScreenOverlay, SetSlotAnimation
@   MoveSlotToAndWait x2, MoveSlotTo, HideScreenOverlay, WaitSceneDelay
@   TestSaveBit, SetPendingMessageId x2
@ reads save bit 0x90f.
.thumb_func_start OvlFunc_953_200a5f0
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
	mov	r1, #0xc3
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0xdc
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0xf5
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__MapActor_TravelTo
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x90f
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2650
	mov	r0, #0x20
	bl	__Func_8091e9c
	b	.L2656
.L2650:
	mov	r0, #0xc
	bl	__Func_8091e9c
.L2656:
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a5f0

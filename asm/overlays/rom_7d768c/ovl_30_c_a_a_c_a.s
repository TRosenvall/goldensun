	.include "macros.inc"

@ Cutscene: roughly 88 instructions of straight-line script --
@ 1 turn, 0 animation changes, 3 dialogue lines, 8 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2009.
.thumb_func_start OvlFunc_952_20083b0
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r5, r0
	bl	__MapActor_GetActor
	mov	r8, r0
	bl	__CutsceneStart
	ldr	r3, =gScript_952__0200c570
	mov	r10, r3
	mov	r1, r10
	mov	r0, r5
	bl	__MapActor_SetBehavior
	ldr	r0, =0x2009
	bl	__MessageID
	mov	r1, #0
	mov	r0, r5
	bl	__ActorMessage
	mov	r6, #0x80
	mov	r0, r5
	bl	__MapActor_SetIdle
	lsl	r6, #9
	mov	r3, r8
	str	r6, [r3, #0x1c]
	str	r6, [r3, #0x18]
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, r5
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, r5
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, r10
	bl	__MapActor_SetBehavior
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0xe0
	mov	r2, #0
	mov	r0, r5
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r3, r8
	str	r6, [r3, #0x1c]
	str	r6, [r3, #0x18]
	mov	r0, r5
	mov	r1, r10
	bl	__MapActor_SetBehavior
	bl	__CutsceneEnd
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_952_20083b0

@ 48 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetActiveMessageId, ShowMessageAndWait, TestSaveBit
@   SetSaveBit, EndEncounterTransition, DialogueWait, PlayInteractionEffect
@   FaceEntityInstant, ShowMessageAndWait, DialogueWait, SetSlotAnimationAndWait
@   DialogueWait, ShowMessageAndWait
@   ... and 2 more
@ message id 0x2052; reads save bit 0x968; sets 0x968.
.thumb_func_start OvlFunc_952_200849c
	push	{r5, lr}
	mov	r5, r1
	bl	__CutsceneStart
	ldr	r0, =0x2052
	bl	__MessageID
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	ldr	r0, =0x968
	bl	__GetFlag
	cmp	r0, #0
	bne	.L512
	ldr	r0, =0x968
	bl	__SetFlag
	bl	__Func_8097608
	mov	r0, #0x32
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, r5
	lsl	r1, #1
	mov	r2, #0x46
	bl	__MapActor_Emote
	mov	r2, #0x28
	mov	r0, r5
	mov	r1, #0
	bl	__Func_809280c
	mov	r1, #0
	mov	r0, r5
	bl	__ActorMessage
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #4
	mov	r0, r5
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, r5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
.L512:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_952_200849c

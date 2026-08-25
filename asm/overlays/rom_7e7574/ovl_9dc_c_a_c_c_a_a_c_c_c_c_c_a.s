	.include "macros.inc"

@ 52 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, MoveSlotBy, SetSlotScriptWithTurn, SetSlotAnimation
@   FaceEntityInstant, PlaySound, PlayInteractionEffect, SetActiveMessageId
@   ShowMessageAndWait, PlayInteractionEffect, SetActiveMessageId, ShowMessageAndWait
@   HideScreenOverlay, DialogueWait
@   ... and 3 more
@ message id 0x240d; sets 0x224.
.thumb_func_start OvlFunc_959_200a134
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	bl	__Func_809228c
	mov	r0, #0
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, #0
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0xc
	bl	__Func_809280c
	mov	r0, #0x71
	bl	__PlaySound
	mov	r1, #0x80
	mov	r2, #0x3c
	lsl	r1, #1
	mov	r0, #0xc
	bl	__MapActor_Emote
	ldr	r5, =0x240d
	mov	r0, r5
	bl	__MessageID
	mov	r0, #0xc
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x81
	mov	r2, #0x32
	lsl	r1, #1
	mov	r0, #0
	add	r5, #1
	bl	__MapActor_Emote
	mov	r0, r5
	bl	__MessageID
	mov	r1, #0
	mov	r0, #0xc
	bl	__ActorMessage
	bl	__MapTransitionOut
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0x3c
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	mov	r0, #0x89
	lsl	r0, #2
	bl	__SetFlag
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_200a134

@ Cutscene: roughly 60 instructions of straight-line script --
@ 0 turns, 1 animation change, 2 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0x240d.
@ Sets save bit 0x225.
.thumb_func_start OvlFunc_959_200a1c4
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	bl	__Func_809228c
	mov	r0, #0
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r1, #1
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0x71
	bl	__PlaySound
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0xd
	lsl	r1, #1
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0
	bl	__Func_809280c
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xd
	bl	__Func_809280c
	ldr	r5, =0x240d
	mov	r0, r5
	bl	__MessageID
	mov	r0, #0xd
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x81
	mov	r2, #0x1e
	lsl	r1, #1
	mov	r0, #0
	add	r5, #1
	bl	__MapActor_Emote
	mov	r0, r5
	bl	__MessageID
	mov	r1, #0
	mov	r0, #0xd
	bl	__ActorMessage
	bl	__MapTransitionOut
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0x3c
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	ldr	r0, =0x225
	bl	__SetFlag
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_200a1c4

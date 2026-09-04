	.include "macros.inc"
	.include "gba.inc"

@ 38 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, TestSaveBit, PlaySound, SetEntityAnimation
@   SetSaveBit
@ reads save bit 0x200; sets 0x200.
.thumb_func_start OvlFunc_881_20097a4
	push	{r5, lr}
	mov	r0, #0xf
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0xe
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	str	r3, [r5, #8]
	ldr	r3, [r0, #0x10]
	mov	r2, #0xa0
	str	r3, [r5, #0x10]
	ldr	r3, [r5, #0xc]
	lsl	r2, #12
	cmp	r3, r2
	bge	.L17f6
	mov	r3, #0xa0
	lsl	r3, #12
	mov	r0, #0x80
	str	r3, [r5, #0xc]
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L17f6
	mov	r0, #0x91
	bl	__PlaySound
	mov	r0, r5
	mov	r1, #3
	bl	__Actor_SetAnim
	mov	r0, #0x80
	lsl	r0, #2
	bl	__SetFlag
	mov	r2, r5
	add	r2, #0x64
	mov	r3, #1
	strh	r3, [r2]
.L17f6:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_20097a4

@ 45 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, MoveCameraTo, WaitFrames
@   PlaceSlotAt, AttachCameraToSlot, ShowScreenOverlay, SetSlotEntitySpeed
@   WalkSlotToAndWait, HideScreenOverlay, WaitSceneDelay, SetSaveBit
@   SetPendingMessageId, EndCutscene
@ sets 0x927.
.thumb_func_start OvlFunc_881_20097fc
	push	{r5, lr}
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r1, r1
	neg	r2, r2
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	bl	__MapActor_SetPos
	mov	r3, #0xa0
	lsl	r3, #9
	str	r3, [r5, #0x1c]
	str	r3, [r5, #0x18]
	mov	r0, #8
	mov	r1, #1
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	mov	r0, #8
	ldr	r1, =0x6666
	ldr	r2, =0x3333
	bl	__MapActor_SetSpeed
	ldr	r1, =0x14a8
	ldr	r2, =0x918
	mov	r0, #8
	bl	__Func_80921c4
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x927
	bl	__SetFlag
	mov	r0, #0x66
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_20097fc

@ 60 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, MoveCameraTo, WaitFrames
@   PlaceSlotAt x2, WaitFrames, AttachCameraToSlot, ShowScreenOverlay
@   SetSlotEntitySpeed, SetSlotScriptWithTurn, WaitFrames, HideScreenOverlay
@   WaitSceneDelay, SetSaveBit
@   ... and 2 more
@ sets 0x927.
.thumb_func_start OvlFunc_881_2009888
	push	{r5, lr}
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r1, r1
	neg	r2, r2
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, #0xc8
	lsl	r2, #16
	ldr	r1, =0x1f080000
	mov	r0, #8
	bl	__MapActor_SetPos
	mov	r3, #0xa0
	lsl	r3, #9
	str	r3, [r5, #0x18]
	str	r3, [r5, #0x1c]
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #1
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	mov	r0, #8
	ldr	r1, =0x9999
	ldr	r2, =0x4ccc
	bl	__MapActor_SetSpeed
	add	r5, #0x64
	mov	r3, #0
	strh	r3, [r5]
	ldr	r1, =gScript_881__0200d158
	mov	r0, #8
	bl	__MapActor_SetBehavior
.L18f6:
	mov	r0, #1
	bl	__WaitFrames
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L18f6
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x927
	bl	__SetFlag
	mov	r0, #0x67
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_2009888

@ 60 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, MoveCameraTo, WaitFrames
@   PlaceSlotAt x2, WaitFrames, AttachCameraToSlot, ShowScreenOverlay
@   SetSlotEntitySpeed, SetSlotScriptWithTurn, WaitFrames, HideScreenOverlay
@   WaitSceneDelay, SetSaveBit
@   ... and 2 more
@ sets 0x927.
.thumb_func_start OvlFunc_881_2009938
	push	{r5, lr}
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r1, r1
	neg	r2, r2
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, #0xc8
	lsl	r2, #16
	ldr	r1, =0x1f080000
	mov	r0, #8
	bl	__MapActor_SetPos
	mov	r3, #0xa0
	lsl	r3, #9
	str	r3, [r5, #0x18]
	str	r3, [r5, #0x1c]
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #1
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	mov	r0, #8
	ldr	r1, =0x9999
	ldr	r2, =0x4ccc
	bl	__MapActor_SetSpeed
	add	r5, #0x64
	mov	r3, #0
	strh	r3, [r5]
	ldr	r1, =gScript_881__0200d158
	mov	r0, #8
	bl	__MapActor_SetBehavior
.L19a6:
	mov	r0, #1
	bl	__WaitFrames
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L19a6
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x927
	bl	__SetFlag
	mov	r0, #0x68
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_2009938

@ 60 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, MoveCameraTo, WaitFrames
@   PlaceSlotAt x2, WaitFrames, AttachCameraToSlot, ShowScreenOverlay
@   SetSlotEntitySpeed, SetSlotScriptWithTurn, WaitFrames, HideScreenOverlay
@   WaitSceneDelay, SetSaveBit
@   ... and 2 more
@ sets 0x927.
.thumb_func_start OvlFunc_881_20099e8
	push	{r5, lr}
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r1, r1
	neg	r2, r2
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, #0xc8
	lsl	r2, #16
	ldr	r1, =0x1f080000
	mov	r0, #8
	bl	__MapActor_SetPos
	mov	r3, #0xa0
	lsl	r3, #9
	str	r3, [r5, #0x18]
	str	r3, [r5, #0x1c]
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #1
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	mov	r0, #8
	ldr	r1, =0x9999
	ldr	r2, =0x4ccc
	bl	__MapActor_SetSpeed
	add	r5, #0x64
	mov	r3, #0
	strh	r3, [r5]
	ldr	r1, =gScript_881__0200d158
	mov	r0, #8
	bl	__MapActor_SetBehavior
.L1a56:
	mov	r0, #1
	bl	__WaitFrames
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L1a56
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x927
	bl	__SetFlag
	mov	r0, #0x69
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_20099e8

@ 67 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, MoveCameraTo, WaitFrames
@   PlaceSlotAt x2, WaitFrames, AttachCameraToSlot, ShowScreenOverlay
@   SetSlotEntitySpeed, OvlFunc_341c, SetSlotScriptWithTurn x2, WaitFrames
@   HideScreenOverlay, WaitSceneDelay
@   ... and 3 more
@ sets 0x927.
.thumb_func_start OvlFunc_881_2009a98
	push	{r5, lr}
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r1, r1
	neg	r2, r2
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, #0xc8
	lsl	r2, #16
	ldr	r1, =0x1f080000
	mov	r0, #8
	bl	__MapActor_SetPos
	mov	r3, #0xa0
	lsl	r3, #9
	str	r3, [r5, #0x18]
	str	r3, [r5, #0x1c]
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #1
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	mov	r0, #8
	ldr	r1, =0x9999
	ldr	r2, =0x4ccc
	bl	__MapActor_SetSpeed
	add	r5, #0x64
	mov	r3, #0
	strh	r3, [r5]
	bl	OvlFunc_881_200b41c
	cmp	r0, #0xb
	bne	.L1b10
	ldr	r1, =gScript_881__0200d1b8
	mov	r0, #8
	bl	__MapActor_SetBehavior
	b	.L1b18
.L1b10:
	ldr	r1, =gScript_881__0200d158
	mov	r0, #8
	bl	__MapActor_SetBehavior
.L1b18:
	mov	r0, #1
	bl	__WaitFrames
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L1b18
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x927
	bl	__SetFlag
	mov	r0, #0x6a
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_2009a98

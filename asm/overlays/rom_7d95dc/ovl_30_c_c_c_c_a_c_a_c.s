	.include "macros.inc"

@ Cutscene: roughly 109 instructions of straight-line script --
@ 14 turns, 2 animation changes, 0 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2112.
.thumb_func_start OvlFunc_953_200a4d8
	push	{r5, lr}
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #0xd
	bl	__MapActor_SetIdle
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0xe0
	mov	r2, #0
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #0xd
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0xd0
	mov	r0, #0xc
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0xe
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xd0
	mov	r0, #0xf
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r0, #0x11
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r2, #0
	lsl	r1, #8
	mov	r0, #0x12
	bl	__Func_8092adc
	ldr	r0, =0x2112
	bl	__MessageID
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r2, r5
	mov	r3, #0xb4
	add	r2, #0x64
	lsl	r3, #2
	strh	r3, [r2]
	add	r5, #0x66
	mov	r3, #0x70
	strh	r3, [r5]
	mov	r0, #0xd
	mov	r1, #2
	bl	__MapActor_SetBehavior
	mov	r1, #0xc0
	mov	r0, #0xc
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r0, #0xe
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0xf
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0x11
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0x12
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a4d8

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

@ Cutscene: roughly 164 instructions of straight-line script --
@ 2 turns, 5 animation changes, 0 dialogue lines, 4 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2134.
.thumb_func_start OvlFunc_953_200a668
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r1, #0xc6
	mov	r2, #0x88
	mov	r0, #1
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xce
	mov	r2, #0x88
	mov	r0, #2
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xca
	mov	r2, #0x98
	lsl	r2, #16
	lsl	r1, #18
	mov	r0, #3
	bl	__MapActor_SetPos
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	ldr	r0, =0x2134
	bl	__MessageID
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r1, #1
	mov	r0, #9
	bl	__Func_80925cc
	mov	r0, #9
	bl	OvlFunc_953_2009c48
	mov	r1, #1
	mov	r0, #0xa
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	OvlFunc_953_2009c48
	mov	r0, #0xb
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	bl	OvlFunc_953_2009c48
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #2
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #2
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	mov	r0, #3
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_953__0200adac
	mov	r0, #1
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #2
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #3
	bl	__MapActor_RunScript
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0
	bl	OvlFunc_953_2009c5c
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xb
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	mov	r0, #0
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r0, #0xb
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	ldr	r1, =0x33e
	mov	r2, #0x98
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r0, #0xb
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r2, #1
	lsl	r1, #2
	mov	r0, #0xb
	bl	__MapActor_TravelTo
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_80933d4
	mov	r0, #0xca
	mov	r1, #1
	mov	r2, #0x9c
	mov	r3, #1
	lsl	r0, #18
	neg	r1, r1
	lsl	r2, #17
	bl	__Func_80933f8
	mov	r1, #0xca
	mov	r0, #0
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_80921c4
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r1, #2
	lsl	r2, #1
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0x3c
	bl	__CutsceneWait
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x43
	bl	__Func_8091e9c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a668

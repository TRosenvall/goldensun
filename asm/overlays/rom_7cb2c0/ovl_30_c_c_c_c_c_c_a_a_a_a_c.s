	.include "macros.inc"

@ 34 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, MoveCameraTo, WaitFrames, OvlFunc_48e8
@   SetFollowerFormationAndRefresh, SetActiveMessageId, OvlFunc_486c, TurnSlotToAngle
@   OvlFunc_48e8
@ message id 0x1e43.
.thumb_func_start OvlFunc_945_200c13c
	push	{lr}
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
	mov	r2, #1
	mov	r0, #0xf
	mov	r1, #1
	bl	OvlFunc_945_200c8e8
	mov	r1, #1
	mov	r0, #8
	bl	__Func_80925cc
	ldr	r0, =0x1e43
	bl	__MessageID
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r1, #0xd0
	mov	r0, #8
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r0, #9
	mov	r1, #0xf
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_200c13c

@ 45 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, OvlFunc_48e8 x2, OvlFunc_37b4, OvlFunc_48e8
@   SetSlotAnimation, SetSlotScriptWithTurn x4, SetSlotPalette x3, OvlFunc_50e4
@   EndCutscene
.thumb_func_start OvlFunc_945_200c198
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r0, #0x18
	mov	r1, #1
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	mov	r0, #0x19
	mov	r1, #1
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	bl	OvlFunc_945_200b7b4
	mov	r2, #0xc
	mov	r0, #0x13
	mov	r1, #0xb
	bl	OvlFunc_945_200c8e8
	mov	r0, #0xa
	mov	r1, #6
	bl	__MapActor_SetAnim
	ldr	r1, =gScript_945__0200e840
	mov	r0, #0xc
	bl	__MapActor_SetBehavior
	ldr	r5, =gScript_945__0200e8e4
	mov	r0, #0x24
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0x25
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0x26
	bl	__MapActor_SetBehavior
	mov	r0, #0x24
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x25
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x26
	mov	r1, #3
	bl	__Func_8092950
	bl	OvlFunc_945_200d0e4
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_200c198


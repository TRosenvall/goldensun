	.include "macros.inc"

@ 60 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   FindAndConsumeWithNotify, BeginCutscene, DialogueWait, SetFollowerFormationAndRefresh
@   SetSlotEntitySpeed, WalkSlotToAndWait, DialogueWait, TurnSlotToAngle
@   SetSlotFacingAndScript, DialogueWait, SetSlotFacingAndScript, DialogueWait
@   SetSlotFacingAndScript, DialogueWait
@   ... and 5 more
@ sets 0x858.
.thumb_func_start OvlFunc_898_20092c0
	push	{lr}
	mov	r0, #0xe7
	bl	__Func_8078a08
	bl	__CutsceneStart
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0x13
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x13
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r2, #0xcc
	mov	r1, #0xd8
	lsl	r2, #1
	mov	r0, #0x13
	bl	__Func_80921c4
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0x13
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #6
	mov	r2, #0
	mov	r0, #0x13
	bl	__MapActor_Jump
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #6
	mov	r2, #0
	mov	r0, #0x13
	bl	__MapActor_Jump
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #6
	mov	r2, #0
	mov	r0, #0x13
	bl	__MapActor_Jump
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r2, #0xc4
	mov	r1, #0xd8
	lsl	r2, #1
	mov	r0, #0x13
	bl	__Func_80921c4
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0x80
	lsl	r1, #7
	mov	r2, #0x14
	mov	r0, #0x13
	bl	__Func_8092adc
	ldr	r0, =0x858
	bl	__SetFlag
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_898_20092c0

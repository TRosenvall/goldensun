	.include "macros.inc"

@ 14 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetActiveMessageId, OvlFunc_3bc, TurnSlotToAngle
@   EndCutscene
@ message id 0x1253.
.thumb_func_start OvlFunc_899_2008428
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =0x1253
	bl	__MessageID
	mov	r0, #0xf
	bl	OvlFunc_899_20083bc
	mov	r1, #0x80
	mov	r0, #0xf
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_899_2008428


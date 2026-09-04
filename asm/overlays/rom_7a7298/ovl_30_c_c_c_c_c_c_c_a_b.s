	.include "macros.inc"


@ 15 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, ShowScreenOverlay, SetSlotEntitySpeed, MoveSlotToAndWait
@   EndCutscene
.thumb_func_start OvlFunc_921_20099bc
	push	{lr}
	bl	__CutsceneStart
	bl	__MapTransitionIn
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #10
	ldr	r2, =0x1999
	bl	__MapActor_SetSpeed
	mov	r0, #0
	mov	r1, #0xe8
	mov	r2, #0xcc
	bl	__Func_8092158
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_921_20099bc

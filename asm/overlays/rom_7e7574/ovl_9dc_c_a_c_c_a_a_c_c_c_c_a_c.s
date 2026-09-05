	.include "macros.inc"

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, PlaceSlotAt x3, SetSlotAnimation x3, FaceEntityInstant
@   EndCutscene, ShowScreenOverlay
.thumb_func_start OvlFunc_959_200a06c
	push	{lr}
	bl	__CutsceneStart
	mov	r1, #0xac
	mov	r2, #0xb0
	mov	r0, #0xc
	lsl	r1, #18
	lsl	r2, #15
	bl	__MapActor_SetPos
	mov	r1, #0xb0
	mov	r2, #0xb0
	mov	r0, #0xd
	lsl	r1, #18
	lsl	r2, #15
	bl	__MapActor_SetPos
	mov	r1, #0xb4
	mov	r2, #0xc0
	lsl	r2, #15
	mov	r0, #0xe
	lsl	r1, #18
	bl	__MapActor_SetPos
	mov	r0, #0xc
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r0, #0xd
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r0, #0xe
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r0, #0
	mov	r1, #0xd
	mov	r2, #0
	bl	__Func_809280c
	bl	__CutsceneEnd
	bl	__MapTransitionIn
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_200a06c

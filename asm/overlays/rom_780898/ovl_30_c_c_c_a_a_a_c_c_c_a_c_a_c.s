	.include "macros.inc"

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, BeginCutscene, SetSlotEntitySpeed, SetActiveMessageId
@   ShowMessageAndPause x2, RunTextBoxModal, DialogueWait, WalkSlotToAndWait
@   EndCutscene
@ message id 0xf4d; reads save bit 0x808.
.thumb_func_start OvlFunc_883_20091d8
	push	{r5, lr}
	ldr	r0, =0x808
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1230
	bl	__CutsceneStart
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #9
	lsl	r2, #8
	mov	r0, #0
	bl	__MapActor_SetSpeed
	ldr	r5, =0xf4d
	mov	r0, r5
	bl	__MessageID
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #2
	bl	__Func_8093040
	add	r5, #2
	mov	r2, #2
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #1
	mov	r0, r5
	bl	__Func_801776c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0x45
	ldr	r2, =0x366
	bl	__Func_80921c4
	bl	__CutsceneEnd
.L1230:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_883_20091d8

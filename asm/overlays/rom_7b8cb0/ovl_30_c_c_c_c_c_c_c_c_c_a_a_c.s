	.include "macros.inc"

@ 35 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, BeginCutscene, SetActiveMessageId, ShowMessageAndWait
@   EndCutscene, OvlFunc_338, RunShop, BeginCutscene
@   SetActiveMessageId, TestSaveBit, SetActiveMessageId, ShowMessageAndWait
@   EndCutscene
@ message ids 0x18ed, 0x18ee, 0x1943; reads save bits 0x241, 0x909.
.thumb_func_start OvlFunc_931_20083d4
	push	{lr}
	ldr	r0, =0x241
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3f8
	bl	__CutsceneStart
	ldr	r0, =0x18ed
	bl	__MessageID
	mov	r0, #0x14
	mov	r1, #0
	bl	__ActorMessage
	bl	__CutsceneEnd
	b	.L430
.L3f8:
	bl	OvlFunc_931_2008338
	cmp	r0, #0
	beq	.L40a
	mov	r0, #0x14
	mov	r1, #0x11
	bl	__Func_80b0278
	b	.L430
.L40a:
	bl	__CutsceneStart
	ldr	r0, =0x18ee
	bl	__MessageID
	ldr	r0, =0x909
	bl	__GetFlag
	cmp	r0, #0
	beq	.L424
	ldr	r0, =0x1943
	bl	__MessageID
.L424:
	mov	r0, #0x11
	mov	r1, #0
	bl	__ActorMessage
	bl	__CutsceneEnd
.L430:
	pop	{r0}
	bx	r0
.func_end OvlFunc_931_20083d4

@ 36 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, BeginCutscene, SetActiveMessageId, ShowMessageAndWait
@   EndCutscene, OvlFunc_338, RunShop, BeginCutscene
@   SetActiveMessageId, TestSaveBit, SetActiveMessageId, ShowMessageAndWait
@   EndCutscene
@ message ids 0x18f1, 0x18f2, 0x1945; reads save bits 0x240, 0x909.
.thumb_func_start OvlFunc_931_2008448
	push	{lr}
	mov	r0, #0x90
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L46e
	bl	__CutsceneStart
	ldr	r0, =0x18f1
	bl	__MessageID
	mov	r0, #0x15
	mov	r1, #0
	bl	__ActorMessage
	bl	__CutsceneEnd
	b	.L4a6
.L46e:
	bl	OvlFunc_931_2008338
	cmp	r0, #0
	beq	.L480
	mov	r0, #0x15
	mov	r1, #0x10
	bl	__Func_80b0278
	b	.L4a6
.L480:
	bl	__CutsceneStart
	ldr	r0, =0x18f2
	bl	__MessageID
	ldr	r0, =0x909
	bl	__GetFlag
	cmp	r0, #0
	beq	.L49a
	ldr	r0, =0x1945
	bl	__MessageID
.L49a:
	mov	r0, #0x10
	mov	r1, #0
	bl	__ActorMessage
	bl	__CutsceneEnd
.L4a6:
	pop	{r0}
	bx	r0
.func_end OvlFunc_931_2008448

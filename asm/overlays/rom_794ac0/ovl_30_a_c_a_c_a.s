	.include "macros.inc"

@ 33 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, TestSaveBit x2, SetActiveMessageId, OvlFunc_354
@   DialogueWait, OvlFunc_463c, SetSaveBit, SetActiveMessageId x2
@   OvlFunc_354, EndCutscene
@ message ids 0x1250, 0x1276, 0x1278; reads save bits 0x851, 0x856; sets 0x851.
.thumb_func_start OvlFunc_899_200852c
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =0x856
	bl	__GetFlag
	cmp	r0, #0
	beq	.L572
	ldr	r0, =0x851
	bl	__GetFlag
	cmp	r0, #0
	bne	.L56a
	ldr	r0, =0x1276
	bl	__MessageID
	mov	r0, #0x10
	bl	OvlFunc_899_2008354
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0x10
	mov	r1, #3
	mov	r2, #0x14
	bl	OvlFunc_899_200c63c
	ldr	r0, =0x851
	bl	__SetFlag
	b	.L578
.L56a:
	ldr	r0, =0x1278
	bl	__MessageID
	b	.L578
.L572:
	ldr	r0, =0x1250
	bl	__MessageID
.L578:
	mov	r0, #0x10
	bl	OvlFunc_899_2008354
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_899_200852c


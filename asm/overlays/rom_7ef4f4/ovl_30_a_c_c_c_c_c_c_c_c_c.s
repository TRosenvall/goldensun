	.include "macros.inc"

@ 53 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, TestSaveBit, RunTextBoxModal, PlaySound
@   CopyMapRectIndices, DialogueWait, CopyMapRectIndices, DialogueWait
@   OvlFunc_24d0, SetActiveMessageId, ShowMessageAndWait, EndCutscene
@ message id 0x2756; reads save bit 0x985.
.thumb_func_start OvlFunc_965_200a5c8
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	sub	sp, #8
	ldr	r5, [r3]
	bl	__CutsceneStart
	ldr	r2, =0xcb8
	add	r5, r2
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L262e
	ldr	r0, =0x985
	bl	__GetFlag
	cmp	r0, #0
	bne	.L263e
	mov	r1, #1
	ldr	r0, =0x1528
	bl	__Func_801776c
	mov	r0, #0x9b
	bl	__PlaySound
	mov	r5, #0x11
	mov	r1, #0x4e
	mov	r2, #1
	mov	r3, #2
	mov	r6, #0x4e
	mov	r0, #0x23
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010788
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0x22
	mov	r1, #0x4e
	mov	r2, #1
	mov	r3, #2
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010788
	mov	r0, #0xa
	bl	__CutsceneWait
	bl	OvlFunc_965_200a4d0
	b	.L263e
.L262e:
	ldr	r0, =0x2756
	bl	__MessageID
	mov	r0, #1
	neg	r0, r0
	mov	r1, #0
	bl	__ActorMessage
.L263e:
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_965_200a5c8

@ 30 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetTerrainHeight, OvlFunc_6c
.thumb_func_start OvlFunc_965_200a660
	push	{r5, r6, lr}
	mov	r5, r0
	ldrh	r3, [r5, #6]
	ldr	r2, =.L2fd4
	lsr	r3, #12
	lsl	r3, #2
	ldr	r0, [r2, r3]
	ldr	r3, =0xffff0000
	ldr	r1, [r5, #8]
	ldr	r2, [r5, #0x10]
	sub	sp, #0xc
	and	r3, r0
	lsl	r0, #16
	mov	r6, sp
	add	r2, r0
	add	r1, r3
	str	r1, [r6]
	str	r2, [r6, #8]
	mov	r3, r5
	add	r3, #0x22
	ldrb	r0, [r3]
	bl	__Func_8011f54
	mov	r1, r5
	str	r0, [r6, #4]
	mov	r0, r6
	bl	OvlFunc_965_200806c
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_965_200a660

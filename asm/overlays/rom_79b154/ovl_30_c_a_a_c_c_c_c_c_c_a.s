	.include "macros.inc"

@ 89 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, GetSlotEntityChecked, PlaySound x2, PlayMapRectAnimation
@   SetSlotEntitySpeed, GetSlotEntityChecked, SetSlotAnimation, MoveSlotBy
@   WalkSlotThroughDoorway, SetSlotDrawPriority x2, DialogueWait, SetPendingMessageId
@   EndCutscene
.thumb_func_start OvlFunc_907_2008328
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r7, [r3]
	bl	__CutsceneStart
	mov	r5, #8
	mov	r6, #0
.L336:
	mov	r0, r5
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L346
	mov	r3, r0
	add	r3, #0x55
	strb	r6, [r3]
.L346:
	add	r5, #1
	cmp	r5, #0x41
	bls	.L336
	mov	r2, #0xb6
	lsl	r2, #1
	add	r3, r7, r2
	ldrh	r3, [r3]
	sub	r3, #3
	lsl	r3, #16
	asr	r5, r3, #16
	cmp	r5, #6
	bne	.L366
	mov	r0, #0xbc
	bl	__PlaySound
	b	.L36c
.L366:
	mov	r0, #0x9e
	bl	__PlaySound
.L36c:
	ldr	r2, =.L1d0c
	lsl	r0, r5, #2
	ldrsh	r1, [r2, r0]
	add	r3, r0, #2
	ldrsh	r2, [r2, r3]
	ldr	r3, =.L1cf0
	ldr	r0, [r3, r0]
	bl	__Func_8010560
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #7
	mov	r0, #0
	lsl	r1, #8
	bl	__MapActor_SetSpeed
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	cmp	r5, #6
	bne	.L3c0
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r2, #4
	mov	r0, #0
	mov	r1, #0
	neg	r2, r2
	bl	__Func_809228c
	b	.L3cc
.L3c0:
	mov	r2, #0x10
	mov	r0, #0
	mov	r1, #3
	neg	r2, r2
	bl	__Func_8092208
.L3cc:
	cmp	r5, #4
	bne	.L3da
	mov	r0, #0
	mov	r1, #3
	bl	__Func_8092b08
	b	.L3e2
.L3da:
	mov	r0, #0
	mov	r1, #2
	bl	__Func_8092b08
.L3e2:
	mov	r0, #0x10
	bl	__CutsceneWait
	add	r0, r5, #3
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_907_2008328

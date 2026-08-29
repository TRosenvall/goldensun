	.include "macros.inc"

@ Cutscene: roughly 149 instructions of straight-line script --
@ 3 turns, 2 animation changes, 0 dialogue lines, 11 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x1164.
@ Sets save bit 0x9f0.
.thumb_func_start OvlFunc_888_2008360
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =0x1164
	bl	__MessageID
	mov	r1, #0
	mov	r0, #8
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L390
	mov	r0, #8
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	b	.L4da
.L390:
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #8
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	beq	.L3ac
	b	.L4da
.L3ac:
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #8
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	beq	.L3c8
	b	.L4da
.L3c8:
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r2, #0xa0
	ldrh	r3, [r0, #6]
	lsl	r2, #8
	cmp	r3, r2
	bcc	.L488
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r2, #0xe0
	ldrh	r3, [r0, #6]
	lsl	r2, #8
	cmp	r3, r2
	bhi	.L488
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #8
	lsl	r1, #8
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r1, #0
	mov	r2, #0
	mov	r0, #8
	bl	__Func_8092adc
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #8
	bl	__MapActor_GetActor
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #0xfe
	and	r3, r2
	strb	r3, [r0]
	mov	r1, #0x98
	mov	r2, #0x78
	mov	r0, #8
	bl	__Func_80921c4
	mov	r0, #1
	bl	__CutsceneWait
	mov	r0, #8
	bl	__MapActor_GetActor
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r0]
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0xa8
	mov	r2, #0x78
	bl	__Func_80921c4
	mov	r1, #0xc0
	mov	r2, #0xa8
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #0xa8
	mov	r2, #0x78
	bl	__Func_80921c4
	mov	r1, #0xc0
	mov	r0, #8
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0
	bl	__MapActor_WaitMovement
	b	.L4aa
.L488:
	mov	r1, #0xc0
	mov	r2, #0xa8
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r0, #8
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0
	bl	__MapActor_WaitMovement
.L4aa:
	bl	OvlFunc_888_200987c
	mov	r1, #0
	mov	r0, #0
	bl	__Func_8091200
	mov	r0, #0x78
	bl	__Func_8091254
	mov	r0, #0x78
	bl	__CutsceneWait
	mov	r0, #0x56
	bl	__PlaySound
	bl	__Func_80f95a0
	mov	r0, #0x9f
	lsl	r0, #4
	bl	__SetFlag
	mov	r0, #0x1e
	bl	__Func_8091e9c
.L4da:
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_888_2008360

@ 54 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, AddSceneRecord, WaitFrames, SetActiveMessageId
@   OpenMessageBoxForSlot, RunFieldAbilityPrompt, ShowMessageAndWait, ShowMessageAtSlotOffset
@   ShowMessageAndWait, EndCutscene
@ message id 0x1bfd.
.thumb_func_start OvlFunc_888_20084e8
	push	{r5, lr}
	sub	sp, #0x1c
	bl	__CutsceneStart
	ldr	r0, =.L3c9c
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =0x1bfd
	bl	__MessageID
	mov	r1, #0
	mov	r0, #9
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L520
	mov	r0, #9
	mov	r1, #0
	bl	__ActorMessage
	b	.L55c
.L520:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, #1
	mov	r3, #3
	mov	r2, #7
	mov	r1, #0x10
	mov	r4, #0xe
	str	r0, [sp]
	str	r3, [sp, #4]
	str	r2, [sp, #8]
	str	r0, [sp, #0x10]
	mov	r5, #0
	mov	r0, #2
	mov	r2, #1
	mov	r3, #0x18
	str	r1, [sp, #0xc]
	str	r4, [sp, #0x14]
	str	r5, [sp, #0x18]
	bl	__Func_80931ec
	mov	r0, #9
	mov	r1, #0
	bl	__ActorMessage
.L55c:
	bl	__CutsceneEnd
	add	sp, #0x1c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_888_20084e8

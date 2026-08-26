	.include "macros.inc"


@ 29 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, TestSaveBit, SetActiveMessageId
@   OvlFunc_4b4, OvlFunc_58c, SetSaveBit, EndCutscene
@   GetSlotEntityChecked
@ message id 0x1cc0; reads save bit 0x307; sets 0x307.
.thumb_func_start OvlFunc_901_2008804
	push	{lr}
	mov	r0, #0xe
	bl	__MapActor_GetActor
	add	r0, #0x64
	ldrh	r2, [r0]
	ldr	r3, =2
	orr	r3, r2
	strh	r3, [r0]
	bl	__CutsceneStart
	ldr	r0, =0x307
	bl	__GetFlag
	cmp	r0, #0
	beq	.L840
	ldr	r0, =_MSG_1cc0
	bl	__MessageID
	mov	r0, #0xe
	bl	OvlFunc_901_20084b4
	b	.L84a

	.pool_aligned

.L840:
	bl	OvlFunc_901_200858c
	ldr	r0, =0x307
	bl	__SetFlag
.L84a:
	bl	__CutsceneEnd
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r3, #1
	add	r0, #0x64
	strh	r3, [r0]
	pop	{r0}
	bx	r0
.func_end OvlFunc_901_2008804

@ 23 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, SetActiveMessageId, OvlFunc_4b4
@   EndCutscene, GetSlotEntityChecked
@ message id 0x1cc1.
.thumb_func_start OvlFunc_901_2008864
	push	{r5, lr}
	mov	r0, #0xf
	bl	__MapActor_GetActor
	add	r0, #0x64
	ldrh	r2, [r0]
	ldr	r3, =2
	orr	r3, r2
	strh	r3, [r0]
	bl	__CutsceneStart
	ldr	r0, =0x1cc1
	bl	__MessageID
	mov	r0, #0xf
	bl	OvlFunc_901_20084b4
	bl	__CutsceneEnd
	mov	r0, #0xf
	bl	__MapActor_GetActor
	mov	r5, #0
	add	r0, #0x64
	strh	r5, [r0]
	b	.L8a0

	.pool_aligned

.L8a0:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_901_2008864

@ Cutscene: roughly 75 instructions of straight-line script --
@ 0 turns, 1 animation change, 1 dialogue line, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x1cb5, 0x1cc2.
@ Reads save bit 0x308.
@ Sets save bit 0x308.
.thumb_func_start OvlFunc_901_20088a8
	push	{r5, lr}
	mov	r0, #0xc2
	lsl	r0, #2
	bl	__GetFlag
	mov	r5, r0
	cmp	r5, #0
	bne	.L93a
	bl	__CutsceneStart
	mov	r0, #0x10
	bl	__MapActor_GetActor
	mov	r3, #1
	add	r0, #0x5b
	strb	r3, [r0]
	mov	r1, #1
	mov	r0, #0x10
	bl	__MapActor_SetAnim
	mov	r1, #1
	mov	r0, #0x10
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x1cb5
	bl	__MessageID
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #2
	bl	__Func_8092848
	mov	r1, #0
	mov	r0, #0x10
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	beq	.L912
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L912:
	mov	r1, #0
	mov	r0, #0x10
	bl	__ActorMessage
	mov	r0, #0x10
	bl	__MapActor_GetActor
	add	r0, #0x5b
	strb	r5, [r0]
	mov	r1, #2
	mov	r0, #0x10
	bl	__MapActor_SetBehavior
	bl	__CutsceneEnd
	mov	r0, #0xc2
	lsl	r0, #2
	bl	__SetFlag
	b	.L95e
.L93a:
	ldr	r0, =0x1cc2
	bl	__MessageID
	mov	r0, #0x10
	bl	__MapActor_GetActor
	mov	r3, #1
	add	r0, #0x5b
	strb	r3, [r0]
	mov	r0, #0x10
	bl	OvlFunc_901_20084b4
	mov	r0, #0x10
	bl	__MapActor_GetActor
	mov	r5, #0
	add	r0, #0x5b
	strb	r5, [r0]
.L95e:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_901_20088a8

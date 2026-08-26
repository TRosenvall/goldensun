	.include "macros.inc"


@ Adjusts slot 0 (the player) directly.
@ Takes the entity with Func_92054 and writes its fields in place rather
@ than going through the slot helpers -- touches +0x8, +0x10.
.thumb_func_start OvlFunc_959_2009918
	push	{r5, lr}
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r2, [r5, #0x10]
	mov	r3, r0
	cmp	r2, #0
	bge	.L1932
	ldr	r0, =0xfffff
	add	r2, r0
.L1932:
	ldr	r0, [r5, #8]
	asr	r1, r2, #20
	cmp	r0, #0
	bge	.L193e
	ldr	r2, =0xfffff
	add	r0, r2
.L193e:
	ldr	r2, [r3, #0x10]
	asr	r4, r0, #20
	cmp	r2, #0
	bge	.L194a
	ldr	r0, =0xfffff
	add	r2, r0
.L194a:
	ldr	r0, [r3, #8]
	asr	r2, #20
	cmp	r0, #0
	bge	.L1956
	ldr	r3, =0xfffff
	add	r0, r3
.L1956:
	asr	r3, r0, #20
	sub	r3, r4, r3
	add	r1, #1
	cmp	r3, #0
	bge	.L1962
	neg	r3, r3
.L1962:
	sub	r0, r1, r2
	cmp	r0, #0
	bge	.L196a
	neg	r0, r0
.L196a:
	add	r3, r0
	mov	r0, #1
	cmp	r3, #4
	ble	.L1974
	mov	r0, #0
.L1974:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_959_2009918

@ 47 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, GetCameraEntity
.thumb_func_start OvlFunc_959_2009980
	push	{r5, lr}
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__Func_8093554
	ldr	r3, [r5, #8]
	mov	r2, r0
	cmp	r3, #0
	bge	.L1998
	ldr	r0, =0xfffff
	add	r3, r0
.L1998:
	ldr	r0, [r5, #0x10]
	asr	r4, r3, #20
	cmp	r0, #0
	bge	.L19a4
	ldr	r1, =0xfffff
	add	r0, r1
.L19a4:
	ldr	r3, [r2, #8]
	asr	r1, r0, #20
	cmp	r3, #0
	bge	.L19b0
	ldr	r0, =0xfffff
	add	r3, r0
.L19b0:
	ldr	r0, [r2, #0x10]
	asr	r3, #20
	cmp	r0, #0
	bge	.L19bc
	ldr	r2, =0xfffff
	add	r0, r2
.L19bc:
	sub	r3, r4, r3
	asr	r0, #20
	cmp	r3, #0
	bge	.L19c6
	neg	r3, r3
.L19c6:
	sub	r0, r1, r0
	cmp	r0, #0
	bge	.L19ce
	neg	r0, r0
.L19ce:
	cmp	r3, #7
	bgt	.L19d6
	cmp	r0, #5
	ble	.L19da
.L19d6:
	mov	r0, #0
	b	.L19dc
.L19da:
	mov	r0, #1
.L19dc:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_959_2009980

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, OvlFunc_f30
@ reads save bit 0x35b.
.thumb_func_start OvlFunc_959_20099e8
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	ldr	r0, =0x35b
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1a32
	ldr	r3, [r5, #8]
	cmp	r3, #0
	bge	.L1a06
	ldr	r2, =0xfffff
	add	r3, r2
.L1a06:
	ldr	r0, [r5, #0x10]
	asr	r3, #20
	cmp	r0, #0
	bge	.L1a12
	ldr	r2, =0xfffff
	add	r0, r2
.L1a12:
	asr	r0, #20
	cmp	r3, #0x2b
	bne	.L1a32
	cmp	r0, #0x1c
	ble	.L1a32
	cmp	r0, #0x1f
	bgt	.L1a32
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xb6
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0x29
	strh	r2, [r3]
	bl	OvlFunc_959_2008f30
.L1a32:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_20099e8

@ 47 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, OvlFunc_e80
@ reads save bit 0x358.
.thumb_func_start OvlFunc_959_2009a44
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0xd6
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1aa2
	ldr	r3, [r5, #8]
	cmp	r3, #0
	bge	.L1a64
	ldr	r1, =0xfffff
	add	r3, r1
.L1a64:
	asr	r2, r3, #20
	ldr	r3, [r5, #0x10]
	cmp	r3, #0
	bge	.L1a70
	ldr	r1, =0xfffff
	add	r3, r1
.L1a70:
	asr	r3, #20
	cmp	r2, #0x10
	bne	.L1aa2
	cmp	r3, #0x37
	ble	.L1aa2
	cmp	r3, #0x3a
	bgt	.L1aa2
	mov	r2, #0xc0
	ldrh	r0, [r5, #6]
	lsl	r2, #8
	cmp	r0, r2
	beq	.L1a90
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bne	.L1aa2
.L1a90:
	ldr	r3, =iwram_3001ebc
	mov	r1, #0xb6
	ldr	r3, [r3]
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0x28
	strh	r2, [r3]
	bl	OvlFunc_959_2008e80
.L1aa2:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2009a44

@ 41 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, MoveSlotBy, SetSlotScriptWithTurn, SetSlotWalkBehaviour
@   SetSlotAnimation, SetSlotScriptWithTurn, SetActiveMessageId, ShowMessageAndWait
@   PlayInteractionEffect, SetActiveMessageId, ShowMessageAndWait, SetPendingMessageId
@   HideScreenOverlay, EndCutscene
@ message id 0x240d.
.thumb_func_start OvlFunc_959_2009ab0
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r2, #0
	mov	r0, #9
	mov	r1, #0
	bl	__Func_809228c
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetBehavior
	mov	r0, #9
	bl	__MapActor_SetIdle
	mov	r0, #9
	mov	r1, #0
	bl	__MapActor_SetAnim
	mov	r1, #1
	mov	r0, #0
	bl	__MapActor_SetBehavior
	ldr	r5, =0x240d
	mov	r0, r5
	bl	__MessageID
	mov	r0, #9
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x81
	mov	r2, #0x3c
	lsl	r1, #1
	mov	r0, #0
	add	r5, #1
	bl	__MapActor_Emote
	mov	r0, r5
	bl	__MessageID
	mov	r1, #0
	mov	r0, #9
	bl	__ActorMessage
	mov	r0, #0x3c
	bl	__Func_8091e9c
	bl	__MapTransitionOut
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2009ab0

@ Cutscene: roughly 71 instructions of straight-line script --
@ 0 turns, 3 animation changes, 2 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0x240d.
.thumb_func_start OvlFunc_959_2009b24
	push	{r5, r6, lr}
	mov	r5, r0
	bl	__CutsceneStart
	bl	__CutsceneStart
	mov	r1, #0x80
	mov	r0, r5
	lsl	r1, #1
	mov	r2, #1
	bl	__MapActor_Emote
	mov	r2, #0
	mov	r0, r5
	mov	r1, #0
	bl	__Func_809228c
	mov	r0, r5
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, r5
	mov	r1, #0
	bl	__MapActor_SetAnim
	mov	r0, r5
	mov	r2, #0
	mov	r1, #0
	bl	__Func_809280c
	mov	r0, #0
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r2, #0
	mov	r0, r5
	mov	r1, #0
	bl	__Func_809228c
	mov	r0, r5
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, r5
	bl	__MapActor_SetIdle
	mov	r0, r5
	mov	r1, #0
	bl	__MapActor_SetAnim
	mov	r1, #1
	mov	r0, #0
	bl	__MapActor_SetBehavior
	ldr	r6, =0x240d
	mov	r0, r6
	bl	__MessageID
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, r5
	mov	r0, #0
	mov	r2, #0
	bl	__Func_809280c
	mov	r1, #0x81
	mov	r2, #0x3c
	lsl	r1, #1
	mov	r0, #0
	add	r6, #1
	bl	__MapActor_Emote
	mov	r0, r6
	bl	__MessageID
	mov	r1, #0
	mov	r0, r5
	bl	__ActorMessage
	bl	__MapTransitionOut
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0x3c
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2009b24

	.include "macros.inc"

@ Cutscene: roughly 97 instructions of straight-line script --
@ 2 turns, 0 animation changes, 4 dialogue lines, 10 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x1f18, 0x1f1c.
@ Reads save bits 0x8a6, 0x8a8.
@ Sets save bit 0x8a8.
.thumb_func_start OvlFunc_942_20086c8
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =0x8a8
	bl	__GetFlag
	cmp	r0, #0
	beq	.L6fc
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0xb
	bl	__Func_809280c
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x1f1c
	bl	__MessageID
	mov	r0, #0xb
	mov	r1, #0
	bl	__ActorMessage
	bl	__CutsceneEnd
	b	.L7c2
.L6fc:
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0xb
	lsl	r1, #1
	mov	r2, #0x32
	bl	__MapActor_Emote
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0xb
	bl	__Func_809280c
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x1f18
	bl	__MessageID
	mov	r0, #0xb
	mov	r1, #0
	bl	__ActorMessage
	ldr	r0, =0x8a6
	bl	__GetFlag
	cmp	r0, #0
	beq	.L7a8
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r0, #0xb
	lsl	r1, #1
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r1, #0
	mov	r0, #0xb
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L772
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xb
	mov	r1, #0
	bl	__ActorMessage
	ldr	r0, =0x8a8
	bl	__SetFlag
	b	.L7be
.L772:
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r1, #0
	mov	r0, #0xb
	bl	__ActorMessage
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0x1e
	bl	__CutsceneWait
	b	.L7be
.L7a8:
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0x1e
	bl	__CutsceneWait
.L7be:
	bl	__CutsceneEnd
.L7c2:
	pop	{r0}
	bx	r0
.func_end OvlFunc_942_20086c8

@ 51 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, PlaySound, SetSlotEntitySpeed, SetSlotDrawPriority
@   WalkSlotTo, PlayMapRectAnimation, WalkSlotTo, PlayMapRectAnimation
@   DialogueWait, SetPendingMessageId, EndCutscene
.thumb_func_start OvlFunc_942_20087dc
	push	{lr}
	bl	__CutsceneStart
	mov	r0, #0x9e
	bl	__PlaySound
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #7
	mov	r0, #0
	lsl	r1, #8
	bl	__MapActor_SetSpeed
	mov	r1, #3
	mov	r0, #0
	bl	__Func_8092b08
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x6b
	cmp	r2, r3
	bne	.L82a
	mov	r1, #0x98
	mov	r2, #0xae
	mov	r0, #0
	lsl	r1, #1
	lsl	r2, #3
	bl	__Func_809218c
	ldr	r0, =gScript_930__020096b8
	mov	r1, #0x4e
	mov	r2, #0x56
	bl	__Func_8010560
	b	.L844
.L82a:
	ldr	r3, =0x70
	cmp	r2, r3
	bne	.L844
	mov	r0, #0
	mov	r1, #0xf8
	mov	r2, #0xc0
	bl	__Func_809218c
	ldr	r0, =.L16ce
	mov	r1, #0x4a
	mov	r2, #9
	bl	__Func_8010560
.L844:
	mov	r0, #0x10
	bl	__CutsceneWait
	mov	r0, #3
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_942_20087dc

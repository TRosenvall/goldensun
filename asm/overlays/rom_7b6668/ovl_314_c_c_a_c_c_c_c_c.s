	.include "macros.inc"

@ 114 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotScriptWithTurn, RegisterTask, WaitForSlotScriptIdle
@   PlaySound, OvlFunc_common0_10c x3, UnregisterTask, GetSlotEntityChecked
@   PlaceSlotAt, DialogueWait, FaceEntityInstant, SetFollowerFormationAndRefresh
@   DialogueWait, SetActiveMessageId
@   ... and 5 more
@ message id 0x17fd; reads save bit 0x203; sets 0x203.
.thumb_func_start OvlFunc_928_2008f30
	push	{r5, r6, lr}
	mov	r6, r11
	mov	r5, r10
	push	{r5, r6}
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6}
	sub	sp, #0x10
	bl	__CutsceneStart
	ldr	r1, =gScript_928__020096a0
	mov	r0, #0x13
	bl	__MapActor_SetBehavior
	ldr	r3, =OvlFunc_928_2008358
	mov	r1, #0xc8
	mov	r11, r3
	lsl	r1, #4
	mov	r0, r11
	bl	__StartTask
	mov	r0, #0x13
	bl	__MapActor_WaitScript
	mov	r0, #0x7c
	bl	__PlaySound
	mov	r3, #0xa8
	lsl	r3, #16
	mov	r9, r3
	mov	r3, #0x80
	lsl	r3, #12
	mov	r8, r3
	mov	r3, #0x9c
	lsl	r3, #17
	mov	r10, r3
	ldr	r6, =0x20001
	mov	r5, #0
	mov	r0, r9
	mov	r1, r8
	mov	r2, r10
	mov	r3, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	str	r6, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	ldr	r3, =0x3333
	mov	r0, r9
	mov	r1, r8
	mov	r2, r10
	str	r5, [sp]
	str	r5, [sp, #4]
	str	r6, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	ldr	r3, =0xffffcccd
	mov	r1, r8
	mov	r2, r10
	mov	r0, r9
	str	r5, [sp]
	str	r5, [sp, #4]
	str	r6, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	mov	r0, r11
	bl	__StopTask
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0x80
	ldr	r2, [r0, #0x50]
	lsl	r3, #8
	strh	r3, [r2, #0x1e]
	mov	r1, r9
	mov	r2, r10
	mov	r0, #0x15
	bl	__MapActor_SetPos
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0
	mov	r0, #0xe
	mov	r1, #0x13
	bl	__Func_809280c
	mov	r1, #2
	mov	r0, #0xe
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x17fd
	bl	__MessageID
	ldr	r0, =0x203
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1014
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L1014:
	mov	r1, #0
	mov	r0, #0xe
	bl	__ActorMessage
	ldr	r0, =0x203
	bl	__SetFlag
	bl	__Func_809202c
	bl	__CutsceneEnd
	add	sp, #0x10
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r3}
	mov	r11, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_928_2008f30

@ Cutscene: roughly 83 instructions of straight-line script --
@ 1 turn, 0 animation changes, 0 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_928_2009060
	push	{r5, r6, lr}
	bl	__CutsceneStart
	mov	r1, #1
	mov	r0, #0x12
	bl	__MapActor_SetBehavior
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r6, #0
	str	r6, [r0, #0x6c]
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r5, #0x80
	lsl	r5, #24
	str	r5, [r0, #0x38]
	mov	r0, #0x12
	bl	__MapActor_GetActor
	str	r5, [r0, #0x40]
	mov	r0, #0x12
	bl	__MapActor_GetActor
	str	r6, [r0, #0x24]
	mov	r0, #0x12
	bl	__MapActor_GetActor
	str	r6, [r0, #0x2c]
	mov	r0, #0x12
	bl	__MapActor_GetActor
	str	r6, [r0, #0x30]
	mov	r0, #0x12
	bl	__MapActor_GetActor
	mov	r2, #0
	str	r6, [r0, #0x34]
	ldr	r1, =0x103
	mov	r0, #0x12
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0x12
	bl	__Func_809259c
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #0x12
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x8c
	mov	r0, #0x12
	lsl	r1, #1
	mov	r2, #0xe8
	bl	__Func_809218c
	mov	r1, #0x94
	lsl	r1, #1
	mov	r2, #0xe8
	mov	r0, #0
	bl	__Func_80921c4
	mov	r0, #0x12
	bl	__MapActor_WaitMovement
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Emote
	ldr	r1, =gScript_928__020095b0
	mov	r0, #0x12
	bl	__MapActor_SetBehavior
	mov	r0, #0x12
	bl	__MapActor_GetActor
	ldr	r3, =OvlFunc_928_2008500
	str	r3, [r0, #0x6c]
	bl	__CutsceneEnd
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_928_2009060

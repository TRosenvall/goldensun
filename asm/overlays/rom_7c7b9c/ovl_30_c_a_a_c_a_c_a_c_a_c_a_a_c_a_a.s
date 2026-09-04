	.include "macros.inc"

@ Cutscene: roughly 61 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bit 0x109.
.thumb_func_start OvlFunc_943_200985c
	push	{r5, lr}
	bl	__CutsceneStart
	ldr	r0, =.L5160
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x14
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xee
	mov	r0, #0x17
	lsl	r1, #16
	ldr	r2, =0x2720000
	bl	__MapActor_SetPos
	mov	r1, #0xcc
	lsl	r1, #16
	ldr	r2, =0x2090000
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #13
	str	r3, [r0, #0xc]
	mov	r0, #0x16
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	mov	r5, #0x80
	orr	r3, r5
	strb	r3, [r0]
	ldr	r2, =0x4ccc
	mov	r0, #0x16
	ldr	r1, =0x9999
	bl	__MapActor_SetSpeed
	ldr	r1, =gScript_943__0200c58c
	mov	r0, #0x16
	bl	__MapActor_SetBehavior
	mov	r0, #0x15
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	orr	r5, r3
	strb	r5, [r0]
	ldr	r1, =0xcccc
	mov	r0, #0x15
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #0x15
	ldr	r1, =gScript_943__0200c628
	bl	__MapActor_SetBehavior
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18ec
	bl	OvlFunc_943_200c218
.L18ec:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_200985c

@ 49 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, AddSceneRecord, WaitFrames, PlaceSlotAt x3
@   GetSlotEntityChecked, SetSlotScriptWithTurn, GetSlotEntityChecked, SetSlotEntitySpeed
@   SetSlotScriptWithTurn, TestSaveBit, OvlFunc_4218, EndCutscene
@ reads save bit 0x109.
.thumb_func_start OvlFunc_943_2009920
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =.L5160
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x14
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xee
	mov	r0, #0x17
	lsl	r1, #16
	ldr	r2, =0x2720000
	bl	__MapActor_SetPos
	mov	r1, #0x86
	ldr	r2, =0x2a60000
	lsl	r1, #17
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r3, #0
	strh	r3, [r0, #6]
	ldr	r1, =gScript_943__0200c980
	mov	r0, #0x16
	bl	__MapActor_SetBehavior
	mov	r0, #0x15
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r2, [r0]
	mov	r3, #0x80
	orr	r3, r2
	strb	r3, [r0]
	ldr	r1, =0xcccc
	mov	r0, #0x15
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #0x15
	ldr	r1, =gScript_943__0200c628
	bl	__MapActor_SetBehavior
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1996
	bl	OvlFunc_943_200c218
.L1996:
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2009920

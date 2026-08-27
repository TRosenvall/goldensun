	.include "macros.inc"

@ 111 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, BeginCutscene, TestSaveBit, GetSlotEntityChecked
@   SetMapTransition, PlaySound, WaitFrames, PlaySound
@   PlaceSlotAt, WalkSlotToAndWait, DialogueWait, PlaySound
@   SetMapTransition, WaitForMapTransition
@   ... and 12 more
@ reads save bits 0x30c, 0x313, 0x833, 0x837, 0x841; sets 0x30c, 0x313, 0x833.
.thumb_func_start OvlFunc_882_2009348
	push	{r5, lr}
	ldr	r0, =0x313
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1356
	b	.L1458
.L1356:
	bl	__CutsceneStart
	ldr	r0, =0x833
	bl	__GetFlag
	cmp	r0, #0
	bne	.L13e8
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r1, #0x80
	mov	r5, r0
	mov	r2, #0x80
	mov	r0, #0x80
	lsl	r1, #11
	lsl	r2, #9
	lsl	r0, #11
	bl	__Func_8012330
	mov	r0, #0x8d
	bl	__PlaySound
	mov	r0, #0x28
	bl	__WaitFrames
	mov	r0, #0x91
	bl	__PlaySound
	mov	r1, #0xed
	mov	r0, #0xe
	lsl	r1, #17
	ldr	r2, =0x47b0000
	bl	__MapActor_SetPos
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r5, #0x30]
	str	r3, [r5, #0x34]
	mov	r2, #0x90
	ldr	r3, [r5, #0xc]
	lsl	r2, #15
	add	r3, r2
	str	r3, [r5, #0xc]
	str	r3, [r5, #0x3c]
	mov	r3, #0x80
	lsl	r3, #8
	mov	r1, #0xd8
	str	r3, [r5, #0x44]
	lsl	r1, #1
	ldr	r2, =0x47b
	mov	r0, #0xe
	bl	__Func_80921c4
	mov	r0, #0x28
	bl	__CutsceneWait
	ldr	r0, =0x121
	bl	__PlaySound
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_8012330
	bl	__Func_8012350
	bl	__Func_809202c
	ldr	r0, =0x833
	bl	__SetFlag
.L13e8:
	bl	OvlFunc_882_2009498
	ldr	r0, =0x313
	bl	__SetFlag
	ldr	r0, =0x837
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1454
	ldr	r0, =0x841
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1454
	mov	r0, #0xc3
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1454
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r2, =0x479ffff
	ldr	r3, [r0, #0x10]
	cmp	r3, r2
	bgt	.L143a
	mov	r0, #0xce
	mov	r1, #0x8c
	lsl	r0, #1
	lsl	r1, #3
	bl	OvlFunc_882_2009a64
	mov	r1, #0xcf
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x42c
	bl	__Func_80921c4
	b	.L144c
.L143a:
	ldr	r0, =0x1bd
	ldr	r1, =0x494
	bl	OvlFunc_882_2009a64
	mov	r0, #0
	ldr	r1, =0x1bf
	ldr	r2, =0x4cb
	bl	__Func_80921c4
.L144c:
	mov	r0, #0xc3
	lsl	r0, #2
	bl	__SetFlag
.L1454:
	bl	__CutsceneEnd
.L1458:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_2009348

@ Map edit: 5 attribute copies.
@ Attributes only, so the artwork is already correct and only
@ collision or priority changes.
.thumb_func_start OvlFunc_882_2009498
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	sub	sp, #8
	mov	r3, #0x47
	str	r3, [sp, #4]
	mov	r5, #0x1a
	mov	r8, r3
	mov	r0, #0x1d
	mov	r1, #0x14
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r6, #0x46
	mov	r0, #0x1d
	mov	r1, #0x14
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010704
	mov	r5, #0x1b
	mov	r0, #0x1d
	mov	r1, #0x14
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x1c
	str	r3, [sp]
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r0, #0x1c
	mov	r1, #0x15
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r3, #0x48
	str	r3, [sp, #4]
	mov	r0, #0x1c
	mov	r1, #0x16
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_2009498

@ Cutscene: roughly 84 instructions of straight-line script --
@ 0 turns, 1 animation change, 3 dialogue lines, 2 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xe67.
@ Sets save bit 0x835.
.thumb_func_start OvlFunc_882_200950c
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r1, #0x83
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x32a
	bl	__Func_80921c4
	mov	r1, #0x83
	mov	r0, #0x14
	lsl	r1, #17
	ldr	r2, =0x3250000
	bl	__MapActor_SetPos
	mov	r1, #0x83
	mov	r0, #0x14
	lsl	r1, #1
	ldr	r2, =0x339
	bl	__Func_809218c
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r0, #0
	mov	r1, #2
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r1, #0x8d
	ldr	r2, =0x357
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r0, #0x14
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r0, #0
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r2, #0
	mov	r1, #0x14
	mov	r0, #0
	bl	__Func_8092848
	bl	__Func_809202c
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0x80
	mov	r2, #0x14
	lsl	r1, #1
	mov	r0, #0x14
	bl	__MapActor_Emote
	ldr	r5, =0xe67
	mov	r0, r5
	bl	__MessageID
	mov	r1, #0
	mov	r0, #0x14
	bl	__ActorMessage
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #0x14
	bl	__Func_8093054
	add	r5, #4
	mov	r1, #2
	mov	r0, #0x14
	bl	__Func_80925cc
	mov	r0, r5
	bl	__MessageID
	mov	r2, #0x14
	mov	r0, #0x14
	mov	r1, #0
	bl	__Func_8093040
	ldr	r1, =gScript_882__0200c8c0
	mov	r0, #0x14
	bl	__MapActor_RunScript
	ldr	r0, =0x835
	bl	__SetFlag
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_200950c

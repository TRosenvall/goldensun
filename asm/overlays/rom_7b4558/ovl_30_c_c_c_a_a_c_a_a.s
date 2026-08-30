	.include "macros.inc"

@ 89 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, GetSlotEntityChecked x2, OvlFunc_244 x5, GetSlotEntityChecked x2
@   SetSaveBit, OvlFunc_244, EndCutscene
@ sets 0x214.
.thumb_func_start OvlFunc_927_2009c34
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	sub	sp, #8
	bl	__CutsceneStart
	mov	r0, #0xe
	bl	__MapActor_GetActor
	ldr	r6, [r0, #8]
	mov	r0, #0xe
	bl	__MapActor_GetActor
	ldr	r5, [r0, #0x10]
	mov	r3, #1
	str	r3, [sp]
	asr	r6, #20
	asr	r5, #20
	mov	r8, r3
	mov	r3, #0xff
	str	r3, [sp, #4]
	mov	r2, r5
	mov	r1, r6
	mov	r10, r3
	mov	r0, #2
	mov	r3, #1
	bl	OvlFunc_927_2008244
	mov	r7, #0
	mov	r3, r8
	mov	r2, r5
	add	r1, r6, #1
	mov	r0, #2
	str	r3, [sp]
	str	r7, [sp, #4]
	bl	OvlFunc_927_2008244
	mov	r3, r8
	mov	r2, r5
	sub	r1, r6, #1
	mov	r0, #2
	str	r3, [sp]
	str	r7, [sp, #4]
	bl	OvlFunc_927_2008244
	add	r2, r5, #1
	mov	r3, r8
	mov	r1, r6
	mov	r0, #2
	str	r3, [sp]
	str	r7, [sp, #4]
	bl	OvlFunc_927_2008244
	sub	r5, #1
	mov	r3, r8
	mov	r1, r6
	mov	r2, r5
	mov	r0, #2
	str	r3, [sp]
	str	r7, [sp, #4]
	bl	OvlFunc_927_2008244
	mov	r0, #0xe
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r3, #0x1b
	bne	.L1cee
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r3, r0
	add	r3, #0x55
	strb	r7, [r3]
	ldr	r3, =0xfffe0000
	str	r3, [r0, #0x14]
	str	r3, [r0, #0xc]
	mov	r0, #0x85
	lsl	r0, #2
	bl	__SetFlag
	mov	r3, r8
	str	r3, [sp]
	mov	r3, r10
	str	r3, [sp, #4]
	mov	r0, #2
	mov	r1, #0x2b
	mov	r2, #0x17
	mov	r3, #1
	bl	OvlFunc_927_2008244
.L1cee:
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_927_2009c34

@ 81 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, BeginCutscene, OvlFunc_ea8, OvlFunc_d90
@   DialogueWait, OvlFunc_ae8, AttachCameraToSlot, TurnSlotsToFaceEachOther
@   DialogueWait, SetFollowerFormationScript, PlayInteractionEffect, PlaySound
@   DialogueWait, GetSlotEntityChecked x2
@   ... and 5 more
@ sets 0x307.
.thumb_func_start OvlFunc_927_2009d04
	push	{r5, r6, lr}
	mov	r0, #0xf
	sub	sp, #0x10
	bl	__MapActor_GetActor
	mov	r6, #0x80
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #0xf
	mov	r1, #0
	bl	OvlFunc_927_2008ea8
	lsl	r6, #12
	mov	r1, #0xec
	mov	r3, r6
	lsl	r1, #1
	mov	r2, #0x68
	mov	r0, #0xf
	bl	OvlFunc_927_2008d90
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r2, [r5, #0x10]
	mov	r3, #1
	mov	r4, #0
	ldr	r0, [r5, #8]
	ldr	r1, [r5, #0xc]
	add	r2, r6
	str	r3, [sp, #8]
	mov	r3, #0
	str	r4, [sp]
	str	r4, [sp, #4]
	str	r4, [sp, #0xc]
	bl	OvlFunc_927_2008ae8
	mov	r0, #0xf
	mov	r1, #1
	bl	__SetCameraTarget
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xf
	bl	__Func_8092848
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0xf
	mov	r1, #2
	bl	__Func_809259c
	mov	r2, #0
	ldr	r1, =0x103
	mov	r0, #0xf
	bl	__MapActor_Emote
	mov	r0, #0x93
	bl	__PlaySound
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0xa
	ldrsh	r5, [r0, r2]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r3, #0xc0
	lsl	r3, #11
	mov	r1, r5
	mov	r0, #0xf
	bl	OvlFunc_927_2008d90
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x307
	bl	__SetFlag
	ldr	r3, =gState
	ldr	r2, =0x22b
	add	r3, r2
	mov	r2, #3
	strb	r2, [r3]
	mov	r0, #0x35
	mov	r1, #0
	bl	__Func_8091eb0
	bl	__CutsceneEnd
	add	sp, #0x10
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_927_2009d04

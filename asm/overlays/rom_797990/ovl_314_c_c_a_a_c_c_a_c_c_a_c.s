	.include "macros.inc"

@ 50 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, PlaySound, DialogueWait, TestSaveBit
@   RunSlotEffectSequence, SetSlotFacingAndScript, DialogueWait, SetSlotFacingAndScript
@   DialogueWait, OvlFunc_970, DialogueWait, WalkSlotToAndWait x2
@   SetSaveBit, EndCutscene
@ reads save bit 0x867; sets 0x867.
.thumb_func_start OvlFunc_901_20089f8
	push	{lr}
	bl	__CutsceneStart
	mov	r0, #0x64
	bl	__PlaySound
	mov	r0, #0x28
	bl	__CutsceneWait
	ldr	r0, =0x867
	bl	__GetFlag
	cmp	r0, #0
	bne	.La72
	mov	r1, #0x81
	mov	r0, #0x15
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r1, #4
	mov	r2, #0
	mov	r0, #0x15
	bl	__MapActor_Jump
	mov	r0, #0xc
	bl	__CutsceneWait
	mov	r1, #4
	mov	r2, #0
	mov	r0, #0x15
	bl	__MapActor_Jump
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xc4
	mov	r3, #0xe0
	lsl	r3, #11
	lsl	r1, #1
	mov	r2, #0x68
	mov	r0, #0x15
	bl	OvlFunc_901_2008970
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xcc
	mov	r0, #0x15
	lsl	r1, #1
	mov	r2, #0x68
	bl	__Func_80921c4
	mov	r1, #0xcc
	mov	r0, #0x15
	lsl	r1, #1
	mov	r2, #0x78
	bl	__Func_80921c4
	ldr	r0, =0x867
	bl	__SetFlag
.La72:
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_901_20089f8

@ 30 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetSlotEntitySpeed, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_901_2008a80
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r6, r1
	mov	r8, r2
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r5, r0
	lsl	r1, #8
	mov	r0, #0
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r2, r6
	mov	r0, #0
	mov	r1, r5
	bl	__Func_809218c
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe4
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0x10
	str	r2, [r3]
	mov	r0, r8
	bl	__Func_8091e9c
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_901_2008a80

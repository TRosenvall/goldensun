	.include "macros.inc"

@ 50 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_25b0, TestSaveBit, BeginCutscene, StartFadeOut
@   WaitForFade, SetSaveBit, ClearSaveBit x2, EndCutscene
@   TestSaveBit, BeginCutscene, StartFadeOut, WaitForFade
@   SetSaveBit, ClearSaveBit x2
@   ... and 3 more
@ reads save bits 0x200, 0x201, 0x80a; sets 0x200, 0x201; clears 0x200, 0x201, 0x202.
.thumb_func_start OvlFunc_890_2008150
	push	{lr}
	bl	OvlFunc_890_200a5b0
	cmp	r0, #0
	beq	.L194
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1d6
	bl	__CutsceneStart
	mov	r0, #0x80
	mov	r1, #1
	lsl	r0, #9
	bl	__Func_8091200
	mov	r0, #0x14
	bl	__Func_8091254
	mov	r0, #0x80
	lsl	r0, #2
	bl	__SetFlag
	ldr	r0, =0x201
	bl	__ClearFlag
	ldr	r0, =0x202
	bl	__ClearFlag
	bl	__CutsceneEnd
	b	.L1d6
.L194:
	ldr	r0, =0x201
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1d6
	bl	__CutsceneStart
	mov	r1, #1
	ldr	r0, =0x2051cc
	bl	__Func_8091200
	mov	r0, #0x14
	bl	__Func_8091254
	ldr	r0, =0x201
	bl	__SetFlag
	mov	r0, #0x80
	lsl	r0, #2
	bl	__ClearFlag
	ldr	r0, =0x202
	bl	__ClearFlag
	ldr	r0, =0x80a
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1d2
	bl	OvlFunc_890_20089f4
.L1d2:
	bl	__CutsceneEnd
.L1d6:
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2008150

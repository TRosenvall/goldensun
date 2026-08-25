	.include "macros.inc"

@ 62 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   StartFadeOut, WaitForFade, DialogueWait, TestSaveBit
@   PlaceSlotAt, OvlFunc_3270, TestSaveBit, OvlFunc_88c
@   ClearSaveBit, OvlFunc_3270, SetSaveBit, TestSaveBit
@   OvlFunc_88c, ClearSaveBit
@ reads save bits 0x109, 0x855; sets 0x201; clears 0x12f.
.thumb_func_start OvlFunc_888_20085cc
	push	{lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0x80
	add	r2, #0x49
	str	r2, [r3]
	mov	r1, #0
	lsl	r0, #9
	bl	__Func_8091200
	mov	r0, #1
	bl	__Func_8091254
	mov	r0, #1
	bl	__CutsceneWait
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #0xa
	cmp	r3, #0x19
	bhi	.L6c6
	ldr	r2, =.L60c
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L60c:
	.word	.L674
	.word	.L674
	.word	.L674
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L68e
	.word	.L6a8
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6c6
	.word	.L6a0
	.word	.L6c6
	.word	.L6c6
	.word	.L6a0
	.word	.L6c6
	.word	.L6c6
	.word	.L6a0
.L674:
	ldr	r0, =0x855
	bl	__GetFlag
	cmp	r0, #0
	beq	.L6a0
	mov	r1, #0xc8
	mov	r2, #0xa0
	mov	r0, #0xa
	lsl	r1, #16
	lsl	r2, #15
	bl	__MapActor_SetPos
	b	.L6a0
.L68e:
	bl	OvlFunc_888_200b270
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L6a0
	bl	OvlFunc_888_200888c
.L6a0:
	ldr	r0, =0x12f
	bl	__ClearFlag
	b	.L6c6
.L6a8:
	bl	OvlFunc_888_200b270
	ldr	r0, =0x201
	bl	__SetFlag
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L6c0
	bl	OvlFunc_888_200888c
.L6c0:
	ldr	r0, =0x12f
	bl	__ClearFlag
.L6c6:
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_888_20085cc

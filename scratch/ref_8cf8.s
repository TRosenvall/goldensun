	.include "macros.inc"
	.include "gba.inc"

@ 38 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TurnSlotToAngle, SetEntityPalette, DialogueWait, SpawnEntity
.thumb_func_start OvlFunc_957_2008c98
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001f30
	ldr	r6, [r3]
	mov	r1, #0x80
	ldr	r5, [r6, #0x10]
	mov	r2, #0
	mov	r3, #0x18
	ldrsh	r0, [r6, r3]
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r1, #0
	mov	r0, r5
	bl	__Actor_SetColorswap
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r1, [r5, #8]
	ldr	r2, [r5, #0xc]
	ldr	r3, [r5, #0x10]
	mov	r0, #0
	bl	__CreateActor
	mov	r4, r0
	cmp	r4, #0
	beq	.Lce6
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	mov	r1, r4
	ldr	r2, =0x8400001c
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, r5
	mov	r2, #0
	add	r3, #0x54
	str	r2, [r5, #0x6c]
	str	r4, [r6, #0x10]
	strb	r2, [r3]
.Lce6:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_957_2008c98

@ 34 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, CopyMapRectAttributes, SetSaveBit
@ sets 0x212.
.thumb_func_start OvlFunc_957_2008cf8
	push	{lr}
	mov	r0, #0xc
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r3, #0x1e
	bne	.Ld3c
	ldr	r3, [r0, #0x10]
	asr	r4, r3, #20
	cmp	r4, #0x14
	bne	.Ld3c
	mov	r1, r0
	mov	r2, #2
	add	r1, #0x55
	mov	r3, #0
	strb	r2, [r1]
	str	r3, [r0, #0x14]
	mov	r3, r0
	add	r3, #0x23
	strb	r2, [r3]
	mov	r3, #0x20
	str	r3, [sp]
	mov	r0, #0x1e
	mov	r1, #0x14
	mov	r2, #1
	mov	r3, #1
	str	r4, [sp, #4]
	bl	__Func_8010704
	ldr	r0, =0x212
	bl	__SetFlag
.Ld3c:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_957_2008cf8

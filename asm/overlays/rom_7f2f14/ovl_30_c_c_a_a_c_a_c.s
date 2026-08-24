	.include "macros.inc"
	.include "gba.inc"

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotEntitySpeed, WalkSlotToAndWait, TurnSlotToAngle
@   OvlFunc_58, PlayLevitateSequence, DialogueWait, SetPendingMessageId
@   EndCutscene
.thumb_func_start OvlFunc_968_200af30
	push	{lr}
	bl	__CutsceneStart
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #8
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r1, #0x82
	mov	r2, #0xb2
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r0, #0x82
	mov	r2, #0xc4
	mov	r3, #0xdf
	lsl	r2, #18
	mov	r1, #0
	lsl	r0, #18
	bl	OvlFunc_968_2008058
	mov	r1, #6
	mov	r2, #0
	mov	r0, #0
	bl	__Func_8092708
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0x14
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_968_200af30

@ Leaf helper, 38 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_968_200af8c
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xb5
	cmp	r2, r3
	bne	.L2fa4
	ldr	r0, =.L6e44
	b	.L2fd4
.L2fa4:
	ldr	r3, =0xb6
	cmp	r2, r3
	beq	.L2fd2
	ldr	r3, =0xb7
	cmp	r2, r3
	bne	.L2fb4
	ldr	r0, =.L7120
	b	.L2fd4
.L2fb4:
	ldr	r3, =0xb8
	cmp	r2, r3
	bne	.L2fbe
	ldr	r0, =.L7300
	b	.L2fd4
.L2fbe:
	ldr	r3, =0xb9
	cmp	r2, r3
	bne	.L2fc8
	ldr	r0, =.L73b4
	b	.L2fd4
.L2fc8:
	ldr	r3, =0xba
	cmp	r2, r3
	bne	.L2fd2
	ldr	r0, =.L74f8
	b	.L2fd4
.L2fd2:
	ldr	r0, =.L6f1c
.L2fd4:
	pop	{r1}
	bx	r1
.func_end OvlFunc_968_200af8c

	.include "macros.inc"
	.include "gba.inc"

@ 41 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, DialogueChoiceB, DialogueChoiceA, OvlFunc_19e8
@   OvlFunc_c4, OvlFunc_1ac8
.thumb_func_start OvlFunc_948_2009b60
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #8
	bl	__MapActor_GetActor
	ldr	r3, [r5, #8]
	cmp	r3, #0
	bge	.L1b7a
	ldr	r2, =0xfffff
	add	r3, r2
.L1b7a:
	ldr	r0, [r0, #8]
	asr	r3, #20
	cmp	r0, #0
	bge	.L1b86
	ldr	r2, =0xfffff
	add	r0, r2
.L1b86:
	asr	r0, #20
	cmp	r3, #0x26
	bne	.L1bae
	cmp	r0, #0x26
	beq	.L1bae
	mov	r3, #0xc0
	ldrh	r0, [r5, #6]
	lsl	r3, #8
	cmp	r0, r3
	bne	.L1ba0
	bl	__Func_8093fa0
	b	.L1bba
.L1ba0:
	mov	r2, #0x80
	lsl	r2, #7
	cmp	r0, r2
	bne	.L1bae
	bl	__Func_8093e28
	b	.L1bba
.L1bae:
	bl	OvlFunc_948_20099e8
	bl	OvlFunc_948_20080c4
	bl	OvlFunc_948_2009ac8
.L1bba:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_948_2009b60

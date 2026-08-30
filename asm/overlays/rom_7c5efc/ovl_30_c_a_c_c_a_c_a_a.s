	.include "macros.inc"

@ 29 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, CopyMapRectAttributes, SetSaveBit
@ sets 0x204.
.thumb_func_start OvlFunc_941_2008094
	push	{lr}
	mov	r0, #9
	sub	sp, #8
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lb2
	mov	r1, r0
	mov	r2, #1
	add	r1, #0x23
	strb	r2, [r1]
	mov	r2, r0
	mov	r3, #0
	add	r2, #0x55
	strb	r3, [r2]
.Lb2:
	mov	r3, #8
	mov	r2, #0x20
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0x20
	mov	r2, #1
	mov	r3, #1
	mov	r0, #7
	bl	__Func_8010704
	mov	r0, #0x81
	lsl	r0, #2
	bl	__SetFlag
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_941_2008094

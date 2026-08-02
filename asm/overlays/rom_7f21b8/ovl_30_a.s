	.include "macros.inc"

@ 8 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   RunSlotEffectSequence
.thumb_func_start OvlFunc_967_2008030
	push	{lr}
	mov	r1, #0x81
	mov	r0, #0xe
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_967_2008030


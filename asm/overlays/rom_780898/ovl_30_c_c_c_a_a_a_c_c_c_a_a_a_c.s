	.include "macros.inc"

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_883_2008e84
	push	{lr}
	mov	r0, #0x9e
	bl	__PlaySound
	ldr	r0, =.L7570
	mov	r1, #0x2d
	mov	r2, #0x27
	bl	__Func_8010560
	mov	r1, #0x83
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x325
	bl	__Func_809218c
	mov	r0, #6
	bl	__Func_8091e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_883_2008e84

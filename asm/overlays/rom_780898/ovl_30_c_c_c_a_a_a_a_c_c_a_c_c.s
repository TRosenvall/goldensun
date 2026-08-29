	.include "macros.inc"

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_883_2008dc0
	push	{lr}
	mov	r0, #0xbc
	bl	__PlaySound
	ldr	r0, =.L7544
	mov	r1, #0x2d
	mov	r2, #0xb
	bl	__Func_8010560
	mov	r2, #0xd2
	mov	r0, #0
	ldr	r1, =0x101
	lsl	r2, #1
	bl	__Func_809218c
	mov	r0, #0xb
	bl	__Func_8091e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_883_2008dc0

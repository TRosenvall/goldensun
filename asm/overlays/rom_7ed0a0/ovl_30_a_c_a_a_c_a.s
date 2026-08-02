	.include "macros.inc"

@ 18 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_cd0
.thumb_func_start OvlFunc_964_2009348
	push	{lr}
	mov	r0, #0
	sub	sp, #0xc
	bl	__MapActor_GetActor
	ldr	r1, =0xffe00000
	ldr	r3, [r0, #8]
	mov	r2, sp
	add	r3, r1
	str	r3, [r2]
	ldr	r3, [r0, #0xc]
	str	r3, [r2, #4]
	ldr	r3, [r0, #0x10]
	mov	r0, r2
	str	r3, [r2, #8]
	bl	OvlFunc_964_2008cd0
	add	sp, #0xc
	pop	{r0}
	bx	r0
.func_end OvlFunc_964_2009348


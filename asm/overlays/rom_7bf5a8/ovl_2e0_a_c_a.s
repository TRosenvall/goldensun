	.include "macros.inc"
	.include "gba.inc"

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, OvlFunc_170, OvlFunc_334, SetSaveBit
@   PlaySound, OvlFunc_398
@ reads save bit 0x9a9; sets 0x9a9.
.thumb_func_start OvlFunc_935_2008368
	push	{lr}
	ldr	r0, =0x9a9
	bl	__GetFlag
	cmp	r0, #0
	bne	.L390
	bl	OvlFunc_935_2008170
	bl	OvlFunc_935_2008334
	cmp	r0, #0
	beq	.L390
	ldr	r0, =0x9a9
	bl	__SetFlag
	mov	r0, #0x50
	bl	__PlaySound
	bl	OvlFunc_935_2008398
.L390:
	pop	{r0}
	bx	r0
.func_end OvlFunc_935_2008368

	.include "macros.inc"
	.include "gba.inc"

@ 20 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_170, TestSaveBit, OvlFunc_458, TestSaveBit
@   PlaySound, OvlFunc_410, SetSaveBit
@ reads save bits 0x207, 0x9aa; sets 0x9aa.
.thumb_func_start OvlFunc_935_200848c
	push	{lr}
	bl	OvlFunc_935_2008170
	ldr	r0, =0x9aa
	bl	__GetFlag
	cmp	r0, #0
	bne	.L4be
	bl	OvlFunc_935_2008458
	cmp	r0, #0
	beq	.L4be
	ldr	r0, =0x207
	bl	__GetFlag
	cmp	r0, #0
	bne	.L4be
	mov	r0, #0x50
	bl	__PlaySound
	bl	OvlFunc_935_2008410
	ldr	r0, =0x9aa
	bl	__SetFlag
.L4be:
	pop	{r0}
	bx	r0
.func_end OvlFunc_935_200848c


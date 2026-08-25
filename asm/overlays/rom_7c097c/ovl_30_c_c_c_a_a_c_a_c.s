	.include "macros.inc"

@ 17 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, GetSlotEntityChecked, SetSaveBit
@ reads save bit 0x200; sets 0x200.
.thumb_func_start OvlFunc_936_20095b4
	push	{r5, lr}
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L15d6
	ldr	r3, =iwram_3001ee0
	mov	r0, #0
	ldr	r5, [r3]
	bl	__MapActor_GetActor
	str	r0, [r5, #0x18]
	mov	r0, #0x80
	lsl	r0, #2
	bl	__SetFlag
.L15d6:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_936_20095b4

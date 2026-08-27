	.include "macros.inc"

@ 26 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_2660, OvlFunc_c4
.thumb_func_start OvlFunc_965_200a6fc
	push	{r5, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	OvlFunc_965_200a660
	cmp	r0, #0
	beq	.L2730
	ldr	r3, [r0, #0xc]
	ldr	r0, [r5, #0xc]
	sub	r2, r3, r0
	cmp	r2, #0
	blt	.L2722
	mov	r3, #0x80
	lsl	r3, #12
	cmp	r2, r3
	bge	.L2730
	b	.L272c
.L2722:
	mov	r2, #0x80
	sub	r3, r0, r3
	lsl	r2, #12
	cmp	r3, r2
	bge	.L2730
.L272c:
	bl	OvlFunc_965_20080c4
.L2730:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_965_200a6fc

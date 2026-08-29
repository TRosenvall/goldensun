	.include "macros.inc"

@ 17 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetTileFlags, OvlFunc_1be8, OvlFunc_2244
.thumb_func_start OvlFunc_891_20095d4
	push	{lr}
	mov	r1, #0xd0
	mov	r2, #0xe0
	mov	r0, #2
	lsl	r1, #16
	lsl	r2, #15
	mov	r3, #0
	bl	__Func_8012078
	mov	r0, #0xa
	mov	r1, #0xe
	mov	r2, #7
	bl	OvlFunc_891_2009be8
	cmp	r0, #0
	beq	.L15f8
	bl	OvlFunc_891_200a244
.L15f8:
	pop	{r0}
	bx	r0
.func_end OvlFunc_891_20095d4

@ 17 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetTileFlags, OvlFunc_1be8, OvlFunc_22f4
.thumb_func_start OvlFunc_891_20095fc
	push	{lr}
	mov	r1, #0xb0
	mov	r2, #0xe0
	mov	r0, #2
	lsl	r1, #17
	lsl	r2, #15
	mov	r3, #0
	bl	__Func_8012078
	mov	r0, #0xc
	mov	r1, #0x15
	mov	r2, #7
	bl	OvlFunc_891_2009be8
	cmp	r0, #0
	beq	.L1620
	bl	OvlFunc_891_200a2f4
.L1620:
	pop	{r0}
	bx	r0
.func_end OvlFunc_891_20095fc

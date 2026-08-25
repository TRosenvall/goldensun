	.include "macros.inc"

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   FindInventorySlotInParty, OvlFunc_e30, PlaySound, SetMapTransition x2
@   SetSaveBit
.thumb_func_start OvlFunc_959_2008e80
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r0, #0xea
	ldr	r5, [r3]
	bl	__CheckPartyItem
	mov	r6, #1
	neg	r6, r6
	cmp	r0, r6
	beq	.Led0
	mov	r2, #0xb6
	lsl	r2, #1
	add	r3, r5, r2
	mov	r2, #0
	ldrsh	r5, [r3, r2]
	mov	r0, r5
	sub	r0, #0x28
	bl	OvlFunc_959_2008e30
	mov	r0, #0x9d
	bl	__PlaySound
	mov	r0, #0xc0
	mov	r1, #0xc0
	mov	r2, #0x80
	lsl	r0, #10
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_8012330
	mov	r0, r6
	mov	r1, r6
	ldr	r2, =0xe666
	bl	__Func_8012330
	mov	r3, #0xcc
	lsl	r3, #2
	add	r0, r5, r3
	bl	__SetFlag
.Led0:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2008e80

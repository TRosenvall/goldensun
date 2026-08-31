	.include "macros.inc"

@ Adjusts slot 0x1c directly.
@ Takes the entity with Func_92054 and writes its fields in place rather
@ than going through the slot helpers.
.thumb_func_start OvlFunc_945_200b7b4
	push	{r5, r6, r7, lr}
	mov	r5, #0x1c
	mov	r6, #8
	mov	r7, #0
.L37bc:
	mov	r0, r5
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	add	r5, #1
	orr	r3, r6
	strb	r3, [r0]
	cmp	r5, #0x23
	bls	.L37bc
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_200b7b4

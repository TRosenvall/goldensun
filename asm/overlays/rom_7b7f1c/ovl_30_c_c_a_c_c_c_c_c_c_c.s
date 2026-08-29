	.include "macros.inc"

@ Adjusts slot 0 (the player) directly.
@ Takes the entity with Func_92054 and writes its fields in place rather
@ than going through the slot helpers -- touches +0xc.
.thumb_func_start OvlFunc_930_2009060
	push	{r5, lr}
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r5, #0xc]
	ldr	r2, [r0, #0xc]
	cmp	r2, r3
	ble	.L107e
	mov	r1, r5
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #2
	orr	r3, r2
	b	.L1088
.L107e:
	mov	r1, r5
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #0xfd
	and	r3, r2
.L1088:
	strb	r3, [r1]
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_930_2009060

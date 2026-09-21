	.include "macros.inc"

@ Adjusts slot 0 (the player) directly.
@ Takes the entity with Func_92054 and writes its fields in place rather
@ than going through the slot helpers -- touches +0x9, +0x15, +0x50.
.thumb_func_start OvlFunc_949_20086e8
	push	{r5, lr}
	mov	r5, r0
	cmp	r5, #0
	beq	.L720
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x50]
	mov	r1, r5
	ldrb	r2, [r3, #9]
	add	r1, #0x23
	mov	r3, #0
	strb	r3, [r1]
	mov	r1, #0xc
	ldr	r4, [r5, #0x50]
	and	r1, r2
	mov	r2, #0xd
	ldrb	r0, [r4, #9]
	neg	r2, r2
	mov	r3, r2
	and	r3, r0
	orr	r3, r1
	strb	r3, [r4, #9]
	ldr	r0, [r5, #0x50]
	ldrb	r3, [r0, #0x15]
	and	r2, r3
	orr	r2, r1
	strb	r2, [r0, #0x15]
.L720:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_949_20086e8

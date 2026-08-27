	.include "macros.inc"

@ Adjusts slot 0 (the player) directly.
@ Takes the entity with Func_92054 and writes its fields in place rather
@ than going through the slot helpers -- touches +0x8, +0x10.
.thumb_func_start OvlFunc_959_2009880
	push	{r5, r6, lr}
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r5, #0x10]
	mov	r1, r0
	cmp	r3, #0
	bge	.L189a
	ldr	r2, =0xfffff
	add	r3, r2
.L189a:
	ldr	r0, [r5, #8]
	asr	r6, r3, #20
	cmp	r0, #0
	bge	.L18a6
	ldr	r3, =0xfffff
	add	r0, r3
.L18a6:
	ldr	r2, [r1, #0x10]
	asr	r4, r0, #20
	cmp	r2, #0
	bge	.L18b2
	ldr	r3, =0xfffff
	add	r2, r3
.L18b2:
	ldr	r0, [r1, #8]
	asr	r2, #20
	cmp	r0, #0
	bge	.L18be
	ldr	r3, =0xfffff
	add	r0, r3
.L18be:
	asr	r3, r0, #20
	sub	r3, r4, r3
	add	r3, #6
	mov	r0, #0
	cmp	r3, #0xc
	bhi	.L18da
	sub	r3, r6, #2
	cmp	r3, r2
	bge	.L18d8
	add	r3, r6, #2
	mov	r0, #1
	cmp	r3, r2
	bgt	.L18da
.L18d8:
	mov	r0, #0
.L18da:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_959_2009880

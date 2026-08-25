	.include "macros.inc"

@ CheckFieldAbilityAvailable
@ r0=packed request: party member index in bits 10-13, ability id in bits 0-9.
@ Returns 0 if the ability can be used now, or a negative reason code:
@    -1  the member index is above 7
@    -2  that member is not in the party (_Func_79338 on the member flag)
@    -3  the member cannot currently use the ability (_Func_78bc0)
.thumb_func_start Func_8091814  @ 0x08091814
	push	{r5, r6, lr}
	lsr	r5, r0, #10
	mov	r3, #0xf
	ldr	r6, =0x3ff
	and	r5, r3
	and	r6, r0
	cmp	r5, #7
	ble	.L9182a
	mov	r0, #1
	neg	r0, r0
	b	.L9184e
.L9182a:
	mov	r0, r5
	bl	_GetFlag
	cmp	r0, #0
	bne	.L9183a
	mov	r0, #2
	neg	r0, r0
	b	.L9184e
.L9183a:
	mov	r0, r5
	mov	r1, r6
	bl	_HasMove
	cmp	r0, #0
	bne	.L9184c
	mov	r0, #3
	neg	r0, r0
	b	.L9184e
.L9184c:
	mov	r0, #0
.L9184e:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_8091814

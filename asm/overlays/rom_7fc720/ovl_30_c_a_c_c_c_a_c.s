	.include "macros.inc"
	.include "gba.inc"

@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x204 and the message
@ delay at +0x1C8 to 0x18. Slot 0x0B gets 0x19999 written to BOTH +0x18 and
@ +0x1C -- its two motion rate fields -- and slots 0x0D and 0x0E are put into
@ animations 5 and 2, so the sanctum's attendants are already in pose when the
@ room appears.
.thumb_func_start OvlFunc_973_200871c
	push	{r5, lr}
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	add	r3, #0x44
	str	r3, [r2]
	sub	r3, #0x3c
	add	r2, r1, r3
	mov	r3, #0x18
	str	r3, [r2]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	ldr	r5, =0x19999
	str	r5, [r0, #0x1c]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r1, #5
	str	r5, [r0, #0x18]
	mov	r0, #0xd
	bl	__MapActor_SetAnim
	mov	r0, #0xe
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_973_200871c

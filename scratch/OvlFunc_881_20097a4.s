	.include "macros.inc"
	.include "gba.inc"

.thumb_func_start OvlFunc_881_20097a4
	push	{r5, lr}
	mov	r0, #0xf
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0xe
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	str	r3, [r5, #8]
	ldr	r3, [r0, #0x10]
	mov	r2, #0xa0
	str	r3, [r5, #0x10]
	ldr	r3, [r5, #0xc]
	lsl	r2, #12
	cmp	r3, r2
	bge	.L17f6
	mov	r3, #0xa0
	lsl	r3, #12
	mov	r0, #0x80
	str	r3, [r5, #0xc]
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L17f6
	mov	r0, #0x91
	bl	__PlaySound
	mov	r0, r5
	mov	r1, #3
	bl	__Actor_SetAnim
	mov	r0, #0x80
	lsl	r0, #2
	bl	__SetFlag
	mov	r2, r5
	add	r2, #0x64
	mov	r3, #1
	strh	r3, [r2]
.L17f6:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_20097a4

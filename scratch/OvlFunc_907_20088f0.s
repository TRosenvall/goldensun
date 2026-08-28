	.include "macros.inc"

.thumb_func_start OvlFunc_907_20088f0
	push	{lr}
	ldr	r0, =0x845
	bl	__GetFlag
	cmp	r0, #0
	beq	.L920
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xc0
	mov	r0, #0xe
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0xf
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	b	.L936
.L920:
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L936:
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r3, #0xc0
	lsl	r3, #9
	str	r3, [r0, #0x1c]
	mov	r2, #0xe1
	ldr	r3, =gState
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0xa
	bne	.L95e
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	b	.L968
.L95e:
	cmp	r3, #9
	bne	.L968
	ldr	r0, =0x12f
	bl	__ClearFlag
.L968:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L990
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0xb
	bne	.L990
	mov	r1, #0xf8
	mov	r2, #0xd8
	mov	r0, #0x14
	lsl	r1, #16
	lsl	r2, #16
	bl	__MapActor_SetPos
.L990:
	bl	OvlFunc_907_20089cc
	ldr	r0, =0x84a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L9b0
	ldr	r0, =0x84b
	bl	__GetFlag
	cmp	r0, #0
	bne	.L9b0
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__SetFlag
.L9b0:
	pop	{r0}
	bx	r0
.func_end OvlFunc_907_20088f0

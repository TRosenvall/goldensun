	.include "macros.inc"

.thumb_func_start OvlFunc_958_2009394
	push	{r5, lr}
	ldr	r5, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r5, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x98
	sub	sp, #8
	cmp	r2, r3
	bne	.L13c8
	mov	r0, #0xa2
	lsl	r0, #1
	bl	__SetFlag
	mov	r0, #0x9a
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L13c8
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L13c8:
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r5, r2
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x9e
	cmp	r2, r3
	bne	.L1464
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r5, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #1
	bne	.L13fa
	mov	r3, #0x6b
	mov	r2, #0x11
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x6c
	mov	r1, #0x11
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.L13fa:
	ldr	r0, =0x9a2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L142e
	mov	r1, #0xdc
	mov	r2, #0x9a
	lsl	r2, #17
	mov	r0, #8
	lsl	r1, #17
	bl	__MapActor_SetPos
	mov	r0, #8
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r3, #0x1b
	mov	r2, #0x13
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1d
	mov	r1, #0x13
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.L142e:
	ldr	r0, =0x9a5
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1458
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xae
	mov	r2, #0x90
	mov	r0, #0xa
	lsl	r1, #18
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r0, #0xa
	mov	r1, #2
	bl	__MapActor_SetAnim
.L1458:
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
.L1464:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_958_2009394

	.include "macros.inc"

.thumb_func_start OvlFunc_945_200b51c
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3564
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xe
	b	.L360c
.L3564:
	mov	r0, #0x8a
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L35f0
	mov	r2, #0xde
	mov	r3, #0xc0
	lsl	r2, #1
	lsl	r3, #6
	mov	r0, #8
	mov	r1, #0x98
	bl	OvlFunc_945_200c890
	ldr	r1, =gScript_945__0200e958
	mov	r0, #8
	bl	__MapActor_SetBehavior
	mov	r3, #0xf0
	lsl	r3, #1
	mov	r10, r3
	mov	r3, #0xb0
	lsl	r3, #8
	mov	r5, #0xf4
	mov	r8, r3
	mov	r0, #0xa
	mov	r1, #0xb8
	mov	r2, r10
	lsl	r5, #1
	mov	r6, #0xd0
	bl	OvlFunc_945_200c890
	lsl	r6, #8
	mov	r0, #0xc
	mov	r1, #0xaa
	mov	r2, r5
	mov	r3, r8
	bl	OvlFunc_945_200c890
	mov	r0, #0xd
	mov	r1, #0x88
	mov	r2, r5
	mov	r3, r6
	bl	OvlFunc_945_200c890
	mov	r0, #0xf
	mov	r1, #0x78
	mov	r2, r10
	mov	r3, r6
	bl	OvlFunc_945_200c890
	ldr	r2, =0x20e
	mov	r0, #0xe
	mov	r1, #0xb8
	mov	r3, r8
	bl	OvlFunc_945_200c890
	mov	r2, #0x92
	mov	r3, #0x80
	mov	r0, #0xb
	mov	r1, #0x88
	lsl	r2, #2
	lsl	r3, #8
	bl	OvlFunc_945_200c890
	ldr	r1, =gScript_945__0200e840
	mov	r0, #0xb
	bl	__MapActor_SetBehavior
	b	.L363e
.L35f0:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3600
	bl	OvlFunc_945_200d004
	b	.L363e
.L3600:
	ldr	r0, =0x925
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3616
	mov	r0, #0x12
.L360c:
	mov	r1, #0
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	b	.L363e
.L3616:
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	beq	.L363e
	ldr	r0, =0x922
	bl	__GetFlag
	cmp	r0, #0
	beq	.L363e
	mov	r0, #0xe
	mov	r1, #0
	mov	r2, #0
	bl	OvlFunc_945_200c8e8
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L363e:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_200b51c

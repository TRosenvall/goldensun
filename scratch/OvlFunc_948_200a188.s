	.include "macros.inc"
	.include "gba.inc"

.thumb_func_start OvlFunc_948_200a188
	push	{lr}
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0xc
	mov	r1, #0xf3
	bl	OvlFunc_948_200a0c4
	mov	r0, #0xb
	mov	r1, #0xf4
	bl	OvlFunc_948_200a0c4
	mov	r0, #0xa
	mov	r1, #0xf4
	bl	OvlFunc_948_200a0c4
	mov	r0, #9
	mov	r1, #0xf4
	bl	OvlFunc_948_200a0c4
	mov	r0, #8
	mov	r1, #0xf4
	bl	OvlFunc_948_200a0c4
	ldr	r0, =0xee7
	bl	__GetFlag
	cmp	r0, #0
	bne	.L21d0
	mov	r1, #0xe8
	mov	r2, #0xda
	mov	r0, #8
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
.L21d0:
	ldr	r0, =0xee8
	bl	__GetFlag
	cmp	r0, #0
	bne	.L21e8
	mov	r1, #0x94
	mov	r2, #0xce
	mov	r0, #9
	lsl	r1, #17
	lsl	r2, #18
	bl	__MapActor_SetPos
.L21e8:
	ldr	r0, =0xee9
	bl	__GetFlag
	cmp	r0, #0
	bne	.L2200
	mov	r1, #0xa4
	mov	r2, #0xbe
	mov	r0, #0xa
	lsl	r1, #17
	lsl	r2, #18
	bl	__MapActor_SetPos
.L2200:
	ldr	r0, =0xeea
	bl	__GetFlag
	cmp	r0, #0
	bne	.L2218
	mov	r1, #0xb4
	mov	r2, #0xda
	mov	r0, #0xb
	lsl	r1, #17
	lsl	r2, #18
	bl	__MapActor_SetPos
.L2218:
	mov	r0, #0x9c
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L222a
	mov	r0, #0
	bl	OvlFunc_948_2008f40
.L222a:
	ldr	r0, =0x9c1
	bl	__GetFlag
	cmp	r0, #0
	beq	.L223a
	mov	r0, #1
	bl	OvlFunc_948_2008f40
.L223a:
	ldr	r0, =0x9c2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L224a
	mov	r0, #2
	bl	OvlFunc_948_2008f40
.L224a:
	ldr	r0, =0x9c3
	bl	__GetFlag
	cmp	r0, #0
	beq	.L225a
	mov	r0, #3
	bl	OvlFunc_948_2008f40
.L225a:
	ldr	r0, =0x9c4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L226a
	mov	r0, #0
	bl	OvlFunc_948_2008fdc
.L226a:
	pop	{r0}
	bx	r0
.func_end OvlFunc_948_200a188

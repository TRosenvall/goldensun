	.include "macros.inc"

@ Cutscene: roughly 251 instructions of straight-line script --
@ 1 turn, 4 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x88f, 0x892, 0x893.
.thumb_func_start OvlFunc_926_200be58
	push	{r5, lr}
	ldr	r5, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r5, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x3d
	sub	sp, #8
	cmp	r2, r3
	beq	.L3e70
	b	.L4010
.L3e70:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0xe1
	add	r2, #0x49
	str	r2, [r3]
	lsl	r1, #1
	add	r3, r5, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #1
	bne	.L3ed2
	ldr	r0, =0x88f
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3ea0
	mov	r0, #8
	mov	r1, #6
	bl	__MapActor_SetAnim
	b	.L4092
.L3ea0:
	mov	r0, #8
	mov	r1, #5
	bl	__MapActor_SetAnim
	ldr	r0, =0xf14
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3eb4
	b	.L4092
.L3eb4:
	ldr	r0, =0x893
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3ec0
	b	.L4092
.L3ec0:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3ecc
	b	.L4092
.L3ecc:
	bl	OvlFunc_926_200a7ec
	b	.L4092
.L3ed2:
	cmp	r3, #2
	beq	.L3eda
	cmp	r3, #4
	bne	.L3fd6
.L3eda:
	ldr	r0, =0x12f
	bl	__ClearFlag
	ldr	r0, =0x895
	bl	__GetFlag
	mov	r5, r0
	cmp	r5, #0
	bne	.L3f6a
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, r0
	add	r3, #0x55
	strb	r5, [r3]
	mov	r3, #0xc0
	lsl	r3, #12
	str	r3, [r0, #0xc]
	str	r3, [r0, #0x3c]
	ldr	r3, =0xcccc
	mov	r2, #0x80
	str	r3, [r0, #0x18]
	ldr	r3, [r0, #0x50]
	lsl	r2, #8
	str	r2, [r0, #0x1c]
	strh	r2, [r3, #0x1e]
	ldr	r0, =0x89a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3fb2
	mov	r1, #0xf8
	mov	r2, #0xd0
	mov	r0, #0x12
	lsl	r1, #16
	lsl	r2, #16
	bl	__MapActor_SetPos
	ldr	r0, =0x89b
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3fb2
	mov	r1, #0x80
	mov	r2, #0xf0
	lsl	r1, #17
	lsl	r2, #16
	mov	r0, #0x10
	bl	__MapActor_SetPos
	mov	r0, #0x12
	bl	__MapActor_GetActor
	ldr	r5, =OvlFunc_926_2008324
	str	r5, [r0, #0x6c]
	mov	r0, #0xd
	bl	__MapActor_GetActor
	str	r5, [r0, #0x6c]
	mov	r0, #0xe
	bl	__MapActor_GetActor
	str	r5, [r0, #0x6c]
	mov	r0, #0xf
	bl	__MapActor_GetActor
	str	r5, [r0, #0x6c]
	mov	r0, #0x10
	bl	__MapActor_GetActor
	str	r5, [r0, #0x6c]
	b	.L3fb2
.L3f6a:
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r2, r0
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	mov	r3, #0xc0
	lsl	r3, #12
	str	r3, [r0, #0xc]
	str	r3, [r0, #0x3c]
	mov	r1, #0x80
	ldr	r3, =0xcccc
	lsl	r1, #8
	str	r3, [r0, #0x18]
	str	r1, [r0, #0x1c]
	mov	r3, #0x59
	add	r3, r0
	ldrb	r2, [r3]
	mov	r12, r3
	mov	r3, #8
	orr	r3, r2
	mov	r2, r12
	strb	r3, [r2]
	ldr	r3, [r0, #0x50]
	mov	r2, #0xa
	strh	r1, [r3, #0x1e]
	mov	r3, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0xb
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.L3fb2:
	mov	r0, #0x98
	mov	r1, #0xc0
	mov	r2, #0xe0
	lsl	r1, #13
	lsl	r2, #16
	mov	r3, #0xdf
	lsl	r0, #17
	bl	OvlFunc_common0_18
	mov	r0, #0xa
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	mov	r1, #5
	bl	__MapActor_SetAnim
	b	.L4092
.L3fd6:
	cmp	r3, #3
	bne	.L4092
	ldr	r0, =0x12f
	bl	__ClearFlag
	ldr	r0, =0x895
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3ff0
	bl	OvlFunc_926_200aad0
	b	.L4092
.L3ff0:
	ldr	r0, =0x8b2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L4092
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	b	.L4092
.L4010:
	mov	r0, #0xaa
	bl	__Func_8091ff0
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r2, [r0]
	mov	r3, #0x10
	mov	r1, #0xe1
	orr	r3, r2
	lsl	r1, #1
	strb	r3, [r0]
	add	r3, r5, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L405c
	ldr	r0, =0xf14
	bl	__GetFlag
	cmp	r0, #0
	beq	.L405c
	ldr	r0, =0x894
	bl	__GetFlag
	cmp	r0, #0
	bne	.L405c
	mov	r3, #0xa
	mov	r2, #0x18
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xa
	mov	r1, #0x54
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.L405c:
	ldr	r0, =0x892
	bl	__GetFlag
	cmp	r0, #0
	beq	.L4092
	mov	r1, #0x98
	mov	r2, #0xc4
	mov	r0, #9
	lsl	r1, #16
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r3, #0xa
	mov	r2, #0x16
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xa
	mov	r1, #0x1a
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.L4092:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_926_200be58

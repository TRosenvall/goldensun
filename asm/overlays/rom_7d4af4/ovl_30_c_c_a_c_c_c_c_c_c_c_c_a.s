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

@ Cutscene: roughly 139 instructions of straight-line script --
@ 1 turn, 2 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x200, 0x201, 0x8c1, 0x950.
.thumb_func_start OvlFunc_949_2008728
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	mov	r1, #0xb0
	mov	r2, #0xb0
	lsl	r2, #17
	mov	r0, #0x10
	lsl	r1, #17
	bl	__MapActor_SetPos
	ldr	r1, =gScript_949__02008ec0
	mov	r0, #0x10
	bl	__MapActor_SetBehavior
	mov	r0, #0x10
	bl	__MapActor_GetActor
	mov	r2, r0
	mov	r3, #1
	ldr	r5, =OvlFunc_949_2008170
	add	r2, #0x64
	strh	r3, [r2]
	mov	r1, #0xb8
	mov	r2, #0xa0
	lsl	r2, #17
	str	r5, [r0, #0x6c]
	lsl	r1, #17
	mov	r0, #0x11
	bl	__MapActor_SetPos
	ldr	r1, =gScript_949__02008f90
	mov	r0, #0x11
	bl	__MapActor_SetBehavior
	mov	r0, #0x11
	bl	__MapActor_GetActor
	mov	r3, r0
	add	r3, #0x64
	mov	r6, #0
	strh	r6, [r3]
	str	r5, [r0, #0x6c]
	mov	r0, #0xe
	bl	__MapActor_GetActor
	ldr	r3, =OvlFunc_949_20086e8
	str	r3, [r0, #0x6c]
	ldr	r0, =0x8c1
	bl	__GetFlag
	cmp	r0, #0
	beq	.L7a8
	mov	r1, #0x9e
	mov	r2, #0xa4
	mov	r0, #0x1c
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
.L7a8:
	ldr	r0, =0x201
	bl	__GetFlag
	cmp	r0, #0
	beq	.L7b6
	bl	OvlFunc_949_2008ca8
.L7b6:
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L7ce
	bl	OvlFunc_949_2008224
	mov	r0, #8
	mov	r1, #4
	bl	__MapActor_SetAnim
.L7ce:
	mov	r0, #0x95
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L83e
	mov	r1, #0x82
	mov	r2, #0x8c
	mov	r0, #0x14
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x82
	mov	r2, #0x8c
	mov	r0, #0x15
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x82
	mov	r2, #0x8c
	mov	r0, #0x16
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x82
	mov	r2, #0x8c
	mov	r0, #0x18
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x82
	mov	r2, #0x8c
	mov	r0, #0x19
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x82
	mov	r2, #0x8c
	mov	r0, #0x1a
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x82
	mov	r2, #0x8c
	mov	r0, #0x1b
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	b	.L86a
.L83e:
	ldr	r0, =0x962
	bl	__GetFlag
	cmp	r0, #0
	beq	.L86a
	mov	r1, #0x8c
	mov	r2, #0xa0
	mov	r0, #0x1b
	lsl	r1, #17
	lsl	r2, #15
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r0, #0x1b
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0x1b
	mov	r1, #1
	bl	__MapActor_SetAnim
.L86a:
	mov	r0, #0
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_949_2008728

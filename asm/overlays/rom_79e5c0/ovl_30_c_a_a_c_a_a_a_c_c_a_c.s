	.include "macros.inc"

@ Cutscene: roughly 133 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x843, 0x845.
.thumb_func_start OvlFunc_911_2008694
	push	{r5, lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x27
	sub	sp, #8
	cmp	r2, r3
	bne	.L6b0
	bl	OvlFunc_911_200a910
	b	.L7d4
.L6b0:
	ldr	r3, =0x26
	cmp	r2, r3
	bne	.L6c6
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x44
	str	r2, [r3]
	b	.L7d4
.L6c6:
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1a
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r5, =gScript_911__0200add8
	mov	r0, #0x17
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x18
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x19
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x1a
	mov	r1, r5
	bl	__MapActor_SetBehavior
	ldr	r0, =0x845
	bl	__GetFlag
	cmp	r0, #0
	bne	.L770
	mov	r5, #8
.L724:
	mov	r0, r5
	bl	__MapActor_GetActor
	add	r5, #1
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	cmp	r5, #0x10
	bls	.L724
	mov	r3, #0xd
	str	r3, [sp]
	mov	r5, #8
	mov	r0, #0xd
	mov	r1, #9
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0xf
	str	r3, [sp]
	mov	r0, #0xd
	mov	r1, #9
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0xe
	mov	r2, #9
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xd
	mov	r1, #9
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.L770:
	ldr	r0, =0x843
	bl	__GetFlag
	cmp	r0, #0
	bne	.L78e
	ldr	r3, =gState
	mov	r1, #0xe1
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #1
	bne	.L78e
	bl	OvlFunc_911_20088ec
.L78e:
	ldr	r0, =0x843
	bl	__GetFlag
	cmp	r0, #0
	beq	.L7d4
	mov	r0, #1
	bl	__DeleteFieldActor
	mov	r0, #2
	bl	__DeleteFieldActor
	mov	r0, #3
	bl	__DeleteFieldActor
	mov	r0, #0x11
	bl	__DeleteFieldActor
	mov	r0, #0x12
	bl	__DeleteFieldActor
	mov	r0, #0x13
	bl	__DeleteFieldActor
	mov	r0, #0x14
	bl	__DeleteFieldActor
	mov	r0, #0x15
	bl	__DeleteFieldActor
	mov	r0, #0x16
	bl	__DeleteFieldActor
	ldr	r0, =.L32d8
	bl	__LoadFieldActors
.L7d4:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_911_2008694

	.include "macros.inc"

@ Cutscene: roughly 178 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x321, 0x322, 0x845.
@ Sets save bit 0x84b.
.thumb_func_start OvlFunc_909_20086e0
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x49
	str	r2, [r3]
	mov	r0, #1
	sub	sp, #8
	bl	__Func_80118c0
	mov	r0, #2
	bl	__Func_80118c0
	ldr	r0, =0x84b
	bl	__SetFlag
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L716
	mov	r0, #0x80
	lsl	r0, #2
	bl	__ClearFlag
.L716:
	ldr	r0, =0x84f
	bl	__GetFlag
	cmp	r0, #0
	bne	.L758
	ldr	r0, =0x845
	bl	__GetFlag
	cmp	r0, #0
	bne	.L758
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x1d
	bne	.L740
	bl	OvlFunc_909_20088c0
	b	.L888
.L740:
	cmp	r3, #9
	beq	.L746
	b	.L888
.L746:
	ldr	r0, =0x321
	bl	__GetFlag
	cmp	r0, #0
	bne	.L752
	b	.L888
.L752:
	bl	OvlFunc_909_200979c
	b	.L888
.L758:
	ldr	r0, =0x84e
	bl	__GetFlag
	mov	r6, r0
	cmp	r6, #0
	beq	.L766
	b	.L888
.L766:
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x1d
	bne	.L792
	ldr	r0, =0x85e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L782
	b	.L888
.L782:
	ldr	r0, =0x845
	bl	__GetFlag
	cmp	r0, #0
	beq	.L888
	bl	OvlFunc_909_20099b0
	b	.L888
.L792:
	cmp	r3, #0x1c
	bne	.L888
	ldr	r0, =0x322
	bl	__GetFlag
	cmp	r0, #0
	beq	.L888
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L884
	mov	r3, #0x2d
	str	r3, [sp, #4]
	mov	r5, #0x26
	mov	r0, #0x26
	mov	r1, #0x37
	mov	r2, #4
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x2e
	str	r3, [sp, #4]
	mov	r0, #0x2a
	mov	r3, #1
	mov	r1, #0x37
	mov	r2, #4
	str	r5, [sp]
	bl	__Func_8010704
	mov	r1, #0x9a
	mov	r2, #0xb6
	mov	r0, #0x15
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x9e
	mov	r2, #0xb6
	mov	r0, #0x16
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0xa2
	mov	r2, #0xb6
	mov	r0, #0x17
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0xa6
	mov	r2, #0xb6
	lsl	r2, #18
	lsl	r1, #18
	mov	r0, #0x18
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x15
	bl	__MapActor_GetActor
	add	r0, #0x55
	strb	r6, [r0]
	mov	r0, #0x16
	bl	__MapActor_GetActor
	add	r0, #0x55
	strb	r6, [r0]
	mov	r0, #0x17
	bl	__MapActor_GetActor
	add	r0, #0x55
	strb	r6, [r0]
	mov	r0, #0x18
	bl	__MapActor_GetActor
	add	r0, #0x55
	strb	r6, [r0]
	mov	r0, #0x15
	bl	__MapActor_GetActor
	ldr	r5, =0xfffc0000
	str	r5, [r0, #0xc]
	mov	r0, #0x16
	bl	__MapActor_GetActor
	str	r5, [r0, #0xc]
	mov	r0, #0x17
	bl	__MapActor_GetActor
	str	r5, [r0, #0xc]
	mov	r0, #0x18
	bl	__MapActor_GetActor
	str	r5, [r0, #0xc]
	b	.L888
.L884:
	bl	OvlFunc_909_200a1bc
.L888:
	mov	r0, #0
	add	sp, #8
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_909_20086e0

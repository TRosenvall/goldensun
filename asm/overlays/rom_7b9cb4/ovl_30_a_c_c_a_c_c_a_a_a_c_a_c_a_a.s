	.include "macros.inc"
	.include "gba.inc"

@ 131 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, SetEntityActorOptions, OvlFunc_40c, OvlFunc_3460
@   TestSaveBit, SetSlotAnimation, CopyMapRectAttributes, GetSlotEntityChecked
@   OvlFunc_3460, TestSaveBit, SetSlotAnimation, CopyMapRectAttributes
@   GetSlotEntityChecked, OvlFunc_3460
@   ... and 10 more
@ reads save bits 0x200, 0x201, 0x204, 0x327.
.thumb_func_start OvlFunc_932_200a6c0
	push	{lr}
	mov	r0, #9
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	bl	OvlFunc_932_200840c
	mov	r0, #9
	bl	OvlFunc_932_200b460
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2710
	mov	r0, #9
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r3, #0x1a
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r2, #1
	mov	r3, #1
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8010704
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r0]
.L2710:
	mov	r0, #0xb
	bl	OvlFunc_932_200b460
	ldr	r0, =0x201
	bl	__GetFlag
	cmp	r0, #0
	beq	.L274c
	mov	r0, #0xb
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r3, #0x11
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #1
	mov	r2, #1
	mov	r0, #1
	mov	r1, #0
	bl	__Func_8010704
	mov	r0, #0xb
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r0]
.L274c:
	mov	r0, #0xc
	bl	OvlFunc_932_200b460
	mov	r0, #0x81
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L278a
	mov	r0, #0xc
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r3, #0x1a
	mov	r2, #0xf
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #1
	mov	r2, #1
	mov	r0, #1
	mov	r1, #0
	bl	__Func_8010704
	mov	r0, #0xc
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r0]
.L278a:
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_932_200b428
	lsl	r1, #4
	bl	__StartTask
	ldr	r0, =0x327
	bl	__GetFlag
	cmp	r0, #0
	beq	.L27c8
	mov	r3, #0x1d
	mov	r2, #0x51
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0x52
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r3, #1
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x1c
	mov	r2, #0x1d
	mov	r3, #0x11
	bl	__CopyMapTiles
	b	.L27f0
.L27c8:
	mov	r3, #0x1d
	mov	r2, #0x51
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1c
	mov	r1, #0x52
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r3, #1
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x1c
	mov	r2, #0x1d
	mov	r3, #0x11
	bl	__CopyMapTiles
.L27f0:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_200a6c0

	.include "macros.inc"

@ 121 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, CopyMapRectAttributes x5, BeginCutscene, PlayInteractionEffect
@   SetSlotEntitySpeed, SetSlotFacingAndScript, MoveSlotToAndWait, TurnSlotToAngle
@   MoveSlotToAndWait, TurnSlotToAngle, EndCutscene
.thumb_func_start OvlFunc_907_20089cc
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r0, #0
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #0x14
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r7, r3, #20
	ldr	r3, [r5, #8]
	asr	r3, #20
	mov	r9, r3
	ldr	r3, [r5, #0x10]
	asr	r3, #20
	mov	r10, r3
	mov	r3, #0xc
	ldr	r6, [r0, #8]
	mov	r5, #0xf
	str	r3, [sp, #4]
	mov	r0, #0xf
	mov	r1, #0xb
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0xd
	str	r3, [sp, #4]
	mov	r0, #0xf
	mov	r1, #0xb
	mov	r2, #3
	mov	r8, r3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xf
	mov	r1, #0xb
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	asr	r6, #20
	mov	r0, #1
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	str	r6, [sp]
	str	r7, [sp, #4]
	bl	__Func_8010704
	cmp	r6, #0x10
	bne	.La4c
	cmp	r7, #0xd
	beq	.La60
.La4c:
	mov	r3, #0x10
	str	r3, [sp]
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.La60:
	mov	r3, r9
	cmp	r3, #0x10
	bne	.Lace
	mov	r3, r10
	cmp	r3, #0xd
	bne	.Lace
	bl	__CutsceneStart
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0x14
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r0, #0
	mov	r1, #6
	mov	r2, #0
	bl	__MapActor_Jump
	cmp	r7, #0xd
	bne	.Lab2
	mov	r1, #0x83
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0xc4
	bl	__Func_8092158
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_8092adc
	b	.Laca
.Lab2:
	mov	r1, #0x8f
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0xda
	bl	__Func_8092158
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
.Laca:
	bl	__CutsceneEnd
.Lace:
	add	sp, #8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_907_20089cc

@ Cutscene: roughly 186 instructions of straight-line script --
@ 2 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x845, 0x848, 0x849, 0x881.
.thumb_func_start OvlFunc_907_2008ae0
	push	{r5, r6, r7, lr}
	mov	r0, #0xa
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r6, r0
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r7, r0
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r0, =0x845
	bl	__GetFlag
	mov	r5, r0
	cmp	r5, #0
	beq	.Lb8c
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
	mov	r3, #1
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x38
	mov	r1, #0xf
	mov	r2, #0x28
	mov	r3, #0xf
	bl	__CopyMapTiles
	mov	r3, #0xa
	mov	r2, #0xf
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1a
	mov	r1, #0xf
	mov	r2, #1
	mov	r3, #3
	bl	__Func_8010704
	ldr	r0, =0x849
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lb72
	ldr	r0, =0x848
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb68
	b	.Lc92
.Lb68:
	mov	r0, #0xe
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.Lb72:
	mov	r1, #0xd0
	mov	r0, #0xc
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r0, #0xd
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	b	.Lc92
.Lb8c:
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xe
	bl	__MapActor_SetPos
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r2, r6
	add	r2, #0x55
	strb	r5, [r2]
	ldr	r0, =0x881
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc80
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	mov	r5, #0x10
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #0x10
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	mov	r1, #0x8e
	orr	r3, r5
	mov	r2, #0x9c
	strb	r3, [r0]
	lsl	r2, #16
	lsl	r1, #16
	mov	r0, #0x10
	bl	__MapActor_SetPos
	mov	r0, #0x10
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r1, #0x8e
	mov	r2, #0x9c
	mov	r0, #0xa
	lsl	r2, #16
	lsl	r1, #16
	bl	__MapActor_SetPos
	ldr	r2, [r6, #0x50]
	mov	r3, #0x80
	lsl	r3, #7
	strh	r3, [r2, #0x1e]
	ldr	r2, =0xfff80000
	ldr	r3, [r6, #0xc]
	add	r3, r2
	str	r3, [r6, #0xc]
	ldr	r0, =0x848
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc5c
	mov	r1, #0x84
	mov	r2, #0xba
	mov	r0, #0xb
	lsl	r1, #16
	lsl	r2, #16
	bl	__MapActor_SetPos
	b	.Lc92
.Lc5c:
	mov	r1, #0xb0
	mov	r2, #0xc4
	lsl	r2, #16
	mov	r0, #0xb
	lsl	r1, #15
	bl	__MapActor_SetPos
	mov	r1, #3
	mov	r0, #0xb
	bl	__Func_8092b08
	mov	r3, r7
	add	r3, #0x59
	ldrb	r2, [r3]
	mov	r1, #4
	orr	r2, r1
	strb	r2, [r3]
	b	.Lc92
.Lc80:
	mov	r3, #0x80
	lsl	r3, #14
	str	r3, [r6, #0xc]
	mov	r3, r7
	add	r3, #0x55
	strb	r0, [r3]
	mov	r3, #0xc0
	lsl	r3, #14
	str	r3, [r7, #0xc]
.Lc92:
	bl	OvlFunc_907_2008cb4
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_907_2008ae0

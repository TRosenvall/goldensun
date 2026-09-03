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

@ 111 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, PlaceSlotAt x2, GetSlotEntityChecked, SetEntityActorOptions
@   GetSlotEntityChecked x2, TestSaveBit x2, CopyMapRectIndicesU, CopyMapRectAttributes
@   UpdateMapView, WaitFrames, TestSaveBit, BeginCutscene
@   GetSlotEntityChecked, MoveCameraTo
@   ... and 3 more
@ reads save bits 0x109, 0x908.
.thumb_func_start OvlFunc_932_200a804
	push	{r5, r6, lr}
	mov	r0, #0xa
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r1, #0
	mov	r6, r0
	mov	r2, #0
	mov	r0, #8
	bl	__MapActor_SetPos
	mov	r2, #0
	mov	r1, #0
	mov	r0, #9
	bl	__MapActor_SetPos
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r2, r6
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	ldr	r3, =0xe666
	str	r3, [r6, #0x18]
	ldr	r3, =0x9999
	ldr	r2, [r6, #0x50]
	str	r3, [r6, #0x1c]
	mov	r3, #0x80
	lsl	r3, #8
	strh	r3, [r2, #0x1e]
	mov	r0, #0xc
	bl	__MapActor_GetActor
	ldr	r5, .L2888	@ 0
	add	r0, #0x55
	strb	r5, [r0]
	mov	r0, #0xc
	bl	__MapActor_GetActor
	ldr	r3, =0xffe40000
	str	r3, [r0, #0xc]
	ldr	r0, =0x908
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2882
	ldr	r3, [r6, #8]
	mov	r2, #0xe0
	lsl	r2, #12
	add	r3, r2
	str	r3, [r6, #8]
	ldr	r2, =0xfff80000
	ldr	r3, [r6, #0xc]
	add	r3, r2
	str	r3, [r6, #0xc]
	ldr	r2, [r6, #0x50]
	mov	r3, #0xc0
	lsl	r3, #8
	strh	r3, [r2, #0x1e]
.L2882:
	ldr	r0, =0x908
	b	.L28a0

	.align	2, 0
.L2888:
	.word	0
	.pool

.L28a0:
	bl	__GetFlag
	cmp	r0, #0
	beq	.L28da
	mov	r3, #0xb
	mov	r2, #9
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x19
	mov	r1, #0x24
	mov	r2, #0x2b
	mov	r3, #0x24
	bl	__CopyMapTiles
	mov	r3, #0x2b
	mov	r2, #0x23
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x19
	mov	r1, #0x23
	mov	r2, #0xa
	mov	r3, #5
	bl	__Func_8010704
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
.L28da:
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #6
	bne	.L291c
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L291c
	bl	__CutsceneStart
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r1, =0xffa80000
	str	r1, [r0, #0xc]
	mov	r0, #0xc6
	lsl	r0, #18
	ldr	r2, =0x2410000
	mov	r3, #0
	bl	__Func_80933f8
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	bl	__CutsceneEnd
.L291c:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_200a804

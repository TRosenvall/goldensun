	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 277 instructions of straight-line script --
@ 1 turn, 2 animation changes, 5 dialogue lines, 6 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xe13, 0xe14, 0xe2e, 0xe2f.
@ Reads save bits 0x109, 0x200, 0x950.
.thumb_func_start OvlFunc_951_20081d8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r6, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r6, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xbd
	cmp	r2, r3
	bne	.L280
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	ldr	r3, =0x3f42
	ldr	r1, =REG_BLDCNT
	mov	r10, r3
	mov	r8, r1
	mov	r2, r10
	mov	r3, r8
	strh	r2, [r3]
	ldr	r3, =0x80c
	ldr	r7, =REG_BLDALPHA
	strh	r3, [r7]
	mov	r0, #0x18
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r1, #2
	mov	r0, #0x19
	bl	__MapActor_SetAnim
	mov	r0, #0x18
	bl	__MapActor_GetActor
	ldr	r5,=0xffff0000
	str	r5, [r0, #0x18]
	mov	r0, #0x19
	bl	__MapActor_GetActor
	str	r5, [r0, #0x18]
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r5, #2
	add	r0, #0x23
	strb	r5, [r0]
	mov	r0, #0x19
	bl	__MapActor_GetActor
	add	r0, #0x23
	strb	r5, [r0]
	bl	__MapTransitionIn
	mov	r1, #0xe1
	lsl	r1, #1
	add	r3, r6, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #1
	beq	.L260
	b	.L462
.L260:
	bl	OvlFunc_951_20096a8
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L272
	b	.L462
.L272:
	mov	r3, r10
	mov	r1, r8
	strh	r3, [r1]
	mov	r3, #0x80
	lsl	r3, #5
	strh	r3, [r7]
	b	.L462
.L280:
	mov	r0, #0x95
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L296
	mov	r0, #0x11
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L296:
	ldr	r2, =iwram_3001d18
	mov	r3, #1
	strb	r3, [r2]
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x49
	str	r2, [r3]
	mov	r3, #0xe1
	lsl	r3, #1
	add	r5, r6, r3
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	ldrh	r2, [r5]
	cmp	r3, #0xa
	bne	.L2cc
	mov	r0, #8
	mov	r1, #1
	bl	__Func_8092950
	mov	r0, #9
	mov	r1, #2
	bl	__Func_8092950
	ldrh	r2, [r5]
.L2cc:
	lsl	r3, r2, #16
	mov	r2, #0xd0
	lsl	r2, #12
	cmp	r3, r2
	bne	.L380
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L380
	bl	__CutsceneStart
	mov	r0, #8
	mov	r1, #1
	bl	__Func_8092950
	mov	r1, #2
	mov	r0, #9
	bl	__Func_8092950
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r2, #0x70
	mov	r0, #0
	mov	r1, #0x78
	bl	__Func_80921c4
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r3, =ewram_2001000
	ldr	r2, [r6, #0x10]
	ldr	r3, [r3]
	sub	r5, r2, r3
	cmp	r5, #0
	ble	.L362
	ldr	r3, =0x4e1f
	cmp	r5, r3
	ble	.L32c
	mov	r0, #0x5d
	bl	__PlaySound
	b	.L340
.L32c:
	ldr	r1, =0x1387
	cmp	r5, r1
	ble	.L33a
	mov	r0, #0x5c
	bl	__PlaySound
	b	.L340
.L33a:
	mov	r0, #0x5b
	bl	__PlaySound
.L340:
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0xe13
	bl	__MessageID
	mov	r0, r5
	mov	r1, #5
	bl	__Func_8019908
	mov	r0, #9
	mov	r1, #0
	bl	__ActorMessage
	bl	__Func_80b04c4
	b	.L37c
.L362:
	cmp	r5, #0
	bge	.L37c
	ldr	r0, =0xe14
	bl	__MessageID
	neg	r0, r5
	mov	r1, #5
	bl	__Func_8019908
	mov	r0, #9
	mov	r1, #0
	bl	__ActorMessage
.L37c:
	bl	__CutsceneEnd
.L380:
	ldr	r5, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r5, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #0xc
	bne	.L462
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L462
	mov	r2, #0x96
	lsl	r2, #1
	add	r7, r5, r2
	bl	__CutsceneStart
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r5, #1
	mov	r3, #0
	ldrsb	r3, [r7, r3]
	neg	r5, r5
	cmp	r3, r5
	bne	.L3c6
	mov	r0, #1
	bl	OvlFunc_951_20088f8
	b	.L45e
.L3c6:
	mov	r1, #2
	neg	r1, r1
	cmp	r3, r1
	beq	.L45e
	ldr	r0, =0xe2e
	bl	__MessageID
	mov	r0, #8
	mov	r1, #0
	bl	__ActorMessage
	mov	r3, #0
	ldrsb	r3, [r7, r3]
	cmp	r3, r5
	beq	.L444
	mov	r8, r5
	mov	r6, r7
.L3e8:
	cmp	r6, r7
	bne	.L3f4
	ldr	r0, =0xe2f
	bl	__MessageID
	b	.L3fa
.L3f4:
	ldr	r0, =_MSG_e30
	bl	__MessageID
.L3fa:
	mov	r0, #0
	ldrsb	r0, [r6, r0]
	bl	OvlFunc_951_2008d70
	mov	r1, #2
	mov	r5, r0
	bl	__Func_8019908
	mov	r0, #8
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r5
	mov	r1, #3
	bl	__Func_808f1c0
	mov	r1, #0
	mov	r0, r5
	bl	__Func_8091a58
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	add	r6, #1
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r3, #0
	ldrsb	r3, [r6, r3]
	cmp	r3, r8
	bne	.L3e8
.L444:
	ldr	r3, =gState
	mov	r2, #0x96
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0xfe
	ldr	r0, =0xe31
	strb	r2, [r3]
	bl	__MessageID
	mov	r0, #8
	mov	r1, #0
	bl	__ActorMessage
.L45e:
	bl	__CutsceneEnd
.L462:
	mov	r0, #0
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_951_20081d8

@ 384 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, DialogueWait, PlaySound, DialogueWait
@   TurnSlotToAngle, DialogueWait, CopyMapRectIndicesU, DialogueWait
@   CopyMapRectIndicesU, PlaySound, DialogueWait, CopyMapRectIndicesU
@   PlaySound, DialogueWait
@   ... and 83 more
.thumb_func_start OvlFunc_951_20084bc
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	sub	sp, #8
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0x94
	bl	__PlaySound
	mov	r0, #0x64
	bl	__CutsceneWait
	mov	r1, #0xc0
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r3, #8
	str	r3, [sp, #4]
	mov	r6, #3
	mov	r8, r3
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x52
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #3
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x55
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x58
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5b
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5e
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x61
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x64
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x4f
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x52
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x55
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x58
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5b
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5e
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x61
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r2, #0x46
	mov	r3, #0
	mov	r1, #0x1d
	mov	r0, #0x64
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #0x46
	bl	__CutsceneWait
	mov	r0, #0x7e
	bl	__PlaySound
	mov	r0, r5
	mov	r1, #3
	bl	__Func_808f1c0
	mov	r1, #0
	mov	r0, r5
	bl	__Func_8091a58
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x61
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5e
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5b
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x58
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x55
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x1d
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x52
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x64
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x61
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5e
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x5b
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x58
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x55
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x52
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r1, #0x14
	mov	r2, #0x46
	mov	r3, #0
	mov	r0, #0x4f
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x9a
	bl	__PlaySound
	mov	r0, #8
	bl	__CutsceneWait
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_951_20084bc

@ 45 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, WaitFrames x2
.thumb_func_start OvlFunc_951_2008880
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001e70
	mov	r0, #0xd8
	ldr	r5, [r3]
	bl	__PlaySound
	mov	r2, #0xb2
	lsl	r2, #1
	add	r5, r2
	mov	r6, #0xf
.L89a:
	ldr	r3, [r5, #0xc]
	ldr	r2, =0xffff0000
	add	r3, r2
	str	r3, [r5, #0xc]
	mov	r0, #4
	sub	r6, #1
	bl	__WaitFrames
	cmp	r6, #0
	bge	.L89a
	ldr	r3, =0x3f42
	ldr	r2, =REG_BLDCNT
	ldr	r5, =.L1fc0
	ldr	r7, =REG_BLDALPHA
	mov	r10, r3
	mov	r8, r2
	mov	r6, #7
.L8bc:
	mov	r3, r10
	mov	r2, r8
	strh	r3, [r2]
	ldrh	r3, [r5]
	add	r5, #2
	strh	r3, [r7]
	mov	r0, #8
	sub	r6, #1
	bl	__WaitFrames
	cmp	r6, #0
	bge	.L8bc
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_951_2008880

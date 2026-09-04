	.include "macros.inc"

@ 154 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
@   SetMapTransition x2
.thumb_func_start OvlFunc_890_2008d9c
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.Ldae
	b	.Leaa
.Ldae:
	ldr	r6, =.L2de4
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.Le0c
	cmp	r5, #2
	bgt	.Ldc4
	cmp	r5, #0
	beq	.Ldd2
	cmp	r5, #1
	beq	.Ldee
	b	.Le8c
.Ldc4:
	cmp	r5, #4
	beq	.Le4c
	cmp	r5, #4
	blt	.Le2c
	cmp	r5, #0x50
	beq	.Le78
	b	.Le8c
.Ldd2:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x21
	bl	__CopyMapTiles
	b	.Le8c
.Ldee:
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x21
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x22
	b	.Le70
.Le0c:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x22
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x23
	b	.Le70
.Le2c:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x23
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x24
	b	.Le70
.Le4c:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x25
.Le70:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.Le8c
.Le78:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x31
	mov	r2, #0x1e
	mov	r3, #0x21
	bl	__CopyMapTiles
.Le8c:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x5a
	cmp	r5, r3
	bls	.Leaa
	ldr	r3, .Lec8	@ 0
	strh	r3, [r6]
.Leaa:
	ldr	r5, =.L2de8
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.Leec
	cmp	r3, #2
	bne	.Led4
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #9
	lsl	r1, #9
	lsl	r2, #9
	bl	__Func_8012330
	b	.Lee6

	.align	2, 0
.Lec8:
	.word	0
	.pool

.Led4:
	cmp	r3, #1
	bne	.Lee6
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_8012330
.Lee6:
	ldr	r3, [r5]
	sub	r3, #1
	str	r3, [r5]
.Leec:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2008d9c

@ 129 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
.thumb_func_start OvlFunc_890_2008ef8
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.Lf0a
	b	.L1006
.Lf0a:
	ldr	r6, =.L2ddc
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.Lf68
	cmp	r5, #2
	bgt	.Lf20
	cmp	r5, #0
	beq	.Lf2e
	cmp	r5, #1
	beq	.Lf4a
	b	.Lfe8
.Lf20:
	cmp	r5, #4
	beq	.Lfa8
	cmp	r5, #4
	blt	.Lf88
	cmp	r5, #0x5a
	beq	.Lfd4
	b	.Lfe8
.Lf2e:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x21
	bl	__CopyMapTiles
	b	.Lfe8
.Lf4a:
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x21
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x22
	b	.Lfcc
.Lf68:
	mov	r5, #1
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x22
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x23
	b	.Lfcc
.Lf88:
	mov	r5, #1
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x23
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x24
	b	.Lfcc
.Lfa8:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x25
.Lfcc:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.Lfe8
.Lfd4:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x31
	mov	r2, #0x2a
	mov	r3, #0x21
	bl	__CopyMapTiles
.Lfe8:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x64
	cmp	r5, r3
	bls	.L1006
	ldr	r3, .L1010	@ 0
	strh	r3, [r6]
.L1006:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0

	.align	2, 0
.L1010:
	.word	0
.func_end OvlFunc_890_2008ef8

@ 129 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
.thumb_func_start OvlFunc_890_200901c
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.L102e
	b	.L112a
.L102e:
	ldr	r6, =.L2de0
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.L108c
	cmp	r5, #2
	bgt	.L1044
	cmp	r5, #0
	beq	.L1052
	cmp	r5, #1
	beq	.L106e
	b	.L110c
.L1044:
	cmp	r5, #4
	beq	.L10cc
	cmp	r5, #4
	blt	.L10ac
	cmp	r5, #0x5f
	beq	.L10f8
	b	.L110c
.L1052:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x24
	bl	__CopyMapTiles
	b	.L110c
.L106e:
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x25
	b	.L10f0
.L108c:
	mov	r5, #1
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x25
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x26
	b	.L10f0
.L10ac:
	mov	r5, #1
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x26
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x27
	b	.L10f0
.L10cc:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x27
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x28
.L10f0:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.L110c
.L10f8:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x31
	mov	r2, #0x1f
	mov	r3, #0x24
	bl	__CopyMapTiles
.L110c:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x69
	cmp	r5, r3
	bls	.L112a
	ldr	r3, .L1134	@ 0
	strh	r3, [r6]
.L112a:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0

	.align	2, 0
.L1134:
	.word	0
.func_end OvlFunc_890_200901c

@ 129 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
.thumb_func_start OvlFunc_890_2009140
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.L1152
	b	.L124e
.L1152:
	ldr	r6, =.L2dec
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.L11b0
	cmp	r5, #2
	bgt	.L1168
	cmp	r5, #0
	beq	.L1176
	cmp	r5, #1
	beq	.L1192
	b	.L1230
.L1168:
	cmp	r5, #4
	beq	.L11f0
	cmp	r5, #4
	blt	.L11d0
	cmp	r5, #0x55
	beq	.L121c
	b	.L1230
.L1176:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x24
	bl	__CopyMapTiles
	b	.L1230
.L1192:
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x25
	b	.L1214
.L11b0:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x25
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x26
	b	.L1214
.L11d0:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x26
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x27
	b	.L1214
.L11f0:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x27
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x28
.L1214:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.L1230
.L121c:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x31
	mov	r2, #0x29
	mov	r3, #0x24
	bl	__CopyMapTiles
.L1230:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x5f
	cmp	r5, r3
	bls	.L124e
	ldr	r3, .L1258	@ 0
	strh	r3, [r6]
.L124e:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0

	.align	2, 0
.L1258:
	.word	0
	.pool
.func_end OvlFunc_890_2009140

@ 106 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   MoveCameraTo, CopyMapRectIndicesU x4, MoveCameraTo, UpdateMapView
@   PlaceSlotAt x2, WaitFrames, StartFadeOut, WaitForFade
@   SetSaveBit, ClearSaveBit x2, ShowScreenOverlay, WaitSceneDelay
@   DialogueWait, PlaySound
@   ... and 5 more
@ sets 0x201; clears 0x200, 0x202.
.thumb_func_start OvlFunc_890_2009264
	push	{r5, r6, lr}
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r0, r0
	neg	r1, r1
	neg	r2, r2
	mov	r3, #0
	sub	sp, #8
	bl	__Func_80933f8
	mov	r6, #8
	mov	r5, #3
	mov	r0, #0x1e
	mov	r1, #0x2b
	mov	r2, #0x20
	mov	r3, #0x28
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #1
	str	r3, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0x2b
	mov	r2, #0x21
	mov	r3, #0x27
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x1e
	mov	r1, #0x2b
	mov	r2, #0x24
	mov	r3, #0x26
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #4
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x29
	mov	r2, #0x20
	mov	r3, #0x29
	str	r6, [sp]
	bl	__CopyMapTiles
	mov	r1, #1
	mov	r2, #0x9e
	mov	r3, #0
	ldr	r0, =0x23e0000
	neg	r1, r1
	lsl	r2, #16
	bl	__Func_80933f8
	bl	__Func_800fe9c
	mov	r2, #0xf0
	mov	r0, #0x10
	ldr	r1, =0x23e0000
	lsl	r2, #15
	bl	__MapActor_SetPos
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0
	bl	__MapActor_SetPos
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #1
	ldr	r0, =0x2051cc
	bl	__Func_8091200
	mov	r0, #0x14
	bl	__Func_8091254
	ldr	r0, =0x201
	bl	__SetFlag
	mov	r0, #0x80
	lsl	r0, #2
	bl	__ClearFlag
	ldr	r0, =0x202
	bl	__ClearFlag
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x20
	str	r3, [r2]
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xab
	bl	__PlaySound
	mov	r1, #1
	ldr	r0, =0x10005
	bl	__Func_8091200
	mov	r0, #8
	bl	__Func_8091254
	mov	r0, #0x20
	bl	__CutsceneWait
	mov	r1, #1
	ldr	r0, =0x2051cc
	bl	__Func_8091200
	mov	r0, #0x18
	bl	__Func_8091254
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2009264

@ 140 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, OvlFunc_1264, SetActiveMessageId, TurnSlotToAngle
@   PlayInteractionEffect, SetSlotFacingAndScript, MoveCameraTo, WaitForCameraArrival
@   DialogueWait, OvlFunc_25fc, PlaySound, OvlFunc_238
@   DialogueWait, PlaySound
@   ... and 35 more
@ message id 0x1018; sets 0x813.
.thumb_func_start OvlFunc_890_2009380
	push	{r5, lr}
	bl	__CutsceneStart
	bl	OvlFunc_890_2009264
	ldr	r0, =0x1018
	bl	__MessageID
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #6
	mov	r2, #0x1e
	bl	__MapActor_Jump
	mov	r1, #1
	mov	r2, #0xae
	neg	r1, r1
	lsl	r2, #16
	mov	r3, #1
	ldr	r0, =0x23e0000
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x1e
	bl	__CutsceneWait
	ldr	r0, =0x8010
	mov	r1, #0x14
	bl	OvlFunc_890_200a5fc
	mov	r5, #0
.L13d6:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #0xc
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #0xc
	bl	__CutsceneWait
	cmp	r5, #4
	bne	.L13d6
	mov	r5, #0
.L13fe:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #8
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #8
	bl	__CutsceneWait
	cmp	r5, #6
	bne	.L13fe
	mov	r5, #0
.L1426:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #6
	bl	__CutsceneWait
	cmp	r5, #8
	bne	.L1426
	mov	r5, #0
.L144e:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #4
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #4
	bl	__CutsceneWait
	cmp	r5, #0xa
	bne	.L144e
	mov	r5, #0
.L1476:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #2
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #2
	bl	__CutsceneWait
	cmp	r5, #0xc
	bne	.L1476
	bl	OvlFunc_890_2008238
	mov	r0, #6
	bl	__CutsceneWait
	ldr	r0, =0x8010
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x10
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x90
	mov	r2, #0x8c
	mov	r0, #0x10
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_80921c4
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x20
	str	r3, [r2]
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x813
	bl	__SetFlag
	mov	r0, #3
	bl	__Func_8091e9c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2009380

@ 222 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, OvlFunc_1264, SetActiveMessageId, TurnSlotToAngle
@   PlayInteractionEffect, SetSlotFacingAndScript, MoveCameraTo, WaitForCameraArrival
@   DialogueWait, OvlFunc_25fc, PlaySound, OvlFunc_238
@   DialogueWait, PlaySound
@   ... and 38 more
@ message id 0x1001.
.thumb_func_start OvlFunc_890_2009510
	push	{r5, lr}
	sub	sp, #8
	bl	__CutsceneStart
	bl	OvlFunc_890_2009264
	ldr	r2, =0
	ldr	r3, =.L2de4
	strh	r2, [r3]
	ldr	r3, =.L2ddc
	strh	r2, [r3]
	ldr	r3, =.L2de0
	strh	r2, [r3]
	ldr	r3, =.L2dec
	ldr	r0, =0x1001
	strh	r2, [r3]
	bl	__MessageID
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #6
	mov	r2, #0x1e
	bl	__MapActor_Jump
	mov	r1, #1
	b	.L1574

	.pool_aligned

.L1574:
	mov	r2, #0xae
	neg	r1, r1
	lsl	r2, #16
	mov	r3, #1
	ldr	r0, =0x23e0000
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x1e
	bl	__CutsceneWait
	ldr	r0, =0x8010
	mov	r1, #0x14
	bl	OvlFunc_890_200a5fc
	mov	r5, #0
.L1596:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #0xc
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #0xc
	bl	__CutsceneWait
	cmp	r5, #4
	bne	.L1596
	mov	r1, #6
	ldr	r0, =0x8010
	bl	OvlFunc_890_200a5fc
	ldr	r5, =.L2de4
	bl	__Random
	lsl	r3, r0, #4
	sub	r3, r0
	lsl	r3, #2
	lsr	r3, #16
	add	r3, #0x14
	strh	r3, [r5]
	ldr	r5, =.L2ddc
	bl	__Random
	lsl	r3, r0, #4
	sub	r3, r0
	lsl	r3, #2
	lsr	r3, #16
	add	r3, #0x14
	strh	r3, [r5]
	ldr	r5, =.L2de0
	bl	__Random
	lsl	r3, r0, #4
	sub	r3, r0
	lsl	r3, #2
	lsr	r3, #16
	add	r3, #0x14
	strh	r3, [r5]
	ldr	r5, =.L2dec
	bl	__Random
	lsl	r3, r0, #4
	sub	r3, r0
	lsl	r3, #2
	lsr	r3, #16
	ldr	r2, =.L2de8
	add	r3, #0x14
	strh	r3, [r5]
	mov	r1, #0xc8
	mov	r3, #0
	str	r3, [r2]
	lsl	r1, #4
	ldr	r0, =OvlFunc_890_2008d9c
	bl	__StartTask
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_890_2008ef8
	bl	__StartTask
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_890_200901c
	bl	__StartTask
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_890_2009140
	lsl	r1, #4
	bl	__StartTask
	mov	r5, #0
.L163c:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #5
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #5
	bl	__CutsceneWait
	cmp	r5, #6
	bne	.L163c
	mov	r5, #0
.L1664:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #4
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #4
	bl	__CutsceneWait
	cmp	r5, #8
	bne	.L1664
	mov	r5, #0
.L168c:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #3
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #3
	bl	__CutsceneWait
	cmp	r5, #0xa
	bne	.L168c
	mov	r5, #0
.L16b4:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_2008238
	mov	r0, #2
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_2008360
	mov	r0, #2
	bl	__CutsceneWait
	cmp	r5, #0xc
	bne	.L16b4
	mov	r3, #4
	mov	r2, #2
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #0xa
	mov	r0, #0x2d
	mov	r1, #0x1e
	mov	r2, #0x22
	bl	__CopyMapTiles
	mov	r2, #0x28
	mov	r0, #0x10
	mov	r1, #6
	bl	__MapActor_Jump
	ldr	r0, =0x8010
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x10
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x90
	mov	r2, #0x8c
	lsl	r1, #2
	lsl	r2, #1
	mov	r0, #0x10
	bl	__Func_80921c4
	ldr	r0, =OvlFunc_890_2008d9c
	bl	__StopTask
	ldr	r0, =OvlFunc_890_2008ef8
	bl	__StopTask
	ldr	r0, =OvlFunc_890_200901c
	bl	__StopTask
	ldr	r0, =OvlFunc_890_2009140
	bl	__StopTask
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x20
	str	r3, [r2]
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #4
	bl	__Func_8091e9c
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2009510

@ 253 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, TestSaveBit x2, ClearSaveBit, CopyMapRectIndicesU
@   TestSaveBit x2, SetSaveBit, TestSaveBit x2, ClearSaveBit
@   CopyMapRectIndicesU, TestSaveBit x2, SetSaveBit, TestSaveBit x2
@   ClearSaveBit, CopyMapRectIndicesU
@   ... and 54 more
@ message id 0x1025; reads save bits 0x80b, 0x80c, 0x80d, 0x80e, 0x822; sets 0x826, 0x827, 0x828, 0x829; clears 0x826, 0x827, 0x828, 0x829.
.thumb_func_start OvlFunc_890_2009790
	push	{r5, lr}
	sub	sp, #8
	bl	__CutsceneStart
	ldr	r0, =0x80b
	bl	__GetFlag
	cmp	r0, #0
	beq	.L17c8
	ldr	r0, =0x826
	bl	__GetFlag
	cmp	r0, #0
	beq	.L17c8
	ldr	r0, =0x826
	bl	__ClearFlag
	mov	r3, #2
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2d
	mov	r1, #0x1c
	mov	r2, #0x22
	mov	r3, #0xa
	bl	__CopyMapTiles
	b	.L17e2
.L17c8:
	ldr	r0, =0x80b
	bl	__GetFlag
	cmp	r0, #0
	beq	.L17e2
	ldr	r0, =0x826
	bl	__GetFlag
	cmp	r0, #0
	bne	.L17e2
	ldr	r0, =0x826
	bl	__SetFlag
.L17e2:
	ldr	r0, =0x80c
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1812
	ldr	r0, =0x827
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1812
	ldr	r0, =0x827
	bl	__ClearFlag
	mov	r3, #2
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x1c
	mov	r2, #0x24
	mov	r3, #0xa
	bl	__CopyMapTiles
	b	.L182c
.L1812:
	ldr	r0, =0x80c
	bl	__GetFlag
	cmp	r0, #0
	beq	.L182c
	ldr	r0, =0x827
	bl	__GetFlag
	cmp	r0, #0
	bne	.L182c
	ldr	r0, =0x827
	bl	__SetFlag
.L182c:
	ldr	r0, =0x80d
	bl	__GetFlag
	cmp	r0, #0
	beq	.L185c
	ldr	r0, =0x828
	bl	__GetFlag
	cmp	r0, #0
	beq	.L185c
	ldr	r0, =0x828
	bl	__ClearFlag
	mov	r3, #2
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2d
	mov	r1, #0x1d
	mov	r2, #0x22
	mov	r3, #0xb
	bl	__CopyMapTiles
	b	.L1876
.L185c:
	ldr	r0, =0x80d
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1876
	ldr	r0, =0x828
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1876
	ldr	r0, =0x828
	bl	__SetFlag
.L1876:
	ldr	r0, =0x80e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18a6
	ldr	r0, =0x829
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18a6
	ldr	r0, =0x829
	bl	__ClearFlag
	mov	r3, #2
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x1d
	mov	r2, #0x24
	mov	r3, #0xb
	bl	__CopyMapTiles
	b	.L18c0
.L18a6:
	ldr	r0, =0x80e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18c0
	ldr	r0, =0x829
	bl	__GetFlag
	cmp	r0, #0
	bne	.L18c0
	ldr	r0, =0x829
	bl	__SetFlag
.L18c0:
	bl	OvlFunc_890_2009264
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r0, #0x10
	mov	r1, #6
	mov	r2, #0x1e
	bl	__MapActor_Jump
	mov	r1, #1
	mov	r2, #0xae
	ldr	r0, =0x23e0000
	neg	r1, r1
	lsl	r2, #16
	mov	r3, #1
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r5, #0
.L18f6:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_20082cc
	mov	r0, #0xc
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_20083f4
	mov	r0, #0xc
	bl	__CutsceneWait
	cmp	r5, #4
	bne	.L18f6
	mov	r5, #0
.L191e:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_20082cc
	mov	r0, #8
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_20083f4
	mov	r0, #8
	bl	__CutsceneWait
	cmp	r5, #6
	bne	.L191e
	mov	r5, #0
.L1946:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_20082cc
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_20083f4
	mov	r0, #6
	bl	__CutsceneWait
	cmp	r5, #8
	bne	.L1946
	mov	r5, #0
.L196e:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_20082cc
	mov	r0, #4
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_20083f4
	mov	r0, #4
	bl	__CutsceneWait
	cmp	r5, #0xa
	bne	.L196e
	mov	r5, #0
.L1996:
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_20082cc
	mov	r0, #2
	bl	__CutsceneWait
	mov	r0, #0xf6
	bl	__PlaySound
	add	r5, #1
	bl	OvlFunc_890_20083f4
	mov	r0, #2
	bl	__CutsceneWait
	cmp	r5, #0xc
	bne	.L1996
	mov	r0, #0xf6
	bl	__PlaySound
	bl	OvlFunc_890_20082cc
	mov	r0, #6
	bl	__CutsceneWait
	ldr	r0, =0x822
	bl	__GetFlag
	cmp	r0, #0
	bne	.L19f6
	ldr	r5, =0x8010
	ldr	r0, =0x1025
	bl	__MessageID
	mov	r0, r5
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r0, #0x10
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, r5
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
.L19f6:
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x20
	str	r3, [r2]
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #5
	bl	__Func_8091e9c
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2009790

	.include "macros.inc"

@ Cutscene: roughly 442 instructions of straight-line script --
@ 4 turns, 10 animation changes, 1 dialogue line, 11 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x102e.
@ Reads save bits 0x811, 0x819.
@ Sets save bit 0x819.
.thumb_func_start OvlFunc_890_2009ca8
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r0, =0x811
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1cbc
	b	.L20fc
.L1cbc:
	bl	__CutsceneStart
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #9
	lsl	r1, #6
	bl	__Func_80933d4
	mov	r1, #1
	mov	r2, #0x94
	mov	r3, #1
	ldr	r0, =0x11f0000
	neg	r1, r1
	lsl	r2, #16
	bl	__Func_80933f8
	mov	r1, #0x90
	mov	r2, #0x78
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #0
	bl	__MapActor_SetAnim
	mov	r0, #0
	mov	r1, #4
	mov	r2, #0x1e
	bl	__MapActor_Jump
	mov	r1, #0x90
	mov	r2, #0xf0
	mov	r0, #0x10
	lsl	r1, #17
	lsl	r2, #15
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x10
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x8a
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0x88
	bl	__Func_80921c4
	mov	r1, #0x84
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0x88
	bl	__Func_809218c
	mov	r1, #0x9c
	mov	r2, #0x88
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r0, #0x10
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0xb0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xd0
	mov	r0, #0x10
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	ldr	r0, =0x819
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1d72
	mov	r0, #0xdc
	bl	__PlaySound
.L1d72:
	mov	r0, #0x28
	bl	__CutsceneWait
	ldr	r0, =0x819
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1d84
	b	.L1eb2
.L1d84:
	mov	r2, #3
	str	r2, [sp, #4]
	mov	r5, #2
	mov	r8, r2
	mov	r0, #0x24
	mov	r1, #0x3e
	mov	r2, #0x11
	mov	r3, #0x24
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r6, #1
	mov	r1, #0x3b
	mov	r2, #0x11
	mov	r3, #0x26
	mov	r0, #0x2c
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r0, #0x26
	mov	r1, #0x3e
	mov	r2, #0x11
	mov	r3, #0x24
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r1, #0x3b
	mov	r2, #0x11
	mov	r3, #0x27
	mov	r0, #0x2c
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r2, r8
	str	r2, [sp, #4]
	mov	r0, #0x28
	mov	r1, #0x3e
	mov	r2, #0x11
	mov	r3, #0x24
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x27
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #0x28
	mov	r1, #0x3b
	mov	r2, #0x11
	mov	r0, #0x2c
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r0, #0x2a
	mov	r1, #0x3e
	mov	r2, #0x11
	mov	r3, #0x24
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x28
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r1, #0x3b
	mov	r2, #0x11
	mov	r3, #0x29
	mov	r0, #0x2c
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x29
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r1, #0x3b
	mov	r2, #0x11
	mov	r3, #0x2a
	mov	r0, #0x2c
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r2, r8
	str	r2, [sp, #4]
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x2a
	mov	r0, #0
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r0, #9
	bl	__Func_80118a8
	mov	r0, #0xa
	bl	__Func_80118a8
	ldr	r0, =0x819
	bl	__SetFlag
.L1eb2:
	mov	r2, #0x1e
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_8092848
	mov	r0, #0x10
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x10
	mov	r1, #1
	bl	__MapActor_SetAnim
	ldr	r5, =0x8010
	mov	r1, #0
	mov	r0, #0
	bl	__MapActor_SetAnim
	ldr	r0, =0x102e
	bl	__MessageID
	mov	r0, r5
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r0, #0x10
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x10
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r1, =0x1333
	mov	r6, r0
	ldr	r0, =0x9999
	bl	__Func_80933d4
	mov	r1, #1
	mov	r2, #0xe4
	mov	r3, #1
	ldr	r0, =0x11f0000
	neg	r1, r1
	lsl	r2, #15
	bl	__Func_80933f8
	mov	r1, #0x90
	lsl	r1, #1
	mov	r2, #0x78
	mov	r0, #0
	bl	__Func_80921c4
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r0, #0
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__MapActor_SetSpeed
	mov	r1, r6
	add	r1, #0x5a
	ldrb	r2, [r1]
	mov	r3, #0xfe
	and	r3, r2
	strb	r3, [r1]
	mov	r3, r6
	add	r3, #0x55
	mov	r7, #0
	strb	r7, [r3]
	mov	r0, #0xc9
	bl	__PlaySound
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_8092950
	ldr	r7, [r6, #0x50]
	mov	r3, r7
	mov	r5, #0
	add	r3, #0x26
	strb	r5, [r3]
.L1f88:
	ldr	r3, [r6, #0xc]
	ldr	r2, =0x3333
	add	r3, r2
	str	r3, [r6, #0xc]
	mov	r0, #1
	add	r5, #1
	bl	__WaitFrames
	cmp	r5, #0x78
	bne	.L1f88
	mov	r0, #0xbe
	bl	__PlaySound
	mov	r5, #0
.L1fa4:
	ldr	r3, [r6, #0xc]
	ldr	r2, =0x1999
	add	r3, r2
	str	r3, [r6, #0xc]
	ldr	r3, [r7, #0x18]
	ldr	r2, =0xfffffc00
	add	r3, r2
	str	r3, [r7, #0x18]
	mov	r0, #1
	add	r5, #1
	bl	__WaitFrames
	cmp	r5, #0x3c
	bne	.L1fa4
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, #0x14
	mov	r0, #0x10
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r0, #0x10
	mov	r1, #6
	bl	OvlFunc_890_200a5fc
	mov	r1, #0x90
	mov	r2, #0x78
	mov	r0, #0x10
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r1, #2
	mov	r0, #0x10
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r0, #0x10
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	mov	r0, #0x10
	bl	__MapActor_SetSpeed
	mov	r0, #0x10
	bl	__MapActor_GetActor
	mov	r6, r0
	mov	r1, r6
	add	r1, #0x5a
	ldrb	r3, [r1]
	mov	r2, #0xfe
	and	r2, r3
	mov	r3, r6
	add	r3, #0x55
	mov	r5, #0
	strb	r2, [r1]
	mov	r0, #0xc9
	strb	r5, [r3]
	bl	__PlaySound
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #1
	bl	__Func_8092950
	ldr	r7, [r6, #0x50]
	mov	r3, r7
	add	r3, #0x26
	strb	r5, [r3]
.L2040:
	ldr	r3, [r6, #0xc]
	ldr	r2, =0x3333
	add	r3, r2
	str	r3, [r6, #0xc]
	mov	r0, #1
	add	r5, #1
	bl	__WaitFrames
	cmp	r5, #0x78
	bne	.L2040
	mov	r0, #0xbe
	bl	__PlaySound
	mov	r5, #0
.L205c:
	ldr	r3, [r6, #0xc]
	ldr	r2, =0x1999
	add	r3, r2
	str	r3, [r6, #0xc]
	ldr	r3, [r7, #0x18]
	ldr	r2, =0xfffffc00
	add	r3, r2
	str	r3, [r7, #0x18]
	mov	r0, #1
	add	r5, #1
	bl	__WaitFrames
	cmp	r5, #0x3c
	bne	.L205c
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0x10
	bl	__MapActor_SetPos
	mov	r0, #0x50
	bl	__CutsceneWait
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	b	.L20c8

	.pool_aligned

.L20c8:
	add	r2, r1, r3
	add	r3, #0x43
	str	r3, [r2]
	sub	r3, #0x3b
	add	r2, r1, r3
	mov	r3, #0x18
	str	r3, [r2]
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r1, #0
	mov	r0, #0
	bl	__Func_8091200
	mov	r0, #1
	bl	__Func_8091254
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #7
	bl	__Func_8091e9c
	bl	__CutsceneEnd
.L20fc:
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2009ca8

@ 454 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   WaitFrames, TestSaveBit x3, SetSaveBit, StartFadeIn
@   TestSaveBit x3, StartFadeOut, WaitForFade, WaitFrames
@   SetSaveBit, TestSaveBit, PlaceSlotAt, TestSaveBit
@   OvlFunc_1380, SetSaveBit
@   ... and 42 more
@ reads save bits 0x109, 0x201, 0x202, 0x309, 0x809; sets 0x144, 0x200, 0x309, 0x812, 0x813; clears 0x80b, 0x80c, 0x80d, 0x80e.
.thumb_func_start OvlFunc_890_200a108
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #1
	sub	sp, #8
	bl	__WaitFrames
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x44
	str	r2, [r3]
	ldr	r0, =0x809
	mov	r5, #0
	bl	__GetFlag
	cmp	r0, #0
	beq	.L214c
	ldr	r0, =0x814
	bl	__GetFlag
	cmp	r0, #0
	bne	.L214c
	ldr	r0, =0x819
	bl	__GetFlag
	cmp	r0, #0
	bne	.L214c
	mov	r0, #0xa2
	lsl	r0, #1
	bl	__SetFlag
.L214c:
	mov	r0, #0x80
	lsl	r0, #9
	mov	r1, #0
	bl	__Func_8091220
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L218e
	ldr	r0, =0x201
	bl	__GetFlag
	cmp	r0, #0
	beq	.L216e
	ldr	r0, =0x2051cc
	b	.L217a
.L216e:
	ldr	r0, =0x202
	bl	__GetFlag
	cmp	r0, #0
	beq	.L21ae
	ldr	r0, =0x202db1
.L217a:
	mov	r1, #1
	bl	__Func_8091200
	mov	r0, #1
	bl	__Func_8091254
	mov	r0, #1
	bl	__WaitFrames
	b	.L21ae
.L218e:
	mov	r0, #0x80
	lsl	r0, #2
	bl	__SetFlag
	ldr	r0, =0x80a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L21ae
	mov	r1, #0x90
	mov	r2, #0xf0
	mov	r0, #0x10
	lsl	r1, #18
	lsl	r2, #15
	bl	__MapActor_SetPos
.L21ae:
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #4
	bne	.L21d4
	ldr	r0, =0x813
	bl	__GetFlag
	cmp	r0, #0
	bne	.L2220
	bl	OvlFunc_890_2009380
	ldr	r0, =0x813
	bl	__SetFlag
	b	.L221e
.L21d4:
	cmp	r3, #5
	bne	.L2206
	ldr	r0, =0x812
	bl	__GetFlag
	cmp	r0, #0
	bne	.L2220
	bl	OvlFunc_890_2009510
	ldr	r0, =0x812
	bl	__SetFlag
	ldr	r0, =0x80b
	bl	__ClearFlag
	ldr	r0, =0x80c
	bl	__ClearFlag
	ldr	r0, =0x80d
	bl	__ClearFlag
	ldr	r0, =0x80e
	bl	__ClearFlag
	b	.L221e
.L2206:
	cmp	r3, #6
	bne	.L2220
	ldr	r0, =0x812
	bl	__GetFlag
	cmp	r0, #0
	beq	.L221e
	bl	OvlFunc_890_2009790
	ldr	r0, =0x822
	bl	__SetFlag
.L221e:
	mov	r5, #1
.L2220:
	ldr	r0, =0x80b
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2230
	ldr	r0, =0x826
	bl	__SetFlag
.L2230:
	ldr	r0, =0x80c
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2240
	ldr	r0, =0x827
	bl	__SetFlag
.L2240:
	ldr	r0, =0x80d
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2250
	ldr	r0, =0x828
	bl	__SetFlag
.L2250:
	ldr	r0, =0x80e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2260
	ldr	r0, =0x829
	bl	__SetFlag
.L2260:
	mov	r0, #4
	bl	__WaitFrames
	cmp	r5, #0
	beq	.L226c
	b	.L2510
.L226c:
	bl	OvlFunc_890_200a5b0
	cmp	r0, #0
	bne	.L2276
	b	.L23e4
.L2276:
	mov	r3, #0xc
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0x2c
	mov	r2, #0x1e
	mov	r3, #0x26
	bl	__CopyMapTiles
	mov	r3, #1
	str	r3, [sp, #4]
	mov	r5, #4
	mov	r0, #0x1e
	mov	r1, #0x2c
	mov	r2, #0x22
	mov	r8, r3
	mov	r3, #0x25
	str	r5, [sp]
	bl	__CopyMapTiles
	mov	r6, #8
	mov	r0, #0xe
	mov	r1, #0x29
	mov	r2, #0x20
	mov	r3, #0x29
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r7, #2
	mov	r0, #0x2d
	mov	r1, #0x1c
	mov	r2, #0x22
	mov	r3, #0xa
	str	r5, [sp]
	str	r7, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x2d
	mov	r1, #0x1e
	mov	r2, #0x10
	mov	r3, #0xa
	str	r5, [sp]
	str	r7, [sp, #4]
	bl	__CopyMapTiles
	mov	r2, #0xe
	mov	r3, #0x29
	mov	r0, #0xe
	mov	r1, #0x2d
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #8
	beq	.L2380
	ldr	r0, =0x814
	bl	__GetFlag
	cmp	r0, #0
	bne	.L2380
	ldr	r0, =0x819
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2362
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x27
	str	r7, [sp]
	bl	__CopyMapTiles
	mov	r5, #3
	mov	r0, #0x2a
	mov	r1, #0x3e
	mov	r2, #0x11
	mov	r3, #0x24
	str	r7, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r2, r8
	str	r2, [sp, #4]
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x28
	str	r7, [sp]
	bl	__CopyMapTiles
	mov	r3, r8
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x29
	str	r7, [sp]
	bl	__CopyMapTiles
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x11
	mov	r3, #0x2a
	str	r7, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	b	.L2374
.L2362:
	mov	r3, #6
	str	r3, [sp, #4]
	mov	r0, #0x2c
	mov	r1, #0x3b
	mov	r2, #0x11
	mov	r3, #0x25
	str	r7, [sp]
	bl	__CopyMapTiles
.L2374:
	mov	r0, #9
	bl	__Func_80118a8
	mov	r0, #0xa
	bl	__Func_80118a8
.L2380:
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	b	.L2510

	.pool_aligned

.L23e4:
	ldr	r0, =0x80b
	mov	r5, #0
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2416
	mov	r5, #2
	mov	r6, #1
	mov	r0, #0x2d
	mov	r1, #0x1c
	mov	r2, #0x22
	mov	r3, #0xa
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x2d
	mov	r1, #0x1e
	mov	r2, #0x10
	mov	r3, #0xa
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r5, #1
.L2416:
	ldr	r0, =0x80c
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2446
	mov	r5, #2
	mov	r6, #1
	mov	r0, #0x2f
	mov	r1, #0x1c
	mov	r2, #0x24
	mov	r3, #0xa
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x2f
	mov	r1, #0x1e
	mov	r2, #0x12
	mov	r3, #0xa
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r5, #1
.L2446:
	ldr	r0, =0x80d
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2476
	mov	r5, #2
	mov	r6, #1
	mov	r0, #0x2d
	mov	r1, #0x1d
	mov	r2, #0x22
	mov	r3, #0xb
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x2d
	mov	r1, #0x1f
	mov	r2, #0x10
	mov	r3, #0xb
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r5, #1
.L2476:
	ldr	r0, =0x80e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L24a6
	mov	r5, #2
	mov	r6, #1
	mov	r0, #0x2f
	mov	r1, #0x1d
	mov	r2, #0x24
	mov	r3, #0xb
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #0x2f
	mov	r1, #0x1f
	mov	r2, #0x12
	mov	r3, #0xb
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__CopyMapTiles
	mov	r5, #1
.L24a6:
	ldr	r0, =0x812
	bl	__GetFlag
	cmp	r0, #0
	bne	.L24b4
	cmp	r5, #0
	beq	.L24fc
.L24b4:
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
	mov	r0, #0x24
	mov	r1, #0x3a
	mov	r2, #0x20
	mov	r3, #0x29
	str	r6, [sp]
	bl	__CopyMapTiles
.L24fc:
	mov	r3, #0x11
	mov	r2, #6
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xf
	mov	r1, #6
	mov	r2, #2
	mov	r3, #1
	bl	__Func_8010704
.L2510:
	ldr	r0, =0x309
	bl	__GetFlag
	cmp	r0, #0
	bne	.L254a
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #8
	bne	.L254a
	bl	OvlFunc_890_200a614
	ldr	r0, =0x309
	bl	__SetFlag
	mov	r3, #0x11
	mov	r2, #6
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xf
	mov	r1, #6
	mov	r2, #2
	mov	r3, #1
	bl	__Func_8010704
	b	.L2582
.L254a:
	ldr	r0, =0x814
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2582
	mov	r0, #0x8d
	bl	__Func_8091ff0
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #9
	lsl	r1, #9
	lsl	r2, #9
	bl	__Func_8012330
	bl	__StartEarthquake
	mov	r3, #0x11
	mov	r2, #6
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xf
	mov	r1, #6
	mov	r2, #2
	mov	r3, #1
	bl	__Func_8010704
.L2582:
	mov	r0, #0
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_890_200a108

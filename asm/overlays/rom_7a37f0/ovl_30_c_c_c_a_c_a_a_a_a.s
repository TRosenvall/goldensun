	.include "macros.inc"
	.include "gba.inc"


@ 412 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetCameraSpeed, MoveCameraTo, WaitForCameraArrival
@   RunTextBoxModal, PlaySound, PlaceSlotAt, CopyMapRectFull
@   WaitFrames, CopyMapRectFull, WaitFrames, CopyMapRectFull
@   WaitFrames, CopyMapRectFull
@   ... and 46 more
.thumb_func_start OvlFunc_916_20083f0
	push	{r5, r6, lr}
	sub	sp, #0xc
	bl	__CutsceneStart
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #9
	lsl	r1, #6
	bl	__Func_80933d4
	mov	r0, #0x84
	mov	r1, #1
	mov	r2, #0xe0
	lsl	r2, #17
	mov	r3, #1
	lsl	r0, #17
	neg	r1, r1
	bl	__Func_80933f8
	bl	__Func_8093530
	ldr	r0, =0x1528
	mov	r1, #1
	bl	__Func_801776c
	mov	r0, #0xe8
	bl	__PlaySound
	ldr	r3, =.L12c8
	ldr	r3, [r3]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	beq	.L436
	b	.L556
.L436:
	mov	r1, #0x80
	mov	r2, #0xe7
	mov	r0, #9
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r6, #0x19
	mov	r5, #0x53
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x4d
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #3
	bl	__WaitFrames
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x4e
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #3
	bl	__WaitFrames
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x4f
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #0x1e
	bl	__WaitFrames
	mov	r5, #0x4f
	mov	r1, #0x22
	mov	r2, #2
	mov	r3, #5
	mov	r0, #0x43
	str	r6, [sp, #4]
	str	r5, [sp]
	bl	__Func_80105d4
	mov	r0, #6
	bl	__WaitFrames
	mov	r2, #2
	mov	r3, #5
	mov	r0, #0x45
	mov	r1, #0x22
	str	r6, [sp, #4]
	str	r5, [sp]
	bl	__Func_80105d4
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #0xf0
	bl	__PlaySound
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #0x22
	mov	r2, #2
	mov	r3, #5
	mov	r0, #0x47
	str	r6, [sp, #4]
	str	r5, [sp]
	bl	__Func_80105d4
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #0x49
	mov	r1, #0x22
	mov	r2, #2
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp]
	bl	__Func_80105d4
	mov	r6, #0x1d
	mov	r1, #0x26
	mov	r2, #2
	mov	r3, #1
	mov	r0, #0x4b
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #4
	bl	__WaitFrames
	mov	r1, #0x26
	mov	r2, #2
	mov	r3, #1
	mov	r0, #0x4d
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #0x26
	mov	r2, #2
	mov	r3, #1
	mov	r0, #0x4f
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #8
	bl	__WaitFrames
	mov	r0, #0x41
	mov	r1, #0x35
	mov	r2, #2
	mov	r3, #1
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0xf
	mov	r2, #0x1c
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x41
	mov	r1, #0x28
	mov	r2, #2
	mov	r3, #4
	bl	__Func_80105d4
	b	.L626
.L556:
	mov	r1, #0x80
	mov	r2, #0xf0
	mov	r0, #9
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r6, #0x19
	mov	r5, #0x53
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x4e
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #3
	bl	__WaitFrames
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x4d
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #3
	bl	__WaitFrames
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0x4c
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #0x1e
	bl	__WaitFrames
	mov	r3, #0xf
	mov	r2, #0x1c
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x41
	mov	r1, #0x2d
	mov	r2, #2
	mov	r3, #4
	bl	__Func_80105d4
	mov	r5, #0x4f
	mov	r2, #2
	mov	r3, #5
	mov	r0, #0x47
	mov	r1, #0x32
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r1, #2
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #0xe6
	bl	__PlaySound
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #0x32
	mov	r2, #2
	mov	r3, #5
	mov	r0, #0x45
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #0x32
	mov	r2, #2
	mov	r3, #5
	mov	r0, #0x43
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #0x41
	mov	r1, #0x32
	mov	r2, #2
	mov	r3, #5
	str	r5, [sp]
	str	r6, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #0x1e
	bl	__WaitFrames
.L626:
	ldr	r3, =.L12c8
	ldr	r3, [r3]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	bne	.L672
	str	r3, [sp]
	mov	r6, #9
	mov	r5, #0x1e
	mov	r0, #9
	mov	r1, #0x13
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #1
	str	r3, [sp]
	mov	r0, #9
	mov	r1, #0x33
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x29
	mov	r1, #0x33
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	b	.L6b2
.L672:
	mov	r3, #0
	str	r3, [sp]
	mov	r6, #9
	mov	r5, #0x1e
	mov	r0, #9
	mov	r1, #0x13
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #1
	str	r3, [sp]
	mov	r0, #9
	mov	r1, #0x53
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x29
	mov	r1, #0x53
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
.L6b2:
	ldr	r5, =.L20dc
	mov	r6, #0
	mov	r1, #0xc8
	lsl	r1, #4
	str	r6, [r5]
	ldr	r0, =OvlFunc_916_20083c0
	bl	__StartTask
	mov	r0, #1
	bl	__WaitFrames
	ldr	r2, =OvlFunc_916_200836c
	mov	r0, #1
	mov	r1, #0
	bl	__SetIntrHandler
	mov	r0, #0xe7
	bl	__PlaySound
	str	r6, [r5]
.L6da:
	mov	r0, #1
	bl	__WaitFrames
	ldr	r3, [r5]
	add	r3, #1
	str	r3, [r5]
	cmp	r3, #0x64
	ble	.L6da
	ldr	r0, =0x121
	bl	__PlaySound
	ldr	r3, =.L12c8
	ldr	r3, [r3]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	bne	.L754
	str	r3, [sp]
	mov	r6, #9
	mov	r5, #0x13
	mov	r0, #9
	mov	r1, #0x13
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #1
	str	r3, [sp]
	mov	r0, #9
	mov	r1, #0x33
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x29
	mov	r1, #0x33
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	b	.L794

	.pool_aligned

.L754:
	mov	r3, #0
	str	r3, [sp]
	mov	r6, #9
	mov	r5, #0x13
	mov	r0, #9
	mov	r1, #0x13
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #1
	str	r3, [sp]
	mov	r0, #9
	mov	r1, #0x53
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
	mov	r3, #2
	str	r3, [sp]
	mov	r0, #0x29
	mov	r1, #0x53
	mov	r2, #0x10
	mov	r3, #5
	str	r6, [sp, #4]
	str	r5, [sp, #8]
	bl	OvlFunc_916_2008098
.L794:
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0
	mov	r2, #0
	mov	r0, #1
	bl	__SetIntrHandler
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =OvlFunc_916_20083c0
	bl	__StopTask
	ldr	r3, =.L12c8
	ldr	r1, [r3]
	ldr	r2, .L7cc	@ 1
	ldrh	r3, [r1]
	eor	r3, r2
	strh	r3, [r1]
	bl	OvlFunc_916_2008194
	bl	__Func_800fe9c
	bl	__CutsceneEnd
	add	sp, #0xc
	b	.L7d8

	.align	2, 0
.L7cc:
	.word	1
	.pool

.L7d8:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_916_20083f0

@ Cutscene: roughly 76 instructions of straight-line script --
@ 0 turns, 5 animation changes, 0 dialogue lines, 3 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_916_20087e0
	push	{r5, lr}
	sub	sp, #8
	bl	__CutsceneStart
	mov	r1, #8
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, #0xef
	bl	__PlaySound
	mov	r1, #0x80
	ldr	r2, =0x3333
	mov	r0, #8
	lsl	r1, #8
	bl	__MapActor_SetSpeed
	mov	r0, #8
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r2, #0xb0
	mov	r1, #0x48
	mov	r0, #8
	bl	__MapActor_TravelTo
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	ldr	r1, =0x4ccc
	ldr	r2, =0x3333
	bl	__MapActor_SetSpeed
	mov	r1, #8
	mov	r2, #0
	neg	r1, r1
	mov	r0, #0
	bl	__Func_809228c
	mov	r0, #0x18
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #8
	bl	__MapActor_WaitMovement
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #0x90
	lsl	r0, #1
	bl	__PlaySound
	mov	r0, #0xd5
	bl	__PlaySound
	mov	r3, #6
	str	r3, [sp]
	mov	r5, #9
	mov	r0, #5
	mov	r1, #9
	mov	r2, #1
	mov	r3, #4
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #4
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	str	r3, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
	ldr	r3, =.L12c4
	ldr	r2, [r3]
	ldr	r3, .L8a0	@ 1
	strh	r3, [r2]
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0

	.align	2, 0
.L8a0:
	.word	1
.func_end OvlFunc_916_20087e0

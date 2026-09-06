	.include "macros.inc"

@ 184 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_1054, GetSlotEntityChecked, CheckTerrainStep, Random x3
@   OvlFunc_common0_10c, RunSlotEffectSequence, SetEntityMoveTarget, SetEntityAnimation
@   WaitForEntityIdle, WaitFrames, SetEntityAnimation, WaitFrames
@   CheckTerrainStep x3
.thumb_func_start OvlFunc_933_20092fc
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x48
	bl	OvlFunc_933_2009054
	ldr	r3, =iwram_3001ebc
	mov	r0, #0xfa
	ldr	r2, [r3]
	ldr	r3, =gState
	lsl	r0, #1
	add	r3, r0
	mov	r1, #0xf0
	ldr	r3, [r3]
	lsl	r1, #1
	add	r2, r1
	mov	r11, r3
	ldr	r2, [r2]
	mov	r0, r11
	str	r2, [sp, #0x10]
	bl	__MapActor_GetActor
	mov	r7, r0
	ldr	r3, [r7, #8]
	add	r6, sp, #0x3c
	str	r3, [r6]
	ldr	r3, [r7, #0xc]
	str	r3, [r6, #4]
	ldr	r3, [r7, #0x10]
	mov	r2, #0xc0
	lsl	r2, #9
	add	r3, r2
	mov	r1, r6
	str	r3, [r6, #8]
	bl	__TestCollision
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r10, r3
	mov	r8, r0
	mov	r3, #4
	mov	r0, r10
	and	r0, r3
	mov	r10, r0
	cmp	r0, #0
	bne	.L13ba
	bl	__Random
	add	r1, sp, #0x14
	lsl	r0, #12
	mov	r2, #0xf8
	mov	r9, r1
	lsl	r2, #8
	lsr	r0, #16
	add	r0, r2
	mov	r3, r9
	strh	r0, [r3, #0x22]
	bl	__Random
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #2
	ldr	r5, [r7, #8]
	lsr	r3, #16
	lsl	r3, #16
	ldr	r0, =0xfffa0000
	add	r5, r3
	add	r5, r0
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	ldr	r2, =0x1999
	lsr	r3, #16
	mul	r3, r2
	ldr	r1, =0x7ffd
	add	r3, r1
	ldr	r2, [r7, #0x10]
	ldr	r1, [r7, #0xc]
	str	r3, [sp, #4]
	mov	r3, #0x80
	lsl	r3, #16
	mov	r0, r10
	str	r3, [sp, #8]
	mov	r3, r9
	str	r0, [sp]
	str	r3, [sp, #0xc]
	mov	r0, r5
	mov	r3, #0
	bl	OvlFunc_common0_10c
.L13ba:
	mov	r0, r8
	cmp	r0, #0
	bge	.L1408
	mov	r1, #0x81
	mov	r0, r11
	lsl	r1, #1
	bl	__MapActor_Surprise
	ldr	r3, [r7, #0x10]
	mov	r0, #0x80
	lsl	r0, #12
	add	r3, r0
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0xc]
	mov	r0, r7
	bl	__Actor_TravelTo
	mov	r0, r7
	mov	r1, #7
	bl	__Actor_SetAnim
	mov	r0, r7
	bl	__Actor_WaitMovement
.L13ea:
	mov	r0, #1
	bl	__WaitFrames
	ldr	r2, [r7, #0xc]
	ldr	r3, [r7, #0x14]
	cmp	r2, r3
	bne	.L13ea
	mov	r0, r7
	mov	r1, #6
	bl	__Actor_SetAnim
	mov	r0, #3
	bl	__WaitFrames
	b	.L147c
.L1408:
	ldr	r3, [r7, #8]
	str	r3, [r6]
	ldr	r3, [r7, #0xc]
	str	r3, [r6, #4]
	ldr	r3, [r7, #0x10]
	mov	r1, #0x80
	lsl	r1, #12
	add	r3, r1
	mov	r0, r7
	mov	r1, r6
	str	r3, [r6, #8]
	bl	__TestCollision
	mov	r8, r0
	cmp	r0, #0
	bgt	.L147c
	ldr	r3, [r7, #8]
	ldr	r2, =0x5b333
	add	r3, r2
	str	r3, [r6]
	ldr	r3, [r7, #0xc]
	str	r3, [r6, #4]
	ldr	r3, [r7, #0x10]
	mov	r0, r7
	add	r3, r2
	mov	r1, r6
	str	r3, [r6, #8]
	bl	__TestCollision
	mov	r8, r0
	cmp	r0, #0
	bgt	.L147c
	ldr	r3, [r7, #8]
	ldr	r2, =0xfffa4ccd
	add	r3, r2
	str	r3, [r6]
	ldr	r3, [r7, #0xc]
	str	r3, [r6, #4]
	ldr	r3, [r7, #0x10]
	ldr	r0, =0x5b333
	mov	r1, r6
	add	r3, r0
	mov	r0, r7
	str	r3, [r6, #8]
	bl	__TestCollision
	mov	r8, r0
	cmp	r0, #0
	bgt	.L147c
	ldr	r1, [sp, #0x10]
	mov	r2, #0xc0
	ldr	r3, [r1, #0x10]
	lsl	r2, #9
	add	r3, r2
	str	r3, [r1, #0x10]
	ldr	r3, [r7, #0x10]
	add	r3, r2
	str	r3, [r7, #0x10]
.L147c:
	add	sp, #0x48
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_933_20092fc

@ Cutscene: roughly 153 instructions of straight-line script --
@ 0 turns, 2 animation changes, 0 dialogue lines, 5 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x200, 0x90a.
@ Sets save bit 0x200.
.thumb_func_start OvlFunc_933_20094b0
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001ebc
	sub	sp, #0xc
	ldr	r5, [r3]
	bl	OvlFunc_933_2009054
	ldr	r0, =0x90a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L14cc
	b	.L1618
.L14cc:
	mov	r0, #0x80
	lsl	r0, #2
	bl	__GetFlag
	mov	r6, r0
	cmp	r6, #0
	beq	.L14dc
	b	.L1618
.L14dc:
	mov	r0, #0x80
	lsl	r0, #2
	bl	__SetFlag
	mov	r0, #1
	bl	OvlFunc_933_2009c78
	ldr	r3, =0xcba
	add	r7, r5, r3
	mov	r3, #0x96
	lsl	r3, #2
	mov	r8, r3
	mov	r3, r8
	strh	r3, [r7]
	mov	r0, #0
	bl	__MapActor_GetActor
	str	r6, [r0, #0x24]
	mov	r0, #0
	bl	__MapActor_GetActor
	str	r6, [r0, #0x2c]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, #0x80
	lsl	r5, #24
	str	r5, [r0, #0x38]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #1
	str	r5, [r0, #0x40]
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r2, #0
	mov	r1, #8
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #2
	bl	__Func_809259c
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0
	bl	__MapActor_Surprise
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #0xfe
	mov	r5, sp
	and	r3, r2
	strb	r3, [r0]
	str	r6, [r5]
	str	r6, [r5, #4]
	str	r6, [r5, #8]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, r5
	ldrh	r1, [r0, #6]
	ldr	r0, =0xfff00000
	bl	__vec3_translate
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r1, #2
	mov	r0, #0
	bl	__MapActor_SetAnim
	ldr	r1, [r5]
	cmp	r1, #0
	bge	.L1594
	ldr	r3, =0xffff
	add	r1, r3
.L1594:
	ldr	r2, [r5, #8]
	asr	r1, #16
	cmp	r2, #0
	bge	.L15a0
	ldr	r3, =0xffff
	add	r2, r3
.L15a0:
	asr	r2, #16
	mov	r0, #0
	bl	__Func_809228c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r0, #2
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	add	r0, #0x5a
	ldrb	r3, [r0]
	mov	r2, #1
	orr	r3, r2
	strb	r3, [r0]
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0x94
	bl	__PlaySound
	mov	r1, #2
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xa0
	mov	r2, #0xa0
	mov	r0, #8
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r0, #8
	mov	r1, #0xa8
	mov	r2, #0x68
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #8
	lsl	r1, #8
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r0, #8
	mov	r1, #0xa8
	mov	r2, #0x5c
	bl	__Func_80921c4
	mov	r3, r8
	strh	r3, [r7]
	mov	r0, #0
	bl	OvlFunc_933_2009c78
.L1618:
	add	sp, #0xc
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_933_20094b0

@ 242 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_1c1c, RegisterTask, CopyMapRectFull x19, StartLoopingSound
@   CopyMapRectFull x3, OvlFunc_4e4
.thumb_func_start OvlFunc_933_2009638
	push	{r5, lr}
	ldr	r0, =gState
	mov	r3, #0x8b
	lsl	r3, #2
	add	r2, r0, r3
	add	r3, #0x2c
	strh	r3, [r2]
	ldr	r2, =0x22e
	mov	r1, #0
	add	r3, r0, r2
	strh	r1, [r3]
	mov	r3, #0x8c
	lsl	r3, #2
	add	r2, r0, r3
	ldr	r3, =0x119
	strh	r3, [r2]
	mov	r2, #0xe0
	lsl	r2, #1
	add	r5, r0, r2
	mov	r3, #0
	ldrsh	r2, [r5, r3]
	ldr	r3, =0x5c
	sub	sp, #8
	cmp	r2, r3
	bne	.L166c
	b	.L1846
.L166c:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	bl	OvlFunc_933_2009c1c
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_933_2008cd0
	lsl	r1, #4
	bl	__StartTask
	mov	r3, #0
	ldrsh	r2, [r5, r3]
	ldr	r3, =0x59
	cmp	r2, r3
	bne	.L16f8
	mov	r3, #0x40
	mov	r5, #0x7e
	str	r3, [sp]
	mov	r0, #0x16
	mov	r1, #7
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x44
	str	r3, [sp]
	mov	r0, #8
	mov	r1, #0xa
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x48
	str	r3, [sp]
	mov	r0, #0x17
	mov	r1, #0x15
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x4c
	str	r3, [sp]
	mov	r0, #0x10
	mov	r1, #0x2a
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x50
	str	r3, [sp]
	mov	r0, #0x24
	mov	r1, #0x2c
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x54
	str	r3, [sp]
	mov	r0, #0xe
	mov	r1, #0x37
	b	.L17f2
.L16f8:
	ldr	r3, =0x5a
	cmp	r2, r3
	bne	.L17fe
	mov	r3, #0x40
	mov	r5, #0x7e
	str	r3, [sp]
	mov	r0, #0x2a
	mov	r1, #5
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x44
	str	r3, [sp]
	mov	r0, #0x14
	mov	r1, #0xb
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x48
	str	r3, [sp]
	mov	r0, #0xe
	mov	r1, #0xc
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x4c
	str	r3, [sp]
	mov	r0, #0x38
	mov	r1, #0x12
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x50
	str	r3, [sp]
	mov	r0, #7
	mov	r1, #0x16
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x54
	str	r3, [sp]
	mov	r0, #0x2c
	mov	r1, #0x17
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x58
	str	r3, [sp]
	mov	r0, #0x26
	mov	r1, #0x18
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x5c
	str	r3, [sp]
	mov	r0, #0x1a
	mov	r1, #0x1c
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x60
	str	r3, [sp]
	mov	r0, #0x11
	mov	r1, #0x23
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x64
	str	r3, [sp]
	mov	r0, #0x32
	mov	r1, #0x24
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x68
	str	r3, [sp]
	mov	r0, #0x22
	mov	r1, #0x2b
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x6c
	str	r3, [sp]
	mov	r0, #6
	mov	r1, #0x2e
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x70
	str	r3, [sp]
	mov	r0, #0x1b
	mov	r1, #0x37
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x74
	str	r3, [sp]
	mov	r0, #0x2b
	mov	r1, #0x38
.L17f2:
	mov	r2, #4
	mov	r3, #2
	str	r5, [sp, #4]
	bl	__Func_80105d4
	b	.L1842
.L17fe:
	ldr	r3, =0x5b
	cmp	r2, r3
	bne	.L1842
	mov	r0, #0xa9
	bl	__Func_8091ff0
	mov	r3, #0x40
	mov	r5, #0x7c
	str	r3, [sp]
	mov	r0, #8
	mov	r1, #0xe
	mov	r2, #4
	mov	r3, #4
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x44
	str	r3, [sp]
	mov	r0, #6
	mov	r1, #0x12
	mov	r2, #4
	mov	r3, #4
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x48
	str	r3, [sp]
	mov	r0, #0xa
	mov	r1, #0x15
	mov	r2, #4
	mov	r3, #4
	str	r5, [sp, #4]
	bl	__Func_80105d4
.L1842:
	bl	OvlFunc_933_20084e4
.L1846:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_933_2009638

@ 19 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetSlotEntitySpeed, SetSlotAnimation, WalkSlotToAndWait, SetSlotAnimation
.thumb_func_start OvlFunc_933_2009874
	push	{lr}
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #7
	mov	r0, #8
	lsl	r1, #8
	bl	__MapActor_SetSpeed
	mov	r0, #8
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r0, #8
	mov	r1, #0xa8
	mov	r2, #0x60
	bl	__Func_80921c4
	mov	r0, #8
	mov	r1, #2
	bl	__MapActor_SetAnim
	pop	{r0}
	bx	r0
.func_end OvlFunc_933_2009874

	.section .data

	.global Events_TolbiSpring
Events_TolbiSpring:
	.incbin "overlays/rom_7bc690/orig.bin", 0x1f30, (0x1f48-0x1f30)
	.global .L1f48  @ data table read from OvlFunc_933_2008e2c; exported so
	                @ the split can separate that function from this table.
.L1f48:
	.incbin "overlays/rom_7bc690/orig.bin", 0x1f48, (0x1f70-0x1f48)
	.global .L1f70  @ data table read from OvlFunc_933_2008e2c; exported so
	                @ the split can separate that function from this table.
.L1f70:
	.incbin "overlays/rom_7bc690/orig.bin", 0x1f70, (0x1f80-0x1f70)

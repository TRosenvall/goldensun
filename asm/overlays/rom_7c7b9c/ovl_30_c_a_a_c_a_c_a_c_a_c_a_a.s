	.include "macros.inc"

@ 173 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetSaveBit, TestSaveBit, ClearSaveBit x2, TestSaveBit
@   OvlFunc_3950, OvlFunc_39b8, SetSlotPalette, TestSaveBit
@   OvlFunc_3950, OvlFunc_39b8, TestSaveBit, OvlFunc_3950
@   OvlFunc_3284, RegisterTask x2
@   ... and 30 more
@ reads save bits 0x109, 0x8a0, 0x911, 0x925, 0x927; sets 0x144; clears 0x271, 0x272.
.thumb_func_start OvlFunc_943_2009444
	push	{r5, lr}
	mov	r0, #0xa2
	lsl	r0, #1
	bl	__SetFlag
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1464
	ldr	r0, =0x271
	bl	__ClearFlag
	ldr	r0, =0x272
	bl	__ClearFlag
.L1464:
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1480
	bl	OvlFunc_943_200b950
	bl	OvlFunc_943_200b9b8
	mov	r0, #0x18
	mov	r1, #2
	bl	__Func_8092950
	b	.L14ee
.L1480:
	mov	r0, #0x8a
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1496
	bl	OvlFunc_943_200b950
	bl	OvlFunc_943_200b9b8
	b	.L14ee
.L1496:
	ldr	r0, =0x927
	bl	__GetFlag
	mov	r5, r0
	cmp	r5, #0
	beq	.L14ce
	bl	OvlFunc_943_200b950
	bl	OvlFunc_943_200b284
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_943_200b4bc
	bl	__StartTask
	ldr	r2, =.L5b50
	mov	r3, #0x80
	lsl	r3, #14
	str	r3, [r2, #4]
	ldr	r2, =.L5b60
	ldr	r3, =0x13333
	mov	r1, #0xc8
	str	r3, [r2, #4]
	ldr	r0, =OvlFunc_943_200b1a8
	lsl	r1, #4
	bl	__StartTask
	b	.L14ee
.L14ce:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L14ee
	bl	OvlFunc_943_200b950
	ldr	r3, =.L5b50
	str	r5, [r3, #4]
	ldr	r3, =.L5b60
	mov	r1, #0xc8
	str	r5, [r3, #4]
	ldr	r0, =OvlFunc_943_200b1a8
	lsl	r1, #4
	bl	__StartTask
.L14ee:
	ldr	r0, =0x927
	bl	__GetFlag
	cmp	r0, #0
	bne	.L14fc
	bl	OvlFunc_943_200b710
.L14fc:
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #4
	cmp	r3, #0xf
	bhi	.L15d0
	ldr	r2, =.L1518
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L1518:
	.word	.L1558
	.word	.L15d0
	.word	.L15d0
	.word	.L15d0
	.word	.L15d0
	.word	.L15d0
	.word	.L1570
	.word	.L1586
	.word	.L158c
	.word	.L1592
	.word	.L1598
	.word	.L159e
	.word	.L15a4
	.word	.L15b4
	.word	.L15ba
	.word	.L15ca
.L1558:
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r1, [r0, #0x50]
	mov	r3, #0xd
	ldrb	r2, [r1, #9]
	neg	r3, r3
	and	r3, r2
	mov	r2, #4
	orr	r3, r2
	strb	r3, [r1, #9]
	b	.L15d0
.L1570:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1580
	bl	OvlFunc_943_2009b58
	b	.L1638
.L1580:
	bl	OvlFunc_943_2009a98
	b	.L1638
.L1586:
	bl	OvlFunc_943_2009d0c
	b	.L1638
.L158c:
	bl	OvlFunc_943_2009db0
	b	.L1638
.L1592:
	bl	OvlFunc_943_2009f90
	b	.L1638
.L1598:
	bl	OvlFunc_943_200a2c0
	b	.L1638
.L159e:
	bl	OvlFunc_943_200a618
	b	.L1638
.L15a4:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1604
	bl	OvlFunc_943_200a9d4
	b	.L1638
.L15b4:
	bl	OvlFunc_943_200ab7c
	b	.L1638
.L15ba:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1638
	bl	OvlFunc_943_200ac84
	b	.L1638
.L15ca:
	bl	OvlFunc_943_200ba0c
	b	.L1638
.L15d0:
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L15e8
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	mov	r2, #0x82
	add	r3, #0xec
	lsl	r2, #15
	str	r2, [r3]
	b	.L1638
.L15e8:
	mov	r0, #0x8a
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L15fa
	bl	OvlFunc_943_20099c0
	b	.L1638
.L15fa:
	ldr	r0, =0x92b
	bl	__GetFlag
	cmp	r0, #0
	beq	.L160a
.L1604:
	bl	OvlFunc_943_2009920
	b	.L1638
.L160a:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L161a
	bl	OvlFunc_943_200985c
	b	.L1638
.L161a:
	ldr	r0, =0x925
	bl	__GetFlag
	cmp	r0, #0
	beq	.L162a
	bl	OvlFunc_943_20097a0
	b	.L1638
.L162a:
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1638
	bl	OvlFunc_943_2009684
.L1638:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2009444

@ Cutscene: roughly 106 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x920, 0x922, 0x923.
.thumb_func_start OvlFunc_943_2009684
	push	{r5, r6, lr}
	mov	r0, #0x1b
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0x17
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0x16
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0x1a
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0x18
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0x92
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L16e6
	mov	r1, #0xa2
	lsl	r1, #16
	ldr	r2, =0x29a0000
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #8
	strh	r3, [r0, #6]
	mov	r1, #0
	mov	r0, #0x17
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x14
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L16e6:
	ldr	r0, =0x922
	bl	__GetFlag
	mov	r5, r0
	cmp	r5, #0
	beq	.L1766
	mov	r1, #0x84
	ldr	r2, =0x2be0000
	lsl	r1, #17
	mov	r0, #0x15
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r3, #0xa0
	lsl	r3, #7
	strh	r3, [r0, #6]
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__Random
	mov	r1, #0x5a
	bl	_umodsi3_RAM
	ldr	r6, =gScript_943__0200c4d8
	add	r0, #0x3c
	add	r5, #0x64
	strh	r0, [r5]
	mov	r1, r6
	mov	r0, #0x15
	bl	__MapActor_SetBehavior
	mov	r1, #0xf8
	mov	r2, #0xaa
	lsl	r2, #18
	lsl	r1, #16
	mov	r0, #0x18
	bl	__MapActor_SetPos
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__Random
	mov	r1, #0x5a
	bl	_umodsi3_RAM
	add	r5, #0x64
	add	r0, #0x3c
	strh	r0, [r5]
	mov	r1, r6
	mov	r0, #0x18
	bl	__MapActor_SetBehavior
	mov	r0, #0x16
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	b	.L1786
.L1766:
	ldr	r0, =0x923
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1786
	mov	r1, #0xf6
	mov	r2, #0x80
	mov	r0, #0x14
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r0, #0x14
	bl	__MapActor_GetActor
	strh	r5, [r0, #6]
.L1786:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2009684

@ Cutscene: roughly 70 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_943_20097a0
	push	{r5, r6, lr}
	mov	r1, #0x83
	lsl	r1, #17
	ldr	r2, =0x2c20000
	mov	r0, #0x15
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r6, #0xa0
	lsl	r6, #7
	mov	r1, #0xa4
	mov	r2, #0xa2
	lsl	r2, #18
	strh	r6, [r0, #6]
	lsl	r1, #16
	mov	r0, #0x18
	bl	__MapActor_SetPos
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r5, #0
	strh	r5, [r0, #6]
	mov	r1, #1
	mov	r0, #0x18
	bl	__Func_8092b08
	mov	r1, #0xc6
	ldr	r2, =0x2990000
	lsl	r1, #16
	mov	r0, #0x19
	bl	__MapActor_SetPos
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #8
	strh	r3, [r0, #6]
	mov	r1, #1
	mov	r0, #0x19
	bl	__Func_8092b08
	mov	r1, #0xbc
	lsl	r1, #16
	ldr	r2, =0x2a60000
	mov	r0, #0x1a
	bl	__MapActor_SetPos
	mov	r0, #0x1a
	bl	__MapActor_GetActor
	mov	r3, #0xb0
	lsl	r3, #8
	mov	r1, #0xba
	strh	r3, [r0, #6]
	lsl	r1, #16
	ldr	r2, =0x27b0000
	mov	r0, #0x1b
	bl	__MapActor_SetPos
	mov	r0, #0x1b
	bl	__MapActor_GetActor
	mov	r1, #0
	strh	r6, [r0, #6]
	mov	r2, #0
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x17
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x14
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_20097a0

@ Cutscene: roughly 61 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bit 0x109.
.thumb_func_start OvlFunc_943_200985c
	push	{r5, lr}
	bl	__CutsceneStart
	ldr	r0, =.L5160
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x14
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xee
	mov	r0, #0x17
	lsl	r1, #16
	ldr	r2, =0x2720000
	bl	__MapActor_SetPos
	mov	r1, #0xcc
	lsl	r1, #16
	ldr	r2, =0x2090000
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #13
	str	r3, [r0, #0xc]
	mov	r0, #0x16
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	mov	r5, #0x80
	orr	r3, r5
	strb	r3, [r0]
	ldr	r2, =0x4ccc
	mov	r0, #0x16
	ldr	r1, =0x9999
	bl	__MapActor_SetSpeed
	ldr	r1, =gScript_943__0200c58c
	mov	r0, #0x16
	bl	__MapActor_SetBehavior
	mov	r0, #0x15
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	orr	r5, r3
	strb	r5, [r0]
	ldr	r1, =0xcccc
	mov	r0, #0x15
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #0x15
	ldr	r1, =gScript_943__0200c628
	bl	__MapActor_SetBehavior
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18ec
	bl	OvlFunc_943_200c218
.L18ec:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_200985c

@ 49 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, AddSceneRecord, WaitFrames, PlaceSlotAt x3
@   GetSlotEntityChecked, SetSlotScriptWithTurn, GetSlotEntityChecked, SetSlotEntitySpeed
@   SetSlotScriptWithTurn, TestSaveBit, OvlFunc_4218, EndCutscene
@ reads save bit 0x109.
.thumb_func_start OvlFunc_943_2009920
	push	{lr}
	bl	__CutsceneStart
	ldr	r0, =.L5160
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x14
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xee
	mov	r0, #0x17
	lsl	r1, #16
	ldr	r2, =0x2720000
	bl	__MapActor_SetPos
	mov	r1, #0x86
	ldr	r2, =0x2a60000
	lsl	r1, #17
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r3, #0
	strh	r3, [r0, #6]
	ldr	r1, =gScript_943__0200c980
	mov	r0, #0x16
	bl	__MapActor_SetBehavior
	mov	r0, #0x15
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r2, [r0]
	mov	r3, #0x80
	orr	r3, r2
	strb	r3, [r0]
	ldr	r1, =0xcccc
	mov	r0, #0x15
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #0x15
	ldr	r1, =gScript_943__0200c628
	bl	__MapActor_SetBehavior
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1996
	bl	OvlFunc_943_200c218
.L1996:
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2009920

@ Cutscene: roughly 79 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bit 0x903.
.thumb_func_start OvlFunc_943_20099c0
	push	{r5, lr}
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	mov	r2, #0x82
	add	r3, #0xec
	lsl	r2, #15
	str	r2, [r3]
	bl	__CutsceneStart
	ldr	r0, =.L5418
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x18
	bl	__DeleteFieldActor
	mov	r1, #0xee
	mov	r0, #0x17
	lsl	r1, #16
	ldr	r2, =0x2720000
	bl	__MapActor_SetPos
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r5, #0xc0
	lsl	r5, #6
	strh	r5, [r0, #6]
	ldr	r0, =0x903
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1a32
	mov	r1, #0xa2
	lsl	r1, #16
	ldr	r2, =0x27a0000
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r1, #0xa2
	mov	r2, #0xa9
	strh	r5, [r0, #6]
	lsl	r1, #16
	mov	r0, #0x15
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r3, #0xd0
	b	.L1a5e
.L1a32:
	mov	r1, #0xa0
	mov	r2, #0xa3
	lsl	r1, #16
	lsl	r2, #18
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r1, #0xa6
	mov	r2, #0xa7
	strh	r5, [r0, #6]
	lsl	r1, #16
	mov	r0, #0x15
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r3, #0xb0
.L1a5e:
	lsl	r3, #8
	strh	r3, [r0, #6]
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #6
	bne	.L1a76
	bl	OvlFunc_943_200bf30
.L1a76:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_20099c0

@ Cutscene: roughly 75 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_943_2009a98
	push	{lr}
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r1, r1
	neg	r2, r2
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x14
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x16
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x18
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x19
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x1a
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x1b
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0x17
	bl	__MapActor_SetPos
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r3, #0xc0
	lsl	r3, #6
	mov	r1, #0xe8
	strh	r3, [r0, #6]
	lsl	r1, #16
	ldr	r2, =0x28a0000
	mov	r0, #0x15
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r3, #0xb0
	lsl	r3, #8
	strh	r3, [r0, #6]
	mov	r1, #1
	mov	r0, #0xe8
	mov	r2, #0x9f
	neg	r1, r1
	lsl	r2, #18
	mov	r3, #0
	lsl	r0, #16
	bl	__Func_80933f8
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x17
	mov	r1, #0x15
	bl	OvlFunc_943_2009c14
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2009a98

	.include "macros.inc"

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

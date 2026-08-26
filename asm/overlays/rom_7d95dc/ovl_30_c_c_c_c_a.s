	.include "macros.inc"


@ 44 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, CopyMapRectIndicesU, CopyMapRectAttributes, SetSlotPalette
@   TestSaveBit, CopyMapRectAttributes
@ reads save bits 0x950, 0x962.
.thumb_func_start OvlFunc_953_2009c6c
	push	{lr}
	mov	r0, #0x95
	lsl	r0, #4
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1ca4
	mov	r3, #2
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r0, #0x40
	mov	r1, #0
	mov	r2, #0x30
	mov	r3, #5
	bl	__CopyMapTiles
	mov	r3, #0x10
	mov	r2, #8
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xe
	mov	r1, #8
	mov	r2, #2
	mov	r3, #1
	bl	__Func_8010704
	b	.L1cca
.L1ca4:
	mov	r0, #0x10
	mov	r1, #2
	bl	__Func_8092950
	ldr	r0, =0x962
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1cca
	mov	r3, #0xe
	mov	r2, #0xb
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1e
	mov	r1, #0x16
	mov	r2, #1
	mov	r3, #2
	bl	__Func_8010704
.L1cca:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_2009c6c

@ Cutscene: roughly 697 instructions of straight-line script --
@ 36 turns, 14 animation changes, 0 dialogue lines, 10 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x20f8.
.thumb_func_start OvlFunc_953_2009cd4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	bl	__CutsceneStart
	mov	r1, #0xc6
	mov	r2, #0x88
	mov	r0, #1
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xce
	mov	r2, #0x88
	mov	r0, #2
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xca
	mov	r2, #0x98
	lsl	r1, #18
	lsl	r2, #16
	mov	r0, #3
	bl	__MapActor_SetPos
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r2, r2
	mov	r3, #0
	neg	r0, r0
	neg	r1, r1
	bl	__Func_80933f8
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091220
	mov	r1, #0
	mov	r0, #0
	bl	__Func_8091200
	mov	r0, #1
	bl	__Func_8091254
	mov	r0, #1
	bl	__WaitFrames
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r9, r3
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	add	r3, #0x43
	str	r3, [r2]
	sub	r3, #0x3b
	add	r2, r1, r3
	mov	r3, #1
	str	r3, [r2]
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091220
	mov	r1, #0
	ldr	r0, =0x10002
	bl	__Func_8091200
	mov	r0, #0x28
	bl	__Func_8091254
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #2
	bl	__Func_80925cc
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r3, #0xc0
	lsl	r3, #6
	mov	r8, r3
	mov	r0, #8
	mov	r1, r8
	bl	OvlFunc_953_2009c5c
	mov	r1, #2
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x80
	mov	r1, #0
	lsl	r0, #9
	bl	__Func_8091200
	mov	r0, #0x28
	bl	__Func_8091254
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #2
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #1
	mov	r0, #2
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x20f8
	bl	__MessageID
	mov	r0, #2
	bl	OvlFunc_953_2009c48
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_DoAnim
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r1, #0x81
	lsl	r1, #1
	mov	r2, #0x28
	mov	r0, #3
	bl	__MapActor_Emote
	mov	r0, #3
	bl	OvlFunc_953_2009c48
	mov	r1, #0x80
	mov	r0, #8
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r2, #0
	mov	r0, #8
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r0, #9
	mov	r1, r8
	bl	OvlFunc_953_2009c5c
	ldr	r1, =0x105
	mov	r2, #0x14
	mov	r0, #9
	bl	__MapActor_Emote
	mov	r0, #9
	bl	OvlFunc_953_2009c48
	mov	r2, #0x14
	mov	r0, #1
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #1
	bl	__Func_809259c
	mov	r0, #1
	bl	OvlFunc_953_2009c48
	mov	r1, #0x81
	mov	r2, #0x28
	lsl	r1, #1
	mov	r0, #0xa
	bl	__MapActor_Emote
	mov	r0, #0xa
	bl	OvlFunc_953_2009c48
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	bl	OvlFunc_953_2009c48
	mov	r3, #0xa0
	lsl	r3, #8
	mov	r10, r3
	mov	r0, #2
	mov	r1, r10
	bl	OvlFunc_953_2009c5c
	mov	r1, #4
	mov	r0, #2
	bl	__MapActor_DoAnim
	mov	r0, #2
	bl	OvlFunc_953_2009c48
	mov	r1, #1
	mov	r0, #3
	bl	__Func_80925cc
	mov	r0, #3
	bl	OvlFunc_953_2009c48
	mov	r2, #0x14
	mov	r0, #1
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r0, #1
	mov	r1, #0
	bl	OvlFunc_953_2009c5c
	mov	r5, #0xc0
	mov	r1, #2
	mov	r0, #1
	lsl	r5, #7
	bl	__Func_809259c
	mov	r0, #1
	bl	OvlFunc_953_2009c48
	mov	r1, r5
	mov	r0, #0
	bl	OvlFunc_953_2009c5c
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #2
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #0x80
	lsl	r1, #8
	mov	r0, #2
	bl	OvlFunc_953_2009c5c
	mov	r0, #2
	bl	OvlFunc_953_2009c48
	mov	r1, #4
	mov	r0, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r6, #0x80
	mov	r0, #3
	bl	OvlFunc_953_2009c48
	lsl	r6, #6
	mov	r0, #1
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, r6
	mov	r0, #1
	bl	OvlFunc_953_2009c5c
	mov	r0, #1
	bl	OvlFunc_953_2009c48
	mov	r0, #2
	ldr	r1, =0x105
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r0, #2
	mov	r1, r6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #3
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #3
	lsl	r1, #1
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r1, #0x83
	mov	r2, #0
	mov	r0, #2
	lsl	r1, #1
	mov	r7, #0xc0
	lsl	r7, #8
	bl	__MapActor_Emote
	mov	r0, #2
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, r7
	mov	r0, #2
	bl	OvlFunc_953_2009c5c
	mov	r0, #2
	bl	OvlFunc_953_2009c48
	mov	r0, #0
	mov	r1, r7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, r7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0x14
	mov	r0, #3
	mov	r1, r7
	bl	__Func_8092adc
	mov	r0, #8
	mov	r1, r8
	bl	OvlFunc_953_2009c5c
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_DoAnim
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #3
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #1
	mov	r0, #9
	bl	__Func_80925cc
	mov	r0, #9
	bl	OvlFunc_953_2009c48
	mov	r1, r5
	mov	r0, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r5
	mov	r0, #2
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r2, #0x28
	mov	r0, #3
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #1
	mov	r0, #0xa
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	OvlFunc_953_2009c48
	mov	r0, #0
	mov	r1, r7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, r7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0
	mov	r0, #2
	mov	r1, r7
	bl	__Func_8092adc
	mov	r0, #3
	mov	r1, r7
	bl	OvlFunc_953_2009c5c
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	bl	OvlFunc_953_2009c48
	mov	r1, r5
	mov	r0, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r5
	mov	r0, #2
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #3
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r0, #1
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #2
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #3
	ldr	r1, =0x105
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r1, #0x83
	mov	r2, #0
	mov	r0, #2
	lsl	r1, #1
	mov	r5, #0xe0
	bl	__MapActor_Emote
	lsl	r5, #8
	mov	r0, #2
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, r5
	mov	r0, #2
	bl	OvlFunc_953_2009c5c
	mov	r0, #2
	bl	OvlFunc_953_2009c48
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r2, #0x3c
	mov	r0, #0xb
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r1, #0
	mov	r0, #2
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, r6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r5
	mov	r0, #1
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #2
	mov	r1, r10
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #3
	mov	r1, r7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0
	mov	r1, #0
	mov	r5, #0
	bl	__Func_8091c7c
	b	.L2154

	.pool_aligned

.L2154:
	cmp	r0, #1
	bne	.L2162
	mov	r0, #2
	bl	OvlFunc_953_2009c48
	mov	r5, #1
	b	.L2188
.L2162:
	mov	r3, r9
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #2
	mov	r1, r7
	bl	OvlFunc_953_2009c5c
	mov	r0, #2
	bl	OvlFunc_953_2009c48
.L2188:
	cmp	r5, #0
	beq	.L219c
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L219c:
	mov	r1, #0x80
	mov	r0, #8
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #0xa
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r0, #0xb
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r0, #8
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #9
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0xa
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r5, #0xc0
	mov	r2, #0x3c
	mov	r0, #0xb
	ldr	r1, =0x105
	bl	__MapActor_Emote
	lsl	r5, #6
	mov	r0, #8
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, r5
	mov	r0, #8
	bl	OvlFunc_953_2009c5c
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #2
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r2, #0x14
	mov	r0, #3
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #9
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, r5
	mov	r0, #9
	bl	OvlFunc_953_2009c5c
	mov	r0, #9
	bl	OvlFunc_953_2009c48
	mov	r0, #0xa
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #0xa0
	lsl	r1, #7
	mov	r0, #0xa
	mov	r6, #0x80
	bl	OvlFunc_953_2009c5c
	lsl	r6, #8
	mov	r0, #0xa
	bl	OvlFunc_953_2009c48
	mov	r1, r6
	mov	r0, #0xb
	bl	OvlFunc_953_2009c5c
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	bl	OvlFunc_953_2009c48
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #2
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #3
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, #0x80
	mov	r2, r6
	mov	r0, #1
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, r6
	mov	r0, #2
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, r6
	mov	r0, #3
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_953__0200adac
	mov	r0, #1
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #2
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #3
	bl	__MapActor_RunScript
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0
	bl	OvlFunc_953_2009c5c
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, #0x80
	mov	r2, r6
	mov	r0, #0xb
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, r6
	mov	r0, #0
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r0, #0xb
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	ldr	r1, =0x33e
	mov	r2, #0x98
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r0, #0xb
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r2, #1
	lsl	r1, #2
	mov	r0, #0xb
	bl	__MapActor_TravelTo
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_80933d4
	mov	r0, #0xca
	mov	r1, #1
	mov	r2, #0x9c
	mov	r3, #1
	lsl	r0, #18
	neg	r1, r1
	lsl	r2, #17
	bl	__Func_80933f8
	mov	r1, #0xca
	mov	r0, #0
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_80921c4
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r1, #2
	lsl	r2, #1
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0x3c
	bl	__CutsceneWait
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x28
	str	r3, [r2]
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x40
	bl	__Func_8091e9c
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_2009cd4

@ Cutscene: roughly 91 instructions of straight-line script --
@ 0 turns, 2 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x5, 0x90f.
@ Sets save bit 0x16d.
.thumb_func_start OvlFunc_953_200a3e0
	push	{lr}
	mov	r0, #5
	bl	__GetFlag
	cmp	r0, #0
	beq	.L23fe
	ldr	r0, =0x16d
	bl	__SetFlag
	mov	r0, #5
	bl	__Func_8079664
	mov	r0, #3
	bl	__AddPartyMember
.L23fe:
	bl	__CutsceneStart
	mov	r1, #0xb2
	mov	r2, #0x93
	lsl	r2, #18
	lsl	r1, #18
	mov	r0, #0xb
	bl	__MapActor_SetPos
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0xb
	mov	r1, #1
	bl	__SetCameraTarget
	mov	r0, #0xb
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	ldr	r2, =0xcccc
	ldr	r1, =0x19999
	mov	r0, #0
	bl	__MapActor_SetSpeed
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r3, #0
	strh	r3, [r0, #6]
	bl	__MapTransitionIn
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r1, #0xc3
	mov	r2, #0x93
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r1, #0xcb
	mov	r2, #0x93
	mov	r0, #0xb
	lsl	r1, #2
	lsl	r2, #2
	bl	__Func_8092158
	mov	r1, #0xdc
	mov	r2, #0x93
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r1, #0xe4
	mov	r2, #0x93
	mov	r0, #0xb
	lsl	r1, #2
	lsl	r2, #2
	bl	__Func_8092158
	mov	r1, #0xf5
	mov	r2, #0x93
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r1, #0xfd
	mov	r2, #0x93
	mov	r0, #0xb
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x90f
	bl	__GetFlag
	cmp	r0, #0
	beq	.L24be
	mov	r0, #0x1f
	bl	__Func_8091e9c
	b	.L24c4
.L24be:
	mov	r0, #0x41
	bl	__Func_8091e9c
.L24c4:
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a3e0

@ Cutscene: roughly 109 instructions of straight-line script --
@ 14 turns, 2 animation changes, 0 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2112.
.thumb_func_start OvlFunc_953_200a4d8
	push	{r5, lr}
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #0xd
	bl	__MapActor_SetIdle
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0xe0
	mov	r2, #0
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #0xd
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0xd0
	mov	r0, #0xc
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0xe
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xd0
	mov	r0, #0xf
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0x10
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r0, #0x11
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r2, #0
	lsl	r1, #8
	mov	r0, #0x12
	bl	__Func_8092adc
	ldr	r0, =0x2112
	bl	__MessageID
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r2, r5
	mov	r3, #0xb4
	add	r2, #0x64
	lsl	r3, #2
	strh	r3, [r2]
	add	r5, #0x66
	mov	r3, #0x70
	strh	r3, [r5]
	mov	r0, #0xd
	mov	r1, #2
	bl	__MapActor_SetBehavior
	mov	r1, #0xc0
	mov	r0, #0xc
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb0
	mov	r0, #0xe
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0xf
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0x11
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0x12
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a4d8

@ 41 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotEntitySpeed, ShowScreenOverlay, SetSlotAnimation
@   MoveSlotToAndWait x2, MoveSlotTo, HideScreenOverlay, WaitSceneDelay
@   TestSaveBit, SetPendingMessageId x2
@ reads save bit 0x90f.
.thumb_func_start OvlFunc_953_200a5f0
	push	{lr}
	bl	__CutsceneStart
	ldr	r2, =0xcccc
	mov	r0, #0
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	bl	__MapTransitionIn
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r1, #0xc3
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0xdc
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0xf5
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__MapActor_TravelTo
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x90f
	bl	__GetFlag
	cmp	r0, #0
	beq	.L2650
	mov	r0, #0x20
	bl	__Func_8091e9c
	b	.L2656
.L2650:
	mov	r0, #0xc
	bl	__Func_8091e9c
.L2656:
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a5f0

@ Cutscene: roughly 164 instructions of straight-line script --
@ 2 turns, 5 animation changes, 0 dialogue lines, 4 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2134.
.thumb_func_start OvlFunc_953_200a668
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r1, #0xc6
	mov	r2, #0x88
	mov	r0, #1
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xce
	mov	r2, #0x88
	mov	r0, #2
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xca
	mov	r2, #0x98
	lsl	r2, #16
	lsl	r1, #18
	mov	r0, #3
	bl	__MapActor_SetPos
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	ldr	r0, =0x2134
	bl	__MessageID
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r1, #1
	mov	r0, #9
	bl	__Func_80925cc
	mov	r0, #9
	bl	OvlFunc_953_2009c48
	mov	r1, #1
	mov	r0, #0xa
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	OvlFunc_953_2009c48
	mov	r0, #0xb
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	bl	OvlFunc_953_2009c48
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #2
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #2
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	mov	r0, #3
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_953__0200adac
	mov	r0, #1
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #2
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #3
	bl	__MapActor_RunScript
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0
	bl	OvlFunc_953_2009c5c
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xb
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	mov	r0, #0
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r0, #0xb
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	ldr	r1, =0x33e
	mov	r2, #0x98
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r0, #0xb
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r2, #1
	lsl	r1, #2
	mov	r0, #0xb
	bl	__MapActor_TravelTo
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_80933d4
	mov	r0, #0xca
	mov	r1, #1
	mov	r2, #0x9c
	mov	r3, #1
	lsl	r0, #18
	neg	r1, r1
	lsl	r2, #17
	bl	__Func_80933f8
	mov	r1, #0xca
	mov	r0, #0
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_80921c4
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r1, #2
	lsl	r2, #1
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0x3c
	bl	__CutsceneWait
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x43
	bl	__Func_8091e9c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a668

@ Cutscene: roughly 85 instructions of straight-line script --
@ 0 turns, 2 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bit 0x5.
@ Sets save bit 0x16d.
.thumb_func_start OvlFunc_953_200a820
	push	{lr}
	mov	r0, #5
	bl	__GetFlag
	cmp	r0, #0
	beq	.L283e
	ldr	r0, =0x16d
	bl	__SetFlag
	mov	r0, #5
	bl	__Func_8079664
	mov	r0, #3
	bl	__AddPartyMember
.L283e:
	bl	__CutsceneStart
	mov	r1, #0xd9
	mov	r2, #0x93
	lsl	r2, #18
	lsl	r1, #18
	mov	r0, #0xb
	bl	__MapActor_SetPos
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0xb
	mov	r1, #1
	bl	__SetCameraTarget
	mov	r0, #0xb
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	ldr	r2, =0xcccc
	ldr	r1, =0x19999
	mov	r0, #0
	bl	__MapActor_SetSpeed
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #8
	strh	r3, [r0, #6]
	bl	__MapTransitionIn
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r1, #0xc8
	mov	r2, #0x93
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r1, #0xc0
	mov	r2, #0x93
	mov	r0, #0xb
	lsl	r1, #2
	lsl	r2, #2
	bl	__Func_8092158
	mov	r1, #0xaf
	mov	r2, #0x93
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r1, #0xa7
	mov	r2, #0x93
	mov	r0, #0xb
	lsl	r1, #2
	lsl	r2, #2
	bl	__Func_8092158
	mov	r1, #0x96
	mov	r2, #0x93
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r1, #0x8e
	mov	r2, #0x93
	mov	r0, #0xb
	lsl	r1, #2
	lsl	r2, #2
	bl	__MapActor_TravelTo
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x15
	bl	__Func_8091e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a820

@ 34 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotEntitySpeed, ShowScreenOverlay, SetSlotAnimation
@   MoveSlotToAndWait x2, MoveSlotTo, HideScreenOverlay, WaitSceneDelay
@   SetPendingMessageId
.thumb_func_start OvlFunc_953_200a904
	push	{lr}
	bl	__CutsceneStart
	ldr	r2, =0xcccc
	mov	r0, #0
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	bl	__MapTransitionIn
	mov	r0, #0
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r1, #0xc8
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0xaf
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__Func_8092158
	mov	r1, #0x96
	mov	r2, #0xd6
	mov	r0, #0
	lsl	r1, #2
	lsl	r2, #1
	bl	__MapActor_TravelTo
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x16
	bl	__Func_8091e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a904

@ Cutscene: roughly 164 instructions of straight-line script --
@ 2 turns, 5 animation changes, 0 dialogue lines, 4 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2138.
.thumb_func_start OvlFunc_953_200a964
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r1, #0xc6
	mov	r2, #0x88
	mov	r0, #1
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xce
	mov	r2, #0x88
	mov	r0, #2
	lsl	r1, #18
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xca
	mov	r2, #0x98
	lsl	r2, #16
	lsl	r1, #18
	mov	r0, #3
	bl	__MapActor_SetPos
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	ldr	r0, =0x2138
	bl	__MessageID
	mov	r0, #8
	bl	OvlFunc_953_2009c48
	mov	r1, #1
	mov	r0, #9
	bl	__Func_80925cc
	mov	r0, #9
	bl	OvlFunc_953_2009c48
	mov	r1, #1
	mov	r0, #0xa
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	OvlFunc_953_2009c48
	mov	r0, #0xb
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	bl	OvlFunc_953_2009c48
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #2
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #2
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	mov	r0, #3
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_953__0200adac
	mov	r0, #1
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #2
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #3
	bl	__MapActor_RunScript
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0
	bl	OvlFunc_953_2009c5c
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xb
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	mov	r0, #0
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r0, #0xb
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	ldr	r1, =0x33e
	mov	r2, #0x98
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r0, #0xb
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_8092158
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r2, #1
	lsl	r1, #2
	mov	r0, #0xb
	bl	__MapActor_TravelTo
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_80933d4
	mov	r0, #0xca
	mov	r1, #1
	mov	r2, #0x9c
	mov	r3, #1
	lsl	r0, #18
	neg	r1, r1
	lsl	r2, #17
	bl	__Func_80933f8
	mov	r1, #0xca
	mov	r0, #0
	lsl	r1, #2
	mov	r2, #0xa4
	bl	__Func_80921c4
	mov	r1, #0xca
	mov	r2, #0x9c
	lsl	r1, #2
	lsl	r2, #1
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0x3c
	bl	__CutsceneWait
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x40
	bl	__Func_8091e9c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_953_200a964

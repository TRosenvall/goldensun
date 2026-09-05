	.include "macros.inc"

@ Cutscene: roughly 226 instructions of straight-line script --
@ 0 turns, 1 animation change, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x801, 0x808, 0x834.
.thumb_func_start OvlFunc_885_200950c
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r5, r3, r2
	ldrh	r3, [r5]
	mov	r2, #0x80
	sub	r3, #5
	lsl	r3, #16
	lsl	r2, #9
	sub	sp, #8
	cmp	r3, r2
	bhi	.L1530
	ldr	r0, =0x12f
	bl	__ClearFlag
.L1530:
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1540
	ldr	r0, =0x242
	bl	__ClearFlag
.L1540:
	ldr	r0, =0x834
	bl	__GetFlag
	cmp	r0, #0
	bne	.L154c
	b	.L1656
.L154c:
	bl	__StartRain
	bl	__StartThunder
	bl	__CutsceneStart
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xe
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0
	mov	r2, #0
	mov	r0, #5
	bl	__MapActor_SetPos
	mov	r0, #8
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r2, [r0]
	mov	r3, #8
	orr	r3, r2
	mov	r1, #0xa6
	strb	r3, [r0]
	lsl	r1, #15
	mov	r0, #0xb
	ldr	r2, =0x1090000
	bl	__MapActor_SetPos
	ldr	r2, =0x111
	mov	r0, #0xb
	mov	r1, #0x53
	bl	__Func_80921c4
	mov	r1, #5
	mov	r0, #0xb
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r3, #0xc
	strh	r3, [r0, #0x20]
	ldr	r1, =gOvl_02009c34
	mov	r0, #0xb
	bl	__MapActor_SetBehavior
	ldr	r0, =0x839
	bl	__GetFlag
	cmp	r0, #0
	beq	.L15de
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L15de:
	bl	__CutsceneEnd
	mov	r3, #0xe
	str	r3, [sp]
	mov	r5, #0x15
	mov	r0, #9
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0xf
	str	r3, [sp]
	mov	r0, #9
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x17
	str	r3, [sp]
	mov	r5, #0x13
	mov	r8, r3
	mov	r0, #9
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r6, #0x18
	mov	r0, #9
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	str	r6, [sp]
	bl	__Func_8010704
	mov	r2, r8
	mov	r5, #0x14
	str	r2, [sp]
	mov	r0, #9
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r0, #9
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #1
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
	b	.L171e
.L1656:
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	bl	__CutsceneStart
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1698
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	cmp	r3, #0xa
	bne	.L1698
	bl	OvlFunc_885_2008964
.L1698:
	ldr	r0, =0x801
	bl	__GetFlag
	cmp	r0, #0
	beq	.L16c2
	mov	r0, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xe
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	b	.L1702
.L16c2:
	ldr	r0, =0x808
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1702
	mov	r1, #0xc4
	mov	r2, #0xbc
	mov	r0, #0xe
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0xbc
	mov	r2, #0xbc
	mov	r0, #0xf
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	ldr	r5, =gScript_885__02009ce0
	mov	r1, #0x80
	mov	r0, #0xe
	lsl	r1, #9
	mov	r2, r5
	bl	__Func_8092a1c
	mov	r1, #0x80
	mov	r0, #0xf
	lsl	r1, #9
	mov	r2, r5
	bl	__Func_8092a1c
.L1702:
	ldr	r0, =0x87a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L171a
	mov	r1, #0x84
	mov	r2, #0x84
	mov	r0, #0x10
	lsl	r1, #16
	lsl	r2, #17
	bl	__MapActor_SetPos
.L171a:
	bl	__CutsceneEnd
.L171e:
	mov	r0, #0
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_885_200950c

@ Cutscene: roughly 205 instructions of straight-line script --
@ 4 turns, 6 animation changes, 6 dialogue lines, 4 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xe85, 0xe8b.
@ Reads save bits 0x82f, 0x839.
@ Sets save bits 0x82f, 0x839.
.thumb_func_start OvlFunc_885_2009760
	push	{r5, lr}
	ldr	r0, =0x839
	bl	__GetFlag
	cmp	r0, #0
	beq	.L176e
	b	.L196c
.L176e:
	ldr	r0, =0x82f
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1798
	bl	__CutsceneStart
	mov	r1, #2
	mov	r0, #0xb
	bl	__Func_80925cc
	ldr	r0, =0xe8b
	bl	__MessageID
	mov	r0, #0xb
	mov	r1, #0
	bl	__ActorMessage
	bl	__CutsceneEnd
	b	.L196c
.L1798:
	bl	__CutsceneStart
	mov	r0, #0xb
	bl	__MapActor_SetIdle
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_80925cc
	ldr	r5, =0xe85
	mov	r0, r5
	bl	__MessageID
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0x1e
	bl	__MapActor_Emote
	mov	r0, #0xc4
	mov	r1, #1
	mov	r3, #1
	lsl	r0, #15
	neg	r1, r1
	ldr	r2, =0x11b0000
	bl	__Func_80933f8
	mov	r0, #0
	mov	r1, #0x5e
	ldr	r2, =0x125
	bl	__Func_80921c4
	mov	r1, #0xa0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1802
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #1
	bl	__MapActor_SetPos
.L1802:
	mov	r0, #1
	mov	r1, #0x6e
	ldr	r2, =0x117
	bl	__Func_80921c4
	mov	r1, #0xa0
	mov	r2, #0x28
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #2
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #0xb
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L185e
	mov	r1, #2
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	add	r0, r5, #2
	bl	__MessageID
	mov	r0, #0xb
	mov	r1, #0
	bl	__ActorMessage
	ldr	r0, =0x82f
	bl	__SetFlag
	b	.L1938
.L185e:
	mov	r1, #2
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	add	r0, r5, #3
	bl	__MessageID
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_8093040
	mov	r2, #0
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_809280c
	mov	r0, #0xb
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r2, #0x28
	mov	r0, #0xb
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r0, #0xb
	mov	r1, #6
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r2, #0xa
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #1
	mov	r0, #0xb
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r2, #0xa
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	ldr	r5, =gScript_885__02009ce0
	mov	r0, #0
	ldr	r1, =0x1000b
	mov	r2, r5
	bl	__Func_8092a1c
	mov	r2, r5
	mov	r0, #1
	ldr	r1, =0x1000b
	bl	__Func_8092a1c
	ldr	r1, =ActorCmd_ARRAY_885__02009bdc
	mov	r0, #0xb
	bl	__MapActor_RunScript
	mov	r0, #0
	bl	__MapActor_SetIdle
	mov	r0, #1
	bl	__MapActor_SetIdle
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0x3c
	bl	__Func_8092adc
	mov	r0, #0
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #1
	ldr	r1, =0x105
	mov	r2, #0x78
	bl	__MapActor_Emote
	ldr	r0, =0x839
	bl	__SetFlag
.L1938:
	mov	r0, #1
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1958
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__MapActor_TravelTo
.L1958:
	mov	r0, #1
	bl	__MapActor_WaitMovement
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	bl	__CutsceneEnd
.L196c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_885_2009760

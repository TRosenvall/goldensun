	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 516 instructions of straight-line script --
@ 10 turns, 19 animation changes, 1 dialogue line, 24 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x10b6, 0x10c4, 0x10c6.
.thumb_func_start OvlFunc_896_2009d04
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r1, #3
	mov	r0, #1
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r2, #0
	lsl	r1, #6
	mov	r0, #1
	bl	__Func_8092adc
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xe
	mov	r1, #0xf
	bl	__Func_8092950
	mov	r1, #0xc4
	mov	r2, #0xe3
	mov	r0, #0xe
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	bl	OvlFunc_896_200c3bc
	mov	r1, #0xd0
	mov	r2, #0xa
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_809259c
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r1, #0xa0
	mov	r2, #0xa
	mov	r0, #0xe
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r1, #2
	mov	r0, #0xe
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r6, =0x10b6
	mov	r0, r6
	bl	__MessageID
	mov	r0, #0xe
	mov	r1, #0
	bl	__ActorMessage
	mov	r2, #0xae
	lsl	r2, #17
	ldr	r1, =0x1d50000
	ldr	r5, =0x200a
	mov	r0, #0xa
	bl	__MapActor_SetPos
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #0xa
	bl	OvlFunc_896_200c248
	mov	r0, r5
	mov	r1, #0x28
	bl	OvlFunc_896_200c248
	mov	r2, #0xae
	lsl	r2, #17
	mov	r0, #0xa
	ldr	r1, =0x1fb0000
	bl	__MapActor_SetPos
	mov	r1, #2
	mov	r0, #1
	bl	__Func_80925cc
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #1
	bl	__MapActor_DoAnim
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #8
	lsl	r2, #7
	bl	__MapActor_SetSpeed
	mov	r2, #0xea
	mov	r0, #1
	ldr	r1, =0x185
	lsl	r2, #1
	bl	__Func_80921c4
	mov	r1, #0xd0
	mov	r2, #0x3c
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #0x14
	bl	OvlFunc_896_200c248
	add	r0, r6, #4
	mov	r1, #1
	mov	r2, #0xa
	bl	__Func_8019aa0
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #8
	lsl	r2, #7
	mov	r0, #1
	bl	__MapActor_SetSpeed
	mov	r0, #1
	bl	__MapActor_GetActor
	mov	r7, r0
	mov	r5, r7
	add	r5, #0x5a
	ldrb	r2, [r5]
	mov	r3, #0xfe
	and	r3, r2
	mov	r2, #0
	mov	r8, r2
	mov	r1, #0xbc
	mov	r2, #0xeb
	strb	r3, [r5]
	lsl	r1, #1
	lsl	r2, #1
	mov	r0, #1
	bl	__Func_80921c4
	mov	r0, #0x1e
	bl	__CutsceneWait
	ldrb	r2, [r5]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r5]
	mov	r1, #4
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	add	r6, #5
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, r6
	bl	__MessageID
	mov	r0, #0xe
	mov	r1, #0x14
	bl	OvlFunc_896_200c248
	mov	r2, #0x3c
	mov	r0, #1
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r0, #0xe
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xe
	mov	r1, #0x14
	bl	OvlFunc_896_200c248
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #1
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #3
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r2, #0x14
	mov	r0, #0xe
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #0x80
	lsl	r1, #1
	mov	r0, #0xe
	bl	__Func_8092950
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r7, r0
	mov	r6, r7
	mov	r3, r8
	add	r6, #0x55
	strb	r3, [r6]
	mov	r0, #0xdc
	bl	__PlaySound
	mov	r5, #0
.L1ee6:
	ldr	r3, [r7, #0xc]
	mov	r2, #0x80
	lsl	r2, #9
	add	r3, r2
	str	r3, [r7, #0xc]
	mov	r0, #1
	add	r5, #1
	bl	__CutsceneWait
	cmp	r5, #0x1e
	bne	.L1ee6
	mov	r3, #5
	strb	r3, [r6]
	mov	r0, #1
	mov	r1, #2
	bl	__Func_809259c
	mov	r0, #1
	mov	r1, #0xa
	bl	OvlFunc_896_200c248
	mov	r0, #0xe
	ldr	r1, =0x101
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r1, #0xa0
	mov	r2, #0xa
	mov	r0, #0xe
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #0x14
	bl	OvlFunc_896_200c248
	mov	r0, #0xe
	mov	r1, #1
	bl	__Func_80925cc
	mov	r0, #0xe
	mov	r1, #0x14
	bl	OvlFunc_896_200c248
	mov	r2, #0x14
	mov	r0, #1
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r0, #1
	mov	r1, #0x1e
	bl	OvlFunc_896_200c248
	mov	r0, #0xe
	ldr	r1, =0x105
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r1, #0xd0
	mov	r0, #0xe
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0xa
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r1, #1
	mov	r2, #0xa7
	lsl	r2, #17
	mov	r3, #0
	neg	r1, r1
	ldr	r0, =0x1dd0000
	bl	__Func_80933f8
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xa
	mov	r1, #4
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	mov	r1, #0xa
	bl	OvlFunc_896_200c248
	mov	r1, #0
	mov	r0, #0xb
	bl	__Func_8092c40
	ldr	r0, =0x66666
	ldr	r1, =0xcccc
	bl	__Func_80933d4
	mov	r0, #0xbb
	mov	r1, #1
	mov	r2, #0xeb
	mov	r3, #1
	lsl	r0, #17
	neg	r1, r1
	lsl	r2, #17
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r1, #0xa0
	mov	r0, #0xe
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #1
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	beq	.L2076
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #4
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	ldr	r0, =0x10c3
	b	.L205e

	.pool_aligned

.L2048:
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #4
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x10c6
.L205e:
	bl	__MessageID
	mov	r1, #0
	mov	r0, #0xe
	bl	__Func_8092c40
	mov	r0, #1
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	beq	.L2048
.L2076:
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x10c4
	bl	__MessageID
	mov	r0, #0xe
	mov	r1, #0x1e
	bl	OvlFunc_896_200c248
	mov	r1, #3
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xe
	mov	r1, #0x1e
	bl	OvlFunc_896_200c248
	mov	r3, #0
	strb	r3, [r6]
	mov	r0, #0xe
	ldr	r1, =0x26666
	ldr	r2, =0x13333
	bl	__MapActor_SetSpeed
	mov	r1, #0xe6
	mov	r3, #0xb4
	lsl	r3, #17
	mov	r2, #0
	mov	r0, r7
	lsl	r1, #17
	bl	__Actor_TravelTo
	mov	r0, #0xe
	bl	__MapActor_WaitMovement
	mov	r1, #0
	mov	r0, #0xe
	bl	__Func_8092950
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #1
	bl	__SetCameraTarget
	bl	__Func_8093530
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r2, #0x28
	mov	r0, #1
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r1, #3
	mov	r0, #1
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #1
	bl	__MapActor_GetActor
	mov	r7, r0
	mov	r1, r7
	add	r1, #0x5a
	ldrb	r2, [r1]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r1]
	mov	r3, #0xc0
	lsl	r3, #10
	str	r3, [r7, #0x30]
	mov	r3, #0x80
	lsl	r3, #10
	mov	r5, #0xc0
	str	r3, [r7, #0x34]
	lsl	r5, #11
	mov	r0, #0x99
	bl	__PlaySound
	mov	r0, #1
	mov	r1, #7
	str	r5, [r7, #0x28]
	bl	__MapActor_SetAnim
	mov	r1, #0xab
	mov	r2, #0xeb
	lsl	r2, #1
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_8092158
	mov	r1, #1
	mov	r0, #1
	bl	__MapActor_SetAnim
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0x99
	bl	__PlaySound
	mov	r0, #1
	mov	r1, #7
	str	r5, [r7, #0x28]
	bl	__MapActor_SetAnim
	mov	r1, #0x9c
	mov	r2, #0xeb
	lsl	r2, #1
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_8092158
	mov	r1, #1
	mov	r0, #1
	bl	__MapActor_SetAnim
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0x99
	bl	__PlaySound
	mov	r0, #1
	mov	r1, #7
	str	r5, [r7, #0x28]
	bl	__MapActor_SetAnim
	mov	r1, #0x8b
	mov	r2, #0xf0
	lsl	r2, #1
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_8092158
	mov	r1, #1
	mov	r0, #1
	bl	__MapActor_SetAnim
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #8
	lsl	r1, #5
	bl	__Func_80933d4
	mov	r0, #0
	mov	r1, #1
	bl	__SetCameraTarget
	mov	r0, #1
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r2, #0
	mov	r1, #1
	mov	r0, #0
	bl	__Func_8092848
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0
	mov	r1, #4
	bl	__MapActor_DoAnim
	mov	r0, #1
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2232
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__MapActor_TravelTo
.L2232:
	mov	r0, #1
	bl	__MapActor_WaitMovement
	mov	r1, #0
	mov	r2, #0
	mov	r0, #1
	bl	__MapActor_SetPos
	mov	r0, #0xdc
	bl	__Func_8078a08
	mov	r0, #0xdd
	bl	__Func_8078a08
	mov	r0, #0xdf
	bl	__Func_8078a08
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_896_2009d04

@ Cutscene: roughly 153 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_896_200a27c
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6}
	mov	r6, r8
	push	{r6}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r10, r0
	bl	__CutsceneStart
	mov	r0, #5
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, #9
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, #0xb
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, #0xa
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, #0xe
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r0, #0xd
	mov	r1, #1
	bl	__MapActor_SetBehavior
	mov	r2, #0xa6
	mov	r0, #5
	ldr	r1, =0x1db0000
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r2, #0xa6
	mov	r0, #9
	ldr	r1, =0x1eb0000
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r2, #0xae
	mov	r0, #0xb
	ldr	r1, =0x1cb0000
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r2, #0xae
	mov	r0, #0xa
	ldr	r1, =0x1fb0000
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0xe6
	mov	r2, #0xb4
	mov	r0, #0xe
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r2, #0x99
	ldr	r1, =0x1d70000
	lsl	r2, #17
	mov	r0, #0xd
	bl	__MapActor_SetPos
	mov	r0, #5
	bl	__MapActor_GetActor
	mov	r1, r10
	str	r1, [r0, #0x68]
	mov	r2, r0
	add	r2, #0x5a
	ldrb	r3, [r2]
	mov	r6, #1
	orr	r3, r6
	strb	r3, [r2]
	ldr	r3, =gScript_896__0200cbd0
	mov	r1, #0
	mov	r8, r3
	mov	r9, r1
	mov	r1, r8
	bl	__Actor_SetScript
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r1, r10
	str	r1, [r0, #0x68]
	mov	r2, r0
	add	r2, #0x5a
	ldrb	r3, [r2]
	orr	r3, r6
	strb	r3, [r2]
	mov	r1, r8
	bl	__Actor_SetScript
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r3, r10
	str	r3, [r0, #0x68]
	mov	r2, r0
	add	r2, #0x5a
	ldrb	r3, [r2]
	orr	r3, r6
	strb	r3, [r2]
	mov	r1, r8
	bl	__Actor_SetScript
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r1, r10
	str	r1, [r0, #0x68]
	mov	r2, r0
	add	r2, #0x5a
	ldrb	r3, [r2]
	orr	r3, r6
	mov	r1, r8
	strb	r3, [r2]
	bl	__Actor_SetScript
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r3, r10
	mov	r5, r0
	str	r3, [r5, #0x68]
	mov	r2, r5
	add	r2, #0x5a
	ldrb	r3, [r2]
	orr	r3, r6
	strb	r3, [r2]
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r5, #0x18]
	str	r3, [r5, #0x1c]
	mov	r0, #0xb
	bl	__MapActor_GetActor
	add	r0, #0x55
	ldrb	r3, [r0]
	mov	r2, r5
	add	r2, #0x55
	mov	r1, r9
	strb	r3, [r2]
	mov	r0, r5
	str	r1, [r5, #0xc]
	mov	r1, r8
	bl	__Actor_SetScript
	mov	r0, #0xd
	bl	__MapActor_GetActor
	mov	r3, r10
	str	r3, [r0, #0x68]
	mov	r2, r0
	add	r2, #0x5a
	ldrb	r3, [r2]
	orr	r6, r3
	strb	r6, [r2]
	mov	r1, r8
	bl	__Actor_SetScript
	bl	__CutsceneEnd
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_896_200a27c

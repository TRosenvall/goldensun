	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 533 instructions of straight-line script --
@ 3 turns, 10 animation changes, 10 dialogue lines, 32 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x20f1, 0x214c.
.thumb_func_start OvlFunc_956_2009474
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r5, r0
	mov	r0, #0x27
	bl	__DeleteFieldActor
	mov	r0, #0x28
	bl	__DeleteFieldActor
	mov	r0, #1
	bl	__Func_807808c
	mov	r0, #0x11
	bl	__PlaySound
	bl	__CutsceneStart
	mov	r1, #0xc1
	mov	r2, #0xc0
	mov	r0, #8
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
	cmp	r5, #0
	bge	.L14b6
	mov	r0, #8
	mov	r1, #0xa
	bl	__MapActor_SetAnim
	b	.L14be
.L14b6:
	mov	r0, #8
	mov	r1, #8
	bl	__MapActor_SetAnim
.L14be:
	ldr	r1, =gScript_956__0200d668
	mov	r0, #8
	bl	__MapActor_SetBehavior
	mov	r1, #0xbc
	mov	r2, #0xc0
	lsl	r2, #16
	lsl	r1, #19
	mov	r0, #0
	bl	__MapActor_SetPos
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, #0
	strh	r5, [r0, #6]
	ldr	r1, =gScript_956__0200d738
	mov	r0, #0
	bl	__MapActor_SetBehavior
	mov	r0, #0
	mov	r1, #0x23
	bl	__MapActor_SetAnim
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
	mov	r0, #3
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xb7
	mov	r2, #0xb8
	mov	r0, #1
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xb7
	mov	r2, #0xc8
	mov	r0, #2
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xb5
	mov	r2, #0xc0
	lsl	r2, #16
	lsl	r1, #19
	mov	r0, #3
	bl	__MapActor_SetPos
	mov	r0, #1
	bl	__MapActor_GetActor
	strh	r5, [r0, #6]
	mov	r0, #2
	bl	__MapActor_GetActor
	strh	r5, [r0, #6]
	mov	r0, #3
	bl	__MapActor_GetActor
	strh	r5, [r0, #6]
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0
	mov	r1, #0
	bl	__SetCameraTarget
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	mov	r5, #0x80
	add	r3, r2
	lsl	r5, #1
	str	r5, [r3]
	mov	r1, #1
	ldr	r0, =0x10001
	bl	__Func_8091200
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	ldr	r0, =0x20f1
	bl	__MessageID
	mov	r0, #0x3c
	bl	__CutsceneWait
	ldr	r6, =gScript_956__0200d950
	mov	r0, #0
	mov	r1, r6
	bl	__MapActor_SetBehavior
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r0, #0x18]
	mov	r0, #0
	mov	r10, r3
	bl	__MapActor_GetActor
	mov	r2, r10
	mov	r1, #0x24
	str	r2, [r0, #0x1c]
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0xc0
	ldr	r3, [r0, #8]
	lsl	r2, #10
	add	r3, r2
	str	r3, [r0, #8]
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r1, =gScript_956__0200d808
	mov	r0, #0
	bl	__MapActor_SetBehavior
	mov	r1, #0
	mov	r0, #1
	bl	__ActorMessage
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xbc
	mov	r0, #1
	lsl	r1, #3
	mov	r2, #0xb0
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r2, #0x14
	mov	r1, r5
	mov	r0, #1
	bl	__MapActor_Emote
	mov	r0, #2
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #2
	mov	r0, #1
	bl	__MapActor_SetExtra
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0xba
	mov	r0, #2
	lsl	r1, #3
	mov	r2, #0xb0
	bl	__Func_80921c4
	mov	r1, #0xbe
	mov	r0, #1
	lsl	r1, #3
	mov	r2, #0xb8
	bl	__Func_809218c
	mov	r1, #0xbc
	mov	r2, #0xb0
	mov	r0, #2
	lsl	r1, #3
	bl	__Func_80921c4
	mov	r0, #1
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0xc0
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0xa
	mov	r0, #2
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r1, #2
	mov	r0, #2
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #2
	bl	__MapActor_SetExtra
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #4
	mov	r0, #1
	bl	__MapActor_DoAnim
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #3
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_SetExtra
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_SetExtra
	mov	r1, #0xba
	mov	r2, #0xb8
	mov	r0, #3
	lsl	r1, #3
	bl	__Func_80921c4
	mov	r0, #2
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r0, #1
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #1
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #2
	mov	r1, #1
	bl	__MapActor_SetExtra
	mov	r1, #2
	mov	r0, #1
	bl	__MapActor_SetExtra
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #3
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_SetExtra
	mov	r1, #3
	mov	r0, #1
	bl	__MapActor_SetExtra
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #3
	mov	r1, #0
	bl	__ActorMessage
	ldr	r1, =gScript_956__0200d8ac
	mov	r0, #0
	bl	__MapActor_SetBehavior
	mov	r0, #1
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0
	mov	r0, #1
	bl	__MapActor_SetExtra
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #2
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0
	mov	r0, #2
	bl	__MapActor_SetExtra
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #2
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #3
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0
	mov	r0, #3
	bl	__MapActor_SetExtra
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #3
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, r6
	mov	r0, #0
	bl	__MapActor_SetBehavior
	mov	r0, #0x3c
	bl	__CutsceneWait
	ldr	r3, =.L5b80
	ldr	r5, =OvlFunc_956_20093c0
	mov	r6, #9
	mov	r1, #0xc8
	str	r6, [r3]
	lsl	r1, #4
	mov	r0, r5
	mov	r8, r3
	bl	__StartTask
	mov	r0, #5
	bl	__CutsceneWait
	mov	r0, r5
	bl	__StopTask
	mov	r0, #0x37
	bl	__CutsceneWait
	mov	r2, #0x3c
	mov	r0, #1
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r5
	bl	__StartTask
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, r5
	bl	__StopTask
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #2
	ldr	r1, =0x101
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r2, r8
	mov	r1, #0xc8
	str	r6, [r2]
	lsl	r1, #4
	mov	r0, r5
	bl	__StartTask
	mov	r0, #0x23
	bl	__CutsceneWait
	mov	r0, r5
	bl	__StopTask
	mov	r0, #0x19
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #3
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r3, r8
	mov	r1, #0xc8
	str	r6, [r3]
	lsl	r1, #4
	mov	r0, r5
	bl	__StartTask
	mov	r0, #0x23
	bl	__CutsceneWait
	mov	r0, r5
	bl	__StopTask
	mov	r0, #0x19
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #2
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #3
	mov	r1, #2
	bl	__MapActor_SetExtra
	mov	r1, #3
	mov	r0, #2
	bl	__MapActor_SetExtra
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #3
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r0, #2
	mov	r1, #0
	bl	__MapActor_SetExtra
	mov	r2, r8
	mov	r1, #0xc8
	str	r6, [r2]
	lsl	r1, #4
	mov	r0, r5
	bl	__StartTask
	mov	r0, #0x23
	bl	__CutsceneWait
	mov	r0, r5
	bl	__StopTask
	mov	r0, #0x19
	bl	__CutsceneWait
	mov	r1, #0x84
	mov	r2, #0x3c
	mov	r0, #3
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #1
	mov	r1, #3
	bl	__Func_809259c
	mov	r0, #2
	mov	r1, #3
	bl	__Func_809259c
	mov	r0, #3
	mov	r1, #3
	bl	__Func_80925cc
	mov	r0, #3
	mov	r1, #2
	bl	__MapActor_SetExtra
	mov	r0, #1
	mov	r1, #2
	b	.L18e8

	.pool_aligned

.L18e8:
	bl	__MapActor_SetExtra
	mov	r3, r8
	mov	r1, #0xc8
	str	r6, [r3]
	mov	r0, r5
	lsl	r1, #4
	bl	__StartTask
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #0xb7
	lsl	r1, #3
	mov	r2, #0xc8
	mov	r0, #3
	bl	__Func_809218c
	mov	r0, #5
	bl	__CutsceneWait
	mov	r1, #0xab
	lsl	r1, #3
	mov	r2, #0xb8
	mov	r0, #2
	bl	__Func_809218c
	mov	r0, #3
	bl	__CutsceneWait
	mov	r1, #0xbd
	mov	r0, #1
	lsl	r1, #3
	mov	r2, #0xb8
	bl	__Func_80921c4
	mov	r1, #0xab
	mov	r2, #0xb8
	lsl	r1, #3
	mov	r0, #1
	bl	__Func_809218c
	mov	r0, #3
	bl	__MapActor_WaitMovement
	mov	r0, #3
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0
	mov	r0, #3
	bl	__MapActor_SetExtra
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #0xb3
	mov	r0, #3
	lsl	r1, #3
	mov	r2, #0xc8
	bl	__Func_80921c4
	mov	r1, #0xab
	lsl	r1, #3
	mov	r2, #0xb8
	mov	r0, #3
	bl	__Func_809218c
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0xbd
	mov	r2, #0xb0
	mov	r0, #1
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xb7
	mov	r2, #0xc0
	mov	r0, #2
	lsl	r1, #19
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xc3
	mov	r2, #0xc8
	lsl	r2, #16
	mov	r0, #3
	lsl	r1, #19
	bl	__MapActor_SetPos
	bl	__Func_800c5b4
	mov	r1, #2
	mov	r0, r10
	bl	__Func_8091200
	mov	r0, #1
	bl	__Func_8091254
	ldr	r0, =0x214c
	bl	__MessageID
	mov	r0, #1
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #2
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0
	mov	r0, #3
	bl	__ActorMessage
	mov	r0, #0x3c
	bl	__CutsceneWait
	bl	__CutsceneEnd
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_956_2009474

@ Cutscene: roughly 207 instructions of straight-line script --
@ 1 turn, 8 animation changes, 4 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0x20ed.
.thumb_func_start OvlFunc_956_2009a0c
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r0
	bl	__MapActor_GetActor
	mov	r3, #0xa
	ldrsh	r2, [r0, r3]
	mov	r9, r2
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r10, r2
	bl	__CutsceneStart
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, r7
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
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
	mov	r0, #3
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r3, r10
	lsl	r5, r3, #16
	mov	r2, r9
	ldr	r3, =0xffd00000
	lsl	r6, r2, #16
	mov	r1, r6
	add	r2, r5, r3
	mov	r0, #0
	bl	__MapActor_SetPos
	ldr	r3, =0xffd80000
	ldr	r2, =0xfff00000
	add	r3, r5
	mov	r8, r3
	add	r1, r6, r2
	mov	r0, #1
	mov	r2, r8
	bl	__MapActor_SetPos
	mov	r2, #0x80
	lsl	r2, #13
	add	r1, r6, r2
	mov	r0, #2
	mov	r2, r8
	bl	__MapActor_SetPos
	ldr	r3, =0xffe00000
	mov	r1, r6
	add	r2, r5, r3
	mov	r0, #3
	bl	__MapActor_SetPos
	ldr	r2, =0xffb00000
	add	r5, r2
	mov	r2, r5
	mov	r1, r6
	mov	r0, r7
	bl	__MapActor_SetPos
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r6, #0xc0
	lsl	r6, #8
	mov	r1, #0
	strh	r6, [r0, #6]
	mov	r0, #0
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	ldr	r0, =0x20ed
	bl	__MessageID
	mov	r0, r7
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r7
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r7
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r7
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #3
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #6
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1b68
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__MapActor_TravelTo
.L1b68:
	mov	r0, #2
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1b88
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #2
	bl	__MapActor_TravelTo
.L1b88:
	mov	r0, #3
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1ba8
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #3
	bl	__MapActor_TravelTo
.L1ba8:
	mov	r5, r9
	sub	r5, #0x10
	mov	r2, r10
	mov	r0, r7
	mov	r1, r5
	sub	r2, #0x40
	bl	__Func_80921c4
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #2
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #3
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r2, r10
	mov	r0, r7
	mov	r1, r5
	sub	r2, #0x10
	bl	__Func_80921c4
	mov	r0, r7
	mov	r1, r9
	mov	r2, r10
	bl	__Func_80921c4
	mov	r0, r7
	mov	r1, r6
	mov	r2, #0xa
	bl	__Func_8092adc
	bl	__CutsceneEnd
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_956_2009a0c

@ Cutscene: roughly 182 instructions of straight-line script --
@ 0 turns, 0 animation changes, 5 dialogue lines, 8 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x20b1, 0x20b2.
.thumb_func_start OvlFunc_956_2009c20
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	mov	r7, r0
	cmp	r3, #2
	bne	.L1c40
	bl	OvlFunc_common1_2c4
	b	.L1dd8
.L1c40:
	bl	__CutsceneStart
	mov	r0, r7
	mov	r1, #1
	bl	OvlFunc_common1_4cc
	mov	r6, r0
	cmp	r6, #0
	beq	.L1c54
	b	.L1db8
.L1c54:
	ldr	r0, =0x20b2
	bl	__MessageID
	mov	r0, #0xc0
	mov	r1, #0xc0
	lsl	r0, #10
	lsl	r1, #7
	bl	__Func_80933d4
	mov	r0, #0x98
	mov	r1, #1
	mov	r2, #0xc8
	mov	r3, #1
	lsl	r2, #16
	neg	r1, r1
	lsl	r0, #16
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x44
	mov	r2, #0
	mov	r0, #0x68
	bl	OvlFunc_common1_1490
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r2, #0xa
	mov	r1, #0x60
	mov	r0, #0xa8
	bl	OvlFunc_common1_14f4
	mov	r0, #0x46
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, r7
	bl	__ActorMessage
	bl	OvlFunc_common1_1550
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r3, r0
	ldr	r2, =0xcccc
	add	r3, #0x55
	strb	r6, [r3]
	ldr	r3, =0x6666
	mov	r10, r2
	str	r2, [r0, #0x30]
	mov	r2, #0x80
	mov	r8, r3
	ldr	r1, [r0, #8]
	str	r3, [r0, #0x34]
	lsl	r2, #11
	ldr	r3, [r0, #0x10]
	bl	__Actor_TravelTo
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r3, r5
	add	r3, #0x55
	strb	r6, [r3]
	mov	r2, r10
	str	r2, [r5, #0x30]
	mov	r3, r8
	mov	r2, #0x80
	ldr	r1, [r5, #8]
	lsl	r2, #14
	str	r3, [r5, #0x34]
	ldr	r3, [r5, #0x10]
	bl	__Actor_TravelTo
	mov	r0, r5
	bl	__Actor_WaitMovement
	mov	r0, #0x2d
	bl	__CutsceneWait
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r3, r0
	add	r3, #0x55
	strb	r6, [r3]
	mov	r2, r10
	str	r2, [r0, #0x30]
	mov	r3, r8
	mov	r2, #0x80
	ldr	r1, [r0, #8]
	str	r3, [r0, #0x34]
	lsl	r2, #14
	ldr	r3, [r0, #0x10]
	bl	__Actor_TravelTo
	mov	r0, #0xb
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r3, r5
	add	r3, #0x55
	strb	r6, [r3]
	mov	r2, r10
	mov	r3, r8
	str	r2, [r5, #0x30]
	mov	r2, #0x80
	lsl	r2, #11
	ldr	r1, [r5, #8]
	str	r3, [r5, #0x34]
	ldr	r3, [r5, #0x10]
	bl	__Actor_TravelTo
	mov	r0, r5
	bl	__Actor_WaitMovement
	mov	r0, #0xf
	bl	__CutsceneWait
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x44
	mov	r2, #0
	mov	r0, #0x68
	bl	OvlFunc_common1_1490
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0x60
	mov	r2, #0xa
	mov	r0, #0xa8
	bl	OvlFunc_common1_14f4
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r2, #0xa
	mov	r1, #0x44
	mov	r0, #0x68
	bl	OvlFunc_common1_14f4
	mov	r0, #0x46
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, r7
	bl	__ActorMessage
	bl	OvlFunc_common1_1550
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0
	mov	r1, #0
	bl	__SetCameraTarget
	mov	r0, r7
	mov	r1, #1
	bl	OvlFunc_common1_588
	b	.L1dca
.L1db8:
	cmp	r6, #1
	bne	.L1dca
	ldr	r0, =0x20b1
	bl	__MessageID
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
.L1dca:
	mov	r1, r7
	mov	r2, #1
	mov	r0, r6
	bl	OvlFunc_common1_5e4
	bl	__CutsceneEnd
.L1dd8:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_956_2009c20

	.include "macros.inc"

@ Cutscene: roughly 257 instructions of straight-line script --
@ 6 turns, 8 animation changes, 1 dialogue line, 12 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2410.
.thumb_func_start OvlFunc_939_200931c
	push	{r5, r6, lr}
	bl	__CutsceneStart
	mov	r1, #0xa0
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #16
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0x98
	mov	r2, #0xe0
	mov	r0, #8
	lsl	r1, #16
	lsl	r2, #15
	bl	__MapActor_SetPos
	mov	r1, #0xa8
	mov	r2, #0xe0
	mov	r0, #9
	lsl	r1, #16
	lsl	r2, #15
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #0x11
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r2, #0
	mov	r0, #0x12
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r1, #0
	mov	r0, #0
	bl	__SetCameraTarget
	bl	__MapTransitionIn
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0
	ldr	r1, =0x1cccc
	ldr	r2, =0xe666
	bl	__MapActor_SetSpeed
	mov	r0, #8
	ldr	r1, =0x1cccc
	ldr	r2, =0xe666
	bl	__MapActor_SetSpeed
	mov	r0, #9
	ldr	r1, =0x1cccc
	ldr	r2, =0xe666
	bl	__MapActor_SetSpeed
	mov	r2, #0x90
	mov	r0, #8
	mov	r1, #0x98
	lsl	r2, #1
	bl	__Func_809218c
	mov	r2, #0x90
	lsl	r2, #1
	mov	r0, #9
	mov	r1, #0xa8
	bl	__Func_809218c
	mov	r0, #0
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r2, #0x94
	lsl	r2, #1
	mov	r0, #0
	mov	r1, #0xa0
	bl	__MapActor_TravelTo
	ldr	r6, =OvlFunc_939_20092a4
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r6
	bl	__StartTask
	mov	r0, #1
	bl	__CutsceneWait
	mov	r0, #0x79
	bl	__PlaySound
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #3
	bl	__Func_8092b08
	mov	r1, #3
	mov	r0, #9
	bl	__Func_8092b08
	mov	r0, #0x79
	bl	__PlaySound
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #8
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r3, [r0]
	mov	r5, #1
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r3, [r0]
	orr	r5, r3
	mov	r1, #4
	strb	r5, [r0]
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0x79
	bl	__PlaySound
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r0, #8
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, r6
	bl	__StopTask
	mov	r0, #0
	bl	__MapActor_GetActor
	add	r0, #0x55
	ldrb	r2, [r0]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r0]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r5, #0xc0
	lsl	r5, #11
	str	r5, [r0, #0x28]
	mov	r0, #0
	bl	__MapActor_GetActor
	str	r5, [r0, #0x2c]
	mov	r0, #1
	bl	__CutsceneWait
	b	.L1480
.L147a:
	mov	r0, #1
	bl	__CutsceneWait
.L1480:
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0xc]
	cmp	r3, #0
	bne	.L147a
	mov	r1, #0xc0
	mov	r2, #0
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #0x13
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0x7f
	bl	__PlaySound
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_939_20092a4
	bl	__StartTask
	mov	r0, #2
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0xc0
	lsl	r3, #10
	str	r3, [r0, #0x28]
	mov	r0, #1
	bl	__CutsceneWait
	b	.L14da
.L14d4:
	mov	r0, #1
	bl	__CutsceneWait
.L14da:
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0xc]
	cmp	r3, #0
	bne	.L14d4
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0
	bl	__MapActor_Surprise
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #0
	bl	__MapActor_SetAnim
	ldr	r0, =OvlFunc_939_20092a4
	bl	__StopTask
	mov	r0, #0x32
	bl	__CutsceneWait
	ldr	r0, =0x2410
	bl	__MessageID
	mov	r1, #0
	mov	r0, #8
	bl	__ActorMessage
	mov	r0, #8
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r3, [r0]
	mov	r5, #1
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r3, [r0]
	mov	r1, #0x80
	orr	r5, r3
	mov	r2, #0x80
	strb	r5, [r0]
	lsl	r1, #9
	mov	r0, #8
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #9
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r0, #8
	mov	r1, #0x90
	mov	r2, #0xc8
	bl	__Func_809218c
	mov	r2, #0xc8
	mov	r1, #0xb0
	mov	r0, #9
	bl	__Func_809218c
	mov	r0, #8
	bl	__MapActor_WaitMovement
	mov	r0, #9
	bl	__MapActor_WaitMovement
	mov	r0, #8
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r0, #8
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #9
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	bl	__CutsceneEnd
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_939_200931c

@ Cutscene: roughly 62 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_939_20095bc
	push	{lr}
	bl	__CutsceneStart
	bl	__MapTransitionIn
	mov	r2, #0xa8
	mov	r1, #0x98
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x92
	mov	r1, #1
	bl	__Func_8096fb0
	mov	r1, #0
	mov	r0, #0
	bl	__Func_80970f8
	bl	__Func_809728c
	mov	r0, #1
	bl	__FieldMove
	bl	__Func_8097174
	mov	r1, #0x90
	mov	r2, #0xb8
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r1, #0x58
	mov	r2, #0xb8
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r1, #0x58
	mov	r2, #0xc8
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r1, #0x48
	mov	r2, #0xc8
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r2, #0x90
	mov	r1, #0x48
	lsl	r2, #1
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r2, #0x90
	mov	r1, #0x58
	lsl	r2, #1
	mov	r0, #0
	bl	__Func_809218c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_939_20095bc

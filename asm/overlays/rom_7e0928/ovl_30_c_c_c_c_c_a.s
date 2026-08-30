	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 155 instructions of straight-line script --
@ 0 turns, 1 animation change, 4 dialogue lines, 3 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x20c2, 0x20c3.
.thumb_func_start OvlFunc_956_200a330
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =gState
	mov	r1, #0xe1
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	mov	r7, r0
	cmp	r3, #2
	bne	.L234e
	bl	OvlFunc_common1_2c4
	b	.L24a2
.L234e:
	bl	__CutsceneStart
	mov	r0, r7
	mov	r1, #5
	bl	OvlFunc_common1_4cc
	mov	r8, r0
	cmp	r0, #0
	beq	.L2362
	b	.L2480
.L2362:
	ldr	r0, =0x20c3
	bl	__MessageID
	mov	r0, #0xc0
	mov	r1, #0xc0
	lsl	r0, #10
	lsl	r1, #7
	bl	__Func_80933d4
	mov	r0, #0x87
	mov	r1, #1
	mov	r2, #0xa8
	mov	r3, #1
	lsl	r2, #16
	neg	r1, r1
	lsl	r0, #19
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0xf6
	lsl	r1, #2
	mov	r2, #0xb8
	mov	r0, #0
	bl	OvlFunc_common1_1078
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xf8
	lsl	r1, #2
	mov	r0, #0
	mov	r2, #0xb8
	bl	OvlFunc_956_200a2f4
	mov	r0, #0
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__MapActor_SetSpeed
	mov	r1, #0x8c
	mov	r2, #0xb8
	lsl	r1, #3
	mov	r0, #0
	bl	OvlFunc_956_200a2c4
	mov	r0, #0x78
	bl	__CutsceneWait
	ldr	r1, =0x101
	mov	r0, #0
	bl	__MapActor_Surprise
	mov	r0, #0x78
	bl	__CutsceneWait
	mov	r0, #0
	bl	OvlFunc_common1_1314
	mov	r0, #0
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r0, #0
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r6, #0x77
	mov	r5, r0
.L241c:
	mov	r1, #0xf8
	ldr	r3, [r5, #8]
	lsl	r1, #18
	cmp	r3, r1
	ble	.L242c
	ldr	r2, =0xfffecccd
	add	r3, r2
	str	r3, [r5, #8]
.L242c:
	mov	r0, #1
	sub	r6, #1
	bl	__WaitFrames
	cmp	r6, #0
	bge	.L241c
	mov	r0, #0
	ldr	r1, =0x103
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r1, #0x8c
	mov	r2, #0xb8
	lsl	r1, #3
	mov	r0, #0
	bl	OvlFunc_956_200a2c4
	mov	r1, #0
	mov	r0, r7
	bl	__ActorMessage
	mov	r0, #0
	bl	OvlFunc_common1_1314
	ldr	r3, =gState
	mov	r1, #0xf9
	lsl	r1, #1
	add	r2, r3, r1
	mov	r3, #1
	strb	r3, [r2]
	mov	r0, #0
	bl	OvlFunc_common1_1254
	mov	r0, #0
	mov	r1, #0
	bl	__SetCameraTarget
	mov	r0, r7
	mov	r1, #5
	bl	OvlFunc_common1_588
	b	.L2494
.L2480:
	mov	r2, r8
	cmp	r2, #1
	bne	.L2494
	ldr	r0, =0x20c2
	bl	__MessageID
	mov	r0, r7
	mov	r1, #0
	bl	__ActorMessage
.L2494:
	mov	r1, r7
	mov	r2, #5
	mov	r0, r8
	bl	OvlFunc_common1_5e4
	bl	__CutsceneEnd
.L24a2:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_956_200a330

	.include "macros.inc"

@ Cutscene: roughly 132 instructions of straight-line script --
@ 1 turn, 9 animation changes, 3 dialogue lines, 9 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xed1, 0xed2.
@ Reads save bit 0x305.
@ Sets save bit 0x305.
.thumb_func_start OvlFunc_882_200be48
	push	{r5, r6, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r6, r0
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	ldr	r0, =0x305
	bl	__GetFlag
	cmp	r0, #0
	beq	.L3eca
	mov	r0, #8
	bl	__MapActor_SetIdle
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r2, #6
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	blt	.L3e94
	mov	r0, #8
	mov	r1, #7
	bl	__MapActor_SetAnim
	b	.L3e9c
.L3e94:
	mov	r0, #8
	mov	r1, #8
	bl	__MapActor_SetAnim
.L3e9c:
	mov	r1, #2
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0xed2
	bl	__MessageID
	mov	r0, #8
	mov	r1, #0
	bl	__ActorMessage
	ldr	r1, =gScript_882__0200cec8
	mov	r0, #8
	bl	__MapActor_SetBehavior
	mov	r0, #8
	mov	r1, #6
	bl	__MapActor_SetAnim
	b	.L3f96
.L3eca:
	mov	r0, #8
	bl	__MapActor_SetIdle
	mov	r3, #0x80
	lsl	r3, #9
	mov	r1, #0x80
	str	r3, [r5, #0x18]
	str	r3, [r5, #0x1c]
	mov	r2, #0
	mov	r0, #8
	lsl	r1, #5
	bl	__Func_8092adc
	mov	r2, #6
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	blt	.L3ef6
	mov	r0, #8
	mov	r1, #7
	bl	__MapActor_SetAnim
	b	.L3efe
.L3ef6:
	mov	r0, #8
	mov	r1, #8
	bl	__MapActor_SetAnim
.L3efe:
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0xed1
	bl	__MessageID
	mov	r2, #0x14
	mov	r0, #8
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #8
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r2, #0
	mov	r1, #4
	mov	r0, #8
	bl	__MapActor_Jump
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r2, #6
	ldrsh	r3, [r6, r2]
	cmp	r3, #0
	blt	.L3f4c
	mov	r0, #8
	mov	r1, #7
	bl	__MapActor_SetAnim
	b	.L3f54
.L3f4c:
	mov	r0, #8
	mov	r1, #8
	bl	__MapActor_SetAnim
.L3f54:
	mov	r0, #2
	bl	__CutsceneWait
	mov	r2, #0
	mov	r1, #2
	mov	r0, #8
	bl	__MapActor_Jump
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #8
	mov	r1, #0
	bl	__ActorMessage
	ldr	r1, =gScript_882__0200cec8
	mov	r0, #8
	bl	__MapActor_SetBehavior
	mov	r0, #8
	mov	r1, #6
	bl	__MapActor_SetAnim
	ldr	r0, =0x305
	bl	__SetFlag
.L3f96:
	bl	__CutsceneEnd
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_200be48

	.include "macros.inc"
	.include "gba.inc"

@ 94 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random x2, SpawnEntity, Random, SetEntityAnimation
@   SetEntityScript, UnsignedRem, Random, MoveCameraTo x2
.thumb_func_start OvlFunc_881_200b1fc
	push	{r5, r6, lr}
	bl	__Random
	lsl	r5, r0, #2
	add	r5, r0
	bl	__Random
	lsl	r5, #3
	lsl	r3, r0, #4
	sub	r3, r0
	ldr	r2, =0x17b00000
	lsr	r5, #16
	lsl	r3, #1
	lsl	r5, #16
	add	r5, r2
	lsr	r3, #16
	ldr	r2, =0xc4c0000
	lsl	r3, #16
	add	r3, r2
	mov	r0, #0xde
	mov	r1, r5
	mov	r2, #0
	bl	__CreateActor
	mov	r6, r0
	cmp	r6, #0
	beq	.L3270
	ldr	r5, [r6, #0x50]
	bl	__Random
	ldr	r3, =0x13333
	lsl	r0, #15
	lsr	r0, #16
	add	r0, r3
	mov	r3, r5
	mov	r1, #0
	add	r3, #0x26
	strb	r1, [r3]
	mov	r3, #0xd
	ldrb	r2, [r5, #9]
	neg	r3, r3
	and	r3, r2
	mov	r2, #8
	orr	r3, r2
	strb	r3, [r5, #9]
	mov	r3, r6
	add	r3, #0x55
	strb	r1, [r3]
	str	r0, [r6, #0x18]
	str	r0, [r6, #0x1c]
	mov	r1, #1
	mov	r0, r6
	bl	__Actor_SetAnim
	ldr	r1, =gScript_881__0200d14c
	mov	r0, r6
	bl	__Actor_SetScript
.L3270:
	ldr	r3, =iwram_3001e40
	mov	r1, #3
	ldr	r0, [r3]
	bl	_umodsi3_RAM
	cmp	r0, #0
	bne	.L32c6
	bl	__Random
	lsl	r0, #2
	lsr	r0, #16
	cmp	r0, #1
	beq	.L329e
	cmp	r0, #1
	bcc	.L3298
	cmp	r0, #2
	beq	.L32a8
	cmp	r0, #3
	beq	.L32b8
	b	.L32c6
.L3298:
	mov	r1, #1
	ldr	r0, =0x17c70000
	b	.L32ac
.L329e:
	mov	r1, #1
	ldr	r0, =0x17c90000
	neg	r1, r1
	ldr	r2, =0xc670000
	b	.L32b0
.L32a8:
	mov	r1, #1
	ldr	r0, =0x17c90000
.L32ac:
	neg	r1, r1
	ldr	r2, =0xc690000
.L32b0:
	mov	r3, #1
	bl	__Func_80933f8
	b	.L32c6
.L32b8:
	mov	r1, #1
	ldr	r0, =0x17c70000
	neg	r1, r1
	ldr	r2, =0xc670000
	mov	r3, #1
	bl	__Func_80933f8
.L32c6:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_200b1fc

@ Cutscene: roughly 99 instructions of straight-line script --
@ 0 turns, 2 animation changes, 0 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_881_200b2f0
	push	{lr}
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r2, r2
	neg	r1, r1
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r2, #0xca
	lsl	r2, #18
	ldr	r1, =0x13080000
	mov	r0, #8
	bl	__MapActor_SetPos
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r3, #0xa0
	lsl	r3, #8
	strh	r3, [r0, #6]
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =0x13333
	mov	r1, #1
	bl	__Func_80936a0
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	bl	__MapActor_SetPos
	mov	r1, #1
	mov	r0, #8
	bl	__SetCameraTarget
	mov	r0, #1
	bl	__WaitFrames
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	bl	__MapTransitionIn
	mov	r0, #8
	ldr	r1, =0x6666
	ldr	r2, =0x3333
	bl	__MapActor_SetSpeed
	mov	r2, #0xb2
	mov	r0, #8
	ldr	r1, =0x12d8
	lsl	r2, #2
	bl	__Func_8092158
	mov	r2, #0x9a
	mov	r0, #8
	ldr	r1, =0x12a8
	lsl	r2, #2
	bl	__Func_8092158
	mov	r0, #8
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__MapActor_SetSpeed
	mov	r2, #0xec
	mov	r0, #8
	ldr	r1, =0x12a8
	lsl	r2, #1
	bl	__Func_8092158
	mov	r0, #8
	ldr	r1, =0x3333
	ldr	r2, =0x1999
	bl	__MapActor_SetSpeed
	mov	r2, #0xe4
	mov	r0, #8
	ldr	r1, =0x1298
	lsl	r2, #1
	bl	__Func_8092158
	mov	r0, #8
	ldr	r1, =0x1999
	ldr	r2, =0xccc
	bl	__MapActor_SetSpeed
	mov	r2, #0xdc
	lsl	r2, #1
	mov	r0, #8
	ldr	r1, =0x1298
	bl	__Func_8092158
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #0x28
	bl	__CutsceneWait
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x6e
	bl	__Func_8091e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_881_200b2f0

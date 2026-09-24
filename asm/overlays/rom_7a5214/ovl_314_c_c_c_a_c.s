	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 126 instructions of straight-line script --
@ 0 turns, 25 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_918_2009424
	push	{lr}
	cmp	r0, #0xc
	bls	.L142c
	b	.L159c
.L142c:
	ldr	r2, =.L1434
	lsl	r3, r0, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L1434:
	.word	.L1468
	.word	.L14e8
	.word	.L1480
	.word	.L1498
	.word	.L14b0
	.word	.L14f2
	.word	.L159c
	.word	.L150a
	.word	.L158a
	.word	.L1522
	.word	.L153a
	.word	.L1552
	.word	.L1594
.L1468:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #3
	bl	__MapActor_SetAnim
	b	.L159c
.L1480:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #5
	bl	__MapActor_SetAnim
	b	.L159c
.L1498:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #4
	bl	__MapActor_SetAnim
	b	.L159c
.L14b0:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
.L14e8:
	mov	r0, #8
	mov	r1, #1
	bl	__MapActor_SetAnim
	b	.L159c
.L14f2:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #2
	bl	__MapActor_SetAnim
	b	.L159c
.L150a:
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #8
	bl	__MapActor_SetAnim
	b	.L159c
.L1522:
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #9
	bl	__MapActor_SetAnim
	b	.L159c
.L153a:
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #0xa
	bl	__MapActor_SetAnim
	b	.L159c
.L1552:
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #8
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #8
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
.L158a:
	mov	r0, #8
	mov	r1, #6
	bl	__MapActor_SetAnim
	b	.L159c
.L1594:
	mov	r0, #8
	mov	r1, #6
	bl	__MapActor_SetAnim
.L159c:
	mov	r0, #0xc
	bl	__WaitFrames
	pop	{r0}
	bx	r0
.func_end OvlFunc_918_2009424

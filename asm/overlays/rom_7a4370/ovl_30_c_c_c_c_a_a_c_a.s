	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 185 instructions of straight-line script --
@ 0 turns, 33 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_917_20092f4
	push	{lr}
	cmp	r0, #0xa
	beq	.L12fc
	b	.L1454
.L12fc:
	cmp	r1, #0xb
	bls	.L1302
	b	.L1518
.L1302:
	ldr	r2, =.L130c
	lsl	r3, r1, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L130c:
	.word	.L133c
	.word	.L134e
	.word	.L1352
	.word	.L1364
	.word	.L1376
	.word	.L13b2
	.word	.L13ca
	.word	.L144a
	.word	.L13e2
	.word	.L13fa
	.word	.L1412
	.word	.L144a
.L133c:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	b	.L1488
.L134e:
	mov	r0, #8
	b	.L14fa
.L1352:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	b	.L14a0
.L1364:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	b	.L14b8
.L1376:
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
	mov	r0, #8
	b	.L14fa
.L13b2:
	mov	r1, #1
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #2
	bl	__MapActor_SetAnim
	b	.L1518
.L13ca:
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #8
	bl	__MapActor_SetAnim
	b	.L1518
.L13e2:
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #9
	bl	__MapActor_SetAnim
	b	.L1518
.L13fa:
	mov	r1, #6
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #8
	mov	r1, #0xa
	bl	__MapActor_SetAnim
	b	.L1518
.L1412:
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
.L144a:
	mov	r0, #8
	mov	r1, #6
	bl	__MapActor_SetAnim
	b	.L1518
.L1454:
	cmp	r1, #5
	bhi	.L1518
	ldr	r2, =.L1460
	lsl	r3, r1, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L1460:
	.word	.L1478
	.word	.L14f8
	.word	.L1490
	.word	.L14a8
	.word	.L14c0
	.word	.L1502
.L1478:
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #9
.L1488:
	mov	r1, #3
	bl	__MapActor_SetAnim
	b	.L1518
.L1490:
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #9
.L14a0:
	mov	r1, #5
	bl	__MapActor_SetAnim
	b	.L1518
.L14a8:
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #9
.L14b8:
	mov	r1, #4
	bl	__MapActor_SetAnim
	b	.L1518
.L14c0:
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #3
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r1, #3
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
.L14f8:
	mov	r0, #9
.L14fa:
	mov	r1, #1
	bl	__MapActor_SetAnim
	b	.L1518
.L1502:
	mov	r1, #1
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #6
	bl	__WaitFrames
	mov	r0, #9
	mov	r1, #2
	bl	__MapActor_SetAnim
.L1518:
	mov	r0, #0xc
	bl	__WaitFrames
	pop	{r0}
	bx	r0
.func_end OvlFunc_917_20092f4

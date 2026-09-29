	.include "macros.inc"

@ DialogueChoiceA
@ Takes no arguments. The first branch of the 0xFD two-way prompt -- taken when
@ A is pressed. Reads the player entity from ewram_240+0x1F4 and follows the
@ affirmative path of the conversation.
.thumb_func_start Func_8093e28  @ 0x08093e28
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r0, =gState
	mov	r1, #0xfa
	mov	r8, r0
	lsl	r1, #1
	add	r1, r8
	ldr	r0, [r1]
	mov	r11, r1
	sub	sp, #0x18
	bl	MapActor_GetActor
	mov	r6, r0
	ldr	r3, =0xfff0
	mov	r2, #0xa
	ldrsh	r5, [r6, r2]
	mov	r1, #0x12
	ldrsh	r7, [r6, r1]
	and	r5, r3
	and	r7, r3
	mov	r0, #8
	mov	r2, #8
	add	r0, r5
	add	r2, r7
	mov	r10, r0
	mov	r9, r2
	bl	CutsceneStart
	mov	r3, #0xf9
	lsl	r3, #1
	add	r8, r3
	mov	r0, r8
	ldrb	r3, [r0]
	cmp	r3, #0
	bne	.L93f2e
	mov	r3, r10
	cmp	r3, #0
	bge	.L93e82
	mov	r3, r5
	add	r3, #0x17
.L93e82:
	asr	r2, r3, #4
	mov	r3, r9
	cmp	r3, #0
	bge	.L93e8e
	mov	r3, r7
	add	r3, #0x17
.L93e8e:
	asr	r3, #4
	lsl	r3, #7
	add	r3, r2, r3
	ldr	r1, =gBuffer
	ldr	r0, =ewram_2010200
	lsl	r3, #2
	add	r2, r3, r1
	add	r3, r0
	ldrb	r2, [r2, #2]
	ldrb	r3, [r3, #2]
	cmp	r2, r3
	bne	.L93f72
	ldr	r3, [r6, #8]
	mov	r0, sp
	str	r3, [r0]
	ldr	r1, =0xfff00000
	ldr	r3, [r6, #0xc]
	add	r3, r1
	str	r3, [r0, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r0, #8]
	bl	_Func_801219c
	mov	r7, r0
	cmp	r7, #0
	bne	.L93f72
	mov	r2, r11
	ldr	r0, [r2]
	mov	r1, r10
	mov	r2, r9
	bl	Func_8092158
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r6, #0x30]
	mov	r1, #0xc0
	mov	r3, r11
	mov	r2, #0
	ldr	r0, [r3]
	lsl	r1, #8
	bl	Func_8092adc
	mov	r1, r11
	ldr	r0, [r1]
	bl	MapActor_WaitScript
	mov	r3, r6
	add	r3, #0x5a
	mov	r5, #1
	strb	r5, [r3]
	sub	r3, #5
	strb	r7, [r3]
	mov	r0, r6
	mov	r1, #0
	bl	_Actor_SetSpriteFlags
	mov	r0, r6
	mov	r1, #0xd
	bl	_Actor_SetAnim
	mov	r2, r10
	lsl	r1, r2, #16
	ldr	r3, =0xfff00000
	ldr	r2, [r6, #0xc]
	mov	r0, r9
	add	r2, r3
	lsl	r3, r0, #16
	mov	r0, #0x80
	lsl	r0, #13
	add	r3, r0
	mov	r0, r6
	bl	_Actor_TravelTo
	mov	r1, r11
	ldr	r0, [r1]
	bl	MapActor_WaitMovement
	mov	r2, r8
	strb	r5, [r2]
	b	.L93f6a
.L93f2e:
	mov	r0, r6
	mov	r1, #0xa
	bl	_Actor_SetAnim
	mov	r2, r6
	add	r2, #0x55
	mov	r3, #3
	strb	r3, [r2]
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r6, #0x28]
	ldr	r3, [r6, #0xc]
	mov	r0, r6
	str	r3, [r6, #0x14]
	mov	r1, #1
	bl	_Actor_SetSpriteFlags
	mov	r0, #6
	bl	CutsceneWait
	mov	r5, #0
	mov	r3, r8
	mov	r2, r6
	strb	r5, [r3]
	add	r2, #0x5a
	mov	r3, #1
	strb	r3, [r2]
	mov	r3, #0xc0
	lsl	r3, #8
	strh	r3, [r6, #6]
.L93f6a:
	bl	CutsceneEnd
	mov	r0, #0
	b	.L93f7a
.L93f72:
	bl	CutsceneEnd
	mov	r0, #1
	neg	r0, r0
.L93f7a:
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8093e28

@ DialogueChoiceB
@ Takes no arguments. The second branch of the 0xFD prompt -- taken when B is
@ pressed. Unlike Func_93e28 it consults the save block at ewram_434 as well,
@ so the decline path can depend on stored progress.
.thumb_func_start Func_8093fa0  @ 0x08093fa0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r1, =ewram_2000434
	ldr	r0, =gState
	mov	r9, r0
	ldr	r0, [r1]
	sub	sp, #0x18
	bl	MapActor_GetActor
	mov	r7, r0
	mov	r3, #0xa
	ldrsh	r5, [r7, r3]
	mov	r1, #0x12
	ldrsh	r6, [r7, r1]
	ldr	r3, =0xfff0
	mov	r2, #1
	and	r5, r3
	and	r6, r3
	mov	r11, r2
	mov	r0, #8
	mov	r2, #8
	add	r0, r5
	add	r2, r6
	mov	r8, r0
	mov	r10, r2
	bl	CutsceneStart
	mov	r3, r7
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L93ff2
	ldr	r3, [r7, #0x50]
	add	r3, #0x26
	ldrb	r3, [r3]
	mov	r11, r3
.L93ff2:
	mov	r3, #0xf9
	lsl	r3, #1
	add	r9, r3
	mov	r0, r9
	ldrb	r3, [r0]
	cmp	r3, #0
	bne	.L940b8
	mov	r3, r8
	cmp	r3, #0
	bge	.L9400a
	mov	r3, r5
	add	r3, #0x17
.L9400a:
	asr	r2, r3, #4
	mov	r3, r10
	cmp	r3, #0
	bge	.L94016
	mov	r3, r6
	add	r3, #0x17
.L94016:
	asr	r3, #4
	lsl	r3, #7
	add	r3, r2, r3
	ldr	r1, =gBuffer
	ldr	r0, =ewram_200fe00
	lsl	r3, #2
	add	r2, r3, r1
	add	r3, r0
	ldrb	r2, [r2, #2]
	ldrb	r3, [r3, #2]
	cmp	r2, r3
	beq	.L94030
	b	.L94138
.L94030:
	ldr	r3, [r7, #8]
	mov	r0, sp
	str	r3, [r0]
	ldr	r3, [r7, #0xc]
	str	r3, [r0, #4]
	ldr	r3, [r7, #0x10]
	str	r3, [r0, #8]
	bl	_Func_801219c
	mov	r5, r0
	cmp	r5, #0
	bne	.L94138
	mov	r6, r7
	add	r6, #0x5a
	ldr	r1, =ewram_2000434
	strb	r5, [r6]
	mov	r2, r10
	ldr	r0, [r1]
	mov	r1, r8
	bl	Func_8092158
	mov	r1, #6
	mov	r0, r7
	bl	_Actor_SetAnim
	mov	r0, #4
	bl	WaitFrames
	mov	r1, #7
	mov	r0, r7
	bl	_Actor_SetAnim
	mov	r3, #0x80
	lsl	r3, #11
	str	r3, [r7, #0x28]
	mov	r0, #4
	bl	WaitFrames
	mov	r3, r7
	add	r3, #0x55
	strb	r5, [r3]
	mov	r2, r11
	mov	r3, #0xfe
	and	r2, r3
	mov	r11, r2
	mov	r0, r7
	mov	r1, r11
	bl	_Actor_SetSpriteFlags
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r7, #0x30]
	mov	r0, r7
	mov	r1, #0xc
	str	r5, [r7, #0x28]
	bl	_Actor_SetAnim
	mov	r0, #4
	bl	WaitFrames
	mov	r3, #1
	mov	r0, r9
	strb	r3, [r0]
	strb	r3, [r6]
	mov	r0, #8
	bl	WaitFrames
	b	.L94112
.L940b8:
	mov	r5, r7
	add	r5, #0x55
	mov	r6, #0
	strb	r6, [r5]
	mov	r0, r7
	mov	r1, #0xb
	bl	_Actor_SetAnim
	mov	r2, r8
	lsl	r1, r2, #16
	mov	r3, #0x80
	ldr	r2, [r7, #0xc]
	lsl	r3, #12
	mov	r0, r10
	add	r2, r3
	lsl	r3, r0, #16
	ldr	r0, =0xfff00000
	add	r3, r0
	mov	r0, r7
	bl	_Actor_TravelTo
	ldr	r1, =ewram_2000434
	ldr	r0, [r1]
	bl	MapActor_WaitMovement
	mov	r3, #3
	strb	r3, [r5]
	ldr	r5, .L9411c	@ 1
	mov	r2, r11
	ldr	r3, [r7, #0xc]
	orr	r2, r5
	mov	r11, r2
	str	r3, [r7, #0x14]
	mov	r0, r7
	mov	r1, r11
	bl	_Actor_SetSpriteFlags
	mov	r0, #4
	bl	CutsceneWait
	mov	r3, r9
	strb	r6, [r3]
	mov	r3, r7
	add	r3, #0x5a
	strb	r5, [r3]
.L94112:
	bl	CutsceneEnd
	mov	r0, #0
	b	.L94140

	.align	2, 0
.L9411c:
	.word	1
	.pool

.L94138:
	bl	CutsceneEnd
	mov	r0, #1
	neg	r0, r0
.L94140:
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8093fa0

@ IsSlotWithinCameraView
@ r0=slot, r1=margin. Returns 0 when the slot entity lies inside the current
@ camera view expanded by r1, and -1 when it does not or the slot is empty.
.thumb_func_start Func_8094154  @ 0x08094154
	push	{r5, r6, lr}
	mov	r5, r1
	bl	GetFieldActor
	mov	r4, r0
	cmp	r4, #0
	bne	.L94168
	mov	r0, #1
	neg	r0, r0
	b	.L941c8
.L94168:
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	add	r3, #0xe4
	ldr	r1, =0xffff0000
	ldr	r0, [r3]
	ldr	r2, [r3, #4]
	ldr	r3, [r4, #0x10]
	and	r2, r1
	and	r0, r1
	ldr	r1, [r4, #8]
	sub	r3, r2
	ldr	r2, [r4, #0xc]
	sub	r1, r0
	sub	r6, r3, r2
	mov	r2, r5
	add	r5, #4
	cmp	r1, #0
	bge	.L94190
	ldr	r3, =0xffff
	add	r1, r3
.L94190:
	asr	r3, r1, #16
	str	r3, [r2]
	mov	r3, r6
	cmp	r3, #0
	bge	.L9419e
	ldr	r2, =0xffff
	add	r3, r2
.L9419e:
	asr	r3, #16
	str	r3, [r5]
	mov	r3, r4
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.L941c6
	ldr	r3, [r4, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	bl	_GetSpriteInfo
	ldr	r3, [r5]
	mov	r2, #8
	ldrsb	r2, [r0, r2]
	sub	r3, r2
	str	r3, [r5]
.L941c6:
	mov	r0, #0
.L941c8:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_8094154

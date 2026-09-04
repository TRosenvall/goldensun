	.include "macros.inc"

@ RunPartySelectInput
@ Takes no arguments. As Func_1b398 but for the character-selection mode; plays
@ the move and confirm sounds through _Func_f9080.
.thumb_func_start Func_801b424  @ 0x0801b424
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001e98
	ldr	r5, [r3]
	mov	r6, r0
.L1b42c:
	mov	r0, #1
	bl	WaitFrames
	mov	r1, #0xe8
	lsl	r1, #2
	add	r3, r5, r1
	ldrh	r3, [r3]
	cmp	r3, #0
	bne	.L1b42c
	ldr	r2, =0x3e7
	cmp	r6, r2
	beq	.L1b4bc
	ldr	r1, =gKeyRepeat
	ldr	r3, [r1]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.L1b45e
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r0, r5
	bl	Func_801b664
	b	.L1b474
.L1b45e:
	ldr	r3, [r1]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.L1b474
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r0, r5
	bl	Func_801b810
.L1b474:
	ldr	r3, =gKeyPress
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L1b4bc
	mov	r1, #0xe7
	lsl	r1, #2
	add	r3, r5, r1
	add	r1, #2
	ldrh	r2, [r3]
	add	r3, r5, r1
	ldrh	r3, [r3]
	add	r6, r2, r3
	mov	r2, #0xd2
	lsl	r2, #2
	add	r3, r5, r2
	ldr	r3, [r3]
	ldrh	r3, [r3, #0xa]
	cmp	r3, #6
	bne	.L1b4b2
	cmp	r6, #0
	bne	.L1b4aa
	mov	r0, #0x70
	bl	_PlaySound
	b	.L1b4b8
.L1b4aa:
	mov	r0, #0x71
	bl	_PlaySound
	b	.L1b4b8
.L1b4b2:
	mov	r0, #0x70
	bl	_PlaySound
.L1b4b8:
	mov	r0, r6
	b	.L1b4d6
.L1b4bc:
	cmp	r6, #0
	beq	.L1b42c
	ldr	r3, =gKeyPress
	ldr	r3, [r3]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.L1b42c
	mov	r0, #0x71
	bl	_PlaySound
	mov	r0, #1
	neg	r0, r0
.L1b4d6:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_801b424

@ ScrollPartyListUp
@ r0.. = parameters. Moves the party list selection up one, repainting through
@ Func_1b010 and Func_1ba68, one frame per step via WaitFrames.
.thumb_func_start Func_801b4ec  @ 0x0801b4ec
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	ldr	r1, =0x39e
	mov	r0, #0xe7
	lsl	r0, #2
	add	r7, r5, r0
	add	r6, r5, r1
	ldrh	r3, [r7]
	ldrh	r1, [r6]
	mov	r2, #0xe5
	add	r3, r1
	lsl	r2, #2
	add	r3, #1
	add	r2, r5
	mov	r10, r3
	ldrh	r3, [r2]
	mov	r8, r2
	cmp	r10, r3
	beq	.L1b5aa
	mov	r0, r5
	bl	Func_801b9a8
	ldr	r3, =0x3a2
	add	r2, r5, r3
	mov	r3, #0x21
	strh	r3, [r2]
	mov	r0, #1
	bl	WaitFrames
	ldrh	r1, [r6]
	mov	r0, #0x80
	add	r3, r1, #1
	strh	r3, [r6]
	lsl	r0, #11
	lsl	r3, #16
	cmp	r3, r0
	bne	.L1b57a
	mov	r0, r8
	mov	r2, r10
	ldrh	r3, [r0]
	add	r2, #1
	cmp	r2, r3
	bcs	.L1b57a
	mov	r2, #0x80
	lsl	r2, #9
	add	r3, r1, r2
	strh	r3, [r6]
	mov	r3, #8
	strh	r3, [r5, #0x3c]
	ldrh	r3, [r7]
	add	r3, #1
	strh	r3, [r7]
	mov	r0, r5
	mov	r1, #1
	bl	Func_801ba68
	ldrh	r2, [r6]
	ldrh	r3, [r7]
	mov	r0, r8
	add	r3, r2
	ldrh	r2, [r0]
	add	r3, #2
	cmp	r3, r2
	bne	.L1b576
	mov	r3, #0
	strh	r3, [r5, #0x3e]
.L1b576:
	mov	r3, #1
	strh	r3, [r5, #0xa]
.L1b57a:
	ldr	r1, =0x3a2
	mov	r3, #1
	add	r2, r5, r1
	strh	r3, [r2]
	ldr	r2, =0x39e
	add	r3, r5, r2
	ldrh	r1, [r3]
	mov	r0, r5
	bl	Func_801b9ec
	mov	r0, #1
	bl	WaitFrames
	mov	r0, #0xd2
	lsl	r0, #2
	add	r3, r5, r0
	ldr	r3, [r3]
	mov	r1, #0
	ldrh	r0, [r3, #0xa]
	bl	Func_801b010
	mov	r0, #1
	bl	WaitFrames
.L1b5aa:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_801b4ec

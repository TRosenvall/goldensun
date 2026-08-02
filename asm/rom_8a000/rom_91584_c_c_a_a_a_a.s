	.include "macros.inc"

@ WaitForAnimationEnd
@ r0=slot, r1=animation index. Blocks while the slot's actor is still playing
@ animation r1, checking the actor's current-animation byte at +0x24 once per
@ frame. Gives up after 0x5A (90) frames, and returns immediately if the slot is
@ empty or not draw kind 1.
.thumb_func_start MapActor_WaitAnim  @ 0x08091c44
	push	{r5, r6, r7, lr}
	mov	r7, r1
	bl	GetFieldActor
	cmp	r0, #0
	beq	.L91c76
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L91c76
	ldr	r3, [r0, #0x50]
	mov	r6, r3
	mov	r5, #0
	add	r6, #0x24
	b	.L91c66
.L91c64:
	add	r5, #1
.L91c66:
	cmp	r5, #0x59
	bgt	.L91c76
	mov	r0, #1
	bl	WaitFrames
	ldrb	r3, [r6]
	cmp	r7, r3
	beq	.L91c64
.L91c76:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end MapActor_WaitAnim

@ RunFieldAbilityPrompt
@ r0=slot, r1=force-simple flag. Returns the prompt result.
@ First drains any held buttons (spins on iwram_1c94 until released) and waits
@ for _Func_17364 to report the message window ready, then pauses 3 frames.
@ When r1 is 0 it compares the combined stat at +0x0E plus +0x0A for the two
@ party records at +0x1F8 and +0x1FC and picks the lower; a value above 15
@ downgrades the prompt style passed to _Func_28df4.
@ The prompt itself is _Func_28df4 with the two counters at iwram_1ebc+0xCC2
@ and +0xCC4. A non-zero result selects animation 4 on the slot, a zero result
@ animation 3, each via Func_924d4 then _Func_19e48 / _Func_19a54 and a
@ MapActor_WaitAnim wait for that animation to finish.
.thumb_func_start Func_8091c7c  @ 0x08091c7c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001ebc
	mov	r8, r0
	ldr	r6, [r3]
	mov	r0, #0xfa
	lsl	r0, #1
	add	r3, r6, r0
	ldr	r0, [r3]
	sub	sp, #4
	mov	r9, r1
	bl	Func_808d394
	mov	r2, #0
	ldrsh	r1, [r0, r2]
	mov	r0, #0xfc
	lsl	r0, #1
	add	r3, r6, r0
	ldr	r3, [r3]
	mov	r11, r1
	mov	r1, #0xfe
	lsl	r1, #1
	mov	r2, #1
	mov	r10, r3
	add	r3, r6, r1
	ldr	r5, [r3]
	str	r2, [sp]
	ldr	r2, =gKeyPress
	ldr	r3, [r2]
	cmp	r3, #0
	beq	.L91cda
	mov	r7, r2
.L91cc6:
	mov	r0, #1
	bl	WaitFrames
	ldr	r3, [r7]
	cmp	r3, #0
	bne	.L91cc6
	b	.L91cda
.L91cd4:
	mov	r0, #1
	bl	WaitFrames
.L91cda:
	bl	_Func_8017364
	cmp	r0, #0
	beq	.L91cd4
	mov	r0, #3
	bl	WaitFrames
	mov	r3, r9
	cmp	r3, #0
	bne	.L91d0e
	mov	r0, r10
	ldrh	r2, [r0, #0xe]
	ldrh	r3, [r0, #0xa]
	add	r1, r2, r3
	cmp	r5, #0
	beq	.L91d06
	ldrh	r2, [r5, #0xe]
	ldrh	r3, [r5, #0xa]
	add	r2, r3
	cmp	r1, r2
	bge	.L91d06
	mov	r1, r2
.L91d06:
	cmp	r1, #0xf
	ble	.L91d0e
	mov	r1, #0
	str	r1, [sp]
.L91d0e:
	ldr	r2, =0xcc2
	add	r3, r6, r2
	add	r2, #2
	mov	r0, #0
	ldrsh	r1, [r3, r0]
	add	r3, r6, r2
	mov	r0, #0
	ldrsh	r2, [r3, r0]
	ldr	r0, [sp]
	mov	r3, #0
	bl	_YesNoMenu
	mov	r5, r0
	cmp	r5, #0
	beq	.L91d48
	mov	r1, #4
	mov	r0, r8
	bl	MapActor_SetAnim
	mov	r0, r11
	bl	_Func_8019e48
	bl	_Func_8019a54
	mov	r0, r8
	mov	r1, #4
	bl	MapActor_WaitAnim
	b	.L91d62
.L91d48:
	mov	r1, #3
	mov	r0, r8
	bl	MapActor_SetAnim
	mov	r0, r11
	bl	_Func_8019e48
	bl	_Func_8019a54
	mov	r0, r8
	mov	r1, #3
	bl	MapActor_WaitAnim
.L91d62:
	mov	r0, r5
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8091c7c


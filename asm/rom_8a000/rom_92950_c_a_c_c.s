	.include "macros.inc"

@ ShowMessageWithPrompt
@ r0=speaker slot, r1=style flags. Returns the player's choice.
@ Opens the box with Func_92c40, puts the yes/no prompt up through
@ Func_91c7c against the player entity, then shows the follow-up line with
@ ActorMessage. The message id at +0x1D8 is advanced by one either way, but the
@ order differs: a non-zero choice advances before the follow-up, a zero choice
@ after -- so the two branches select different lines.
.thumb_func_start Func_8093054  @ 0x08093054
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r5, r0
	bl	Func_8092c40
	ldr	r3, =gState
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	mov	r1, #0
	bl	Func_8091c7c
	mov	r7, r0
	cmp	r7, #0
	bne	.L9308e
	mov	r0, r5
	mov	r1, r6
	bl	ActorMessage
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	b	.L930a6
.L9308e:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, r5
	mov	r1, r6
	bl	ActorMessage
.L930a6:
	mov	r0, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8093054


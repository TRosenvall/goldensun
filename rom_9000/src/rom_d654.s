	.include "macros.inc"

@ ScriptOp_Wait
@ Script opcode handler. r0=entity. Loads the operand at script[cursor+1] as a
@ frame count and writes count-1 to the wait timer at +0x5E, advances the cursor
@ by 2, and returns 0 so the VM stops for this frame. The update loop decrements
@ +0x5E each frame and only resumes the script when it reaches zero.
.thumb_func_start Func_d654
	mov	r2, #4
	ldrsh	r3, [r0, r2]
	ldr	r2, [r0]
	lsl	r3, #2
	add	r3, r2
	ldr	r3, [r3, #4]
	mov	r2, r0
	sub	r3, #1
	add	r2, #0x5e
	strh	r3, [r2]
	ldrh	r3, [r0, #4]
	add	r3, #2
	strh	r3, [r0, #4]
	mov	r0, #0
	bx	lr
.func_end Func_d654

@ ScriptOp_WaitForIdle
@ Script opcode handler. r0=entity. Blocks the script until Func_ca98 reports
@ every movement target cleared, returning 0 each frame until then. Advances
@ the cursor by 1 and returns 1 once idle.
@ The blocked-frame counter at +0x60 acts as an escape hatch: once it passes
@ 0x3B (59) the handler resets it and proceeds regardless, so an entity wedged
@ against geometry cannot stall its script forever.
.thumb_func_start Func_d674
	push	{r5, lr}
	mov	r5, r0
	mov	r2, r5
	add	r2, #0x60
	ldrb	r3, [r2]
	cmp	r3, #0x3b
	bls	.Ld688
	mov	r3, #0
	strb	r3, [r2]
	b	.Ld692
.Ld688:
	mov	r0, r5
	bl	Func_ca98
	cmp	r0, #0
	beq	.Ld69c
.Ld692:
	ldrh	r3, [r5, #4]
	add	r3, #1
	strh	r3, [r5, #4]
	mov	r0, #1
	b	.Ld69e
.Ld69c:
	mov	r0, #0
.Ld69e:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d674

@ ScriptOp_CallPredicate
@ Script opcode handler. r0=entity. Treats the operand at script[cursor+1] as a
@ function pointer and calls it with the entity. A non-zero result means "not
@ finished": the handler returns 0 and the same opcode runs again next frame.
@ On a zero result it advances the cursor by 2 -- but only if the callee did not
@ move the cursor itself, so a predicate is free to jump.
.thumb_func_start Func_d6a4
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r1, #4
	ldrsh	r6, [r5, r1]
	ldr	r2, [r5]
	lsl	r3, r6, #2
	add	r3, r2
	ldr	r3, [r3, #4]
	bl	_call_via_r3
	cmp	r0, #0
	beq	.Ld6c0
	mov	r0, #0
	b	.Ld6d0
.Ld6c0:
	mov	r1, #4
	ldrsh	r3, [r5, r1]
	ldrh	r2, [r5, #4]
	cmp	r3, r6
	bne	.Ld6ce
	add	r3, r2, #2
	strh	r3, [r5, #4]
.Ld6ce:
	mov	r0, #1
.Ld6d0:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_d6a4

@ FindScriptLabel
@ r0=entity, r1=label id. Helper shared by every jump opcode below; not itself a
@ dispatch-table entry. Clears the wait timer at +0x5E, then scans the script
@ from its base for a word equal to the label (with bit 30 masked off) and
@ returns the cursor position just past it. Returns 0 -- the start of the script
@ -- for a null label or after 0x400 words without a match.
.thumb_func_start Func_d6d8
	push	{lr}
	mov	r2, r0
	add	r2, #0x5e
	mov	r3, #0
	strh	r3, [r2]
	cmp	r1, #0
	bne	.Ld6ec
	b	.Ld702
.Ld6e8:
	add	r0, #1
	b	.Ld704
.Ld6ec:
	ldr	r3, =0xbfffffff
	ldr	r4, =0x3ff
	ldr	r2, [r0]
	and	r1, r3
	mov	r0, #0
.Ld6f6:
	ldmia	r2!, {r3}
	cmp	r3, r1
	beq	.Ld6e8
	add	r0, #1
	cmp	r0, r4
	ble	.Ld6f6
.Ld702:
	mov	r0, #0
.Ld704:
	pop	{r1}
	bx	r1
.func_end Func_d6d8

@ ScriptOp_LoopN
@ Script opcode handler. r0=entity. Two operands: an iteration count at
@ script[cursor+1] and a target label at script[cursor+2].
@ A count of 0xFFFF means loop forever and always jumps. Otherwise the iteration
@ counter at +0x5D is incremented and compared (as a byte) against the count: if
@ it is still lower, control jumps back to the label via Func_d6d8; once it
@ reaches the count the counter is reset to 0 and the cursor advances by 3 to
@ fall out of the loop. Always returns 1.
.thumb_func_start Func_d710
	push	{r5, lr}
	mov	r5, r0
	mov	r3, #4
	ldrsh	r2, [r5, r3]
	ldr	r3, [r5]
	lsl	r2, #2
	add	r3, r2
	add	r3, #4
	ldmia	r3!, {r4}
	ldr	r1, [r3]
	ldr	r3, =0xffff
	cmp	r4, r3
	beq	.Ld742
	mov	r0, r5
	add	r0, #0x5d
	ldrb	r2, [r0]
	add	r2, #1
	strb	r2, [r0]
	lsl	r3, r4, #16
	lsl	r2, #24
	lsr	r2, #24
	asr	r3, #16
	cmp	r2, r3
	bge	.Ld74a
	mov	r0, r5
.Ld742:
	bl	Func_d6d8
	strh	r0, [r5, #4]
	b	.Ld754
.Ld74a:
	mov	r3, #0
	strb	r3, [r0]
	ldrh	r3, [r5, #4]
	add	r3, #3
	strh	r3, [r5, #4]
.Ld754:
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d710

@ ScriptOp_Jump
@ Script opcode handler. r0=entity. Unconditional jump: resolves the label at
@ script[cursor+1] with Func_d6d8 and sets the cursor to the result. Returns 1.
.thumb_func_start Func_d760
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r1, [r3, #4]
	bl	Func_d6d8
	strh	r0, [r5, #4]
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d760

@ ScriptOp_JumpIfTrue
@ Script opcode handler. r0=entity. Jumps to the label at script[cursor+1] when
@ the condition byte at +0x57 is non-zero, otherwise advances the cursor by 2.
@ The condition is set by the event-flag opcodes below. Returns 1.
.thumb_func_start Func_d780
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r1, [r3, #4]
	mov	r3, r5
	add	r3, #0x57
	ldrb	r3, [r3]
	ldrh	r0, [r5, #4]
	cmp	r3, #0
	beq	.Ld7a6
	mov	r0, r5
	bl	Func_d6d8
	strh	r0, [r5, #4]
	b	.Ld7aa
.Ld7a6:
	add	r3, r0, #2
	strh	r3, [r5, #4]
.Ld7aa:
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d780

@ ScriptOp_JumpIfFalse
@ Script opcode handler. r0=entity. The inverse of Func_d780: jumps to the label
@ at script[cursor+1] when the condition byte at +0x57 is zero, otherwise
@ advances the cursor by 2. Returns 1.
.thumb_func_start Func_d7b4
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r1, [r3, #4]
	mov	r3, r5
	add	r3, #0x57
	ldrb	r3, [r3]
	ldrh	r0, [r5, #4]
	cmp	r3, #0
	bne	.Ld7da
	mov	r0, r5
	bl	Func_d6d8
	strh	r0, [r5, #4]
	b	.Ld7de
.Ld7da:
	add	r3, r0, #2
	strh	r3, [r5, #4]
.Ld7de:
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d7b4

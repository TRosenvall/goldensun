	.include "macros.inc"

@ ScriptOp_JumpIfTrue
@ Script opcode handler. r0=entity. Jumps to the label at script[cursor+1] when
@ the condition byte at +0x57 is non-zero, otherwise advances the cursor by 2.
@ The condition is set by the event-flag opcodes below. Returns 1.
.thumb_func_start ActorCmd_GotoIfNZ  @ 0x0800d780
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
	bl	Actor_FindScriptMarker
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
.func_end ActorCmd_GotoIfNZ

@ ScriptOp_JumpIfFalse
@ Script opcode handler. r0=entity. The inverse of ActorCmd_GotoIfNZ: jumps to the label
@ at script[cursor+1] when the condition byte at +0x57 is zero, otherwise
@ advances the cursor by 2. Returns 1.
.thumb_func_start ActorCmd_GotoIfZ  @ 0x0800d7b4
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
	bl	Actor_FindScriptMarker
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
.func_end ActorCmd_GotoIfZ


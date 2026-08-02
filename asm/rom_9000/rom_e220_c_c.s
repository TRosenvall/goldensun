	.include "macros.inc"

@ ScriptOp_SetField
@ Script opcode handler. r0=entity. Reads a field id from script[cursor+1]
@ and a value from script[cursor+2], looks the id up in the accessor table
@ Data_136e0, and calls it with operation 0 to set the field. A null table
@ entry is skipped silently. Advances the cursor by 3 and returns 1.
.thumb_func_start ActorCmd_SetAttr  @ 0x0800e9a0
	push	{r5, lr}
	mov	r5, r0
	mov	r3, #4
	ldrsh	r2, [r5, r3]
	ldr	r3, [r5]
	lsl	r2, #2
	add	r3, r2
	add	r1, r3, #4
	ldr	r3, [r1]
	ldr	r2, =Data_80136e0
	lsl	r3, #2
	ldr	r3, [r2, r3]
	ldrh	r0, [r5, #4]
	cmp	r3, #0
	beq	.Le9ca
	ldr	r2, [r1, #4]
	mov	r0, r5
	mov	r1, #0
	bl	_call_via_r3
	ldrh	r0, [r5, #4]
.Le9ca:
	add	r3, r0, #3
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end ActorCmd_SetAttr

@ ScriptOp_AddField
@ Script opcode handler. r0=entity. Reads a field id from script[cursor+1]
@ and a value from script[cursor+2], looks the id up in the accessor table
@ Data_136e0, and calls it with operation 1 to add to the field. A null table
@ entry is skipped silently. Advances the cursor by 3 and returns 1.
.thumb_func_start ActorCmd_IncAttr  @ 0x0800e9dc
	push	{r5, lr}
	mov	r5, r0
	mov	r3, #4
	ldrsh	r2, [r5, r3]
	ldr	r3, [r5]
	lsl	r2, #2
	add	r3, r2
	add	r1, r3, #4
	ldr	r3, [r1]
	ldr	r2, =Data_80136e0
	lsl	r3, #2
	ldr	r3, [r2, r3]
	ldrh	r0, [r5, #4]
	cmp	r3, #0
	beq	.Lea06
	ldr	r2, [r1, #4]
	mov	r0, r5
	mov	r1, #1
	bl	_call_via_r3
	ldrh	r0, [r5, #4]
.Lea06:
	add	r3, r0, #3
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end ActorCmd_IncAttr

@ ScriptOp_CompareField
@ Script opcode handler. r0=entity. Reads a field id from script[cursor+1]
@ and a value from script[cursor+2], looks the id up in the accessor table
@ Data_136e0, and calls it with operation 2 to compare the field. A null table
@ entry is skipped silently. Advances the cursor by 3 and returns 1.
.thumb_func_start ActorCmd_CmpAttr  @ 0x0800ea18
	push	{r5, lr}
	mov	r5, r0
	mov	r3, #4
	ldrsh	r2, [r5, r3]
	ldr	r3, [r5]
	lsl	r2, #2
	add	r3, r2
	add	r1, r3, #4
	ldr	r3, [r1]
	ldr	r2, =Data_80136e0
	lsl	r3, #2
	ldr	r3, [r2, r3]
	ldrh	r0, [r5, #4]
	cmp	r3, #0
	beq	.Lea42
	ldr	r2, [r1, #4]
	mov	r0, r5
	mov	r1, #2
	bl	_call_via_r3
	ldrh	r0, [r5, #4]
.Lea42:
	add	r3, r0, #3
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end ActorCmd_CmpAttr

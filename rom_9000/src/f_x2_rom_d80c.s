	.include "macros.inc"
	.include "gba.inc"

@ ScriptOp_TestEventFlag
@ Script opcode handler. r0=entity. Reads the global event flag named by the
@ operand at script[cursor+1] with _Func_79338 and stores the result in the
@ condition byte at +0x57 for a following Func_d780 / Func_d7b4. Advances the
@ cursor by 2 and returns 1.
.thumb_func_start Func_d7f8
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r0, [r3, #4]
	bl	_Func_79338
	mov	r3, r5
	add	r3, #0x57
	strb	r0, [r3]
	ldrh	r3, [r5, #4]
	add	r3, #2
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d7f8

@ ScriptOp_TestAndSetEventFlag
@ Script opcode handler. r0=entity. Reads the event flag named by the operand
@ into the condition byte at +0x57 (as Func_d7f8 does), then sets that flag with
@ _Func_79358 -- so the condition reflects the value *before* the write.
@ Advances the cursor by 2 and returns 1.
.thumb_func_start Func_d820
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r6, [r3, #4]
	mov	r0, r6
	bl	_Func_79338
	mov	r3, r5
	add	r3, #0x57
	strb	r0, [r3]
	mov	r0, r6
	bl	_Func_79358
	ldrh	r3, [r5, #4]
	add	r3, #2
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_d820

@ ScriptOp_TestAndClearEventFlag
@ Script opcode handler. r0=entity. As Func_d820 but calls _Func_79374 to clear
@ the flag after capturing its previous value into +0x57. Advances the cursor by
@ 2 and returns 1.
.thumb_func_start Func_d850
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r6, [r3, #4]
	mov	r0, r6
	bl	_Func_79338
	mov	r3, r5
	add	r3, #0x57
	strb	r0, [r3]
	mov	r0, r6
	bl	_Func_79374
	ldrh	r3, [r5, #4]
	add	r3, #2
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_d850

@ ScriptOp_ToggleEventFlag
@ Script opcode handler. r0=entity. Reads the event flag named by the operand
@ into the condition byte at +0x57, then flips it: a previous value of exactly 1
@ is cleared with _Func_79374, anything else is set with _Func_79358. Advances
@ the cursor by 2 and returns 1.
.thumb_func_start Func_d880
	push	{r5, r6, lr}
	mov	r6, r0
	mov	r2, #4
	ldrsh	r3, [r6, r2]
	ldr	r2, [r6]
	lsl	r3, #2
	add	r3, r2
	ldr	r5, [r3, #4]
	mov	r0, r5
	bl	_Func_79338
	mov	r3, r6
	add	r3, #0x57
	strb	r0, [r3]
	mov	r3, #0x80
	lsl	r0, #24
	lsl	r3, #17
	cmp	r0, r3
	bne	.Ld8ae
	mov	r0, r5
	bl	_Func_79374
	b	.Ld8b4
.Ld8ae:
	mov	r0, r5
	bl	_Func_79358
.Ld8b4:
	ldrh	r3, [r6, #4]
	add	r3, #2
	strh	r3, [r6, #4]
	mov	r0, #1
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_d880

@ ScriptOp_SetAnimation
@ Script opcode handler. r0=entity. Passes the operand at script[cursor+1] to
@ Func_c300, switching the animation on the entity's actor or actor array.
@ Advances the cursor by 2 and returns 1.
.thumb_func_start Func_d8c4
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	add	r3, r2
	ldr	r1, [r3, #4]
	bl	Func_c300
	ldrh	r3, [r5, #4]
	add	r3, #2
	strh	r3, [r5, #4]
	mov	r0, #1
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_d8c4

@ ScriptOp_DestroySelf
@ Script opcode handler. r0=entity. Tears the entity down with Func_c0f4 --
@ releasing its actors and zeroing the slot -- and returns 0. The cursor is not
@ advanced because the entity no longer exists.
.thumb_func_start Func_d8e8
	push	{lr}
	bl	Func_c0f4
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end Func_d8e8

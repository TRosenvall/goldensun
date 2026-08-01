	.include "macros.inc"
	.include "gba.inc"

@ Sub_de9bc
@ Battle animation routine, 20 instructions.
@ Calls out to: _Func_b7dd0, _Func_c300, _Func_c344.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Func_de9bc
	push	{r5, r6, lr}
	mov	r6, r0
	ldr	r0, [r6, #8]
	bl	_Func_b7dd0
	ldr	r5, [r0]
	mov	r1, #2
	mov	r0, r5
	bl	_Func_c300
	mov	r0, r5
	mov	r1, #0x30
	bl	_Func_c344
	mov	r0, r6
	mov	r1, #5
	bl	Func_dea70
	mov	r0, r5
	mov	r1, #0x10
	bl	_Func_c344
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_de9bc

@ Sub_de9f0
@ Battle animation routine, 20 instructions.
@ Calls out to: _Func_b7dd0, _Func_c300, _Func_c344.
@ Body NOT traced instruction by instruction -- the facts above are extracted
@ from the code; the behavioural detail is not yet documented.
.thumb_func_start Func_de9f0
	push	{r5, r6, lr}
	mov	r6, r0
	ldr	r0, [r6, #8]
	bl	_Func_b7dd0
	ldr	r5, [r0]
	mov	r1, #2
	mov	r0, r5
	bl	_Func_c300
	mov	r0, r5
	mov	r1, #0x30
	bl	_Func_c344
	mov	r0, r6
	mov	r1, #6
	bl	Func_dea70
	mov	r0, r5
	mov	r1, #0x10
	bl	_Func_c344
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_de9f0

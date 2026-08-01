	.include "macros.inc"
	.include "gba.inc"

@ StepEffectMotionA
@ r0=record. Advances the record using the target side at +0x24 and the value
@ at +0x08, scaled by the constant 0xC3333. Paired with Func_eceac, which is
@ the same routine with 0x73333 -- the two give different travel speeds.
.thumb_func_start Func_ece7c
	push	{r5, lr}
	mov	r5, r0
	mov	r3, #0x24
	ldrsh	r1, [r5, r3]
	ldr	r0, [r5, #8]
	ldr	r3, =0xc3333
	mov	r2, #0x18
	bl	_Func_b82c4
	mov	r0, #0x1d
	bl	Func_30f8
	mov	r3, #4
	mov	r0, r5
	mov	r1, #2
	str	r3, [r5, #0x18]
	bl	Func_d4604
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_ece7c

@ StepEffectMotionB
@ r0=record. Func_ece7c with the 0x73333 constant; see there.
.thumb_func_start Func_eceac
	push	{r5, lr}
	mov	r5, r0
	mov	r3, #0x24
	ldrsh	r1, [r5, r3]
	ldr	r0, [r5, #8]
	ldr	r3, =0x73333
	mov	r2, #0x18
	bl	_Func_b82c4
	mov	r0, #0xc
	bl	Func_30f8
	mov	r3, #3
	mov	r0, r5
	mov	r1, #2
	str	r3, [r5, #0x18]
	bl	Func_d4604
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_eceac

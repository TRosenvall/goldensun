	.include "macros.inc"
  
.thumb_func_start Func_b684
	push	{lr}
	mov	r12, r0
	cmp	r0, #0
	beq	.Lb6b4
	mov	r3, r12
	add	r3, #0x27
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.Lb6ac
	mov	r4, r12
	add	r4, #0x28
	mov	r0, r3
.Lb69c:
	ldmia	r4!, {r2}
	ldrb	r3, [r2, #5]
	cmp	r3, #0xf
	beq	.Lb6a6
	strb	r1, [r2, #5]
.Lb6a6:
	sub	r0, #1
	cmp	r0, #0
	bne	.Lb69c
.Lb6ac:
	mov	r2, r12
	add	r2, #0x25
	mov	r3, #1
	strb	r3, [r2]
.Lb6b4:
	pop	{r0}
	bx	r0
.func_end Func_b684
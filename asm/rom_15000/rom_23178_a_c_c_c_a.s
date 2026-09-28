	.include "macros.inc"
	.include "gba.inc"

@ ComputeRowMetrics
@ r0.. = parameters. Pure arithmetic; no calls out.
.thumb_func_start Func_8029274  @ 0x08029274
	push	{r5, r6, lr}
	sub	sp, #8
	mov	r5, r2
	cmp	r1, #5
	bls	.L29280
	mov	r1, #5
.L29280:
	mov	r2, #0
	cmp	r1, #0
	beq	.L292a4
	mov	r6, #0xf
	mov	r4, sp
.L2928a:
	mov	r3, r0
	and	r3, r6
	cmp	r3, #9
	bhi	.L29296
	add	r3, #0x30
	b	.L29298
.L29296:
	add	r3, #0x37
.L29298:
	strb	r3, [r4]
	add	r2, #1
	lsr	r0, #4
	add	r4, #1
	cmp	r2, r1
	bne	.L2928a
.L292a4:
	sub	r2, r1, #1
	cmp	r2, #0
	blt	.L292bc
	mov	r3, sp
	add	r1, r2, r3
	mov	r12, r3
.L292b0:
	ldrb	r3, [r1]
	sub	r1, #1
	strb	r3, [r5]
	add	r5, #1
	cmp	r1, r12
	bge	.L292b0
.L292bc:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8029274

@ DrawDjinnRow
@ r0.. = parameters. Draws one row, reading the character with _Func_79338 and
@ emitting through UIDrawText.
.thumb_func_start Func_80292c4  @ 0x080292c4
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r10, r0
	sub	sp, #0x24
	mov	r5, r1
	bl	Func_8016478
	mov	r1, r10
	mov	r2, #0x30
	mov	r3, #0
	ldr	r0, =.L3742c
	bl	UIDrawText
	add	r2, sp, #8
	mov	r8, r2
	mov	r2, sp
	mov	r1, #0
	mov	r3, #0x1c
	add	r2, #0x21
	str	r1, [sp, #4]
	add	r3, sp
	mov	r1, #0x10
	str	r2, [sp]
	lsl	r7, r5, #8
	mov	r11, r3
	mov	r9, r1
.L29302:
	mov	r3, r11
.L29304:
	mov	r1, #0
	strb	r1, [r3]
	ldr	r2, [sp]
	add	r3, #1
	cmp	r3, r2
	bne	.L29304
	mov	r0, r7
	mov	r1, #3
	mov	r2, r11
	bl	Func_8029274
	mov	r0, r11
	mov	r1, r10
	mov	r2, #0
	mov	r3, r9
	bl	UIDrawText
	ldr	r0, =.L37428
	mov	r1, r10
	mov	r2, #0x20
	mov	r3, r9
	bl	UIDrawText
	mov	r6, r8
	mov	r5, r8
	add	r6, #0xf
.L29338:
	mov	r0, r7
	bl	_GetFlag
	neg	r3, r0
	orr	r3, r0
	lsr	r3, #31
	add	r3, #0x30
	strb	r3, [r5]
	add	r5, #1
	add	r7, #1
	cmp	r5, r6
	ble	.L29338
	mov	r3, #0x10
	mov	r2, #0
	mov	r1, r8
	strb	r2, [r1, r3]
	mov	r0, r8
	mov	r3, r9
	mov	r1, r10
	mov	r2, #0x30
	bl	UIDrawText
	ldr	r1, [sp, #4]
	mov	r3, #8
	add	r1, #1
	add	r9, r3
	str	r1, [sp, #4]
	cmp	r1, #0x10
	bne	.L29302
	add	sp, #0x24
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80292c4

	.include "macros.inc"

@ ScanStatusTable
@ r0.. = parameters. Searches the status table of the record .gcc2_compiled.
@ selects. 96 lines; traced structurally.
.thumb_func_start Func_807a3a8  @ 0x0807a3a8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r8, r1
	mov	r1, #0
	mov	r10, r2
	mov	r9, r1
	mov	r3, #0
	cmp	r0, #7
	bls	.L7a3c2
	mov	r3, #1
.L7a3c2:
	mov	r0, r3
	bl	Func_8077330
	mov	r1, #0x84
	mov	r3, r0
	lsl	r1, #1
	mov	r2, #8
	add	r2, r3
	add	r7, r3, r1
	mov	r12, r2
	ldr	r2, [r7]
	mov	r4, #0
	add	r0, #9
	mov	r5, #0
	mov	r1, r12
	cmp	r9, r2
	bge	.L7a448
	ldrb	r3, [r1]
	mov	r14, r3
	mov	r6, r9
	cmp	r8, r14
	bne	.L7a3fe
	ldrb	r3, [r0]
	cmp	r10, r3
	bne	.L7a3fe
	sub	r3, r2, #1
	mov	r1, #1
	str	r3, [r7]
	mov	r9, r1
	b	.L7a422
.L7a3fe:
	ldr	r2, [r7]
	add	r4, #1
	add	r0, #4
	add	r1, #4
	add	r5, #4
	cmp	r4, r2
	bge	.L7a448
	ldrb	r3, [r1]
	mov	r6, r5
	cmp	r8, r3
	bne	.L7a3fe
	ldrb	r3, [r0]
	cmp	r10, r3
	bne	.L7a3fe
	sub	r3, r2, #1
	str	r3, [r7]
	mov	r2, #1
	mov	r9, r2
.L7a422:
	mov	r3, #0x80
	lsl	r3, #1
	add	r3, r12
	ldr	r3, [r3]
	cmp	r4, r3
	bge	.L7a448
	mov	r2, #0x80
	lsl	r2, #1
	add	r2, r12
	b	.L7a438
.L7a436:
	lsl	r6, r4, #2
.L7a438:
	mov	r1, r12
	add	r3, r6, #4
	ldr	r3, [r1, r3]
	str	r3, [r1, r6]
	ldr	r3, [r2]
	add	r4, #1
	cmp	r4, r3
	blt	.L7a436
.L7a448:
	mov	r0, r9
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_807a3a8

@ FindStatusEntry
@ r0 = entry. Func_7a3a8 with the scratch record, returning the matching slot.
.thumb_func_start Func_807a458  @ 0x0807a458
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r6, r1
	mov	r7, r2
	bl	Func_807a3a8
	mov	r0, #0
	cmp	r5, #7
	bls	.L7a46c
	mov	r0, #1
.L7a46c:
	bl	Func_8077330
	mov	r1, #0x84
	mov	r3, r0
	lsl	r1, #1
	add	r0, r3, r1
	ldr	r1, [r0]
	mov	r2, r3
	add	r2, #8
	lsl	r3, r1, #2
	strb	r6, [r2, r3]
	add	r1, #1
	add	r2, r3
	mov	r3, #0xff
	strb	r7, [r2, #1]
	strb	r5, [r2, #2]
	strb	r3, [r2, #3]
	str	r1, [r0]
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_807a458

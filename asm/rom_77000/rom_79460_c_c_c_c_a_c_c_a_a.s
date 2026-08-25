	.include "macros.inc"

@ RollWeightedA
@ r0.. = parameters. A Func_79b24 wrapper with one weighting.
.thumb_func_start Func_8079bf8  @ 0x08079bf8
	push	{r5, r6, lr}
	mov	r5, r0
	sub	r5, r1
	mov	r6, r2
	mov	r0, r3
	cmp	r5, #0
	bge	.L79c08
	mov	r5, #0
.L79c08:
	mov	r1, #1
	bl	Func_8079b24
	lsl	r3, r6, #1
	add	r3, r5, r3
	mul	r0, r3
	cmp	r0, #0
	bge	.L79c1c
	ldr	r3, =0x1ff
	add	r0, r3
.L79c1c:
	asr	r0, #9
	cmp	r0, #0
	bge	.L79c24
	mov	r0, #0
.L79c24:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_8079bf8

@ RollWeightedB
@ r0.. = parameters. A second Func_79b24 wrapper.
.thumb_func_start Func_8079c30  @ 0x08079c30
	push	{r5, r6, lr}
	mov	r6, r0
	mov	r0, r1
	mov	r1, #0
	mov	r5, r2
	bl	Func_8079b24
	mov	r3, r6
	mul	r3, r0
	mov	r0, r5
	mul	r0, r3
	cmp	r0, #0
	bge	.L79c4e
	ldr	r3, =0xffff
	add	r0, r3
.L79c4e:
	asr	r0, #16
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_8079c30

@ RollWeightedC
@ r0.. = parameters. A third Func_79b24 wrapper; the three differ only in the
@ table they weight against.
.thumb_func_start Func_8079c5c  @ 0x08079c5c
	push	{r5, r6, lr}
	lsl	r1, #1
	sub	r1, #0xc8
	mov	r6, r0
	mov	r0, r1
	mov	r1, #0
	mov	r5, r2
	bl	Func_8079b24
	mov	r3, r6
	mul	r3, r0
	mov	r0, r5
	mul	r0, r3
	cmp	r0, #0
	bge	.L79c7e
	ldr	r3, =0xffff
	add	r0, r3
.L79c7e:
	asr	r0, #16
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_8079c5c

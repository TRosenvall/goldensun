	.include "macros.inc"
	.include "gba.inc"

@ DrawTextScratch
@ r0.. = parameters. Allocates scratch with Func_4970, emits through
@ Func_17aa4, releases with free.
.thumb_func_start Func_801e858  @ 0x0801e858
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	mov	r0, #0x80
	lsl	r0, #2
	mov	r8, r2
	mov	r10, r3
	mov	r7, r1
	bl	Func_8004970
	ldrb	r3, [r5]
	mov	r6, r0
	mov	r2, r6
	cmp	r3, #0
	beq	.L1e888
.L1e87a:
	ldrb	r3, [r5]
	strh	r3, [r2]
	add	r5, #1
	ldrb	r3, [r5]
	add	r2, #2
	cmp	r3, #0
	bne	.L1e87a
.L1e888:
	ldr	r3, =0
	mov	r0, r6
	strh	r3, [r2]
	mov	r1, r7
	mov	r2, r8
	mov	r3, r10
	bl	Func_8017aa4
	mov	r0, r6
	bl	free
	b	.L1e8a4

	.pool_aligned

.L1e8a4:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_801e858

@ DrawTextScratchWide
@ r0.. = parameters. As Func_1e858 but rendering through Func_1de5c.
.thumb_func_start Func_801e8b0  @ 0x0801e8b0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	mov	r0, #0x80
	lsl	r0, #2
	mov	r10, r2
	mov	r8, r3
	mov	r7, r1
	bl	Func_8004970
	ldr	r3, =iwram_3001e8c
	mov	r6, r0
	ldr	r0, [r3]
	ldrb	r3, [r5]
	mov	r2, r6
	cmp	r3, #0
	beq	.L1e8e4
.L1e8d6:
	ldrb	r3, [r5]
	strh	r3, [r2]
	add	r5, #1
	ldrb	r3, [r5]
	add	r2, #2
	cmp	r3, #0
	bne	.L1e8d6
.L1e8e4:
	ldr	r3, .L1e91c	@ 0
	strh	r3, [r2]
	mov	r1, r8
	ldrh	r3, [r7, #0xe]
	lsr	r2, r1, #3
	add	r3, r2
	mov	r4, r10
	ldrh	r2, [r7, #0xc]
	lsr	r1, r4, #3
	add	r3, #1
	add	r2, r1
	lsl	r3, #5
	add	r3, r2
	add	r1, r3, #1
	mov	r3, #0xa0
	lsl	r3, #2
	cmp	r1, r3
	bcs	.L1e932
	ldr	r4, =0x6002000
	lsl	r1, #1
	add	r2, r1, r4
	mov	r3, #7
	add	r1, r0, r1
	mov	r0, r10
	and	r3, r0
	mov	r0, r6
	b	.L1e928

	.align	2, 0
.L1e91c:
	.word	0
	.pool

.L1e928:
	bl	Func_801de5c
	mov	r0, r6
	bl	free
.L1e932:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_801e8b0

@ DrawTextScratchEntry
@ r0.. = parameters. As Func_1e858 but appending a layout entry with
@ Func_17c8c.
.thumb_func_start UIDrawText  @ 0x0801e940
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	mov	r0, #0x80
	lsl	r0, #2
	mov	r7, r2
	mov	r8, r3
	mov	r10, r1
	bl	Func_8004970
	ldrb	r3, [r5]
	mov	r6, r0
	mov	r2, r6
	cmp	r3, #0
	beq	.L1e970
.L1e962:
	ldrb	r3, [r5]
	strh	r3, [r2]
	add	r5, #1
	ldrb	r3, [r5]
	add	r2, #2
	cmp	r3, #0
	bne	.L1e962
.L1e970:
	ldr	r3, =0
	lsr	r7, #3
	strh	r3, [r2]
	mov	r3, r8
	lsr	r3, #3
	mov	r0, r6
	mov	r1, r10
	mov	r2, r7
	mov	r8, r3
	bl	Func_8017c8c
	mov	r0, r6
	bl	free
	b	.L1e994

	.pool_aligned

.L1e994:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end UIDrawText

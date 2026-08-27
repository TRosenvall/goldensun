	.include "macros.inc"
	.include "gba.inc"

@ ClipIconToWindow
@ r0.. = parameters. Clips an icon against its window with Func_19000.
.thumb_func_start Func_80218dc  @ 0x080218dc
	push	{r5, r6, lr}
	mov	r6, r11
	mov	r5, r10
	push	{r5, r6}
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6}
	lsl	r5, r3, #1
	ldr	r3, =0xf315
	mov	r8, r1
	mov	r1, #0x80
	mov	r9, r2
	add	r3, r5
	lsl	r1, #3
	sub	sp, #4
	mov	r6, #0
	mov	r11, r3
	orr	r1, r3
	mov	r2, r8
	mov	r3, r9
	str	r6, [sp]
	mov	r10, r0
	bl	Func_8019000
	ldr	r3, =0xf314
	mov	r2, r8
	add	r5, r3
	mov	r0, r10
	mov	r1, r5
	mov	r3, r9
	add	r2, #1
	str	r6, [sp]
	bl	Func_8019000
	mov	r3, #2
	add	r8, r3
	mov	r0, r10
	mov	r1, r11
	mov	r2, r8
	mov	r3, r9
	str	r6, [sp]
	bl	Func_8019000
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r3}
	mov	r11, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80218dc

@ ComputeGridPosition
@ r0.. = parameters. Maps an index to a grid cell; no calls out.
.thumb_func_start Func_8021950  @ 0x08021950
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r3
	sub	sp, #8
	neg	r3, r6
	str	r0, [sp, #4]
	mov	r7, r2
	mov	r0, #0
	lsl	r3, #2
	lsl	r2, r6, #2
	str	r1, [sp]
	mov	r12, r0
	mov	r8, r3
	mov	r14, r2
.L2196e:
	ldr	r0, [sp]
	ldmia	r0!, {r4}
	mov	r3, r0
	str	r3, [sp]
	ldr	r0, [sp, #4]
	ldmia	r0!, {r1}
	mov	r3, r0
	mov	r2, #0
	str	r3, [sp, #4]
	cmp	r6, #0
	bge	.L2198a
	mov	r3, r8
	lsr	r4, r3
	b	.L2198e
.L2198a:
	mov	r0, r14
	lsl	r4, r0
.L2198e:
	ldr	r5, =0xfffffff
	mov	r0, #7
.L21992:
	lsl	r2, #4
	cmp	r4, r5
	bls	.L2199c
	lsr	r3, r4, #28
	b	.L2199e
.L2199c:
	lsr	r3, r1, #28
.L2199e:
	add	r2, r3
	sub	r0, #1
	lsl	r4, #4
	lsl	r1, #4
	cmp	r0, #0
	bge	.L21992
	stmia	r7!, {r2}
	mov	r2, #1
	add	r12, r2
	mov	r3, r12
	cmp	r3, #7
	ble	.L2196e
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8021950

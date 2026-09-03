	.include "macros.inc"

@ ComputeItemValue
@ r0.. = parameters. Derives an item's value from its record fields; no calls
@ out.
.thumb_func_start Func_8078aa0  @ 0x08078aa0
	push	{lr}
	mov	r2, r0
	ldr	r4, =ewram_2000380
	mov	r0, #0
	cmp	r2, #0x7f
	bgt	.L78ac6
	ldrb	r3, [r4, r2]
	add	r3, r1
	cmp	r3, #0
	bge	.L78ab8
	mov	r3, #0
	b	.L78ac4
.L78ab8:
	cmp	r3, #0x63
	ble	.L78ac2
	mov	r3, #0x63
	mov	r0, #0x63
	b	.L78ac4
.L78ac2:
	mov	r0, r3
.L78ac4:
	strb	r3, [r4, r2]
.L78ac6:
	pop	{r1}
	bx	r1
.func_end Func_8078aa0

@ NotifyItemUsed
@ r0.. = parameters. Reports a consumed item through Func_78aa0.
.thumb_func_start Func_8078ad0  @ 0x08078ad0
	push	{lr}
	ldr	r3, =0x1ff
	ldr	r2, =.L7b490
	and	r3, r0
	ldrb	r0, [r2, r3]
	mov	r4, #0
	cmp	r0, #0
	beq	.L78ae8
	sub	r0, #1
	bl	Func_8078aa0
	mov	r4, r0
.L78ae8:
	mov	r0, r4
	pop	{r1}
	bx	r1
.func_end Func_8078ad0

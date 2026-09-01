	.include "macros.inc"

@ GetItemProperty
@ r0 = item id, r1 = which. Returns one of the record's property fields.
.thumb_func_start Func_8078870  @ 0x08078870
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r2, #0x80
	ldr	r3, =0x1ff
	lsl	r2, #2
	mov	r5, r0
	mov	r7, r1
	mov	r6, #0
	mov	r8, r2
	mov	r10, r3
	add	r5, #0xd8
.L7888a:
	ldrh	r3, [r5]
	mov	r2, r8
	and	r3, r2
	cmp	r3, #0
	beq	.L788a8
	ldrh	r0, [r5]
	bl	GetItemInfo
	ldrb	r3, [r0, #2]
	cmp	r3, r7
	bne	.L788a8
	ldrh	r3, [r5]
	mov	r0, r10
	and	r0, r3
	b	.L788b2
.L788a8:
	add	r6, #1
	add	r5, #2
	cmp	r6, #0xe
	ble	.L7888a
	mov	r0, #0
.L788b2:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8078870

@ ConsumeInventoryItem
@ r0 = combatant id, r1 = slot. Decrements the quantity field by 0x800 -- one
@ unit -- and clears the slot entirely when the count reaches zero. Returns 1 on
@ success, -1 when the slot was already empty. Rebuilds the summary with
@ CalcStats.
.thumb_func_start Func_80788c4  @ 0x080788c4
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r0
	bl	GetUnit
	lsl	r5, #1
	add	r5, #0xd8
	ldrh	r3, [r0, r5]
	mov	r6, #1
	neg	r6, r6
	cmp	r3, #0
	beq	.L7893a
	mov	r2, #0xf8
	lsl	r2, #8
	and	r2, r3
	cmp	r2, #0
	beq	.L788f4
	ldr	r2, =0xfffff800
	add	r3, r2
	strh	r3, [r0, r5]
	mov	r6, #1
	b	.L7893a

	.pool_aligned

.L788f4:
	mov	r6, r0
	add	r6, #0xd8
	strh	r2, [r0, r5]
	mov	r4, r6
	mov	r5, #0
	mov	r1, r6
	mov	r0, #0xe
.L78902:
	ldrh	r2, [r4]
	lsl	r3, r2, #16
	add	r4, #2
	cmp	r3, #0
	beq	.L78912
	strh	r2, [r1]
	add	r5, #1
	add	r1, #2
.L78912:
	sub	r0, #1
	cmp	r0, #0
	bge	.L78902
	cmp	r5, #0xe
	bgt	.L78938
	lsl	r3, r5, #1
	add	r0, r3, r6
	ldr	r2, =0
	mov	r3, #0xf
	sub	r5, r3, r5
.L78926:
	sub	r5, #1
	strh	r2, [r0]
	add	r0, #2
	cmp	r5, #0
	bne	.L78926
	b	.L78938

	.pool_aligned

.L78938:
	mov	r6, #2
.L7893a:
	mov	r0, r7
	bl	CalcStats
	mov	r0, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80788c4

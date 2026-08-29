	.include "macros.inc"

@ CheckEntityCollision
@ r0=entity being moved, r1=candidate position (vec3, 16.16). Returns -1 if the
@ candidate overlaps another entity, 0 if it is clear.
@ Scans all 0x40 slots at [iwram_1e64], skipping inactive slots, the mover
@ itself, and any entity without bit 0 of +0x59 (collidable) set. Each pair is
@ tested by Func_eba0 using both radii from +0x20, biased by -2.
.thumb_func_start Func_800d924  @ 0x0800d924
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e64
	ldr	r5, [r3]
	mov	r6, r5
	sub	sp, #4
	mov	r7, r0
	mov	r8, r1
	mov	r4, #0
	add	r6, #0x59
.Ld93a:
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.Ld96e
	ldrb	r2, [r6]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Ld96e
	cmp	r5, r7
	beq	.Ld96e
	ldrh	r1, [r5, #0x20]
	ldrh	r3, [r7, #0x20]
	mov	r0, r5
	add	r0, #8
	sub	r1, #2
	sub	r3, #2
	mov	r2, r8
	str	r4, [sp]
	bl	Func_800eba0
	ldr	r4, [sp]
	cmp	r0, #0
	blt	.Ld96e
	mov	r0, #1
	neg	r0, r0
	b	.Ld97a
.Ld96e:
	add	r4, #1
	add	r6, #0x70
	add	r5, #0x70
	cmp	r4, #0x3f
	ble	.Ld93a
	mov	r0, #0
.Ld97a:
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_800d924

@ FindCollidingEntity
@ r0=entity being moved, r1=candidate position (vec3, 16.16). Identical scan to
@ Func_d924 but returns the first colliding entity instead of -1, or 0 when the
@ candidate is clear -- used where the caller needs to know what it hit.
.thumb_func_start Func_800d98c  @ 0x0800d98c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e64
	ldr	r5, [r3]
	mov	r6, r5
	sub	sp, #4
	mov	r7, r0
	mov	r8, r1
	mov	r4, #0
	add	r6, #0x59
.Ld9a2:
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.Ld9d4
	ldrb	r2, [r6]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Ld9d4
	cmp	r5, r7
	beq	.Ld9d4
	ldrh	r1, [r5, #0x20]
	ldrh	r3, [r7, #0x20]
	mov	r0, r5
	add	r0, #8
	sub	r1, #2
	sub	r3, #2
	mov	r2, r8
	str	r4, [sp]
	bl	Func_800eba0
	ldr	r4, [sp]
	cmp	r0, #0
	blt	.Ld9d4
	mov	r0, r5
	b	.Ld9e0
.Ld9d4:
	add	r4, #1
	add	r6, #0x70
	add	r5, #0x70
	cmp	r4, #0x3f
	ble	.Ld9a2
	mov	r0, #0
.Ld9e0:
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_800d98c


	.include "macros.inc"
	.include "gba.inc"

@ GetEwramFree
@ Takes no arguments. Returns 0x2040000 - [iwram_1e50+0], the bytes left in the
@ EWRAM arena. The limit is written as the literal `0x81 << 18`.
.thumb_func_start Func_48a0
	ldr	r3, =iwram_1e50
	mov	r0, #0x81
	ldr	r3, [r3]
	lsl	r0, #18
	sub	r0, r3
	bx	lr
.func_end Func_48a0

@ AllocTagged -- IWRAM first
@ r0 = tag (2..63), r1 = size in bytes. Returns the block, or 0 when neither
@ arena has room.
@ IDEMPOTENT: if [iwram_1e50 + tag*4] is already non-zero it is returned
@ unchanged and nothing is allocated.
@ Otherwise the size is rounded up to a word and taken from the IWRAM arena if
@ it fits below iwram_77ff, ELSE FROM THE EWRAM ARENA below 0x2040000. The
@ pointer is recorded in the tag slot either way, so the caller cannot tell
@ which arena it got -- and does not need to, because Func_2dd8 works it out
@ from the address.
@ Func_48f4 below is the same function with the two arenas tried in the other
@ order; that is the ONLY difference between them.
.thumb_func_start Func_48b0
	push	{r5, lr}
	ldr	r4, =iwram_1e50
	lsl	r5, r0, #2
	ldr	r0, [r4, r5]
	cmp	r0, #0
	bne	.L48e6
	add	r3, r1, #3
	lsr	r3, #2
	ldr	r0, [r4, #4]
	lsl	r1, r3, #2
	ldr	r3, =iwram_77ff
	add	r2, r0, r1
	cmp	r2, r3
	bls	.L48e2
	ldr	r0, [r4]
	mov	r3, #0x81
	add	r1, r0, r1
	lsl	r3, #18
	cmp	r1, r3
	bcc	.L48dc
	mov	r0, #0
	b	.L48e6
.L48dc:
	str	r1, [r4]
	str	r0, [r4, r5]
	b	.L48e6
.L48e2:
	str	r2, [r4, #4]
	str	r0, [r4, r5]
.L48e6:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_48b0

@ AllocTagged -- EWRAM first
@ r0 = tag (2..63), r1 = size in bytes. Identical to Func_48b0 in every respect
@ except that it tries the EWRAM arena first and falls back to IWRAM.
@ Use this for large or long-lived blocks so the small fast IWRAM arena is left
@ for things that need it; rom_b5000's Func_b63c8 allocates all five of its
@ battle blocks this way.
.thumb_func_start Func_48f4
	push	{r5, lr}
	ldr	r4, =iwram_1e50
	lsl	r5, r0, #2
	ldr	r0, [r4, r5]
	cmp	r0, #0
	bne	.L492a
	add	r3, r1, #3
	lsr	r3, #2
	ldr	r0, [r4]
	lsl	r1, r3, #2
	mov	r3, #0x81
	add	r2, r0, r1
	lsl	r3, #18
	cmp	r2, r3
	bcc	.L4926
	ldr	r0, [r4, #4]
	ldr	r3, =iwram_77ff
	add	r1, r0, r1
	cmp	r1, r3
	bls	.L4920
	mov	r0, #0
	b	.L492a
.L4920:
	str	r1, [r4, #4]
	str	r0, [r4, r5]
	b	.L492a
.L4926:
	str	r2, [r4]
	str	r0, [r4, r5]
.L492a:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_48f4

@ AllocAnonymous -- IWRAM first
@ r0 = size in bytes. Bumps the IWRAM arena, falling back to EWRAM, and returns
@ the block without recording it anywhere. Returns 0 when neither fits.
@ NOTHING CAN FREE THIS INDIVIDUALLY -- Func_2dd8 works from a tag. Callers pair
@ it with a later Func_2dd8 on a tag allocated BEFORE it, which rewinds past
@ this block too. That is the intended usage, not a leak.
.thumb_func_start Func_4938
	push	{lr}
	ldr	r1, =iwram_1e50
	add	r3, r0, #3
	lsr	r3, #2
	ldr	r2, [r1, #4]
	lsl	r0, r3, #2
	ldr	r4, =iwram_77ff
	add	r3, r2, r0
	cmp	r3, r4
	bls	.L4960
	ldr	r2, [r1]
	mov	r3, #0x81
	add	r0, r2, r0
	lsl	r3, #18
	cmp	r0, r3
	bcc	.L495c
	mov	r0, #0
	b	.L4964
.L495c:
	str	r0, [r1]
	b	.L4962
.L4960:
	str	r3, [r1, #4]
.L4962:
	mov	r0, r2
.L4964:
	pop	{r1}
	bx	r1
.func_end Func_4938

@ AllocAnonymous -- EWRAM first
@ r0 = size in bytes. The Func_4938 counterpart with the arenas tried in the
@ other order. Same caveat about freeing.
.thumb_func_start Func_4970
	push	{lr}
	ldr	r1, =iwram_1e50
	add	r3, r0, #3
	lsr	r3, #2
	ldr	r2, [r1]
	lsl	r0, r3, #2
	mov	r4, #0x81
	add	r3, r2, r0
	lsl	r4, #18
	cmp	r3, r4
	bcc	.L4998
	ldr	r2, [r1, #4]
	ldr	r3, =iwram_77ff
	add	r0, r2, r0
	cmp	r0, r3
	bls	.L4994
	mov	r0, #0
	b	.L499c
.L4994:
	str	r0, [r1, #4]
	b	.L499a
.L4998:
	str	r3, [r1]
.L499a:
	mov	r0, r2
.L499c:
	pop	{r1}
	bx	r1
.func_end Func_4970

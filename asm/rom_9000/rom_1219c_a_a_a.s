	.include "macros.inc"
	.include "gba.inc"

@ IsPositionOnMap
@ r0=position vec3 (16.16). Returns 0 when the point is on a valid tile and -1
@ when it is not. Resolves the tile from x (+0x00) and the ground-projected
@ depth (+0x08 minus +0x04) against the layer at [iwram_1e70]+0x190, and treats
@ a tile flags byte of 0xFF as off-map. Returns 0 with no test when no map is
@ loaded. The branchless tail computes the -1/0 result without a compare.
.thumb_func_start Func_801219c  @ 0x0801219c
	push	{lr}
	ldr	r3, [r0]
	cmp	r3, #0
	bge	.L121a8
	ldr	r2, =0xffff
	add	r3, r2
.L121a8:
	asr	r4, r3, #16
	ldr	r2, [r0, #8]
	ldr	r3, [r0, #4]
	sub	r0, r2, r3
	cmp	r0, #0
	bge	.L121b8
	ldr	r3, =0xffff
	add	r0, r3
.L121b8:
	ldr	r3, =iwram_3001e70
	ldr	r3, [r3]
	asr	r0, #16
	cmp	r3, #0
	bne	.L121c6
	mov	r0, #0
	b	.L121f6
.L121c6:
	mov	r2, #0xc8
	lsl	r2, #1
	add	r3, r2
	ldr	r1, [r3]
	mov	r3, r4
	cmp	r3, #0
	bge	.L121d6
	add	r3, #0xf
.L121d6:
	asr	r4, r3, #4
	cmp	r0, #0
	bge	.L121de
	add	r0, #0xf
.L121de:
	asr	r3, r0, #4
	lsl	r3, #7
	add	r3, r4, r3
	lsl	r3, #2
	add	r1, r3
	ldrb	r3, [r1, #2]
	mov	r2, #0xff
	eor	r3, r2
	neg	r0, r3
	orr	r0, r3
	lsr	r0, #31
	sub	r0, #1
.L121f6:
	pop	{r1}
	bx	r1
.func_end Func_801219c

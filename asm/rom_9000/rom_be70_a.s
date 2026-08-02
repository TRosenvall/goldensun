	.include "macros.inc"

@ DissolveSpriteStep
@ r0=actor, r1=dissolve step. Erases one pixel pair from each of the actor's
@ OBJ tiles, giving a scrambled "dissolve" wipe when stepped repeatedly.
@ The tile base is iwram_1b10[+0x1C size code].halfword + 0x6010000; the tile
@ count is (+0x20 * +0x21) / 64. For each tile, .L1314c (a 0x44-byte scramble
@ table) is indexed by (step + tile*16) & 0x3F to pick a byte offset within the
@ tile: bit 0 of the entry selects which half of the 16-bit unit survives --
@ set keeps the low byte, clear keeps the high byte -- and the other half is
@ zeroed.
@ NOTE the guard at .Lbeae is unsigned: (step - 0x40) > 0x3F skips the tile, so
@ steps 0x00-0x3F do nothing at all and only steps 0x40-0x7F erase.
.thumb_func_start Func_800be70  @ 0x0800be70
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldrb	r3, [r0, #0x1c]
	ldr	r2, =gSpriteSlots
	lsl	r3, #2
	add	r3, r2
	ldrh	r3, [r3, #2]
	ldr	r2, =0x6010000
	add	r5, r3, r2
	mov	r3, r0
	add	r3, #0x20
	add	r0, #0x21
	ldrb	r2, [r3]
	ldrb	r3, [r0]
	mul	r3, r2
	cmp	r3, #0
	bge	.Lbe96
	add	r3, #0x3f
.Lbe96:
	asr	r6, r3, #6
	mov	r4, #0
	cmp	r4, r6
	bcs	.Lbee6
	ldr	r3, =.L1314c
	mov	r0, #0xff
	lsl	r0, #8
	mov	r2, #0x3f
	mov	r8, r3
	mov	r14, r0
	mov	r12, r2
	mov	r7, #0x3e
.Lbeae:
	mov	r3, r1
	sub	r3, #0x40
	cmp	r3, #0x3f
	bhi	.Lbedc
	lsl	r3, r4, #4
	mov	r0, r12
	add	r3, r1, r3
	and	r3, r0
	mov	r0, r8
	ldrb	r2, [r0, r3]
	mov	r3, r2
	and	r3, r7
	add	r0, r5, r3
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	beq	.Lbed4
	ldrb	r3, [r0]
	b	.Lbeda
.Lbed4:
	ldrh	r2, [r0]
	mov	r3, r14
	and	r3, r2
.Lbeda:
	strh	r3, [r0]
.Lbedc:
	add	r4, #1
	add	r5, #0x40
	add	r1, #1
	cmp	r4, r6
	bcc	.Lbeae
.Lbee6:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_800be70


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
.thumb_func_start Func_be70
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldrb	r3, [r0, #0x1c]
	ldr	r2, =iwram_1b10
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
.func_end Func_be70

@ PlaySpriteDissolve
@ r0=actor. Runs the whole 0x00-0x7F dissolve on one actor, applying four
@ Func_be70 steps per frame and yielding with Func_30f8(1) between groups.
@ Blocks for 32 frames.
.thumb_func_start Func_befc
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r6, #0
.Lbf02:
	mov	r1, r6
	mov	r0, r5
	bl	Func_be70
	add	r1, r6, #1
	mov	r0, r5
	bl	Func_be70
	add	r1, r6, #2
	mov	r0, r5
	bl	Func_be70
	add	r1, r6, #3
	mov	r0, r5
	bl	Func_be70
	add	r6, #4
	mov	r0, #1
	bl	Func_30f8
	cmp	r6, #0x7f
	bls	.Lbf02
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_befc

@ PlaySpriteDissolveMulti
@ r0=array of actor pointers, r1=count. Same wipe as Func_befc but driven
@ across several actors at once: each group of four steps is applied to every
@ actor in the array before yielding with Func_30f8(1), so they dissolve in
@ lockstep. Blocks for 32 frames.
.thumb_func_start Func_bf34
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #4
	str	r0, [sp]
	mov	r11, r1
	mov	r7, #0
.Lbf4a:
	mov	r3, r11
	cmp	r3, #0
	ble	.Lbf86
	add	r3, r7, #1
	mov	r9, r3
	add	r3, r7, #2
	mov	r10, r3
	ldr	r5, [sp]
	add	r3, r7, #3
	mov	r8, r3
	mov	r6, r11
.Lbf60:
	ldr	r0, [r5]
	mov	r1, r7
	bl	Func_be70
	ldr	r0, [r5]
	mov	r1, r9
	bl	Func_be70
	ldr	r0, [r5]
	mov	r1, r10
	bl	Func_be70
	sub	r6, #1
	ldmia	r5!, {r0}
	mov	r1, r8
	bl	Func_be70
	cmp	r6, #0
	bne	.Lbf60
.Lbf86:
	mov	r0, #1
	add	r7, #4
	bl	Func_30f8
	cmp	r7, #0x7f
	bls	.Lbf4a
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_bf34

@ GetEntityScreenPos
@ r0=entity, r1=out vec2 (two s32). Subtracts the camera origin at
@ [iwram_1e70]+0xE4/+0xE8 (truncated to whole pixels) from the entity position
@ at +0x08/+0x10, both 16.16. Returns 0 and writes the pixel coordinates when
@ the result is on-screen -- x within [-32, 272) after the +0x1FFFFF bias test,
@ y strictly inside (0, 224) -- otherwise zeroes the output and returns -1.
.thumb_func_start Func_bfa4
	push	{r5, lr}
	ldr	r3, =iwram_1e70
	ldr	r3, [r3]
	add	r3, #0xe4
	ldr	r4, =0xffff0000
	mov	r5, r1
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	ldr	r3, [r0, #8]
	and	r1, r4
	sub	r1, r3, r1
	ldr	r3, [r0, #0x10]
	ldr	r0, =0x1fffff
	and	r2, r4
	sub	r2, r3, r2
	add	r3, r1, r0
	ldr	r0, =0x12ffffe
	cmp	r3, r0
	bhi	.Lbfe2
	cmp	r2, #0
	ble	.Lbfe2
	mov	r3, #0xe0
	lsl	r3, #16
	cmp	r2, r3
	bge	.Lbfe2
	asr	r3, r1, #16
	stmia	r5!, {r3}
	asr	r3, r2, #16
	str	r3, [r5]
	mov	r0, #0
	b	.Lbfec
.Lbfe2:
	mov	r3, #0
	stmia	r5!, {r3}
	mov	r0, #1
	str	r3, [r5]
	neg	r0, r0
.Lbfec:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_bfa4

	.section .rodata

@ .L1314c -- 0x44-byte scramble table driving the dissolve order in Func_be70.
@ Each entry is a byte offset within a tile; bit 0 selects which half of the
@ addressed halfword is kept.
.L1314c:
	.incrom 0x1314c, 0x13190

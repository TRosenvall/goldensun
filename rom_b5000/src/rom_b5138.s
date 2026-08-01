	.include "macros.inc"

@ ============================================================================
@ Battle logic.
@
@ rom_b5000 owns a battle: the turn order, the action resolution, the numbers.
@ It reads the party data from rom_77000, tells rom_c9000 which animation to
@ play, and drives the menus through rom_15000. If rom_c9000 is what a battle
@ looks like, this is what a battle IS.
@
@ ENTRY POINT. Func_b63c8(encounterId) sets a battle up and runs it. Everything
@ it needs is allocated there, and the tag map is what makes the module legible
@ (pointer at iwram_1e50 + tag*4, as everywhere else in this ROM):
@
@     tag 0x09  iwram_1e74  0x82C   the battle state block
@     tag 0x0B  iwram_1e7c  0x280
@     tag 0x0C  iwram_1e80  0x4C    view / camera, shared with rom_c9000
@     tag 0x2C  iwram_1f00  0x20
@     tag 0x36  iwram_1f28  0x7C8   THE ENEMY RECORDS
@
@ That last one settles a question left open in rom_77000: 0x7C8 / 0x14C is
@ exactly 6, so the enemy block holds six combatant records -- which is why
@ Func_77394 accepts enemy ids 0x80..0x85 and no more, and why it returns 0 for
@ them when iwram_1f28 is null, i.e. outside battle.
@
@ HANDING OFF TO THE ANIMATION LAYER. Func_bd7dc(code) raises a ONE-SHOT flag at
@ [iwram_1e74]+0x800 and stores the code at +0x820; only the first call per
@ frame wins. rom_c9000's handlers call it at their hit frame -- 85 sites -- so
@ this is how an animation tells the turn logic "the blow has landed". +0x824 is
@ cleared alongside it.
@
@ APPLYING THE RESULT goes back out to rom_77000: _Func_783a4(id, -damage) for
@ HP and _Func_783dc for PP, both clamped and both refreshing the 14-bit HP
@ fraction the UI bars read.
@
@ Func_b7dd0(id) is the module's own combatant lookup (88 external call sites),
@ distinct from rom_77000's Func_77394 -- this one returns the battle-side
@ display record rather than the persistent character record.
@
@ DEBUG HARNESS. Func_b56e0 is not the shipped entry path. Holding DOWN
@ (iwram_1ae8 & 0x80) at the top drops into a live encounter-id picker: Right
@ and Left step the id by 1, Up and Down by 10, L and R cycle party presets
@ through Func_b5368, Start opens Func_b5534, Select opens Func_c2a08, and A
@ launches Func_b63c8 with whatever id is showing. Without Down held it just
@ calls Func_b63c8(0x101) in a loop.
@ ============================================================================

@ DecompressBattleGraphic -- designed to be copied into RAM and run there
@ r0 = compressed source, r1 = destination.
@ THE FIRST LOOP RELOCATES ITS OWN JUMP TABLE: it computes the delta between
@ where the table was assembled and where the code is actually executing, adds
@ that to each of eight entries, and writes them to a scratch table. That is
@ what makes the routine position-independent -- and it is necessary, because
@ rom_c9000's Func_c08ec DMA3-copies this function into a 0x230-byte scratch
@ buffer and calls it there rather than calling it in place.
@ The decoder itself walks a bit stream a halfword at a time, assembling output
@ through the relocated dispatch. ARM rather than Thumb, like every inner loop
@ of this kind in the ROM.
.arm_func_start Func_b5138
	push	{r5, r6, r7, r8, r9, r10, r11, lr}
	ldr	r2, .Lb5208
	adr	r3, .Lb5238
	sub	r2, r3, r2
	adr	r4, .Lb5218
	mov	r5, #8
.Lb5150:
	ldr	r6, [r3], #4
	add	r6, r2
	str	r6, [r4], #4
	subs	r5, #1
	bne	.Lb5150
	mov	r11, #7
	ldrh	r3, [r0], #2
	mov	r2, #0
	mov	r5, #0
	mov	r9, #0xf
	ldr	r10, .Lb5204
.Lb517c:
	mov	r8, #8
.Lb5180:
	mov	r7, #0x20
.Lb5184:
	bl	.Lb520c
	lsr	r6, r5, #25
	bl	.Lb520c
	orr	r6, r5, lsr #17
	bl	.Lb520c
	orr	r6, r5, lsr #9
	bl	.Lb520c
	orr	r6, r5, lsr #1
	add	r6, r10
	str	r6, [r1], #4
	bl	.Lb520c
	lsr	r6, r5, #25
	bl	.Lb520c
	orr	r6, r5, lsr #17
	bl	.Lb520c
	orr	r6, r5, lsr #9
	bl	.Lb520c
	orr	r6, r5, lsr #1
	add	r6, r10
	str	r6, [r1], #0x3c
	subs	r7, #1
	bne	.Lb5184
	sub	r1, #0x800
	add	r1, #8
	subs	r8, #1
	bne	.Lb5180
	sub	r1, #0x40
	add	r1, #0x800
	subs	r9, #1
	bne	.Lb517c
	pop	{r5, r6, r7, r8, r9, r10, r11, lr}
	bx	lr

.Lb5204:
	.word	0x60606060
.Lb5208:
	.word	.Lb5238

.Lb520c:
	and	r12, r3, #7
	ldr	pc, [pc, r12, lsl #2]
	.align	3

.Lb5218:
	.space	0x20

.Lb5238:
	.word	.Lb5258
	.word	.Lb5274
	.word	.Lb5314
	.word	.Lb52c4
	.word	.Lb5258
	.word	.Lb529c
	.word	.Lb5344
	.word	.Lb52ec

.Lb5258:
	lsr	r3, #2
	subs	r2, #2
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lb5274:
	and	r12, r3, #8
	lsrs	r3, #4
	add	r12, #8
	add	r5, r12, lsl #22
	subs	r2, #4
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lb529c:
	and	r12, r3, #8
	lsrs	r3, #4
	add	r12, #8
	sub	r5, r12, lsl #22
	subs	r2, #4
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lb52c4:
	and	r12, r11, r3, lsr #3
	lsr	r3, #6
	add	r12, #3
	add	r5, r12, lsl #25
	subs	r2, #6
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lb52ec:
	and	r12, r11, r3, lsr #3
	lsr	r3, #6
	add	r12, #3
	sub	r5, r12, lsl #25
	subs	r2, #6
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lb5314:
	mov	r12, #0xf
	ands	r12, r3, lsr #4
	add	r12, #0xb
	rsbcs	r12, #0
	add	r5, r12, lsl #25
	lsr	r3, #8
	subs	r2, #8
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.Lb5344:
	ror	r5, r3, #10
	and	r5, #0xfe000000
	lsr	r3, #10
	subs	r2, #0xa
	movpl	pc, lr
	ldrh	r12, [r0], #2
	add	r2, #0x10
	orr	r3, r12, lsl r2
	mov	pc, lr
.func_end Func_b5138

	.include "macros.inc"
	.include "gba.inc"

@ BuildWindowScanlineTable
@ Takes no arguments. Fills the 160-entry WIN0H table at ewram_10082 and arms
@ the DMA0 HBlank transfer that feeds it.
@ Rows outside 8..0x87 get the 0xFFF1 "closed" value. Inside that band the left
@ edge is the base width at ewram_10000 minus the per-row inset from
@ ewram_fffa, clamped to 0..0xF0 -- so the window opens as a shape that follows
@ the inset table rather than a plain rectangle.
.thumb_func_start Func_80d66cc  @ 0x080d66cc
	push	{r5, r6, lr}
	ldr	r6, =gBuffer
	ldr	r5, .Ld66f8	@ 0xfff1
	ldr	r1, =ewram_2010082
	ldr	r4, =ewram_200fffa
	mov	r0, #0
.Ld66d8:
	mov	r3, r0
	sub	r3, #8
	cmp	r3, #0x7f
	bhi	.Ld6708
	ldrh	r2, [r6]
	ldrb	r3, [r4]
	sub	r2, r3
	cmp	r2, #0
	bge	.Ld66ec
	mov	r2, #0
.Ld66ec:
	cmp	r2, #0xf0
	ble	.Ld66f2
	mov	r2, #0xf0
.Ld66f2:
	strh	r2, [r1]
	b	.Ld670a

	.align	2, 0
.Ld66f8:
	.word	0xfff1
	.pool

.Ld6708:
	strh	r5, [r1]
.Ld670a:
	add	r0, #1
	add	r1, #2
	add	r4, #1
	cmp	r0, #0xa0
	bne	.Ld66d8
	ldr	r3, =REG_DMA0SAD
	ldr	r2, =0xc5ff
	ldrh	r1, [r3, #0xa]
	and	r2, r1
	strh	r2, [r3, #0xa]
	ldr	r2, =0x7fff
	ldrh	r1, [r3, #0xa]
	and	r2, r1
	strh	r2, [r3, #0xa]
	ldr	r0, =ewram_2010082
	ldrh	r2, [r3, #0xa]
	ldr	r1, =REG_WIN0H
	ldr	r2, =0xa2600001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80d66cc

@ CollectLivingCombatants
@ r0=action descriptor. Builds the list of combatants still standing and hands
@ it to _Func_b7b6c.
@ The side is chosen by the halfword at descriptor+0x24: above 0x7F scans the
@ enemy ids 0x80..0x85, otherwise the player ids 0..7. Each id is resolved with
@ _Func_77394 and kept only when its current HP at +0x38 is positive. The list
@ is terminated with 0xFF.
.thumb_func_start Func_80d6750  @ 0x080d6750
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r1, #0x24
	ldrsh	r3, [r0, r1]
	sub	sp, #0x20
	mov	r2, #0
	cmp	r3, #0x7f
	ble	.Ld678e
	add	r3, sp, #4
	mov	r8, r3
	mov	r6, #0
	mov	r7, r8
.Ld676a:
	mov	r5, r6
	add	r5, #0x80
	mov	r0, r5
	str	r2, [sp]
	bl	_GetUnit
	mov	r1, #0x38
	ldrsh	r3, [r0, r1]
	ldr	r2, [sp]
	cmp	r3, #0
	ble	.Ld6786
	strh	r5, [r7]
	add	r2, #1
	add	r7, #2
.Ld6786:
	add	r6, #1
	cmp	r6, #6
	bne	.Ld676a
	b	.Ld67b8
.Ld678e:
	add	r3, sp, #4
	mov	r8, r3
	mov	r1, r8
	lsl	r3, r2, #1
	mov	r6, #0
	add	r5, r3, r1
.Ld679a:
	mov	r0, r6
	str	r2, [sp]
	bl	_GetUnit
	mov	r1, #0x38
	ldrsh	r3, [r0, r1]
	ldr	r2, [sp]
	cmp	r3, #0
	ble	.Ld67b2
	strh	r6, [r5]
	add	r2, #1
	add	r5, #2
.Ld67b2:
	add	r6, #1
	cmp	r6, #8
	bne	.Ld679a
.Ld67b8:
	ldr	r3, =0xff
	lsl	r2, #1
	mov	r1, r8
	strh	r3, [r1, r2]
	mov	r0, r8
	mov	r1, #0
	bl	_CreateBattleSpriteOverlays
	add	sp, #0x20
	b	.Ld67d0

	.pool_aligned

.Ld67d0:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80d6750

@ SetUpBattleBackdrop
@ Takes no arguments. Prepares the display for an animation: forces DISPCNT
@ mode 1, sets the window width at iwram_1ad0+0x06 to 0x20, converts the
@ palette through _Func_c08ec, and DMAs 0x4000 bytes of tile data from
@ [iwram_1e74]+0x7C to 0x6004000.
@ After a frame it programmes the blend: REG_BLDCNT 0x3F46, REG_BLDALPHA
@ 0x100E, DISPCNT 0x7741, and sets the projection reference at iwram_1ce0+0x10
@ to 0x78.
.thumb_func_start Func_80d67dc  @ 0x080d67dc
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	ldr	r3, =iwram_3001e74
	mov	r1, #0x80
	ldr	r6, [r3, #0x7c]
	ldr	r2, [r3]
	ldr	r3, .Ld6820	@ 1
	lsl	r1, #19
	strh	r3, [r1]
	mov	r8, r1
	ldr	r1, =iwram_3001ad0
	mov	r3, #0x20
	strh	r3, [r1, #6]
	mov	r3, #0xc9
	lsl	r3, #3
	add	r2, r3
	ldrh	r1, [r2]
	mov	r0, #1
	mov	r2, #0x18
	sub	sp, #4
	bl	_AnimTransitionIn
	mov	r5, #0
	mov	r4, sp
	str	r5, [r4]
	ldr	r3, =REG_DMA3SAD
	mov	r0, r4
	mov	r1, r6
	ldr	r2, =0x85001000
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r5, [r4]
	b	.Ld6834

	.align	2, 0
.Ld6820:
	.word	1
	.pool

.Ld6834:
	mov	r0, r4
	ldr	r1, =0x6004000
	ldr	r2, =0x85001000
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, #1
	bl	WaitFrames
	ldr	r2, =REG_BLDALPHA
	ldr	r3, .Ld6860	@ 0x100e
	strh	r3, [r2]
	ldr	r3, .Ld6864	@ 0x3f46
	sub	r2, #2
	strh	r3, [r2]
	ldr	r3, .Ld6868	@ 0x7741
	mov	r1, r8
	ldr	r2, =gPhysVec
	strh	r3, [r1]
	mov	r3, #0x78
	str	r3, [r2, #0x10]
	add	sp, #4
	b	.Ld687c

	.align	2, 0
.Ld6860:
	.word	0x100e
.Ld6864:
	.word	0x3f46
.Ld6868:
	.word	0x7741
	.pool

.Ld687c:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80d67dc

@ SetCombatantSpriteState
@ r0=palette source, r1=palette index (-1 to leave alone), r2=animation index
@ (-1 to leave alone), r3=per-combatant byte to record (-1 to skip).
@ Walks every combatant in the current action via _Func_b7dd0 and _Func_b7f70.
@ For each one:
@   - stores r3 into the per-combatant array at [iwram_1eec]+0x7818
@   - for every part of the combatant's actor except the two held at +0x20 and
@     +0x24 of the battle record (the ones the action itself is animating),
@     sets the palette byte at +0x05 -- computed by _Func_b6cd0 from r0 when
@     r1 is 0, otherwise r1 directly -- and invalidates the cached frame by
@     writing 0xFF to +0x16
@   - applies animation r2 with _Func_ba30
@ Combatants whose battle record has a non-zero halfword at +0x2A are skipped
@ entirely.
.thumb_func_start Func_80d6888  @ 0x080d6888
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x14
	mov	r11, r3
	str	r0, [sp, #0x10]
	mov	r9, r1
	str	r2, [sp, #0xc]
	bl	_GetBattleActor
	ldr	r3, =iwram_3001eec
	ldr	r3, [r3]
	mov	r8, r0
	mov	r0, #0
	str	r3, [sp, #8]
	str	r0, [sp, #4]
	b	.Ld6936
.Ld68b2:
	mov	r2, #1
	neg	r2, r2
	cmp	r11, r2
	beq	.Ld68c6
	add	r1, sp, #0x34
	ldr	r3, =0x7818
	ldrb	r0, [r1]
	ldr	r1, [sp, #8]
	add	r3, r11
	strb	r0, [r1, r3]
.Ld68c6:
	mov	r1, r8
	mov	r0, #0x2a
	ldrsh	r3, [r1, r0]
	cmp	r3, #0
	bne	.Ld6930
	cmp	r9, r2
	beq	.Ld6920
	mov	r3, #0x27
	add	r3, r7
	mov	r10, r3
	ldrb	r3, [r3]
	mov	r2, #0
	cmp	r3, #0
	beq	.Ld6920
	mov	r6, r7
	add	r6, #0x28
.Ld68e6:
	ldmia	r6!, {r5}
	cmp	r5, #0
	beq	.Ld6916
	mov	r0, r8
	ldr	r3, [r0, #0x24]
	cmp	r5, r3
	beq	.Ld6916
	ldr	r3, [r0, #0x20]
	cmp	r5, r3
	beq	.Ld6916
	mov	r1, r9
	cmp	r1, #0
	bne	.Ld690e
	ldr	r0, [sp, #0x10]
	str	r2, [sp]
	bl	_Func_80b6cd0
	strb	r0, [r5, #5]
	ldr	r2, [sp]
	b	.Ld6912
.Ld690e:
	mov	r3, r9
	strb	r3, [r5, #5]
.Ld6912:
	mov	r3, #0xff
	strb	r3, [r5, #0x16]
.Ld6916:
	mov	r0, r10
	ldrb	r3, [r0]
	add	r2, #1
	cmp	r2, r3
	bne	.Ld68e6
.Ld6920:
	mov	r2, #1
	ldr	r1, [sp, #0xc]
	neg	r2, r2
	cmp	r1, r2
	beq	.Ld6930
	mov	r0, r7
	bl	_Sprite_SetAnim
.Ld6930:
	ldr	r3, [sp, #4]
	add	r3, #1
	str	r3, [sp, #4]
.Ld6936:
	mov	r1, r8
	ldr	r0, [r1]
	ldr	r1, [sp, #4]
	bl	_Func_80b7f70
	mov	r7, r0
	cmp	r7, #0
	bne	.Ld68b2
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80d6888

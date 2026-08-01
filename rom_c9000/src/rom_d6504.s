	.include "macros.inc"

@ ============================================================================
@ Battle animation dispatch.
@
@ rom_c9000 is the presentation layer for battle: it plays the animation for an
@ action while rom_b5000 owns the turn logic and the numbers. rom_b5000 calls
@ into it through the two entry points here.
@
@ ALLOCATION TAGS. Func_48b0(tag, size) allocates and Func_2dd8(tag) frees; the
@ pointer lives at iwram_1e50 + tag*4, so the symbol a handler dereferences
@ tells you which tag it wants. The three Func_d6578 allocates land adjacently:
@     tag 0x27  iwram_1eec  0x782C  the battle-animation state block
@     tag 0x28  iwram_1ef0  0x4000  destination render buffer (128x128 8bpp)
@     tag 0x29  iwram_1ef4  0x302   sprite scratch
@ and the two blitters Func_ed408 generates sit just past them, which is why
@ handlers reach them as [iwram_1ef0 + 0x18] and [iwram_1ef0 + 0x1c]:
@     tag 0x2E  iwram_1ef0+0x18     generated blitter A
@     tag 0x2F  iwram_1ef0+0x1c     generated blitter B
@ Both are called as blit(dst, srcGfx, x, y, w, h), x and y naming the CENTRE.
@
@ SHARED SPRITE TABLE. Data_ede48 is the cumulative offset of each sprite size
@ in the tag-0x29 buffer: size n is n wide by 2n tall, so entry n is the sum of
@ 2k^2 for k < n -- 0, 2, 10, 28, 60, 110, 182, 280, 408, 570. Ten sizes ending
@ at 570 + 200 = 770 = 0x302, exactly the buffer's size.
@
@ State: the 0x782C block at [iwram_1eec]. Notable fields:
@     +0x77AC/+0x77B0  screen-shake amount and mode
@     +0x77D8..+0x7807 effect-actor array, filled by Func_dbb24
@     +0x7818          per-combatant byte array
@     +0x7824          set to 1 each frame to arm the Func_cd260 effect task
@     +0x7828          the current action descriptor
@ Action descriptor fields used by the handlers: +0x00 animation class,
@ +0x04 a side/direction selector, +0x08 the acting combatant, +0x14 the target
@ count, +0x24 onward an array of halfword target ids.
@
@ EVERY handler is synchronous: it runs its own frame loop, calling Func_30f8(1)
@ once per frame, and returns only when the animation is over. Holding A or B
@ (iwram_1b04 & 3) fast-forwards by jumping the frame counter. Func_bd7dc(code)
@ raises the one-shot flag at [iwram_1e74]+0x800 (code at +0x820) that hands
@ control back to rom_b5000.
@
@ Combatants are addressed by id through _Func_77394 (rom_8a000's party record
@ lookup): ids 0..7 are the player's side, 0x80..0x85 the enemy side, and the
@ halfword at +0x38 of a record is current HP -- zero means down.
@ ============================================================================
	.include "gba.inc"

@ StepScreenShake
@ Takes no arguments. Advances the battle screen shake one frame using the
@ amount at [iwram_1eec]+0x77AC and the mode at +0x77B0, adding the offset into
@ the view's y at [iwram_1e80]+0x36.
@ Mode 1 applies the full amount once and then clears the mode -- a single
@ jolt. Any other mode applies half the amount and alternates between 2 and 0,
@ giving the oscillating shake.
.thumb_func_start Func_d6504
	push	{lr}
	ldr	r3, =iwram_1eec
	ldr	r2, [r3]
	sub	r3, #0x6c
	ldr	r1, [r3]
	ldr	r3, =0x77b0
	add	r0, r2, r3
	ldr	r3, [r0]
	cmp	r3, #1
	bne	.Ld652a
	ldr	r4, =0x77ac
	add	r3, r2, r4
	ldr	r2, [r3]
	ldrh	r3, [r1, #0x36]
	add	r3, r2
	mov	r2, #0
	strh	r3, [r1, #0x36]
	str	r2, [r0]
	b	.Ld654a
.Ld652a:
	ldr	r4, =0x77ac
	add	r3, r2, r4
	ldr	r2, [r3]
	lsr	r3, r2, #31
	add	r2, r3
	ldrh	r3, [r1, #0x36]
	asr	r2, #1
	add	r3, r2
	strh	r3, [r1, #0x36]
	ldr	r3, [r0]
	cmp	r3, #2
	bne	.Ld6546
	mov	r3, #0
	b	.Ld6548
.Ld6546:
	mov	r3, #2
.Ld6548:
	str	r3, [r0]
.Ld654a:
	pop	{r0}
	bx	r0
.func_end Func_d6504

@ AdvanceFrames
@ r0=count. Calls Func_dbb98 r0 times -- the module's per-frame step -- so an
@ animation can hold for a fixed number of frames. A count of 0 returns
@ immediately.
.thumb_func_start Func_d655c
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r6, #0
	cmp	r5, #0
	beq	.Ld6570
.Ld6566:
	add	r6, #1
	bl	Func_dbb98
	cmp	r6, r5
	bne	.Ld6566
.Ld6570:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_d655c

@ PlayActionAnimation
@ r0=action descriptor. The main entry point rom_b5000 calls to play a battle
@ action. Allocates the three working buffers (0x302 under tag 0x29, 0x782C
@ under 0x27, 0x4000 under 0x28), dispatches on the animation class in
@ descriptor[0], and releases all three before returning.
@ Class 0 and 11 both fall through to Func_e7320; classes 1..12 select one of
@ twelve handlers via the table at .Ld65ac, each living in its own file:
@     1 Func_e823c    2 Func_d2d98    3 Func_eb754    4 Func_dc968
@     5 Func_d6970    6 Func_ec100    7 Func_d2458    8 Func_d1714
@     9 Func_ea0d8   10 Func_d765c   11 Func_e7320   12 Func_e15e8
@ A class above 12 allocates and frees the buffers without playing anything.
.thumb_func_start Func_d6578
	push	{r5, lr}
	mov	r5, r0
	ldr	r1, =0x302
	mov	r0, #0x29
	bl	Func_48b0
	ldr	r1, =0x782c
	mov	r0, #0x27
	bl	Func_48b0
	mov	r1, #0x80
	lsl	r1, #7
	mov	r0, #0x28
	bl	Func_48b0
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.Ld662c
	sub	r3, #1
	cmp	r3, #0xb
	bhi	.Ld663a
	ldr	r2, =.Ld65ac
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.Ld65ac:
	.word	.Ld65dc
	.word	.Ld65e4
	.word	.Ld65ec
	.word	.Ld65f4
	.word	.Ld65fc
	.word	.Ld6604
	.word	.Ld660c
	.word	.Ld6614
	.word	.Ld661c
	.word	.Ld6624
	.word	.Ld662c
	.word	.Ld6634
.Ld65dc:
	mov	r0, r5
	bl	Func_e823c
	b	.Ld663a
.Ld65e4:
	mov	r0, r5
	bl	Func_d2d98
	b	.Ld663a
.Ld65ec:
	mov	r0, r5
	bl	Func_eb754
	b	.Ld663a
.Ld65f4:
	mov	r0, r5
	bl	Func_dc968
	b	.Ld663a
.Ld65fc:
	mov	r0, r5
	bl	Func_d6970
	b	.Ld663a
.Ld6604:
	mov	r0, r5
	bl	Func_ec100
	b	.Ld663a
.Ld660c:
	mov	r0, r5
	bl	Func_d2458
	b	.Ld663a
.Ld6614:
	mov	r0, r5
	bl	Func_d1714
	b	.Ld663a
.Ld661c:
	mov	r0, r5
	bl	Func_ea0d8
	b	.Ld663a
.Ld6624:
	mov	r0, r5
	bl	Func_d765c
	b	.Ld663a
.Ld662c:
	mov	r0, r5
	bl	Func_e7320
	b	.Ld663a
.Ld6634:
	mov	r0, r5
	bl	Func_e15e8
.Ld663a:
	mov	r0, #0x28
	bl	Func_2dd8
	mov	r0, #0x27
	bl	Func_2dd8
	mov	r0, #0x29
	bl	Func_2dd8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_d6578

@ PlayActionAnimationByTable
@ r0=action descriptor. The variant used for actions whose handler comes from
@ the Data_ee2b4 table rather than the fixed dispatch in Func_d6578.
@ Allocates the same three buffers, stores the descriptor at [iwram_1eec]+0x7828
@ so the handlers can reach it, then calls Data_ee2b4[class - 1]. A class of 0
@ clears descriptor+0x18 and plays nothing. Buffers are released either way.
.thumb_func_start Func_d6660
	push	{r5, lr}
	mov	r5, r0
	ldr	r1, =0x302
	mov	r0, #0x29
	bl	Func_48b0
	ldr	r1, =0x782c
	mov	r0, #0x27
	bl	Func_48b0
	mov	r1, #0x80
	lsl	r1, #7
	mov	r0, #0x28
	bl	Func_48b0
	ldr	r3, =iwram_1eec
	ldr	r2, =0x7828
	ldr	r3, [r3]
	add	r3, r2
	ldr	r2, [r5]
	str	r5, [r3]
	cmp	r2, #0
	bne	.Ld6692
	str	r2, [r5, #0x18]
	b	.Ld66a0
.Ld6692:
	ldr	r3, =Data_ee2b4
	lsl	r2, #2
	sub	r2, #4
	ldr	r3, [r3, r2]
	mov	r0, r5
	bl	_call_via_r3
.Ld66a0:
	mov	r0, #0x28
	bl	Func_2dd8
	mov	r0, #0x27
	bl	Func_2dd8
	mov	r0, #0x29
	bl	Func_2dd8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_d6660

@ BuildWindowScanlineTable
@ Takes no arguments. Fills the 160-entry WIN0H table at ewram_10082 and arms
@ the DMA0 HBlank transfer that feeds it.
@ Rows outside 8..0x87 get the 0xFFF1 "closed" value. Inside that band the left
@ edge is the base width at ewram_10000 minus the per-row inset from
@ ewram_fffa, clamped to 0..0xF0 -- so the window opens as a shape that follows
@ the inset table rather than a plain rectangle.
.thumb_func_start Func_d66cc
	push	{r5, r6, lr}
	ldr	r6, =ewram_10000
	ldr	r5, .Ld66f8	@ 0xfff1
	ldr	r1, =ewram_10082
	ldr	r4, =ewram_fffa
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
	ldr	r0, =ewram_10082
	ldrh	r2, [r3, #0xa]
	ldr	r1, =REG_WIN0H
	ldr	r2, =0xa2600001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_d66cc

@ CollectLivingCombatants
@ r0=action descriptor. Builds the list of combatants still standing and hands
@ it to _Func_b7b6c.
@ The side is chosen by the halfword at descriptor+0x24: above 0x7F scans the
@ enemy ids 0x80..0x85, otherwise the player ids 0..7. Each id is resolved with
@ _Func_77394 and kept only when its current HP at +0x38 is positive. The list
@ is terminated with 0xFF.
.thumb_func_start Func_d6750
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
	bl	_Func_77394
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
	bl	_Func_77394
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
	bl	_Func_b7b6c
	add	sp, #0x20
	b	.Ld67d0

	.pool_aligned

.Ld67d0:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_d6750

@ SetUpBattleBackdrop
@ Takes no arguments. Prepares the display for an animation: forces DISPCNT
@ mode 1, sets the window width at iwram_1ad0+0x06 to 0x20, converts the
@ palette through _Func_c08ec, and DMAs 0x4000 bytes of tile data from
@ [iwram_1e74]+0x7C to 0x6004000.
@ After a frame it programmes the blend: REG_BLDCNT 0x3F46, REG_BLDALPHA
@ 0x100E, DISPCNT 0x7741, and sets the projection reference at iwram_1ce0+0x10
@ to 0x78.
.thumb_func_start Func_d67dc
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	ldr	r3, =iwram_1e74
	mov	r1, #0x80
	ldr	r6, [r3, #0x7c]
	ldr	r2, [r3]
	ldr	r3, .Ld6820	@ 1
	lsl	r1, #19
	strh	r3, [r1]
	mov	r8, r1
	ldr	r1, =iwram_1ad0
	mov	r3, #0x20
	strh	r3, [r1, #6]
	mov	r3, #0xc9
	lsl	r3, #3
	add	r2, r3
	ldrh	r1, [r2]
	mov	r0, #1
	mov	r2, #0x18
	sub	sp, #4
	bl	_Func_c08ec
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
	bl	Func_30f8
	ldr	r2, =REG_BLDALPHA
	ldr	r3, .Ld6860	@ 0x100e
	strh	r3, [r2]
	ldr	r3, .Ld6864	@ 0x3f46
	sub	r2, #2
	strh	r3, [r2]
	ldr	r3, .Ld6868	@ 0x7741
	mov	r1, r8
	ldr	r2, =iwram_1ce0
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
.func_end Func_d67dc

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
.thumb_func_start Func_d6888
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
	bl	_Func_b7dd0
	ldr	r3, =iwram_1eec
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
	bl	_Func_b6cd0
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
	bl	_Func_ba30
.Ld6930:
	ldr	r3, [sp, #4]
	add	r3, #1
	str	r3, [sp, #4]
.Ld6936:
	mov	r1, r8
	ldr	r0, [r1]
	ldr	r1, [sp, #4]
	bl	_Func_b7f70
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
.func_end Func_d6888

@ RestoreBattleView
@ Takes no arguments. Undoes the animation's display changes: Func_cdb24(1)
@ followed by Func_cdbc0, putting the battle screen back to its normal state.
.thumb_func_start Func_d6960
	push	{lr}
	mov	r0, #1
	bl	Func_cdb24
	bl	Func_cdbc0
	pop	{r0}
	bx	r0
.func_end Func_d6960

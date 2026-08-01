	.include "macros.inc"
	.include "gba.inc"

@ ============================================================================
@ Party, character records and event flags.
@
@ rom_77000 is the game's data layer. It owns the character records, the derived
@ stats, the inventory and the global event flags, and it hands them out to
@ everyone else. 100 functions are exported and there is essentially no hardware
@ use (four DMA3 setups), so nothing here draws -- it answers questions.
@
@ Func_77394 is the MOST-CALLED CROSS-MODULE FUNCTION IN THE ROM at 226 call
@ sites. If you are reading any other module and see `_Func_77394`, it is
@ fetching a combatant record.
@
@ COMBATANT RECORDS -- 0x14C bytes each, Func_77394(id) resolves them:
@     ids 0x00..0x07   ewram_500 + id * 0x14C            the player's side
@     ids 0x80..0x85   [iwram_1f28] + id * 0x14C - 0xA600  the enemy side
@     anything else    0
@ The enemy bias is exact: 0x80 * 0x14C = 0xA600, so enemy 0x80 sits at offset 0
@ of the block iwram_1f28 points at. That is the whole reason the ids are split
@ 0..7 / 0x80..0x85 rather than being contiguous.
@
@ Record fields established so far:
@     +0x0F  a per-character byte summed across the party by Func_77348
@     +0x10, +0x12  signed pair copied out first by Func_77428
@     +0x14  CURRENT HP AS A FRACTION, (curHP << 14) / maxHP, clamped 0..0x4000
@     +0x34  max HP        +0x36  max PP
@     +0x38  current HP    +0x3A  current PP
@ Func_7822c recomputes +0x14 and is called after every HP or PP change.
@
@ THE SAVE VARIABLE STORE at ewram_40 -- 512 bytes, addressed at four
@ granularities by the accessors in rom_79338.s. It is not just a flag array:
@     bit     Func_79338 test, Func_79358 set, Func_79374 clear, Func_79390
@             toggle -- 288 call sites between them
@     byte    Func_793b8 read, Func_793c8 write
@     counter Func_793d8 increment, Func_793f8 decrement, both SATURATING at
@             0xFF and 0 rather than wrapping
@     nibble  Func_79418 read, Func_79434 write, with bit 2 of the index picking
@             the low or high nibble
@ All of them index the same byte as `(idx & 0xFFF) >> 3`, written
@ `(idx << 20) >> 23` to mask and shift in one instruction pair.
@ Because the views overlap, a "flag" and a "counter" can occupy the same byte;
@ do not assume an index is only ever used one way.
@ INDICES 0..7 ARE PARTY MEMBERSHIP: Func_795fc counts exactly those eight bits
@ to get the active party size, so bit N means "character N has joined". That
@ also matches the eight entries in the Func_78ed8 base-stat table.
@
@ ITEM RECORDS -- 0x54 bytes each at Data_80ec8, resolved by Func_773d8(id) for
@ ids 8..0x101; out-of-range ids fall back to entry 0.
@
@ SAVE DATA lives at ewram_240. The active party's member ids are the byte list
@ at ewram_240 + 0x1F8, and ewram_240 + 0x205 is the in-game hour (see
@ rom_15000's Func_1ca1c).
@ ============================================================================

@ InitPartyData
@ Takes no arguments. Builds the party tables with Func_77d38 and hands off to
@ _Func_8a8e4 in rom_8a000.
.thumb_func_start Func_77320
	push	{lr}
	bl	Func_77d38
	mov	r0, #0
	bl	_Func_8a8e4
	pop	{r0}
	bx	r0
.func_end Func_77320

@ GetRecordOrDefault
@ r0 = non-zero to fetch combatant 0x83, 0 to return ewram_24c instead.
@ A two-way selector some callers use to fall back on a scratch record rather
@ than a real combatant.
.thumb_func_start Func_77330
	push	{lr}
	cmp	r0, #0
	beq	.L7733e
	mov	r0, #0x83
	bl	Func_77394
	b	.L77340
.L7733e:
	ldr	r0, =ewram_24c
.L77340:
	pop	{r1}
	bx	r1
.func_end Func_77330

@ SumPartyField
@ Takes no arguments. Returns the sum of byte +0x0F across every active party
@ member, then divides by the party size with Func_af0 -- so this is a PARTY
@ AVERAGE, not a total.
@ The member ids come from the byte list at ewram_240+0x1F8 and the count from
@ Func_795fc.
.thumb_func_start Func_77348
	push	{r5, r6, r7, lr}
	sub	sp, #4
	bl	Func_795fc
	mov	r7, r0
	mov	r6, #0
	mov	r0, #0
	cmp	r7, #0
	beq	.L77388
	cmp	r6, r7
	bge	.L7737e
	ldr	r3, =ewram_240
	mov	r1, #0xfc
	lsl	r1, #1
	add	r2, r3, r1
	mov	r5, r7
.L77368:
	ldrb	r0, [r2]
	add	r2, #1
	str	r2, [sp]
	bl	Func_77394
	ldrb	r3, [r0, #0xf]
	sub	r5, #1
	add	r6, r3
	ldr	r2, [sp]
	cmp	r5, #0
	bne	.L77368
.L7737e:
	mov	r0, r6
	mov	r1, r7
	bl	Func_af0_from_thumb
	mov	r6, r0
.L77388:
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_77348

@ GetCombatantRecord
@ r0 = combatant id. Returns its 0x14C-byte record, or 0 for an unknown id.
@     0x00..0x07  ewram_500 + id * 0x14C
@     0x80..0x85  [iwram_1f28] + id * 0x14C - 0xA600, so 0x80 lands at offset 0
@ Enemy ids also return 0 when iwram_1f28 is null, i.e. outside battle.
@ The most-called cross-module function in the ROM -- 226 sites.
.thumb_func_start Func_77394
	push	{lr}
	mov	r3, r14
	ldr	r2, =ewram_500
	cmp	r0, #7
	bhi	.L773a8
	mov	r3, #0xa6
	lsl	r3, #1
	mul	r3, r0
	add	r0, r3, r2
	b	.L773c8
.L773a8:
	mov	r3, r0
	sub	r3, #0x80
	cmp	r3, #5
	bhi	.L773c6
	ldr	r3, =iwram_1f28
	ldr	r2, [r3]
	cmp	r2, #0
	beq	.L773c6
	mov	r3, #0xa6
	lsl	r3, #1
	mul	r3, r0
	add	r3, r2, r3
	ldr	r2, =0xffff5a00
	add	r0, r3, r2
	b	.L773c8
.L773c6:
	mov	r0, #0
.L773c8:
	pop	{r1}
	bx	r1
.func_end Func_77394

@ GetItemRecord
@ r0 = item id. Returns Data_80ec8 + (id - 8) * 0x54.
@ Ids below 8 or above 0x101 are clamped to entry 0, so a bad id yields the
@ null item rather than reading out of bounds. Item records are 0x54 bytes.
.thumb_func_start Func_773d8
	push	{lr}
	sub	r0, #8
	cmp	r0, #0xf9
	bls	.L773e2
	mov	r0, #0
.L773e2:
	mov	r3, #0x54
	mul	r0, r3
	ldr	r3, =Data_80ec8
	add	r0, r3
	pop	{r1}
	bx	r1
.func_end Func_773d8

@ CopyBytesDirectional
@ r0, r1 = the two buffers, r2 = length, r3 = direction.
@ A non-zero r3 copies r0 -> r1; zero copies r1 -> r0. Both arms are plain
@ forward byte loops, so overlapping ranges are NOT handled -- this is a memcpy
@ with a swappable sense, not a memmove.
.thumb_func_start Func_773f4
	push	{lr}
	cmp	r3, #0
	beq	.L77410
	mov	r4, #0
	cmp	r4, r2
	bge	.L77424
.L77400:
	ldrb	r3, [r0]
	add	r4, #1
	strb	r3, [r1]
	add	r0, #1
	add	r1, #1
	cmp	r4, r2
	blt	.L77400
	b	.L77424
.L77410:
	cmp	r2, #0
	ble	.L77424
	mov	r4, r2
.L77416:
	ldrb	r3, [r1]
	sub	r4, #1
	strb	r3, [r0]
	add	r1, #1
	add	r0, #1
	cmp	r4, #0
	bne	.L77416
.L77424:
	pop	{r0}
	bx	r0
.func_end Func_773f4

@ BuildCharacterSummary
@ r0 = combatant id. Allocates a 0x60-byte scratch with Func_4970 and fills it
@ with everything the UI needs about a character: the signed pair at +0x10/+0x12,
@ the halfwords at +0x18/+0x1A/+0x1C, the byte at +0x1E, the nibble fields of
@ +0x1F, and much more, consulting item records through Func_78414 and event
@ flags through Func_79338.
@ 1023 lines and the largest routine in the module; traced structurally. This is
@ where derived stats are computed, so it is the function to read when a
@ displayed number does not match a stored one.
.thumb_func_start Func_77428
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	mov	r0, #0x60
	sub	sp, #4
	bl	Func_4970
	mov	r6, r0
	mov	r0, r5
	bl	Func_77394
	mov	r7, r0
	mov	r0, #0x10
	ldrsh	r3, [r7, r0]
	str	r3, [r6]
	mov	r1, #0x12
	ldrsh	r3, [r7, r1]
	str	r3, [r6, #4]
	ldrh	r3, [r7, #0x18]
	str	r3, [r6, #8]
	ldrh	r3, [r7, #0x1a]
	str	r3, [r6, #0xc]
	ldrh	r3, [r7, #0x1c]
	str	r3, [r6, #0x10]
	ldrb	r3, [r7, #0x1e]
	str	r3, [r6, #0x18]
	ldrb	r2, [r7, #0x1f]
	mov	r3, #0xf
	and	r3, r2
	str	r3, [r6, #0x1c]
	mov	r3, r7
	add	r3, #0x20
	ldrb	r3, [r3]
	str	r3, [r6, #0x20]
	mov	r3, r7
	add	r3, #0x21
	ldrb	r3, [r3]
	mov	r1, r7
	mov	r2, r6
	str	r3, [r6, #0x24]
	add	r1, #0x24
	add	r2, #0x28
	mov	r5, #3
.L77482:
	mov	r0, #0
	ldrsh	r3, [r1, r0]
	str	r3, [r2]
	mov	r0, #2
	ldrsh	r3, [r1, r0]
	sub	r5, #1
	str	r3, [r2, #4]
	add	r1, #4
	add	r2, #8
	cmp	r5, #0
	bge	.L77482
	mov	r1, #0x34
	ldrsh	r2, [r7, r1]
	mov	r0, #0x14
	ldrsh	r3, [r7, r0]
	mul	r3, r2
	mov	r2, r3
	cmp	r3, #0
	bge	.L774ac
	ldr	r1, =0x3fff
	add	r2, r3, r1
.L774ac:
	asr	r0, r2, #14
	mov	r2, #0x38
	ldrsh	r1, [r7, r2]
	sub	r2, r0, r1
	cmp	r2, #0
	blt	.L774be
	cmp	r2, #1
	bgt	.L774f0
	b	.L774c4
.L774be:
	sub	r3, r1, r0
	cmp	r3, #1
	bgt	.L774f0
.L774c4:
	mov	r3, #0x36
	ldrsh	r2, [r7, r3]
	mov	r0, #0x16
	ldrsh	r3, [r7, r0]
	mul	r3, r2
	mov	r2, r3
	cmp	r3, #0
	bge	.L774d8
	ldr	r1, =0x3fff
	add	r2, r3, r1
.L774d8:
	asr	r0, r2, #14
	mov	r2, #0x3a
	ldrsh	r1, [r7, r2]
	sub	r2, r0, r1
	cmp	r2, #0
	blt	.L774ea
	cmp	r2, #1
	bgt	.L774f0
	b	.L77500
.L774ea:
	sub	r3, r1, r0
	cmp	r3, #1
	ble	.L77500
.L774f0:
	mov	r3, #0x80
	lsl	r3, #7
	strh	r3, [r7, #0x14]
	strh	r3, [r7, #0x16]
	ldrh	r3, [r7, #0x34]
	strh	r3, [r7, #0x38]
	ldrh	r3, [r7, #0x36]
	strh	r3, [r7, #0x3a]
.L77500:
	mov	r3, #0x98
	lsl	r3, #1
	add	r1, r7, r3
	ldrb	r3, [r1]
	mov	r0, #4
	neg	r0, r0
	and	r0, r3
	mov	r3, #4
	and	r3, r0
	mov	r4, #0
	strb	r0, [r1]
	cmp	r3, #0
	beq	.L77522
	mov	r2, #1
	mov	r3, r0
	orr	r3, r2
	strb	r3, [r1]
.L77522:
	mov	r0, #0xa2
	lsl	r0, #1
	add	r3, r7, r0
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L77534
	ldr	r3, [r6, #0x1c]
	add	r3, #1
	str	r3, [r6, #0x1c]
.L77534:
	mov	r1, #0xa1
	lsl	r1, #1
	ldr	r2, =0x143
	add	r3, r7, r1
	strb	r4, [r3]
	ldr	r0, =0x129
	add	r3, r7, r2
	strb	r4, [r3]
	add	r3, r7, r0
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L7754e
	b	.L7790c
.L7754e:
	mov	r5, #0
.L77550:
	lsl	r3, r5, #1
	mov	r1, r3
	add	r1, #0xd8
	ldrh	r2, [r7, r1]
	mov	r3, #0x80
	lsl	r3, #2
	and	r3, r2
	cmp	r3, #0
	bne	.L77564
	b	.L77708
.L77564:
	ldrh	r0, [r7, r1]
	bl	Func_78414
	ldrb	r2, [r0, #3]
	mov	r3, #1
	and	r3, r2
	str	r0, [r6, #0x58]
	cmp	r3, #0
	beq	.L77584
	mov	r2, #0x98
	lsl	r2, #1
	add	r1, r7, r2
	ldrb	r2, [r1]
	mov	r3, #3
	orr	r3, r2
	strb	r3, [r1]
.L77584:
	ldr	r1, [r6, #0x58]
	mov	r3, #8
	ldrsh	r2, [r1, r3]
	ldr	r3, [r6, #8]
	add	r3, r2
	str	r3, [r6, #8]
	mov	r2, #0xa
	ldrsb	r2, [r1, r2]
	ldr	r3, [r6, #0xc]
	add	r3, r2
	mov	r0, #0
	str	r3, [r6, #0xc]
	mov	r8, r0
.L7759e:
	mov	r1, r8
	ldr	r2, [r6, #0x58]
	lsl	r3, r1, #2
	add	r3, #0x18
	ldrb	r1, [r2, r3]
	add	r2, r3
	mov	r3, #1
	ldrsb	r3, [r2, r3]
	str	r1, [r6, #0x48]
	str	r3, [r6, #0x54]
	cmp	r1, #0x1a
	bls	.L775b8
	b	.L776fc
.L775b8:
	ldr	r2, =.L775c0
	lsl	r3, r1, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L775c0:
	.word	.L776fc
	.word	.L7762c
	.word	.L77636
	.word	.L77640
	.word	.L7764a
	.word	.L77654
	.word	.L7765e
	.word	.L776fc
	.word	.L776fc
	.word	.L776fc
	.word	.L776fc
	.word	.L776fc
	.word	.L776fc
	.word	.L776fc
	.word	.L776fc
	.word	.L77668
	.word	.L77672
	.word	.L7767c
	.word	.L77686
	.word	.L77690
	.word	.L7769a
	.word	.L776a4
	.word	.L776ae
	.word	.L776b8
	.word	.L776c4
	.word	.L776d2
	.word	.L776f4
.L7762c:
	ldr	r3, [r6]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6]
	b	.L776fc
.L77636:
	ldr	r3, [r6, #0x20]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x20]
	b	.L776fc
.L77640:
	ldr	r3, [r6, #4]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #4]
	b	.L776fc
.L7764a:
	ldr	r3, [r6, #0x24]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x24]
	b	.L776fc
.L77654:
	ldr	r3, [r6, #0x10]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x10]
	b	.L776fc
.L7765e:
	ldr	r3, [r6, #0x18]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x18]
	b	.L776fc
.L77668:
	ldr	r3, [r6, #0x28]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x28]
	b	.L776fc
.L77672:
	ldr	r3, [r6, #0x30]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x30]
	b	.L776fc
.L7767c:
	ldr	r3, [r6, #0x38]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x38]
	b	.L776fc
.L77686:
	ldr	r3, [r6, #0x40]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x40]
	b	.L776fc
.L77690:
	ldr	r3, [r6, #0x2c]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x2c]
	b	.L776fc
.L7769a:
	ldr	r3, [r6, #0x34]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x34]
	b	.L776fc
.L776a4:
	ldr	r3, [r6, #0x3c]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x3c]
	b	.L776fc
.L776ae:
	ldr	r3, [r6, #0x44]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x44]
	b	.L776fc
.L776b8:
	mov	r2, #0xa1
	lsl	r2, #1
	add	r1, r7, r2
	ldrb	r3, [r1]
	ldr	r2, [r6, #0x54]
	b	.L776cc
.L776c4:
	ldr	r3, =0x143
	add	r1, r7, r3
	ldr	r2, [r6, #0x54]
	ldrb	r3, [r1]
.L776cc:
	add	r3, r2
	strb	r3, [r1]
	b	.L776fc
.L776d2:
	mov	r0, #0x98
	lsl	r0, #1
	add	r3, r7, r0
	ldrb	r1, [r3]
	mov	r2, #8
	orr	r2, r1
	strb	r2, [r3]
	b	.L776fc

	.pool_aligned

.L776f4:
	ldr	r3, [r6, #0x1c]
	ldr	r2, [r6, #0x54]
	add	r3, r2
	str	r3, [r6, #0x1c]
.L776fc:
	mov	r1, #1
	add	r8, r1
	mov	r2, r8
	cmp	r2, #3
	bgt	.L77708
	b	.L7759e
.L77708:
	add	r5, #1
	cmp	r5, #0xe
	bgt	.L77710
	b	.L77550
.L77710:
	mov	r3, #0x98
	lsl	r3, #1
	add	r1, r7, r3
	ldrb	r2, [r1]
	mov	r3, #8
	and	r3, r2
	cmp	r3, #0
	beq	.L77728
	mov	r3, #0xa
	neg	r3, r3
	and	r3, r2
	strb	r3, [r1]
.L77728:
	mov	r0, #0x84
	lsl	r0, #1
	add	r0, r7
	mov	r4, #0
	mov	r8, r0
.L77732:
	mov	r1, r8
	ldr	r1, [r1]
	mov	r5, #0
	mov	r10, r1
.L7773a:
	mov	r3, #1
	lsl	r3, r5
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.L7778e
	mov	r0, r4
	mov	r1, r5
	str	r4, [sp]
	bl	Func_7a0cc
	ldr	r3, [r6]
	mov	r2, #4
	ldrsb	r2, [r0, r2]
	add	r3, r2
	str	r3, [r6]
	mov	r2, #5
	ldrsb	r2, [r0, r2]
	ldr	r3, [r6, #4]
	add	r3, r2
	str	r3, [r6, #4]
	mov	r2, #6
	ldrsb	r2, [r0, r2]
	ldr	r3, [r6, #8]
	add	r3, r2
	str	r3, [r6, #8]
	mov	r2, #7
	ldrsb	r2, [r0, r2]
	ldr	r3, [r6, #0xc]
	add	r3, r2
	str	r3, [r6, #0xc]
	mov	r2, #8
	ldrsb	r2, [r0, r2]
	ldr	r3, [r6, #0x10]
	add	r3, r2
	str	r3, [r6, #0x10]
	mov	r2, #9
	ldrsb	r2, [r0, r2]
	ldr	r3, [r6, #0x18]
	add	r3, r2
	str	r3, [r6, #0x18]
	ldr	r4, [sp]
.L7778e:
	add	r5, #1
	cmp	r5, #0x13
	ble	.L7773a
	mov	r3, #4
	add	r4, #1
	add	r8, r3
	cmp	r4, #3
	ble	.L77732
	ldr	r0, =0x129
	add	r3, r7, r0
	ldrb	r0, [r3]
	bl	Func_79ad8
	mov	r5, r0
	ldrb	r2, [r5, #8]
	ldr	r3, [r6]
	mov	r1, #0xa
	mov	r0, r2
	mul	r0, r3
	bl	Func_af0_from_thumb
	ldrb	r2, [r5, #9]
	ldr	r3, [r6, #4]
	str	r0, [r6]
	mov	r1, #0xa
	mov	r0, r2
	mul	r0, r3
	bl	Func_af0_from_thumb
	ldrb	r2, [r5, #0xa]
	ldr	r3, [r6, #8]
	str	r0, [r6, #4]
	mov	r1, #0xa
	mov	r0, r2
	mul	r0, r3
	bl	Func_af0_from_thumb
	ldrb	r2, [r5, #0xb]
	ldr	r3, [r6, #0xc]
	str	r0, [r6, #8]
	mov	r1, #0xa
	mov	r0, r2
	mul	r0, r3
	bl	Func_af0_from_thumb
	ldrb	r2, [r5, #0xc]
	ldr	r3, [r6, #0x10]
	str	r0, [r6, #0xc]
	mov	r1, #0xa
	mov	r0, r2
	mul	r0, r3
	bl	Func_af0_from_thumb
	ldrb	r2, [r5, #0xd]
	ldr	r3, [r6, #0x18]
	str	r0, [r6, #0x10]
	mov	r1, #0xa
	mov	r0, r2
	mul	r0, r3
	bl	Func_af0_from_thumb
	str	r0, [r6, #0x18]
	mov	r5, #0
.L7780c:
	lsl	r3, r5, #1
	mov	r1, r3
	add	r1, #0xd8
	ldrh	r2, [r7, r1]
	mov	r3, #0x80
	lsl	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.L77904
	ldrh	r0, [r7, r1]
	bl	Func_78414
	mov	r1, #0
	str	r0, [r6, #0x58]
	mov	r8, r1
.L7782a:
	mov	r0, r8
	ldr	r2, [r6, #0x58]
	lsl	r3, r0, #2
	add	r3, #0x18
	ldrb	r1, [r2, r3]
	add	r2, r3
	mov	r3, #1
	ldrsb	r3, [r2, r3]
	str	r1, [r6, #0x48]
	sub	r1, #7
	str	r3, [r6, #0x54]
	cmp	r1, #7
	bhi	.L778fa
	ldr	r2, =.L7784c
	lsl	r3, r1, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L7784c:
	.word	.L7786c
	.word	.L7787e
	.word	.L77890
	.word	.L778a2
	.word	.L778b4
	.word	.L778c6
	.word	.L778d8
	.word	.L778ea
.L7786c:
	ldr	r2, [r6]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6]
	b	.L778fa
.L7787e:
	ldr	r2, [r6, #0x20]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6, #0x20]
	b	.L778fa
.L77890:
	ldr	r2, [r6, #4]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6, #4]
	b	.L778fa
.L778a2:
	ldr	r2, [r6, #0x24]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6, #0x24]
	b	.L778fa
.L778b4:
	ldr	r2, [r6, #8]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6, #8]
	b	.L778fa
.L778c6:
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6, #0xc]
	b	.L778fa
.L778d8:
	ldr	r2, [r6, #0x10]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6, #0x10]
	b	.L778fa
.L778ea:
	ldr	r2, [r6, #0x18]
	ldr	r3, [r6, #0x54]
	mov	r1, #0xa
	mov	r0, r3
	mul	r0, r2
	bl	Func_af0_from_thumb
	str	r0, [r6, #0x18]
.L778fa:
	mov	r1, #1
	add	r8, r1
	mov	r2, r8
	cmp	r2, #3
	ble	.L7782a
.L77904:
	add	r5, #1
	cmp	r5, #0xe
	bgt	.L7790c
	b	.L7780c
.L7790c:
	ldr	r0, =0x133
	add	r3, r7, r0
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	ldr	r2, [r6, #8]
	add	r3, #8
	mul	r3, r2
	cmp	r3, #0
	bge	.L77922
	add	r3, #7
.L77922:
	asr	r3, #3
	ldr	r1, =0x135
	str	r3, [r6, #8]
	add	r3, r7, r1
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	ldr	r2, [r6, #0xc]
	add	r3, #8
	mul	r3, r2
	cmp	r3, #0
	bge	.L7793c
	add	r3, #7
.L7793c:
	asr	r3, #3
	ldr	r2, =0x147
	str	r3, [r6, #0xc]
	add	r3, r7, r2
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	ldr	r2, [r6, #0x10]
	add	r3, #8
	mul	r3, r2
	cmp	r3, #0
	bge	.L77956
	add	r3, #7
.L77956:
	asr	r3, #3
	str	r3, [r6, #0x10]
	mov	r3, #0x96
	lsl	r3, #1
	mov	r4, #0x28
	mov	r5, #3
	add	r0, r7, r3
.L77964:
	ldrb	r3, [r0]
	lsl	r3, #24
	asr	r3, #24
	mov	r2, r3
	mul	r2, r3
	add	r2, r3
	lsl	r1, r2, #2
	ldr	r3, [r4, r6]
	add	r1, r2
	add	r3, r1
	sub	r5, #1
	str	r3, [r4, r6]
	add	r0, #1
	add	r4, #8
	cmp	r5, #0
	bge	.L77964
	ldr	r1, =0x137
	mov	r5, #3
	add	r0, r7, r1
	mov	r1, #0x2c
.L7798c:
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r2, r3, #2
	add	r2, r3
	ldr	r3, [r1, r6]
	lsl	r2, #2
	add	r3, r2
	sub	r5, #1
	str	r3, [r1, r6]
	add	r1, #8
	cmp	r5, #0
	bge	.L7798c
	ldr	r2, =0x129
	add	r3, r7, r2
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L77a32
	mov	r1, #0x94
	lsl	r1, #1
	add	r3, r7, r1
	ldrb	r3, [r3]
	mov	r0, #0
	cmp	r3, #5
	bhi	.L77a28
	ldr	r2, =.L779c4
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L779c4:
	.word	.L779dc
	.word	.L779e0
	.word	.L779ea
	.word	.L779f2
	.word	.L77a28
	.word	.L77a20
.L779dc:
	mov	r0, #0x88
	b	.L779e2
.L779e0:
	mov	r0, #0x89
.L779e2:
	lsl	r0, #1
	bl	Func_79338
	b	.L77a28
.L779ea:
	ldr	r0, =0x113
	bl	Func_79338
	b	.L77a28
.L779f2:
	ldr	r0, =0x111
	bl	Func_79338
	b	.L77a28

	.pool_aligned

.L77a20:
	mov	r0, #0x89
	lsl	r0, #1
	bl	Func_79338
.L77a28:
	cmp	r0, #0
	beq	.L77a32
	ldr	r3, [r6, #0x24]
	add	r3, #4
	str	r3, [r6, #0x24]
.L77a32:
	ldr	r3, [r6, #8]
	cmp	r3, #0
	bge	.L77a3c
	mov	r3, #0
	str	r3, [r6, #8]
.L77a3c:
	ldr	r2, =0x3e7
	cmp	r3, r2
	ble	.L77a44
	str	r2, [r6, #8]
.L77a44:
	ldr	r3, [r6, #0xc]
	cmp	r3, #0
	bge	.L77a4e
	mov	r3, #0
	str	r3, [r6, #0xc]
.L77a4e:
	cmp	r3, r2
	ble	.L77a54
	str	r2, [r6, #0xc]
.L77a54:
	ldr	r3, [r6, #0x10]
	cmp	r3, #0
	bge	.L77a5e
	mov	r3, #0
	str	r3, [r6, #0x10]
.L77a5e:
	cmp	r3, r2
	ble	.L77a64
	str	r2, [r6, #0x10]
.L77a64:
	ldr	r3, [r6, #0x18]
	cmp	r3, #0
	bge	.L77a6e
	mov	r3, #0
	str	r3, [r6, #0x18]
.L77a6e:
	cmp	r3, #0x63
	ble	.L77a76
	mov	r3, #0x63
	str	r3, [r6, #0x18]
.L77a76:
	ldr	r3, [r6, #0x1c]
	cmp	r3, #0
	bge	.L77a80
	mov	r3, #0
	str	r3, [r6, #0x1c]
.L77a80:
	cmp	r3, #2
	ble	.L77a88
	mov	r3, #2
	str	r3, [r6, #0x1c]
.L77a88:
	ldr	r3, [r6, #0x20]
	cmp	r3, #0
	bge	.L77a92
	mov	r3, #0
	str	r3, [r6, #0x20]
.L77a92:
	ldr	r2, =0x2710
	cmp	r3, r2
	ble	.L77a9a
	str	r2, [r6, #0x20]
.L77a9a:
	ldr	r3, [r6, #0x24]
	cmp	r3, #0
	bge	.L77aa4
	mov	r3, #0
	str	r3, [r6, #0x24]
.L77aa4:
	cmp	r3, #0xc8
	ble	.L77aac
	mov	r3, #0xc8
	str	r3, [r6, #0x24]
.L77aac:
	mov	r2, #0xc8
	mov	r12, r2
	mov	r1, r6
	mov	r2, r6
	mov	r5, #0
	mov	r0, #0
	mov	r4, #0x2c
	add	r2, #0x28
	add	r1, #0x2c
.L77abe:
	ldr	r3, [r2]
	cmp	r3, #0
	bge	.L77ac8
	str	r0, [r2]
	mov	r3, r0
.L77ac8:
	cmp	r3, #0xc8
	ble	.L77ad0
	mov	r3, r12
	str	r3, [r2]
.L77ad0:
	ldr	r3, [r1]
	cmp	r3, #0
	bge	.L77ada
	str	r0, [r1]
	mov	r3, r0
.L77ada:
	cmp	r3, #0xc8
	ble	.L77ae2
	mov	r3, r12
	str	r3, [r6, r4]
.L77ae2:
	add	r5, #1
	add	r1, #8
	add	r4, #8
	add	r2, #8
	cmp	r5, #3
	ble	.L77abe
	ldr	r3, [r6, #8]
	strh	r3, [r7, #0x3c]
	ldr	r3, [r6, #0xc]
	strh	r3, [r7, #0x3e]
	mov	r3, r7
	ldr	r2, [r6, #0x10]
	add	r3, #0x40
	strh	r2, [r3]
	mov	r2, r7
	ldr	r3, [r6, #0x18]
	add	r2, #0x42
	strb	r3, [r2]
	ldr	r3, [r6, #0x1c]
	add	r2, #1
	strb	r3, [r2]
	mov	r3, r7
	ldr	r2, [r6, #0x20]
	add	r3, #0x44
	strb	r2, [r3]
	mov	r2, r7
	ldr	r3, [r6, #0x24]
	add	r2, #0x45
	strb	r3, [r2]
	mov	r1, r7
	mov	r2, r6
	add	r1, #0x48
	add	r2, #0x28
	mov	r5, #3
.L77b26:
	ldr	r3, [r2]
	strh	r3, [r1]
	ldr	r3, [r2, #4]
	sub	r5, #1
	strh	r3, [r1, #2]
	add	r2, #8
	add	r1, #4
	cmp	r5, #0
	bge	.L77b26
	ldr	r0, =0x129
	add	r3, r7, r0
	ldrb	r3, [r3]
	ldr	r1, =0x270f
	cmp	r3, #0
	beq	.L77b46
	ldr	r1, =0x7cf
.L77b46:
	mov	r3, #0x34
	ldrsh	r2, [r7, r3]
	ldr	r3, [r6]
	cmp	r3, #0
	bge	.L77b54
	mov	r3, #0
	str	r3, [r6]
.L77b54:
	cmp	r3, r1
	ble	.L77b5c
	str	r1, [r6]
	mov	r3, r1
.L77b5c:
	strh	r3, [r7, #0x34]
	lsl	r3, #16
	asr	r3, #16
	cmp	r2, r3
	beq	.L77b94
	mov	r0, #0x14
	ldrsh	r2, [r7, r0]
	ldr	r3, [r6]
	mul	r2, r3
	cmp	r2, #0
	bge	.L77b76
	ldr	r3, =0x3fff
	add	r2, r3
.L77b76:
	asr	r2, #14
	cmp	r2, #0
	bge	.L77b7e
	mov	r2, #0
.L77b7e:
	cmp	r2, r1
	ble	.L77b84
	mov	r2, r1
.L77b84:
	mov	r0, #0x38
	ldrsh	r3, [r7, r0]
	cmp	r3, #0
	beq	.L77b92
	cmp	r2, #0
	bne	.L77b92
	mov	r2, #1
.L77b92:
	strh	r2, [r7, #0x38]
.L77b94:
	mov	r3, #0x36
	ldrsh	r2, [r7, r3]
	ldr	r3, [r6, #4]
	cmp	r3, #0
	bge	.L77ba2
	mov	r3, #0
	str	r3, [r6, #4]
.L77ba2:
	cmp	r3, r1
	ble	.L77baa
	str	r1, [r6, #4]
	mov	r3, r1
.L77baa:
	strh	r3, [r7, #0x36]
	lsl	r3, #16
	asr	r3, #16
	cmp	r2, r3
	beq	.L77be2
	mov	r0, #0x16
	ldrsh	r2, [r7, r0]
	ldr	r3, [r6, #4]
	mul	r2, r3
	cmp	r2, #0
	bge	.L77bc4
	ldr	r3, =0x3fff
	add	r2, r3
.L77bc4:
	asr	r2, #14
	cmp	r2, #0
	bge	.L77bcc
	mov	r2, #0
.L77bcc:
	cmp	r2, r1
	ble	.L77bd2
	mov	r2, r1
.L77bd2:
	mov	r0, #0x3a
	ldrsh	r3, [r7, r0]
	cmp	r3, #0
	beq	.L77be0
	cmp	r2, #0
	bne	.L77be0
	mov	r2, #1
.L77be0:
	strh	r2, [r7, #0x3a]
.L77be2:
	mov	r0, r6
	bl	Func_2df0
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_77428

@ RecomputePartyFlags
@ r0.. = parameters. Walks the active party (size from Func_795fc), reads each
@ record and its equipment through Func_78414, and sets or clears the
@ corresponding event flags with Func_79358 / Func_79374.
.thumb_func_start Func_77c10
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r0, =0x167
	sub	sp, #8
	bl	Func_79374
	bl	Func_795fc
	mov	r10, r0
	mov	r0, #0
	mov	r8, r0
	cmp	r8, r10
	bge	.L77c9e
	ldr	r3, =ewram_240
	mov	r2, #0xfc
	lsl	r2, #1
	add	r2, r3
	mov	r3, #0x80
	lsl	r3, #2
	mov	r9, r2
	mov	r11, r3
.L77c44:
	mov	r2, r9
	ldrb	r0, [r2]
	bl	Func_77394
	mov	r2, #0xd8
	mov	r7, r0
	mov	r1, #0xe
.L77c52:
	ldrh	r3, [r2, r7]
	mov	r0, r11
	and	r3, r0
	cmp	r3, #0
	beq	.L77c8c
	ldrh	r0, [r2, r7]
	str	r1, [sp, #4]
	str	r2, [sp]
	bl	Func_78414
	mov	r5, r0
	ldr	r2, [sp]
	ldr	r1, [sp, #4]
	add	r5, #0x18
	mov	r6, #3
.L77c70:
	ldrb	r3, [r5]
	add	r5, #4
	cmp	r3, #0x1b
	bne	.L77c86
	ldr	r0, =0x167
	str	r1, [sp, #4]
	str	r2, [sp]
	bl	Func_79358
	ldr	r2, [sp]
	ldr	r1, [sp, #4]
.L77c86:
	sub	r6, #1
	cmp	r6, #0
	bge	.L77c70
.L77c8c:
	sub	r1, #1
	add	r2, #2
	cmp	r1, #0
	bge	.L77c52
	mov	r2, #1
	add	r8, r2
	add	r9, r2
	cmp	r8, r10
	blt	.L77c44
.L77c9e:
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_77c10

@ ReadPackedTable
@ Takes no arguments. Fetches asset 2 with Func_2f40 and walks it as a run of
@ packed byte triples, each decoded as `(b * 5) * 2 - 0x1E0` plus two more
@ bytes. Returns the decoded pointers.
.thumb_func_start Func_77cb8
	push	{r5, lr}
	ldr	r0, =2
	bl	Func_2f40
	ldrb	r2, [r0]
	lsl	r3, r2, #2
	ldr	r1, =0xfffffe20
	add	r3, r2
	lsl	r3, #1
	add	r0, #1
	add	r5, r3, r1
	ldrb	r3, [r0]
	add	r0, #1
	ldrb	r2, [r0]
	add	r3, r5, r3
	mov	r5, r3
	lsl	r3, r2, #2
	add	r3, r2
	lsl	r3, #1
	add	r0, #1
	add	r4, r3, r1
	ldrb	r3, [r0]
	add	r0, #1
	ldrb	r2, [r0]
	add	r3, r4, r3
	mov	r4, r3
	lsl	r3, r2, #2
	add	r3, r2
	lsl	r3, #1
	add	r2, r3, r1
	ldrb	r3, [r0, #1]
	sub	r5, #0x30
	add	r3, r2, r3
	mov	r2, r3
	sub	r4, #0x30
	lsl	r3, r5, #4
	add	r3, r4
	sub	r2, #0x30
	lsl	r3, #6
	add	r3, r2
	mov	r2, #0x80
	lsl	r3, #16
	lsl	r2, #21
	orr	r2, r3
	ldr	r3, =iwram_1f54
	ldrb	r3, [r3]
	asr	r0, r2, #16
	cmp	r3, #0
	beq	.L77d1e
	ldr	r3, =0xffff8000
	orr	r0, r3
.L77d1e:
	lsl	r0, #16
	lsr	r0, #16
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_77cb8

@ BuildPartyTables
@ Takes no arguments. Assembles the party tables at startup from the packed data
@ Func_77cb8 decodes, seeding each character through Func_78ee8 and Func_7961c.
@ 232 lines; traced structurally.
.thumb_func_start Func_77d38
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	sub	sp, #8
	add	r5, sp, #4
	mov	r4, #0
	str	r4, [r5]
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	ldr	r1, =ewram_240
	ldr	r2, =0x850000b0
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r4, [r5]
	mov	r0, r5
	ldr	r1, =ewram_1000
	ldr	r2, =0x850003e1
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, #0x80
	ldr	r2, =REG_DMA3SAD
	lsl	r3, #24
.L77d66:
	ldr	r4, [r2, #8]
	and	r4, r3
	cmp	r4, #0
	bne	.L77d66
	str	r4, [r5]
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	ldr	r1, =ewram_40
	ldr	r2, =0x85000080
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =ewram_1000
	mov	r1, #0x82
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0xff
	strb	r2, [r3]
	mov	r0, r5
	str	r4, [r5]
	ldr	r3, =REG_DMA3SAD
	ldr	r1, =ewram_500
	ldr	r2, =0x85000298
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r4, [sp]
	bl	Func_78ee8
	ldr	r7, =ewram_240
	mov	r3, #0x84
	lsl	r3, #2
	add	r2, r7, r3
	mov	r3, #1
	strh	r3, [r2]
	ldr	r1, .L77de4	@ 0
	ldr	r2, =0x212
	mov	r10, r1
	add	r3, r7, r2
	mov	r1, #2
	strh	r1, [r3]
	mov	r3, #0x85
	lsl	r3, #2
	add	r2, r7, r3
	ldr	r3, .L77de8	@ 4
	mov	r8, r3
	mov	r3, #4
	strh	r3, [r2]
	ldr	r3, =0x216
	add	r2, r7, r3
	mov	r3, #8
	strh	r3, [r2]
	mov	r3, #0x86
	lsl	r3, #2
	add	r2, r7, r3
	sub	r3, #0x18
	strh	r3, [r2]
	add	r3, #0x1a
	add	r2, r7, r3
	mov	r3, #0x80
	lsl	r3, #1
	strh	r3, [r2]
	mov	r2, #0x87
	lsl	r2, #2
	b	.L77e18

	.align	2, 0
.L77de4:
	.word	0
.L77de8:
	.word	4
	.pool

.L77e18:
	add	r3, r7, r2
	strh	r1, [r3]
	mov	r1, #0x88
	ldr	r4, [sp]
	lsl	r1, #2
	add	r3, r7, r1
	add	r2, #6
	strh	r4, [r3]
	sub	r1, #0x2c
	add	r3, r7, r2
	strh	r4, [r3]
	add	r3, r7, r1
	str	r4, [r3]
	mov	r0, #0
	bl	Func_7961c
	mov	r2, #0x83
	ldr	r4, [sp]
	ldr	r5, .L77e78	@ 1
	lsl	r2, #2
	ldr	r1, =0x20a
	add	r3, r7, r2
	str	r4, [r7, #0x10]
	sub	r2, #1
	strb	r5, [r3]
	add	r3, r7, r1
	strb	r5, [r3]
	sub	r1, #5
	add	r3, r7, r2
	strb	r5, [r3]
	ldr	r6, .L77e7c	@ 8
	add	r3, r7, r1
	mov	r2, r10
	add	r1, #1
	strb	r2, [r3]
	add	r3, r7, r1
	strb	r6, [r3]
	str	r4, [r7]
	bl	Func_77cb8
	mov	r2, #0xae
	lsl	r2, #2
	add	r3, r7, r2
	str	r0, [r3]
	ldr	r4, [sp]
	ldr	r3, =iwram_1c9c
	b	.L77e88

	.align	2, 0
.L77e78:
	.word	1
.L77e7c:
	.word	8
	.pool

.L77e88:
	str	r4, [r3]
	ldr	r3, =iwram_1d08
	mov	r1, r10
	strb	r1, [r3]
	ldr	r1, =0x22a
	ldrb	r2, [r3]
	add	r3, r7, r1
	str	r4, [r7, #4]
	strb	r2, [r3]
	ldr	r3, =iwram_1d24
	ldr	r2, =ewram_2004
	strh	r4, [r3]
	ldr	r3, .L77ee0	@ 0xffffffff
	strh	r3, [r2]
	ldr	r2, =0x11d
	mov	r1, r8
	add	r3, r7, r2
	add	r2, #1
	strb	r1, [r3]
	add	r3, r7, r2
	strb	r1, [r3]
	ldr	r1, =0x11f
	mov	r2, r8
	add	r3, r7, r1
	strb	r2, [r3]
	add	r1, #1
	ldr	r2, =0x121
	add	r3, r7, r1
	strb	r6, [r3]
	add	r1, #2
	add	r3, r7, r2
	strb	r6, [r3]
	add	r2, #2
	add	r3, r7, r1
	strb	r6, [r3]
	add	r1, #2
	add	r3, r7, r2
	mov	r2, #0x10
	strb	r2, [r3]
	add	r3, r7, r1
	add	r1, #1
	strb	r2, [r3]
	b	.L77f00

	.align	2, 0
.L77ee0:
	.word	0xffffffff
	.pool

.L77f00:
	add	r3, r7, r1
	strb	r2, [r3]
	mov	r2, #0x93
	lsl	r2, #1
	add	r3, r7, r2
	add	r1, #2
	mov	r2, #0x20
	strb	r2, [r3]
	add	r3, r7, r1
	add	r1, #1
	strb	r2, [r3]
	add	r3, r7, r1
	strb	r2, [r3]
	ldr	r3, =0x129
	add	r1, #2
	add	r2, r7, r3
	mov	r3, #0x40
	strb	r3, [r2]
	add	r2, r7, r1
	add	r1, #1
	strb	r3, [r2]
	add	r2, r7, r1
	strb	r3, [r2]
	add	sp, #8
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_77d38

@ JoinParty
@ r0 = character id. Rebuilds the character's summary with Func_77428, sets its
@ membership event flag with Func_79358, and notifies Func_79ae8.
@ Since flags 0..7 are membership, this is what makes Func_795fc's count go up.
.thumb_func_start Func_77f40
	push	{lr}
	mov	r0, #0x20
	bl	Func_79358
	mov	r0, #0
	bl	Func_79ae8
	mov	r0, #1
	bl	Func_79ae8
	mov	r0, #5
	bl	Func_79ae8
	mov	r0, #0
	bl	Func_77428
	mov	r0, #1
	bl	Func_77428
	mov	r0, #5
	bl	Func_77428
	pop	{r0}
	bx	r0
.func_end Func_77f40

@ SetPartyMembership
@ r0.. = parameters. The general form of Func_77f40: adds or removes characters,
@ setting flags with Func_79358 and clearing with Func_79374, rebuilding
@ summaries with Func_77428, and updating equipment through Func_78708 and
@ Func_78e28.
.thumb_func_start Func_77f70
	push	{r5, r6, lr}
	mov	r0, #0x20
	bl	Func_79374
	mov	r0, #0x21
	bl	Func_79374
	ldr	r0, =0x901
	bl	Func_79358
	mov	r0, #5
	bl	Func_79ae8
	mov	r0, #5
	bl	Func_77428
	ldr	r0, =0x11b
	bl	Func_79374
	mov	r0, #0x8d
	lsl	r0, #1
	bl	Func_79358
	mov	r6, #0
.L77fa0:
	mov	r0, r6
	bl	Func_77394
	mov	r5, r0
	ldrh	r1, [r5, #0x34]
	ldrh	r3, [r5, #0x36]
	strh	r1, [r5, #0x38]
	strh	r3, [r5, #0x3a]
	lsl	r1, #16
	asr	r1, #16
	lsl	r0, r1, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L77fca
	mov	r3, #0
	cmp	r0, #0
	blt	.L77fca
	mov	r3, r0
.L77fca:
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L77fde
	mov	r1, #0x38
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	beq	.L77fde
	mov	r3, #1
	strh	r3, [r5, #0x14]
.L77fde:
	mov	r2, #0x3a
	ldrsh	r0, [r5, r2]
	mov	r3, #0x36
	ldrsh	r1, [r5, r3]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L77ffc
	mov	r3, #0
	cmp	r0, #0
	blt	.L77ffc
	mov	r3, r0
.L77ffc:
	strh	r3, [r5, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L78010
	mov	r1, #0x3a
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	beq	.L78010
	mov	r3, #1
	strh	r3, [r5, #0x16]
.L78010:
	ldr	r0, .L78018	@ 0x1ff
	mov	r1, #0
	mov	r2, #0xd8
	b	.L78028

	.align	2, 0
.L78018:
	.word	0x1ff
	.pool

.L78024:
	add	r2, #2
	add	r1, #1
.L78028:
	cmp	r1, #0xe
	bgt	.L78044
	ldrh	r3, [r2, r5]
	and	r3, r0
	cmp	r3, #0xf
	bne	.L78024
	ldr	r3, =0x10
	mov	r0, r6
	strh	r3, [r2, r5]
	bl	Func_78708
	b	.L78044

	.pool_aligned

.L78044:
	mov	r0, r6
	bl	Func_79ae8
	mov	r0, r6
	add	r6, #1
	bl	Func_77428
	cmp	r6, #1
	ble	.L77fa0
	mov	r1, #0x8c
	mov	r0, #0
	bl	Func_78e28
	mov	r1, #0x95
	mov	r0, #0
	bl	Func_78e28
	mov	r1, #0x8c
	mov	r0, #1
	bl	Func_78e28
	mov	r1, #0x8d
	mov	r0, #2
	bl	Func_78e28
	ldr	r2, =ewram_240
	mov	r1, #0x96
	ldr	r3, [r2, #0x10]
	lsl	r1, #1
	add	r3, r1
	str	r3, [r2, #0x10]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_77f70

@ AveragePartyStat
@ r0 = field selector. Sums one field across the active party and divides by the
@ member count with Func_af0. Same shape as Func_77348 over a different field.
.thumb_func_start Func_7808c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r8, r0
	bl	Func_795fc
	mov	r6, #0
	mov	r7, r0
	cmp	r6, r7
	bge	.L78132
.L780a0:
	mov	r1, #0xfc
	ldr	r2, =ewram_240
	lsl	r1, #1
	add	r3, r6, r1
	ldrb	r0, [r2, r3]
	bl	Func_77394
	mov	r5, r0
	ldrh	r1, [r5, #0x34]
	ldrh	r3, [r5, #0x36]
	strh	r1, [r5, #0x38]
	strh	r3, [r5, #0x3a]
	lsl	r1, #16
	asr	r1, #16
	lsl	r0, r1, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L780d2
	mov	r3, #0
	cmp	r0, #0
	blt	.L780d2
	mov	r3, r0
.L780d2:
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L780e6
	mov	r2, #0x38
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L780e6
	mov	r3, #1
	strh	r3, [r5, #0x14]
.L780e6:
	mov	r3, #0x3a
	ldrsh	r0, [r5, r3]
	mov	r2, #0x36
	ldrsh	r1, [r5, r2]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L78104
	mov	r3, #0
	cmp	r0, #0
	blt	.L78104
	mov	r3, r0
.L78104:
	strh	r3, [r5, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L78118
	mov	r1, #0x3a
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	beq	.L78118
	mov	r3, #1
	strh	r3, [r5, #0x16]
.L78118:
	mov	r2, r8
	cmp	r2, #1
	bne	.L7812c
	ldr	r1, =0x131
	mov	r2, #0
	add	r3, r5, r1
	add	r1, #0xf
	strb	r2, [r3]
	add	r3, r5, r1
	strb	r2, [r3]
.L7812c:
	add	r6, #1
	cmp	r6, r7
	blt	.L780a0
.L78132:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_7808c

@ AveragePartyStatFiltered
@ r0 = field selector. As Func_7808c but skipping members whose event flag
@ (Func_79338) is clear, so downed or absent characters do not drag the average
@ down.
.thumb_func_start Func_78144
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	bl	Func_795fc
	mov	r7, #0
	mov	r8, r0
	cmp	r7, r8
	bge	.L7820c
.L78156:
	mov	r1, #0xfc
	ldr	r2, =ewram_240
	lsl	r1, #1
	add	r3, r7, r1
	ldrb	r6, [r2, r3]
	ldr	r3, =.L7a828
	ldrb	r3, [r3, r6]
	mov	r5, #0
	cmp	r3, #0
	bne	.L7817c
	mov	r0, #0x88
	lsl	r0, #1
	bl	Func_79338
	cmp	r0, #0
	bne	.L78190
	mov	r0, #0x89
	lsl	r0, #1
	b	.L78188
.L7817c:
	ldr	r0, =0x111
	bl	Func_79338
	cmp	r0, #0
	bne	.L78190
	ldr	r0, =0x113
.L78188:
	bl	Func_79338
	cmp	r0, #0
	beq	.L78192
.L78190:
	mov	r5, #1
.L78192:
	cmp	r5, #0
	beq	.L78206
	mov	r0, r6
	bl	Func_77394
	mov	r5, r0
	ldrh	r3, [r5, #0x36]
	strh	r3, [r5, #0x3a]
	mov	r2, #0x38
	ldrsh	r0, [r5, r2]
	mov	r3, #0x34
	ldrsh	r1, [r5, r3]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L781c0
	mov	r3, #0
	cmp	r0, #0
	blt	.L781c0
	mov	r3, r0
.L781c0:
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L781d4
	mov	r1, #0x38
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	beq	.L781d4
	mov	r3, #1
	strh	r3, [r5, #0x14]
.L781d4:
	mov	r2, #0x3a
	ldrsh	r0, [r5, r2]
	mov	r3, #0x36
	ldrsh	r1, [r5, r3]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L781f2
	mov	r3, #0
	cmp	r0, #0
	blt	.L781f2
	mov	r3, r0
.L781f2:
	strh	r3, [r5, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L78206
	mov	r1, #0x3a
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	beq	.L78206
	mov	r3, #1
	strh	r3, [r5, #0x16]
.L78206:
	add	r7, #1
	cmp	r7, r8
	blt	.L78156
.L7820c:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_78144

@ NoOp
@ A bare `bx lr`, present as a table entry or placeholder.
.thumb_func_start Func_78228
	bx	lr
.func_end Func_78228

@ RecomputeHpFraction
@ r0 = combatant id. Sets +0x14 to (curHP << 14) / maxHP -- Func_af0 supplies
@ the signed quotient -- clamped to 0..0x4000.
@ 0x4000 is full health, so +0x14 is a 14-bit fraction rather than a percentage;
@ UI bars scale from it directly. Called after every HP or PP change.
.thumb_func_start Func_7822c
	push	{r5, lr}
	bl	Func_77394
	mov	r5, r0
	mov	r2, #0x38
	ldrsh	r0, [r5, r2]
	mov	r3, #0x34
	ldrsh	r1, [r5, r3]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L78252
	mov	r3, #0
	cmp	r0, #0
	blt	.L78252
	mov	r3, r0
.L78252:
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L78266
	mov	r2, #0x38
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L78266
	mov	r3, #1
	strh	r3, [r5, #0x14]
.L78266:
	mov	r3, #0x3a
	ldrsh	r0, [r5, r3]
	mov	r2, #0x36
	ldrsh	r1, [r5, r2]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L78284
	mov	r3, #0
	cmp	r0, #0
	blt	.L78284
	mov	r3, r0
.L78284:
	strh	r3, [r5, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L78298
	mov	r2, #0x3a
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L78298
	mov	r3, #1
	strh	r3, [r5, #0x16]
.L78298:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_7822c

@ ScaleStat
@ r0, r1, r2 = value, numerator, denominator. Scales a stat with Func_af0 and
@ clamps the result.
.thumb_func_start Func_782a0
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #0x34
	ldrsh	r3, [r5, r2]
	mov	r0, r3
	cmp	r1, r3
	bgt	.L782b6
	mov	r0, #0
	cmp	r1, #0
	blt	.L782b6
	mov	r0, r1
.L782b6:
	strh	r0, [r5, #0x38]
	lsl	r0, #16
	mov	r3, #0x34
	ldrsh	r1, [r5, r3]
	asr	r0, #2
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L782d4
	mov	r3, #0
	cmp	r0, #0
	blt	.L782d4
	mov	r3, r0
.L782d4:
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L782e8
	mov	r2, #0x38
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L782e8
	mov	r3, #1
	strh	r3, [r5, #0x14]
.L782e8:
	mov	r3, #0x3a
	ldrsh	r0, [r5, r3]
	mov	r2, #0x36
	ldrsh	r1, [r5, r2]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L78306
	mov	r3, #0
	cmp	r0, #0
	blt	.L78306
	mov	r3, r0
.L78306:
	strh	r3, [r5, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L7831a
	mov	r2, #0x3a
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L7831a
	mov	r3, #1
	strh	r3, [r5, #0x16]
.L7831a:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_782a0

@ ScaleStatAlt
@ r0, r1, r2 = value, numerator, denominator. As Func_782a0 with different
@ clamping bounds.
.thumb_func_start Func_78320
	push	{r5, lr}
	mov	r5, r0
	mov	r2, #0x36
	ldrsh	r3, [r5, r2]
	mov	r2, r3
	cmp	r1, r3
	bgt	.L78336
	mov	r2, #0
	cmp	r1, #0
	blt	.L78336
	mov	r2, r1
.L78336:
	strh	r2, [r5, #0x3a]
	mov	r3, #0x38
	ldrsh	r0, [r5, r3]
	lsl	r0, #14
	mov	r2, #0x34
	ldrsh	r1, [r5, r2]
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L78356
	mov	r3, #0
	cmp	r0, #0
	blt	.L78356
	mov	r3, r0
.L78356:
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L7836a
	mov	r2, #0x38
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L7836a
	mov	r3, #1
	strh	r3, [r5, #0x14]
.L7836a:
	mov	r3, #0x3a
	ldrsh	r0, [r5, r3]
	mov	r2, #0x36
	ldrsh	r1, [r5, r2]
	lsl	r0, #14
	bl	Func_af0_from_thumb
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L78388
	mov	r3, #0
	cmp	r0, #0
	blt	.L78388
	mov	r3, r0
.L78388:
	strh	r3, [r5, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L7839c
	mov	r2, #0x3a
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L7839c
	mov	r3, #1
	strh	r3, [r5, #0x16]
.L7839c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_78320

@ ChangeHp
@ r0 = combatant id, r1 = SIGNED delta. Adds the delta to current HP at +0x38,
@ clamps to 0..maxHP (+0x34), recomputes the fraction with Func_7822c, and
@ returns the new HP.
@ This is the damage and healing entry point -- rom_b5000 calls it as
@ _Func_783a4(id, -damage). Reaching 0 is how a combatant goes down.
.thumb_func_start Func_783a4
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r0
	bl	Func_77394
	mov	r6, r0
	mov	r1, #0x38
	ldrsh	r3, [r6, r1]
	mov	r1, #0x34
	ldrsh	r2, [r6, r1]
	add	r3, r5
	mov	r1, r2
	cmp	r3, r2
	bgt	.L783c8
	mov	r1, #0
	cmp	r3, #0
	blt	.L783c8
	mov	r1, r3
.L783c8:
	mov	r0, r7
	strh	r1, [r6, #0x38]
	bl	Func_7822c
	mov	r2, #0x38
	ldrsh	r0, [r6, r2]
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_783a4

@ ChangePp
@ r0 = combatant id, r1 = SIGNED delta. The Func_783a4 counterpart for PP:
@ current at +0x3A, maximum at +0x36, same clamp and the same Func_7822c
@ refresh, returning the new PP.
.thumb_func_start Func_783dc
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r0
	bl	Func_77394
	mov	r6, r0
	mov	r1, #0x3a
	ldrsh	r3, [r6, r1]
	mov	r1, #0x36
	ldrsh	r2, [r6, r1]
	add	r3, r5
	mov	r1, r2
	cmp	r3, r2
	bgt	.L78400
	mov	r1, #0
	cmp	r3, #0
	blt	.L78400
	mov	r1, r3
.L78400:
	mov	r0, r7
	strh	r1, [r6, #0x3a]
	bl	Func_7822c
	mov	r2, #0x3a
	ldrsh	r0, [r6, r2]
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_783dc

	.section .rodata

.L7a828:
	.incrom 0x7a828, 0x7a830

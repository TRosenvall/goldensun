	.include "macros.inc"

@ RebuildPartyRoster
@ r0.. = parameters. Rebuilds the active roster and every member's derived
@ stats, releasing the UI's cached menu buffers through _Func_196c4 so the
@ screens pick up the change. 209 lines; traced structurally.
.thumb_func_start Func_79460
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r11, r1
	mov	r5, r11
	sub	sp, #0x24
	mov	r9, r0
	mov	r8, r2
	sub	r5, #8
	cmp	r0, #0x7f
	bgt	.L79482
	mov	r0, #0
	b	.L795da
.L79482:
	mov	r2, r9
	mov	r0, #0
	cmp	r2, #0x86
	ble	.L7948c
	b	.L795da
.L7948c:
	cmp	r5, #0xf2
	bls	.L79492
	b	.L795da
.L79492:
	mov	r0, r9
	bl	Func_77394
	mov	r1, #0xa6
	ldr	r3, =Func_8d4
	lsl	r1, #1
	mov	r6, r0
	bl	_call_via_r3
	cmp	r5, #0xa4
	bls	.L794aa
	mov	r5, #0
.L794aa:
	mov	r3, #0x54
	mov	r2, r5
	mul	r2, r3
	ldr	r3, =Data_80ec8
	add	r7, r2, r3
	ldrb	r3, [r7, #0xf]
	strb	r3, [r6, #0xf]
	ldrh	r3, [r7, #0x10]
	strh	r3, [r6, #0x10]
	strh	r3, [r6, #0x38]
	strh	r3, [r6, #0x34]
	ldrh	r3, [r7, #0x12]
	strh	r3, [r6, #0x12]
	strh	r3, [r6, #0x3a]
	strh	r3, [r6, #0x36]
	mov	r3, #0x80
	lsl	r3, #7
	strh	r3, [r6, #0x14]
	strh	r3, [r6, #0x16]
	ldrh	r3, [r7, #0x14]
	strh	r3, [r6, #0x18]
	ldrh	r3, [r7, #0x16]
	strh	r3, [r6, #0x1a]
	ldrh	r3, [r7, #0x18]
	strh	r3, [r6, #0x1c]
	ldrb	r3, [r7, #0x1a]
	strb	r3, [r6, #0x1e]
	ldrb	r3, [r7, #0x1b]
	ldrb	r2, [r7, #0x1c]
	strb	r3, [r6, #0x1f]
	mov	r3, r6
	add	r3, #0x20
	strb	r2, [r3]
	ldr	r0, =0x28f
	ldrb	r3, [r7, #0x1d]
	mov	r2, r6
	add	r4, sp, #4
	add	r2, #0x21
	strb	r3, [r2]
	add	r0, r5, r0
	mov	r1, r4
	mov	r2, #0xf
	str	r4, [sp]
	bl	_Func_196c4
	ldr	r4, [sp]
	mov	r5, #0
	ldrh	r3, [r4, r5]
	cmp	r3, #0
	beq	.L79528
	mov	r0, r4
	mov	r1, r6
	mov	r2, #0
.L79514:
	ldrh	r3, [r2, r0]
	add	r5, #1
	strb	r3, [r1]
	add	r2, #2
	add	r1, #1
	cmp	r5, #0xd
	bgt	.L79528
	ldrh	r3, [r2, r4]
	cmp	r3, #0
	bne	.L79514
.L79528:
	mov	r3, r8
	cmp	r3, #8
	bgt	.L79534
	add	r3, #0x31
	strb	r3, [r6, r5]
	add	r5, #1
.L79534:
	mov	r3, #0
	strb	r3, [r6, r5]
	mov	r2, #0x28
	mov	r3, #0
	strb	r3, [r6, #0xe]
	mov	r8, r3
	mov	r10, r2
	mov	r3, #0x28
	mov	r2, #0x30
	add	r3, r7
	add	r2, r7
	mov	r14, r3
	mov	r5, #3
	mov	r12, r2
	mov	r0, r6
.L79552:
	mov	r2, r14
	ldrh	r3, [r2]
	mov	r2, #2
	add	r14, r2
	cmp	r3, #0
	beq	.L79586
	mov	r2, r12
	ldrb	r3, [r2]
	cmp	r3, #0
	beq	.L79586
	mov	r2, r0
	mov	r4, r10
	mov	r1, r3
	add	r2, #0xd8
.L7956e:
	mov	r3, r8
	cmp	r3, #0xe
	bgt	.L79580
	ldrh	r3, [r7, r4]
	strh	r3, [r2]
	mov	r3, #1
	add	r2, #2
	add	r0, #2
	add	r8, r3
.L79580:
	sub	r1, #1
	cmp	r1, #0
	bne	.L7956e
.L79586:
	mov	r2, #2
	mov	r3, #1
	sub	r5, #1
	add	r10, r2
	add	r12, r3
	cmp	r5, #0
	bge	.L79552
	mov	r3, #0x90
	lsl	r3, #1
	add	r2, r6, r3
	ldr	r3, [r7, #0x20]
	str	r3, [r2]
	ldr	r3, =0x129
	add	r2, r6, r3
	mov	r3, #0
	strb	r3, [r2]
	mov	r2, #0x94
	lsl	r2, #1
	add	r5, r6, r2
	mov	r3, r11
	mov	r1, r6
	strb	r3, [r5]
	add	r1, #0x24
	mov	r0, r9
	bl	Func_798e0
	mov	r0, r9
	bl	Func_77428
	mov	r3, #0x95
	lsl	r3, #1
	add	r2, r6, r3
	mov	r3, #1
	strb	r3, [r2]
	ldrb	r3, [r5]
	cmp	r3, #0xab
	bgt	.L795d8
	cmp	r3, #0x9e
	blt	.L795d8
	mov	r3, #2
	strb	r3, [r2]
.L795d8:
	mov	r0, #1
.L795da:
	add	sp, #0x24
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_79460

@ GetPartySize
@ Takes no arguments. Counts save bits 0..7 with Func_79338 and returns the
@ total -- the number of characters who have joined. Every party walk in this
@ module is bounded by this.
.thumb_func_start Func_795fc
	push	{r5, r6, lr}
	mov	r6, #0
	mov	r5, #0
.L79602:
	mov	r0, r5
	bl	Func_79338
	cmp	r0, #0
	beq	.L7960e
	add	r6, #1
.L7960e:
	add	r5, #1
	cmp	r5, #7
	ble	.L79602
	mov	r0, r6
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_795fc

@ AddPartyMember
@ r0 = character id. Sets the membership bit with Func_79358 and appends the id
@ to the roster byte list at ewram_240+0x1F8, skipping the append when the
@ character is already listed.
.thumb_func_start Func_7961c
	push	{r5, r6, lr}
	mov	r6, r0
	bl	Func_795fc
	mov	r5, r0
	mov	r0, r6
	bl	Func_79358
	mov	r2, #0
	cmp	r2, r5
	bge	.L7964e
	ldr	r0, =ewram_240
	mov	r3, #0xfc
	lsl	r3, #1
	add	r1, r0, r3
.L7963a:
	ldrb	r3, [r1]
	add	r1, #1
	cmp	r3, r6
	beq	.L7964a
	add	r2, #1
	cmp	r2, r5
	blt	.L7963a
	b	.L79650
.L7964a:
	mov	r0, r5
	b	.L7965a
.L7964e:
	ldr	r0, =ewram_240
.L79650:
	mov	r1, #0xfc
	lsl	r1, #1
	add	r3, r2, r1
	strb	r6, [r0, r3]
	add	r0, r5, #1
.L7965a:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_7961c

@ RemovePartyMember
@ r0 = character id. Clears the membership bit with Func_79374 and removes the
@ id from the roster list, shifting the remainder down so the list stays
@ compact.
.thumb_func_start Func_79664
	push	{r5, r6, lr}
	mov	r5, r0
	bl	Func_795fc
	mov	r6, r0
	mov	r0, r5
	bl	Func_79374
	mov	r1, #0
	cmp	r1, r6
	bge	.L79696
	ldr	r0, =ewram_240
	mov	r2, #0xfc
	lsl	r2, #1
	ldrb	r3, [r0, r2]
	cmp	r3, r5
	beq	.L79696
	add	r2, r0, r2
.L79688:
	add	r1, #1
	cmp	r1, r6
	bge	.L79696
	add	r2, #1
	ldrb	r3, [r2]
	cmp	r3, r5
	bne	.L79688
.L79696:
	sub	r0, r6, #1
	cmp	r1, r0
	bge	.L796b4
	ldr	r3, =ewram_240
	mov	r4, #0xfc
	add	r3, r1, r3
	lsl	r4, #1
	add	r2, r3, r4
	sub	r1, r0, r1
.L796a8:
	ldrb	r3, [r2, #1]
	sub	r1, #1
	strb	r3, [r2]
	add	r2, #1
	cmp	r1, #0
	bne	.L796a8
.L796b4:
	bl	Func_795fc
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_79664

@ CopyRosterIds
@ r0 = destination halfword array. Widens the roster bytes at ewram_240+0x1F8
@ into halfwords and returns the count from Func_795fc. The standard way the
@ rest of the ROM enumerates the party.
.thumb_func_start Func_796c4
	push	{r5, lr}
	mov	r5, r0
	mov	r0, #0
	cmp	r5, #0
	beq	.L796f2
	bl	Func_795fc
	mov	r1, #0
	cmp	r0, #0
	beq	.L796ee
	ldr	r3, =ewram_240
	mov	r4, #0xfc
	lsl	r4, #1
	add	r2, r3, r4
.L796e0:
	ldrb	r3, [r2]
	add	r1, #1
	strh	r3, [r5]
	add	r2, #1
	add	r5, #2
	cmp	r1, r0
	bne	.L796e0
.L796ee:
	ldr	r3, .L796f8	@ 0xff
	strh	r3, [r5]
.L796f2:
	pop	{r5}
	pop	{r1}
	bx	r1
	.align	2, 0
.L796f8:
	.word	0xff
.func_end Func_796c4

@ ChangeMoney
@ r0 = signed delta. Adds it to the money field at ewram_240+0x10 and returns
@ the new total, CLAMPED TO 0..0xF423F -- 999,999, the gold cap. Negative
@ results clamp to zero rather than underflowing.
.thumb_func_start Func_79700
	push	{lr}
	ldr	r1, =ewram_240
	ldr	r3, [r1, #0x10]
	ldr	r2, =0xf423f
	add	r3, r0
	cmp	r3, r2
	ble	.L79710
	mov	r3, r2
.L79710:
	cmp	r3, #0
	bge	.L79716
	mov	r3, #0
.L79716:
	str	r3, [r1, #0x10]
	mov	r0, r3
	pop	{r1}
	bx	r1
.func_end Func_79700

@ GetCounterA
@ r0 = index. Reads a counter out of the save block.
.thumb_func_start Func_79728
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0x8c
	lsl	r2, #1
	add	r3, r2
	ldr	r2, [r3]
	ldr	r1, =0xf423f
	add	r2, r0
	cmp	r2, r1
	ble	.L7973e
	mov	r2, r1
.L7973e:
	cmp	r2, #0
	bge	.L79744
	mov	r2, #0
.L79744:
	str	r2, [r3]
	mov	r0, r2
	pop	{r1}
	bx	r1
.func_end Func_79728

@ GetCounterB
@ r0 = index. Reads a second save-block counter, same shape as Func_79728.
.thumb_func_start Func_79754
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0x8e
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsb	r2, [r3, r2]
	add	r2, r0
	cmp	r2, #0x1c
	ble	.L7976a
	mov	r2, #0x1c
.L7976a:
	cmp	r2, #0
	bge	.L79770
	mov	r2, #0
.L79770:
	strb	r2, [r3]
	mov	r0, r2
	pop	{r1}
	bx	r1
.func_end Func_79754

@ ReadScratchRecordField
@ r0.. = parameters. Reads fields from the record Func_77330 selects.
.thumb_func_start Func_7977c
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r7, =L84a8c
	mov	r3, #0xf
	add	r3, r7
	mov	r10, r3
	mov	r3, #1
	sub	sp, #4
	mov	r5, r0
	mov	r1, #0
	mov	r8, r3
.L79796:
	mov	r0, #0
	ldrb	r6, [r7]
	str	r1, [sp]
	bl	Func_77330
	mov	r2, r8
	ldr	r3, [r0]
	lsl	r2, r6
	and	r3, r2
	add	r7, #1
	ldr	r1, [sp]
	cmp	r3, #0
	beq	.L797b6
	strb	r6, [r5]
	add	r1, #1
	add	r5, #1
.L797b6:
	cmp	r7, r10
	bls	.L79796
	mov	r3, #0x20
	mov	r0, r1
	strb	r3, [r5]
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_7977c

@ GetElementRecord
@ r0 = index (0..0xF). Returns .L84a9c + index * 8; an index above 15 returns 0
@ rather than reading past the table. Sixteen 8-byte entries.
.thumb_func_start Func_797d4
	push	{lr}
	cmp	r0, #0xf
	bls	.L797de
	mov	r0, #0
	b	.L797e4
.L797de:
	ldr	r3, =L84a9c
	lsl	r0, #3
	add	r0, r3
.L797e4:
	pop	{r1}
	bx	r1
.func_end Func_797d4

@ GetGrowthEntry
@ r0, r1 = row and column. Returns the word at .L88db8[(r0 * 4 + r1)], a flat
@ word table indexed as a 4-column grid.
.thumb_func_start Func_797ec
	lsl	r0, #2
	ldr	r3, =L88db8
	add	r0, r1
	lsl	r0, #2
	ldr	r0, [r3, r0]
	bx	lr
.func_end Func_797ec

@ ComputeDerivedStat
@ r0.. = parameters. Combines a character's 0xB4-byte base entry (Func_78ed8)
@ with an item record (Func_773d8) to produce one derived stat.
.thumb_func_start Func_797fc
	push	{r5, r6, r7, lr}
	mov	r7, r0
	mov	r6, r2
	cmp	r7, #7
	ble	.L79838
	bl	Func_773d8
	add	r0, #0x34
	ldrb	r1, [r0]
	cmp	r1, #0x2b
	bls	.L79814
	mov	r1, #0
.L79814:
	lsl	r3, r1, #1
	add	r3, r1
	ldr	r2, =L88e38
	lsl	r3, #3
	add	r3, r2
	mov	r5, #0
	mov	r0, r6
	add	r1, r3, #4
.L79824:
	ldrb	r2, [r1]
	lsl	r3, r2, #2
	add	r3, r2
	lsl	r3, #1
	add	r5, #1
	add	r1, #1
	stmia	r0!, {r3}
	cmp	r5, #3
	ble	.L79824
	b	.L79870
.L79838:
	mov	r0, r6
	add	r1, #0x24
	mov	r5, #3
.L7983e:
	ldrb	r2, [r1]
	lsl	r3, r2, #2
	add	r3, r2
	lsl	r3, #1
	sub	r5, #1
	add	r1, #1
	stmia	r0!, {r3}
	cmp	r5, #0
	bge	.L7983e
	cmp	r7, #7
	bgt	.L79870
	mov	r5, #0
.L79856:
	mov	r0, r7
	bl	Func_78ed8
	mov	r3, r5
	add	r3, #0x90
	add	r0, #2
	ldrb	r2, [r0, r3]
	ldr	r3, [r6]
	add	r5, #1
	add	r3, r2
	stmia	r6!, {r3}
	cmp	r5, #3
	ble	.L79856
.L79870:
	mov	r0, #0
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_797fc

@ GetScaledStat
@ r0 = combatant id, r1 = which. Func_797fc for the raw value, then Func_af0 to
@ scale it.
.thumb_func_start Func_7987c
	push	{r5, r6, lr}
	mov	r6, r1
	sub	sp, #0x10
	bl	Func_77394
	mov	r1, r0
	mov	r0, #0
	cmp	r6, #3
	bgt	.L798aa
	mov	r2, #0x94
	lsl	r2, #1
	add	r3, r1, r2
	mov	r5, sp
	ldrb	r0, [r3]
	add	r1, #0xf8
	mov	r2, r5
	bl	Func_797fc
	lsl	r3, r6, #2
	ldr	r0, [r5, r3]
	mov	r1, #0xa
	bl	Func_af0_from_thumb
.L798aa:
	add	sp, #0x10
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_7987c

@ GetItemStatField
@ r0 = item id, r1 = which. Reads one field from the 0x54-byte item record.
.thumb_func_start Func_798b4
	push	{lr}
	mov	r3, #0x94
	lsl	r3, #1
	add	r0, r3
	ldrb	r0, [r0]
	bl	Func_773d8
	add	r0, #0x34
	ldrb	r1, [r0]
	cmp	r1, #0x2b
	bls	.L798cc
	mov	r1, #0
.L798cc:
	lsl	r2, r1, #1
	ldr	r3, =L88e38
	add	r2, r1
	lsl	r2, #3
	ldr	r0, [r3, r2]
	pop	{r1}
	bx	r1
.func_end Func_798b4

@ RecomputeAllStats
@ r0 = combatant id. Recomputes every derived stat from the base entry and the
@ equipped items, with Func_af0 and Func_b1c supplying the division and
@ remainder. 103 lines; traced structurally.
.thumb_func_start Func_798e0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	sub	sp, #0x14
	mov	r9, r1
	bl	Func_77394
	mov	r2, r0
	ldr	r0, =0x129
	add	r3, r2, r0
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L79932
	mov	r1, #0x94
	lsl	r1, #1
	add	r3, r2, r1
	ldrb	r0, [r3]
	bl	Func_773d8
	add	r0, #0x34
	ldrb	r0, [r0]
	cmp	r0, #0x2b
	bls	.L79914
	mov	r0, #0
.L79914:
	lsl	r3, r0, #1
	ldr	r2, =L88e38
	add	r3, r0
	lsl	r3, #3
	add	r3, r2
	mov	r2, r3
	mov	r7, #0
	mov	r1, r9
	add	r2, #8
.L79926:
	ldmia	r2!, {r3}
	add	r7, #1
	stmia	r1!, {r3}
	cmp	r7, #3
	ble	.L79926
	b	.L79994
.L79932:
	mov	r0, #0x94
	lsl	r0, #1
	add	r3, r2, r0
	ldrb	r0, [r3]
	add	r3, sp, #4
	mov	r1, r2
	mov	r8, r3
	add	r1, #0xf8
	mov	r2, r8
	bl	Func_797fc
	ldr	r0, =L88df8
	mov	r4, #0
	mov	r10, r0
	mov	r7, #3
.L79950:
	mov	r1, r8
	ldr	r5, [r4, r1]
	mov	r1, #0xa
	mov	r0, r5
	str	r4, [sp]
	bl	Func_b1c_from_thumb
	mov	r1, #0xa
	mov	r6, r0
	mov	r0, r5
	bl	Func_af0_from_thumb
	ldr	r4, [sp]
	cmp	r0, #0xf
	ble	.L79970
	mov	r0, #0xf
.L79970:
	cmp	r0, #0
	bge	.L79976
	mov	r0, #0
.L79976:
	lsl	r2, r0, #2
	mov	r3, r9
	mov	r0, r10
	add	r1, r4, r3
	ldrh	r3, [r0, r2]
	add	r3, r6
	strh	r3, [r1]
	add	r2, r10
	ldrh	r3, [r2, #2]
	sub	r7, #1
	add	r3, r6
	strh	r3, [r1, #2]
	add	r4, #4
	cmp	r7, #0
	bge	.L79950
.L79994:
	add	sp, #0x14
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_798e0

@ ApplyElementalBonuses
@ r0 = combatant id. Walks the element table (Func_797fc, Func_797ec) applying
@ each element's contribution, gated by save bits through Func_79338.
@ 156 lines; traced structurally.
.thumb_func_start Func_799b0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r2, #1
	mov	r5, r0
	neg	r2, r2
	sub	sp, #0x14
	mov	r8, r2
	mov	r0, #0
	cmp	r5, #7
	bgt	.L79aba
	add	r6, sp, #4
	mov	r0, r5
	mov	r2, r6
	bl	Func_797fc
	mov	r0, #0x20
	bl	Func_79338
	cmp	r0, #0
	beq	.L799ee
	mov	r0, #0xc8
	cmp	r5, #0
	beq	.L79aba
	mov	r0, #0xc9
	cmp	r5, #1
	beq	.L79aba
.L799ee:
	mov	r0, #0xca
	cmp	r5, #5
	beq	.L79aba
	mov	r0, #1
	neg	r0, r0
	cmp	r8, r0
	bne	.L79aba
	mov	r12, r8
	mov	r5, r8
	mov	r0, #0
	mov	r2, r6
.L79a04:
	ldmia	r2!, {r3}
	cmp	r12, r3
	bge	.L79a0e
	mov	r12, r3
	mov	r5, r0
.L79a0e:
	add	r0, #1
	cmp	r0, #3
	ble	.L79a04
	mov	r4, #1
	neg	r4, r4
	mov	r12, r4
	mov	r0, #0
	mov	r2, r6
.L79a1e:
	cmp	r0, r5
	beq	.L79a2c
	ldr	r3, [r2]
	cmp	r12, r3
	bge	.L79a2c
	mov	r12, r3
	mov	r4, r0
.L79a2c:
	add	r0, #1
	add	r2, #4
	cmp	r0, #3
	ble	.L79a1e
	lsl	r3, r4, #2
	ldr	r3, [r6, r3]
	mov	r1, r5
	cmp	r3, #9
	ble	.L79a46
	mov	r1, r4
	b	.L79a46
.L79a42:
	mov	r8, r0
	b	.L79aac
.L79a46:
	mov	r0, r5
	bl	Func_797ec
	ldr	r3, =L84b1c
	ldr	r7, =0x424c
	mov	r10, r3
	mov	r14, r10
	str	r6, [sp]
	ldr	r5, =0x4248
	mov	r11, r0
	mov	r9, r6
	mov	r0, #0xca
	add	r7, r14
.L79a60:
	mov	r6, r10
	ldr	r3, [r5, r6]
	cmp	r3, r11
	bne	.L79aa2
	ldrb	r3, [r7]
	ldr	r1, [sp]
	lsl	r2, r3, #2
	add	r2, r3
	ldr	r3, [r1]
	lsl	r2, #1
	mov	r4, #0
	cmp	r3, r2
	blt	.L79a9e
	mov	r2, r14
	add	r3, r5, r2
	mov	r12, r9
	add	r1, r3, #4
.L79a82:
	add	r4, #1
	cmp	r4, #3
	bgt	.L79a9e
	add	r1, #1
	ldrb	r3, [r1]
	lsl	r2, r3, #2
	add	r2, r3
	mov	r3, #4
	add	r12, r3
	mov	r6, r12
	ldr	r3, [r6]
	lsl	r2, #1
	cmp	r3, r2
	bge	.L79a82
.L79a9e:
	cmp	r4, #4
	beq	.L79a42
.L79aa2:
	sub	r0, #1
	sub	r7, #0x54
	sub	r5, #0x54
	cmp	r0, #0
	bge	.L79a60
.L79aac:
	mov	r1, #1
	neg	r1, r1
	cmp	r8, r1
	bne	.L79ab8
	mov	r2, #0
	mov	r8, r2
.L79ab8:
	mov	r0, r8
.L79aba:
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_799b0

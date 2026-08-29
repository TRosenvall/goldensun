	.include "macros.inc"

@ ============================================================================
@ Overlay 0x7a6ae4 -- three areas (0x2F, 0x30, 0x31) sharing a PRESSURE-PLATE
@ PUZZLE and a set of small map edits.
@
@ Slot 0  OvlFunc_46c  map-load entry, dispatching to one setup per area
@ Slot 1  OvlFunc_40   edge transitions   -> one table per area
@ Slot 2  OvlFunc_98   map event list     -> .Lbcc (constant)
@ Slot 3  OvlFunc_a0   read after slot 4  -> one per area
@ Slot 4  OvlFunc_f4   map objects        -> one per area
@ Slot 5  OvlFunc_94   interactions       -> none (returns 0)
@
@ Every table slot uses the same three-way area test in the same order
@ (0x31, then 0x30, then 0x2F, else a fallback), so the four stay in step.
@
@ The puzzle is in OvlFunc_304: two blocks in slots 0x0B and 0x0C, one target
@ tile, and a gate that opens when EITHER block is on it.
@ ============================================================================

@ Trigger: resets the entity in r0 with Func_c528.
.thumb_func_start OvlFunc_30
	push	{lr}
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_30

@ Slot 1: edge-transition table, one per area.
.thumb_func_start OvlFunc_40
	push	{lr}
	ldr	r3, =ewram_240
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x31
	cmp	r2, r3
	bne	.L58
	ldr	r0, =.L9ec
	b	.L6e
.L58:
	ldr	r3, =0x30
	cmp	r2, r3
	bne	.L62
	ldr	r0, =.La64
	b	.L6e
.L62:
	ldr	r3, =0x2f
	cmp	r2, r3
	bne	.L6c
	ldr	r0, =.Lb24
	b	.L6e
.L6c:
	ldr	r0, =.L9bc
.L6e:
	pop	{r1}
	bx	r1
.func_end OvlFunc_40

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_94
	mov	r0, #0
	bx	lr
.func_end OvlFunc_94

@ Slot 2: map event list. Shared by all three areas.
.thumb_func_start OvlFunc_98
	ldr	r0, =.Lbcc
	bx	lr
.func_end OvlFunc_98

@ Slot 3: read after slot 4, one table per area.
.thumb_func_start OvlFunc_a0
	push	{lr}
	ldr	r3, =ewram_240
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x31
	cmp	r2, r3
	bne	.Lb8
	ldr	r0, =.Lc2c
	b	.Lce
.Lb8:
	ldr	r3, =0x30
	cmp	r2, r3
	bne	.Lc2
	ldr	r0, =.Lc5c
	b	.Lce
.Lc2:
	ldr	r3, =0x2f
	cmp	r2, r3
	bne	.Lcc
	ldr	r0, =.Lcbc
	b	.Lce
.Lcc:
	ldr	r0, =.Lc14
.Lce:
	pop	{r1}
	bx	r1
.func_end OvlFunc_a0

@ Slot 4: map object table, one per area.
.thumb_func_start OvlFunc_f4
	push	{lr}
	ldr	r3, =ewram_240
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x31
	cmp	r2, r3
	bne	.L10c
	ldr	r0, =.Lea8
	b	.L122
.L10c:
	ldr	r3, =0x30
	cmp	r2, r3
	bne	.L116
	ldr	r0, =.Lefc
	b	.L122
.L116:
	ldr	r3, =0x2f
	cmp	r2, r3
	bne	.L120
	ldr	r0, =.Lf80
	b	.L122
.L120:
	ldr	r0, =.Le9c
.L122:
	pop	{r1}
	bx	r1
.func_end OvlFunc_f4

@ Repaint: a 1x1 attribute cell from source (1, 0) to (0x15, 0x0E) -- one half
@ of a two-state tile, paired with OvlFunc_168.
.thumb_func_start OvlFunc_148
	push	{lr}
	sub	sp, #8
	mov	r3, #0x15
	mov	r2, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #1
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_148

@ Repaint: the same cell from source (0, 0) -- the other state.
.thumb_func_start OvlFunc_168
	push	{lr}
	sub	sp, #8
	mov	r3, #0x15
	mov	r2, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_168

@ Repaint: a 3x1 metatile copy from (0x6F, 0x25) to (0x61, 0x15), then a 3x2
@ attribute block from (0x2E, 0x26) to (0x18, 0x20).
.thumb_func_start OvlFunc_188
	push	{lr}
	sub	sp, #8
	mov	r3, #1
	mov	r2, #3
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x6f
	mov	r1, #0x25
	mov	r2, #0x61
	mov	r3, #0x15
	bl	__Func_10424
	mov	r3, #0x20
	mov	r2, #0x18
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x26
	mov	r2, #3
	mov	r3, #2
	bl	__Func_10704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_188

@ Repaint: the counterpart of OvlFunc_188, taking the metatiles from
@ (0x5F, 0x15) and the attributes to (0x19, 0x20) instead.
.thumb_func_start OvlFunc_1bc
	push	{lr}
	sub	sp, #8
	mov	r3, #1
	mov	r2, #3
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x5f
	mov	r1, #0x15
	mov	r2, #0x61
	mov	r3, #0x15
	bl	__Func_10424
	mov	r3, #0x20
	mov	r2, #0x19
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x26
	mov	r2, #3
	mov	r3, #1
	bl	__Func_10704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_1bc

@ Trigger: teleports slot 9 to the origin and sets save bit 0x882.
.thumb_func_start OvlFunc_1f0
	push	{lr}
	bl	__Func_916b0
	mov	r1, #0
	mov	r2, #0
	mov	r0, #9
	bl	__Func_923e4
	ldr	r0, =0x882
	bl	__Func_79358
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_1f0

@ RevealSlotF
@ Takes no arguments. Teleports slot 8 to the origin, sets save bit 0x883, then
@ after a forty-frame beat brings slot 0x0F into view: animation 2, its flag
@ byte +0x55 cleared, bit 1 set in +0x23, draw priority 2, and a 1x1 attribute
@ cell repainted from (0, 0) to (0x0E, 0x12).
.thumb_func_start OvlFunc_214
	push	{lr}
	sub	sp, #8
	bl	__Func_916b0
	mov	r2, #0
	mov	r1, #0
	mov	r0, #8
	bl	__Func_923e4
	ldr	r0, =0x883
	bl	__Func_79358
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r1, #2
	mov	r0, #0xf
	bl	__Func_92548
	mov	r0, #0xf
	bl	__Func_92054
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #0xf
	bl	__Func_92054
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r0]
	mov	r1, #2
	mov	r0, #0xf
	bl	__Func_92b08
	mov	r3, #0x12
	mov	r2, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	bl	__Func_91750
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_214

@ Trigger: slot 0x0F -- palette change through Func_92950, a forty-frame beat,
@ sound 0xD2, then animation 6.
.thumb_func_start OvlFunc_280
	push	{lr}
	bl	__Func_916b0
	mov	r1, #0
	mov	r0, #0xf
	bl	__Func_92950
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0xd2
	bl	__Func_f9080
	mov	r0, #0xf
	mov	r1, #6
	bl	__Func_92548
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_280

@ Trigger: the same sequence on slot 0x10.
.thumb_func_start OvlFunc_2ac
	push	{lr}
	bl	__Func_916b0
	mov	r1, #0
	mov	r0, #0x10
	bl	__Func_92950
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0xd2
	bl	__Func_f9080
	mov	r0, #0x10
	mov	r1, #6
	bl	__Func_92548
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_2ac

@ Trigger: the same sequence on slot 0x11.
.thumb_func_start OvlFunc_2d8
	push	{lr}
	bl	__Func_916b0
	mov	r1, #0
	mov	r0, #0x11
	bl	__Func_92950
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0xd2
	bl	__Func_f9080
	mov	r0, #0x11
	mov	r1, #6
	bl	__Func_92548
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_2d8

@ CheckPlateePuzzle
@ Takes no arguments. The puzzle logic, re-run whenever a block moves.
@
@ Slots 0x0B and 0x0C are the blocks. Each is tested against the single target
@ tile (0x23, 0x17) -- both coordinates at whole-tile resolution -- and its own
@ save bit is set or CLEARED accordingly: 0x303 for slot 0x0B, 0x304 for slot
@ 0x0C. Clearing on failure is what lets the player undo a wrong move.
@
@ The gate then follows the OR of the two: if either bit is set and the gate
@ bit 0x302 is not, sound 0xD2 plays, slot 0x11 runs animation 6 and two
@ attribute cells are repainted from column 0 -- the gate opening. If neither
@ is set and 0x302 IS, sound 0xDC plays, slot 0x11 returns to animation 2 and
@ the same two cells are repainted from column 1 -- the gate closing again.
@ 0x302 tracks which state the gate is currently in, so the transition fires
@ once per change rather than every frame.
.thumb_func_start OvlFunc_304
	push	{r5, r6, lr}
	mov	r0, #0xb
	sub	sp, #8
	bl	__Func_92054
	mov	r5, r0
	mov	r0, #0xc
	bl	__Func_92054
	ldr	r3, [r5, #8]
	asr	r3, #20
	mov	r6, r0
	cmp	r3, #0x23
	bne	.L330
	ldr	r3, [r5, #0x10]
	asr	r3, #20
	cmp	r3, #0x17
	bne	.L330
	ldr	r0, =0x303
	bl	__Func_79358
	b	.L336
.L330:
	ldr	r0, =0x303
	bl	__Func_79374
.L336:
	ldr	r3, [r6, #8]
	asr	r3, #20
	cmp	r3, #0x23
	bne	.L350
	ldr	r3, [r6, #0x10]
	asr	r3, #20
	cmp	r3, #0x17
	bne	.L350
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__Func_79358
	b	.L358
.L350:
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__Func_79374
.L358:
	ldr	r0, =0x303
	bl	__Func_79338
	cmp	r0, #0
	bne	.L36e
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__Func_79338
	cmp	r0, #0
	beq	.L3c2
.L36e:
	ldr	r0, =0x302
	bl	__Func_79338
	cmp	r0, #0
	bne	.L3ba
	bl	__Func_916b0
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0xd2
	bl	__Func_f9080
	mov	r0, #0x11
	mov	r1, #6
	bl	__Func_92548
	mov	r3, #0x16
	str	r3, [sp, #4]
	mov	r5, #0x24
	mov	r0, #0
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	mov	r3, #0x18
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #2
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	bl	__Func_91750
.L3ba:
	ldr	r0, =0x302
	bl	__Func_79358
	b	.L414
.L3c2:
	ldr	r0, =0x302
	bl	__Func_79338
	cmp	r0, #0
	beq	.L40e
	bl	__Func_916b0
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0xdc
	bl	__Func_f9080
	mov	r0, #0x11
	mov	r1, #2
	bl	__Func_92548
	mov	r3, #0x16
	str	r3, [sp, #4]
	mov	r5, #0x24
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	mov	r3, #0x18
	str	r3, [sp, #4]
	mov	r0, #1
	mov	r1, #2
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	bl	__Func_91750
.L40e:
	ldr	r0, =0x302
	bl	__Func_79374
.L414:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_304

@ Trigger: repaints a 1x1 attribute cell from (0x1F, 0) to (0x0D, 8) and sets
@ save bit 0x305.
.thumb_func_start OvlFunc_424
	push	{lr}
	sub	sp, #8
	mov	r3, #8
	mov	r2, #0xd
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	mov	r0, #0x1f
	bl	__Func_10704
	ldr	r0, =0x305
	bl	__Func_79358
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_424

@ Sets byte +0x17 of the map descriptor at [iwram_1e70] to 1.
.thumb_func_start OvlFunc_44c
	ldr	r3, =iwram_1e70
	ldr	r2, [r3]
	mov	r3, #1
	strb	r3, [r2, #0x17]
	bx	lr
.func_end OvlFunc_44c

@ Clears the same byte -- the pair toggles one map-wide flag.
.thumb_func_start OvlFunc_45c
	ldr	r3, =iwram_1e70
	ldr	r2, [r3]
	mov	r3, #0
	strb	r3, [r2, #0x17]
	bx	lr
.func_end OvlFunc_45c

@ Slot 0: map-load entry. Dispatches to OvlFunc_4b4, _4e8 or _538 for areas
@ 0x31, 0x30 and 0x2F respectively; any other area needs no setup.
.thumb_func_start OvlFunc_46c
	push	{lr}
	ldr	r3, =ewram_240
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x31
	cmp	r2, r3
	bne	.L486
	bl	OvlFunc_4b4
	b	.L49c
.L486:
	ldr	r3, =0x30
	cmp	r2, r3
	bne	.L492
	bl	OvlFunc_4e8
	b	.L49c
.L492:
	ldr	r3, =0x2f
	cmp	r2, r3
	bne	.L49c
	bl	OvlFunc_538
.L49c:
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_46c

@ SetupArea31
@ Replays the OvlFunc_424 repaint and puts slot 8 into animation 0, but only
@ when save bit 0x305 records that the player already triggered it.
.thumb_func_start OvlFunc_4b4
	push	{lr}
	ldr	r0, =0x305
	sub	sp, #8
	bl	__Func_79338
	cmp	r0, #0
	beq	.L4de
	mov	r3, #8
	mov	r2, #0xd
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x1f
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	mov	r0, #8
	mov	r1, #0
	bl	__Func_924d4
.L4de:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_4b4

@ SetupArea30
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x204, puts slots 8 and
@ 0x0A into animations 1 and 2, and moves slot 9 depending on save bit 0x882 --
@ to the origin once the trigger has fired, elsewhere otherwise.
.thumb_func_start OvlFunc_4e8
	push	{lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x44
	str	r2, [r3]
	mov	r0, #8
	mov	r1, #1
	bl	__Func_924d4
	mov	r0, #0xa
	mov	r1, #2
	bl	__Func_924d4
	ldr	r0, =0x882
	bl	__Func_79338
	cmp	r0, #0
	beq	.L51e
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	b	.L52a
.L51e:
	mov	r0, #9
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
.L52a:
	pop	{r0}
	bx	r0
.func_end OvlFunc_4e8

@ SetupArea2F
@ The largest of the three setups, roughly 250 instructions: it reconstructs
@ the puzzle state from the save bits so the blocks and gate appear as the
@ player left them, then applies the same repaints the triggers would have.
.thumb_func_start OvlFunc_538
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x44
	str	r2, [r3]
	mov	r0, #0x12
	sub	sp, #8
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x13
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x14
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x15
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x16
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x17
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x18
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x19
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x1a
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0x12
	mov	r1, #5
	bl	__Func_924d4
	mov	r0, #0x13
	mov	r1, #5
	bl	__Func_924d4
	mov	r0, #0x14
	mov	r1, #5
	bl	__Func_924d4
	mov	r0, #0x15
	mov	r1, #5
	bl	__Func_924d4
	mov	r0, #0x16
	mov	r1, #5
	bl	__Func_924d4
	mov	r0, #0x17
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #0x18
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #0x19
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #0x1a
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #9
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0xa
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0xb
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0xc
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0xd
	mov	r1, #2
	bl	__Func_924d4
	mov	r1, #2
	mov	r0, #0xe
	bl	__Func_924d4
	mov	r0, #0x12
	bl	OvlFunc_904
	mov	r0, #0x13
	bl	OvlFunc_904
	mov	r0, #0x14
	bl	OvlFunc_904
	mov	r0, #0x15
	bl	OvlFunc_904
	mov	r0, #0x16
	bl	OvlFunc_904
	mov	r0, #0x17
	bl	OvlFunc_904
	mov	r0, #0x18
	bl	OvlFunc_904
	mov	r0, #0x19
	bl	OvlFunc_904
	mov	r0, #0x1a
	bl	OvlFunc_904
	mov	r0, #9
	bl	OvlFunc_904
	mov	r0, #0xa
	bl	OvlFunc_904
	mov	r0, #0xb
	bl	OvlFunc_904
	mov	r0, #0xc
	bl	OvlFunc_904
	mov	r0, #0xd
	bl	OvlFunc_904
	mov	r0, #0xe
	bl	OvlFunc_904
	ldr	r0, =0x883
	bl	__Func_79338
	cmp	r0, #0
	beq	.L6e8
	mov	r2, #0
	mov	r0, #8
	mov	r1, #0
	bl	__Func_923e4
	mov	r1, #5
	mov	r0, #0xf
	bl	__Func_924d4
	mov	r0, #0xf
	bl	__Func_92054
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #0xf
	bl	__Func_92054
	ldr	r3, =0xfffc0000
	str	r3, [r0, #0xc]
	mov	r0, #0xf
	bl	__Func_92054
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r0]
	mov	r1, #2
	mov	r0, #0xf
	bl	__Func_92b08
	mov	r3, #0x12
	mov	r2, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	b	.L704
.L6e8:
	mov	r1, #2
	mov	r0, #8
	bl	__Func_924d4
	mov	r0, #8
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xf
	mov	r1, #1
	bl	__Func_924d4
.L704:
	mov	r0, #0x10
	mov	r1, #1
	bl	__Func_924d4
	ldr	r0, =0x302
	bl	__Func_79338
	cmp	r0, #0
	beq	.L746
	mov	r0, #0x11
	mov	r1, #1
	bl	__Func_924d4
	mov	r3, #0x16
	mov	r5, #0x24
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	mov	r3, #0x18
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #2
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	b	.L774
.L746:
	mov	r0, #0x11
	mov	r1, #5
	bl	__Func_924d4
	mov	r3, #0x16
	mov	r5, #0x24
	str	r3, [sp, #4]
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	mov	r3, #0x18
	str	r3, [sp, #4]
	mov	r0, #1
	mov	r1, #2
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
.L774:
	ldr	r0, =0x303
	bl	__Func_79338
	cmp	r0, #0
	beq	.L78a
	mov	r2, #0xbc
	mov	r0, #0xb
	ldr	r1, =0x23a0000
	lsl	r2, #17
	bl	__Func_923e4
.L78a:
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__Func_79338
	cmp	r0, #0
	beq	.L7a2
	mov	r2, #0xbc
	mov	r0, #0xc
	ldr	r1, =0x23a0000
	lsl	r2, #17
	bl	__Func_923e4
.L7a2:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_538

@ Trigger: a short handler.
.thumb_func_start OvlFunc_7c4
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	mov	r2, r3
	mov	r5, r0
	mov	r4, #8
	add	r2, #0x34
.L7d2:
	ldmia	r2!, {r0}
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r5, r3
	bne	.L7e4
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r1, r3
	beq	.L7ec
.L7e4:
	add	r4, #1
	cmp	r4, #0x41
	bls	.L7d2
	mov	r0, #0
.L7ec:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_7c4

@ Cutscene: roughly 120 instructions of staged conversation.
.thumb_func_start OvlFunc_7f8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r0, #0
	sub	sp, #0xc
	bl	__Func_92054
	ldrh	r3, [r0, #6]
	ldr	r4, =.L1064
	lsr	r3, #12
	lsl	r5, r3, #2
	ldr	r3, [r4, r5]
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r8, r0
	asr	r2, r3, #16
	add	r1, r2
	mov	r9, r4
	mov	r4, r8
	asr	r0, r1, #4
	lsl	r3, #16
	mov	r1, #0x12
	ldrsh	r2, [r4, r1]
	asr	r3, #16
	add	r2, r3
	asr	r1, r2, #4
	bl	OvlFunc_7c4
	mov	r6, r0
	cmp	r6, #0
	beq	.L8e6
	mov	r1, r9
	ldr	r2, [r1, r5]
	mov	r0, #0xa
	ldrsh	r3, [r6, r0]
	asr	r1, r2, #16
	add	r3, r1
	asr	r0, r3, #4
	lsl	r2, #16
	mov	r4, #0x12
	ldrsh	r3, [r6, r4]
	asr	r2, #16
	add	r3, r2
	asr	r1, r3, #4
	bl	OvlFunc_7c4
	mov	r10, r0
	cmp	r0, #0
	bne	.L8e6
	mov	r2, r6
	add	r2, #0x22
	mov	r3, #2
	strb	r3, [r2]
	mov	r0, r9
	ldr	r1, [r0, r5]
	ldr	r2, =0xffff0000
	ldr	r3, [r6, #8]
	and	r2, r1
	mov	r7, sp
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	lsl	r1, #16
	add	r3, r1
	str	r3, [r7, #8]
	mov	r0, r6
	mov	r1, r7
	bl	__Func_120dc
	cmp	r0, #0
	bgt	.L8e6
	mov	r1, #8
	mov	r0, r8
	bl	__Func_c300
	ldr	r5, =0x3333
	mov	r0, #0xf
	bl	__Func_30f8
	mov	r0, #0xb9
	bl	__Func_f9080
	str	r5, [r6, #0x30]
	str	r5, [r6, #0x34]
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	mov	r0, r6
	bl	__Func_d14c
	mov	r1, r8
	str	r5, [r1, #0x30]
	str	r5, [r1, #0x34]
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	mov	r0, r8
	bl	__Func_d14c
	mov	r0, r6
	bl	__Func_ca6c
	ldr	r3, [r7]
	str	r3, [r6, #8]
	ldr	r3, [r7, #8]
	mov	r2, r10
	str	r3, [r6, #0x10]
	str	r2, [r6, #0x24]
	str	r2, [r6, #0x2c]
	mov	r0, r8
	mov	r1, #1
	bl	__Func_c300
	bl	OvlFunc_304
.L8e6:
	add	sp, #0xc
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_7f8

@ Trigger: a short closing handler.
.thumb_func_start OvlFunc_904
	bx	lr
.func_end OvlFunc_904

	.section .data

	.incbin "overlays/rom_7a6ae4/orig.bin", 0x9b0, (0x9bc-0x9b0)
.L9bc:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0x9bc, (0x9ec-0x9bc)
.L9ec:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0x9ec, (0xa64-0x9ec)
.La64:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xa64, (0xb24-0xa64)
.Lb24:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xb24, (0xbcc-0xb24)
.Lbcc:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xbcc, (0xc14-0xbcc)
.Lc14:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xc14, (0xc2c-0xc14)
.Lc2c:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xc2c, (0xc5c-0xc2c)
.Lc5c:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xc5c, (0xcbc-0xc5c)
.Lcbc:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xcbc, (0xe9c-0xcbc)
.Le9c:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xe9c, (0xea8-0xe9c)
.Lea8:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xea8, (0xefc-0xea8)
.Lefc:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xefc, (0xf80-0xefc)
.Lf80:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0xf80, (0x1064-0xf80)
.L1064:
	.incbin "overlays/rom_7a6ae4/orig.bin", 0x1064

	.include "macros.inc"

@ ============================================================================
@ Overlay 0x797740 -- a small interior with three NPCs in slots 8, 9 and 0xA.
@
@ Slot 0  OvlFunc_1e4  map-load entry
@ Slot 1  OvlFunc_30   edge transitions   -> .L2d0
@ Slot 2  OvlFunc_3c   map event list     -> .L348
@ Slot 3  OvlFunc_44   read after slot 4  -> two variants
@ Slot 4  OvlFunc_6c   map objects        -> two variants
@ Slot 5  OvlFunc_38   interactions       -> none (returns 0)
@
@ Slots 3 and 4 both branch on the entrance id at ewram_240+0x1C2, and both
@ single out entrance 0x0A -- so arriving by that door gives a different object
@ layout, and the two tables stay in step.
@
@ Each NPC has two lines: a scripted greeting used once (0x1388..0x138A) and a
@ plain repeat line (0x138C..0x138E). The three greeting handlers are
@ OvlFunc_94, _110 and _140; the three repeats are OvlFunc_170, _190 and _1B0.
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.L2d0
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L348
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: entrance 0x0A -> .L3bc, otherwise .L35c. Pairs with OvlFunc_6c.
.thumb_func_start OvlFunc_44
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0xa
	bne	.L5a
	ldr	r0, =.L3bc
	b	.L5c
.L5a:
	ldr	r0, =.L35c
.L5c:
	pop	{r1}
	bx	r1
.func_end OvlFunc_44

@ Slot 4: entrance 0x0A -> .L4a0, otherwise .L3ec.
.thumb_func_start OvlFunc_6c
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0xa
	bne	.L82
	ldr	r0, =.L4a0
	b	.L84
.L82:
	ldr	r0, =.L3ec
.L84:
	pop	{r1}
	bx	r1
.func_end OvlFunc_6c

@ GreetSlot8
@ Takes no arguments. The slot-8 NPC's scripted greeting.
@ Turns slot 8 toward slots 9 and 0xA in turn with Func_9280c, forty frames
@ apart, then speaks line 0x138A. Re-forms the followers -- slot 9 to formation
@ 2, slot 0xA to formation 2 with a refresh -- turns 8 and 0 to face each other,
@ drops slot 8 back to formation 1, and closes with a second line.
.thumb_func_start OvlFunc_94
	push	{lr}
	bl	__Func_916b0
	mov	r1, #9
	mov	r2, #0
	mov	r0, #8
	bl	__Func_9280c
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0
	mov	r1, #0xa
	mov	r0, #8
	bl	__Func_9280c
	mov	r0, #0x28
	bl	__Func_9163c
	ldr	r0, =0x138a
	bl	__Func_92b94
	mov	r0, #8
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #9
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #0xa
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0
	mov	r1, #0
	mov	r0, #8
	bl	__Func_92848
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #1
	mov	r0, #8
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #8
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_94

@ GreetSlot9. Formation 2 on slot 9, then line 0x1388.
.thumb_func_start OvlFunc_110
	push	{lr}
	bl	__Func_916b0
	mov	r1, #2
	mov	r0, #9
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	ldr	r0, =0x1388
	bl	__Func_92b94
	mov	r0, #9
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_110

@ GreetSlotA. Animation 4 on slot 0xA and wait, then line 0x1389.
.thumb_func_start OvlFunc_140
	push	{lr}
	bl	__Func_916b0
	mov	r1, #4
	mov	r0, #0xa
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	ldr	r0, =0x1389
	bl	__Func_92b94
	mov	r0, #0xa
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_140

@ RepeatSlot8. The plain line 0x138E, no staging.
.thumb_func_start OvlFunc_170
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x138e
	bl	__Func_92b94
	mov	r0, #8
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_170

@ RepeatSlot9. Line 0x138C.
.thumb_func_start OvlFunc_190
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x138c
	bl	__Func_92b94
	mov	r0, #9
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_190

@ RepeatSlotA. Line 0x138D.
.thumb_func_start OvlFunc_1b0
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x138d
	bl	__Func_92b94
	mov	r0, #0xa
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_1b0

@ Trigger: plays sound 0x7B and leaves message id 1 pending.
.thumb_func_start OvlFunc_1d0
	push	{lr}
	mov	r0, #0x7b
	bl	__Func_f9080
	mov	r0, #1
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_1d0

@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x209, then branches on
@ the entrance id at ewram_240+0x1C2:
@   entrance 2     clears save bit 0x12F and does nothing else,
@   entrance 0x0A  ORs 0x14 into byte +0x59 of slot 8 only,
@   otherwise      ORs 0x14 into byte +0x59 of slots 8, 9 and 0xA.
@ Byte +0x59 carries the per-entity interaction flags, so this is how the three
@ NPCs are made talkable -- and why arriving through entrance 0x0A leaves two
@ of them inert.
.thumb_func_start OvlFunc_1e4
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x49
	str	r2, [r3]
	ldr	r3, =ewram_240
	sub	r2, #0x47
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #2
	bne	.L20a
	ldr	r0, =0x12f
	bl	__Func_79374
	b	.L24c
.L20a:
	cmp	r3, #0xa
	bne	.L220
	mov	r0, #8
	bl	__Func_92054
	add	r0, #0x59
	ldrb	r2, [r0]
	mov	r3, #0x14
	orr	r3, r2
	strb	r3, [r0]
	b	.L24c
.L220:
	mov	r0, #8
	bl	__Func_92054
	add	r0, #0x59
	ldrb	r3, [r0]
	mov	r5, #0x14
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #9
	bl	__Func_92054
	add	r0, #0x59
	ldrb	r3, [r0]
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #0xa
	bl	__Func_92054
	add	r0, #0x59
	ldrb	r3, [r0]
	orr	r5, r3
	strb	r5, [r0]
.L24c:
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_1e4

	.section .data

.L2d0:
	.incbin "overlays/rom_797740/orig.bin", 0x2d0, (0x348-0x2d0)
.L348:
	.incbin "overlays/rom_797740/orig.bin", 0x348, (0x35c-0x348)
.L35c:
	.incbin "overlays/rom_797740/orig.bin", 0x35c, (0x3bc-0x35c)
.L3bc:
	.incbin "overlays/rom_797740/orig.bin", 0x3bc, (0x3ec-0x3bc)
.L3ec:
	.incbin "overlays/rom_797740/orig.bin", 0x3ec, (0x4a0-0x3ec)
.L4a0:
	.incbin "overlays/rom_797740/orig.bin", 0x4a0

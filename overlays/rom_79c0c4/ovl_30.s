	.include "macros.inc"

@ ============================================================================
@ Overlay 0x79c0c4 -- a town with shops, an inn, and a dozen villagers.
@
@ Slot 0  OvlFunc_4c8  map-load entry
@ Slot 1  OvlFunc_30   edge transitions   -> .L598
@ Slot 2  OvlFunc_3c   map event list     -> .L688
@ Slot 3  OvlFunc_44   -> .L6b0, tagged in place by Func_8b868
@ Slot 4  OvlFunc_11c  map objects        -> .L8f0
@ Slot 5  OvlFunc_38   interactions       -> none (returns 0)
@
@ All four tables are constant -- this town has no story variants at the table
@ level. Instead save bit 0x845 swaps individual lines from the 0x13xx block to
@ the 0x16xx block, handler by handler.
@
@ The counters use the standard facing arc (`facing - 0xA001 <= 0x3FFE`);
@ see overlays/rom_7b7790/ovl_314.s. Several villagers finish by turning back
@ to a resting angle with Func_92adc, which is what makes them face the street
@ again after a conversation.
@ ============================================================================

@ Slot 1: edge-transition table.
.thumb_func_start OvlFunc_30
	ldr	r0, =.L598
	bx	lr
.func_end OvlFunc_30

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_38
	mov	r0, #0
	bx	lr
.func_end OvlFunc_38

@ Slot 2: map event list.
.thumb_func_start OvlFunc_3c
	ldr	r0, =.L688
	bx	lr
.func_end OvlFunc_3c

@ Slot 3: .L6b0, passed through Func_8b868 so the in-bounds records are tagged.
.thumb_func_start OvlFunc_44
	push	{r5, lr}
	ldr	r5, =.L6b0
	mov	r0, r5
	bl	__Func_8b868
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_44

@ Counter: shop 7, speaker slot 0x10. Lines 0x16F5 (after 0x845) / 0x13E3.
.thumb_func_start OvlFunc_5c
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L7e
	mov	r0, #7
	mov	r1, #0x10
	bl	__Func_b0278
	b	.L9e
.L7e:
	ldr	r0, =0x845
	bl	__Func_79338
	cmp	r0, #0
	bne	.L90
	ldr	r0, =0x13e3
	bl	__Func_92b94
	b	.L96
.L90:
	ldr	r0, =0x16f5
	bl	__Func_92b94
.L96:
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_92f84
.L9e:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5c

@ Counter: shop 9, speaker slot 0x12. Lines 0x16F9 / 0x13E9.
.thumb_func_start OvlFunc_bc
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.Lde
	mov	r0, #9
	mov	r1, #0x12
	bl	__Func_b0278
	b	.Lfe
.Lde:
	ldr	r0, =0x845
	bl	__Func_79338
	cmp	r0, #0
	bne	.Lf0
	ldr	r0, =0x13e9
	bl	__Func_92b94
	b	.Lf6
.Lf0:
	ldr	r0, =0x16f9
	bl	__Func_92b94
.Lf6:
	mov	r0, #0x12
	mov	r1, #0
	bl	__Func_92f84
.Lfe:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_bc

@ Slot 4: map object table.
.thumb_func_start OvlFunc_11c
	ldr	r0, =.L8f0
	bx	lr
.func_end OvlFunc_11c

@ Counter: shop 8, speaker slot 0x11.
@ The before-0x845 branch is the elaborate one -- line 0x13E5, the shopkeeper
@ turns to face the player, a ten-frame beat, a question through Func_93054,
@ then a turn to 0x3000. After the bit is set it collapses to a single line.
.thumb_func_start OvlFunc_124
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L146
	mov	r0, #8
	mov	r1, #0x11
	bl	__Func_b0278
	b	.L18a
.L146:
	ldr	r0, =0x845
	bl	__Func_79338
	cmp	r0, #0
	bne	.L17c
	ldr	r0, =0x13e5
	bl	__Func_92b94
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x11
	bl	__Func_9280c
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0
	mov	r0, #0x11
	bl	__Func_93054
	mov	r1, #0xc0
	mov	r0, #0x11
	lsl	r1, #6
	mov	r2, #0xa
	bl	__Func_92adc
	b	.L18a
.L17c:
	ldr	r0, =0x16f7
	bl	__Func_92b94
	mov	r0, #0x11
	mov	r1, #0
	bl	__Func_92f84
.L18a:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_124

@ Talk: slot 0x15, line 0x13ED. Turns to face the player, speaks, then
@ settles back to angle 0xC000.
.thumb_func_start OvlFunc_1a8
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x13ed
	bl	__Func_92b94
	mov	r2, #0
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_9280c
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_1a8

@ TalkAndCount
@ Takes no arguments. Slot 0x18, line 0x13F0 through Func_93040, a turn toward
@ the player and a ten-frame beat, then a yes/no question.
@
@ A "yes" INCREMENTS the halfword at [iwram_1ebc]+0x1D8 rather than setting a
@ flag -- so this villager is counting something across repeat visits, not
@ recording a one-off.
.thumb_func_start OvlFunc_1e0
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x13f0
	bl	__Func_92b94
	mov	r0, #0x18
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0x18
	bl	__Func_9280c
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0
	mov	r0, #0x18
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	beq	.L22a
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L22a:
	mov	r0, #0x18
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0x80
	mov	r0, #0x18
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_1e0

@ Talk: slot 0x1B, line 0x13F6, with the same turn-and-ask shape.
.thumb_func_start OvlFunc_250
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x13f6
	bl	__Func_92b94
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0x1b
	bl	__Func_9280c
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0
	mov	r0, #0x1b
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	beq	.L290
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L290:
	mov	r0, #0x1b
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0x80
	mov	r0, #0x1b
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_250

@ Talk: slot 8, line 0x16E1, asked as a question.
.thumb_func_start OvlFunc_2b4
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x16e1
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #8
	bl	__Func_93054
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_2b4

@ Talk: slot 0x0D, line 0x16EC, asked as a question.
.thumb_func_start OvlFunc_2d4
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x16ec
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0xd
	bl	__Func_93054
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_2d4

@ Counter: INN 2, speaker slot 0x13. Lines 0x16FB (a question) / 0x13EB.
.thumb_func_start OvlFunc_2f4
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	ldrh	r5, [r0, #6]
	bl	__Func_916b0
	ldr	r3, =0xffff5fff
	add	r5, r3
	ldr	r3, =0x3ffe
	cmp	r5, r3
	bhi	.L316
	mov	r0, #2
	mov	r1, #0x13
	bl	__Func_b3284
	b	.L33e
.L316:
	ldr	r0, =0x845
	bl	__Func_79338
	cmp	r0, #0
	beq	.L330
	ldr	r0, =0x16fb
	bl	__Func_92b94
	mov	r0, #0x13
	mov	r1, #0
	bl	__Func_93054
	b	.L33e
.L330:
	ldr	r0, =0x13eb
	bl	__Func_92b94
	mov	r0, #0x13
	mov	r1, #0
	bl	__Func_92f84
.L33e:
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_2f4

@ TalkOnceThenRepeat
@ Takes no arguments. Save bit 0x300 gates a one-time exchange: line 0x16FF from
@ slot 0x15, a turn to 0x8000, a second line, then slot 0x16 re-forms and runs
@ effect sequence 0x102 through Func_93874, a sixty-frame beat and its own line
@ -- after which 0x300 is set so it never replays.
@
@ Either way it falls through to the repeat line 0x1702 and a turn to 0xC000.
.thumb_func_start OvlFunc_35c
	push	{lr}
	bl	__Func_916b0
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__Func_79338
	cmp	r0, #0
	bne	.L3be
	ldr	r0, =0x16ff
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0x80
	mov	r2, #0x14
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #0x16
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0x16
	bl	__Func_93874
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #0x16
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__Func_79358
.L3be:
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_9280c
	ldr	r0, =0x1702
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_35c

@ Talk: slot 0x16, line 0x1703, turn, second line, settle to angle 0.
.thumb_func_start OvlFunc_3f4
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x1703
	bl	__Func_92b94
	mov	r0, #0x16
	mov	r1, #0
	bl	__Func_92f84
	mov	r2, #0
	mov	r0, #0x16
	mov	r1, #0
	bl	__Func_9280c
	mov	r0, #0x16
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #0x16
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_92adc
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_3f4

@ Talk: slot 0x17, line 0x1705.
.thumb_func_start OvlFunc_430
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x1705
	bl	__Func_92b94
	mov	r0, #0x17
	mov	r1, #0
	bl	__Func_92f84
	mov	r2, #0
	mov	r0, #0x17
	mov	r1, #0
	bl	__Func_9280c
	mov	r0, #0x17
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0xc0
	mov	r0, #0x17
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_430

@ Talk: a short line handler.
.thumb_func_start OvlFunc_470
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x170a
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x1b
	bl	__Func_93054
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_470

@ Talk: a short line handler.
.thumb_func_start OvlFunc_490
	push	{lr}
	bl	__Func_916b0
	mov	r0, #3
	bl	__Func_79338
	cmp	r0, #0
	beq	.L4a8
	ldr	r0, =0x146f
	bl	__Func_92b94
	b	.L4ae
.L4a8:
	ldr	r0, =0x13d9
	bl	__Func_92b94
.L4ae:
	mov	r0, #0xa
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_490

@ Slot 0: map-load entry.
@
@ Sets the scene step delay at [iwram_1ebc]+0x1C0 to 0x209, then fixes up slot
@ 0x1B's presentation: the entity's +0x23 is cleared, and byte +0x09 of its
@ actor has bits 0 and 1 cleared before bit 3 is set -- the read-modify-write
@ is spelled `sub r3, #0xd` on a register still holding 0, giving the mask
@ 0xFFFFFFF3. That selects the OAM priority this one object needs.
.thumb_func_start OvlFunc_4c8
	push	{lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x49
	str	r2, [r3]
	mov	r0, #0x1b
	bl	__Func_92054
	mov	r2, r0
	add	r2, #0x23
	mov	r3, #0
	strb	r3, [r2]
	ldr	r1, [r0, #0x50]
	ldrb	r2, [r1, #9]
	sub	r3, #0xd
	and	r3, r2
	mov	r2, #8
	orr	r3, r2
	strb	r3, [r1, #9]
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_4c8

	.section .data

.L598:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x598, (0x688-0x598)
.L688:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x688, (0x6b0-0x688)
.L6b0:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x6b0, (0x8f0-0x6b0)
.L8f0:
	.incbin "overlays/rom_79c0c4/orig.bin", 0x8f0

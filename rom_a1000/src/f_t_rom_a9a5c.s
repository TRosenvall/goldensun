	.include "macros.inc"
	.include "gba.inc"

@ DrawEquipSlots
@ r0 = window, r1 = character id, r2 = non-zero to skip the icons.
@ Draws the four equipment slot labels -- 0xB24 at row 0, 0xB25 at 0x20, 0xB26
@ at 0x10 and 0xB27 at 0x30 -- then Func_a9aec fills in what is equipped. Unless
@ r2 says otherwise it also lets a frame pass, loads the icons with Func_a3e28
@ and positions them with Func_a9c18.
.thumb_func_start Func_a9a5c
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r6, r0
	mov	r0, r1
	mov	r10, r2
	mov	r8, r3
	bl	_Func_77394
	bl	Func_a9cbc
	bl	Func_a345c
	ldr	r5, =0xb24
	mov	r1, r6
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_1e7c0
	add	r0, r5, #1
	mov	r1, r6
	mov	r2, #0
	mov	r3, #0x20
	bl	_Func_1e7c0
	add	r0, r5, #2
	mov	r1, r6
	mov	r2, #0
	mov	r3, #0x10
	add	r5, #3
	bl	_Func_1e7c0
	mov	r0, r5
	mov	r5, #0xe4
	lsl	r5, #1
	mov	r3, #0x30
	mov	r1, r6
	mov	r2, #0
	add	r5, r8
	bl	_Func_1e7c0
	mov	r0, r6
	mov	r1, r5
	bl	Func_a9aec
	mov	r3, r10
	cmp	r3, #0
	bne	.La9ad8
	mov	r0, #1
	bl	Func_30f8
	mov	r0, r5
	mov	r1, #1
	bl	Func_a3e28
	mov	r0, r5
	bl	Func_a9c18
.La9ad8:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_a9a5c

@ DrawEquippedNames
@ r0 = window, r1 = the fifteen-entry inventory list. For each slot with BIT 9
@ SET -- the equipped flag -- it resolves the ability record and prints the item
@ name (0x182 + id) at x 8 on the row its kind selects:
@
@     kind 1 -> row 0x08     kind 3 -> row 0x28
@     kind 2 -> row 0x38     kind 4 -> row 0x18
@
@ So the ability record's +0x02 doubles as the equipment slot type, and the four
@ kinds are the four slots Func_a9a5c labels.
.thumb_func_start Func_a9aec
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =0x182
	mov	r8, r3
	mov	r3, #0xe
	mov	r6, r0
	mov	r7, r1
	mov	r10, r3
.La9b00:
	ldrh	r0, [r7]
	ldr	r3, .La9b28	@ 0x200
	and	r3, r0
	add	r7, #2
	cmp	r3, #0
	beq	.La9b7c
	ldr	r3, .La9b2c	@ 0x1ff
	mov	r5, r3
	and	r5, r0
	mov	r0, r5
	bl	_Func_78414
	ldrb	r3, [r0, #2]
	cmp	r3, #2
	beq	.La9b4e
	cmp	r3, #2
	bgt	.La9b34
	cmp	r3, #1
	beq	.La9b3e
	b	.La9b7c

	.align	2, 0
.La9b28:
	.word	0x200
.La9b2c:
	.word	0x1ff
	.pool

.La9b34:
	cmp	r3, #3
	beq	.La9b5e
	cmp	r3, #4
	beq	.La9b6e
	b	.La9b7c
.La9b3e:
	mov	r3, r8
	add	r0, r5, r3
	mov	r1, r6
	mov	r2, #8
	mov	r3, #8
	bl	_Func_1e7c0
	b	.La9b7c
.La9b4e:
	mov	r3, r8
	add	r0, r5, r3
	mov	r1, r6
	mov	r2, #8
	mov	r3, #0x38
	bl	_Func_1e7c0
	b	.La9b7c
.La9b5e:
	mov	r3, r8
	add	r0, r5, r3
	mov	r1, r6
	mov	r2, #8
	mov	r3, #0x28
	bl	_Func_1e7c0
	b	.La9b7c
.La9b6e:
	mov	r3, r8
	add	r0, r5, r3
	mov	r1, r6
	mov	r2, #8
	mov	r3, #0x18
	bl	_Func_1e7c0
.La9b7c:
	mov	r3, #1
	neg	r3, r3
	add	r10, r3
	mov	r3, r10
	cmp	r3, #0
	bge	.La9b00
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a9aec

@ LayOutEquipGrid
@ r0 = x origin, r1 = y origin, r2 = columns. Walks all 32 nodes at state+0x48
@ and places each through Func_a9bd8. The Func_a1bdc of this file.
.thumb_func_start Func_a9b94
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r6, r3
	sub	sp, #4
	mov	r10, r0
	mov	r8, r1
	mov	r7, r2
	mov	r5, #0
	add	r6, #0x48
.La9bae:
	ldmia	r6!, {r0}
	cmp	r0, #0
	beq	.La9bc0
	mov	r1, r5
	mov	r2, r10
	mov	r3, r8
	str	r7, [sp]
	bl	Func_a9bd8
.La9bc0:
	add	r5, #1
	cmp	r5, #0x1f
	ble	.La9bae
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a9b94

@ PlaceEquipNode
@ r0 = node, r1 = index (wrapped above 31), r2 = x origin, r3 = y origin,
@ arg5 = columns. Row is index / columns, column index % columns, both scaled by
@ 16, then Func_a17c4. Same arithmetic as Func_a1c2c but taking the node
@ directly instead of an array and an index.
.thumb_func_start Func_a9bd8
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r5, r1
	mov	r6, r0
	mov	r8, r2
	mov	r7, r3
	cmp	r5, #0x1f
	ble	.La9bec
	mov	r5, #0
.La9bec:
	ldr	r1, [sp, #0x14]
	mov	r0, r5
	bl	Func_af0_from_thumb
	lsl	r0, #4
	add	r0, r7
	strh	r0, [r6, #8]
	ldr	r1, [sp, #0x14]
	mov	r0, r5
	bl	Func_b1c_from_thumb
	lsl	r0, #4
	add	r0, r8
	strh	r0, [r6, #6]
	mov	r0, r6
	bl	Func_a17c4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a9bd8

@ PlaceEquippedIcons
@ r0 = the fifteen-entry list. Parks every sprite off screen with Func_a9cbc,
@ then for each EQUIPPED entry (bit 9 set) moves the matching node to x 0xD8 and
@ the y its ability kind selects -- 1 to 0x20, 2 to 0x50, 3 to 0x40, 4 to 0x30 --
@ so the icons line up with the labels Func_a9aec wrote.
.thumb_func_start Func_a9c18
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r6, [r3]
	mov	r5, r0
	mov	r7, r6
	sub	sp, #4
	bl	Func_a9cbc
	mov	r3, #0xe
	mov	r1, #0xd8
	add	r7, #0x48
	mov	r6, r5
	mov	r8, r3
.La9c36:
	ldrh	r2, [r6]
	mov	r3, r2
	add	r6, #2
	cmp	r3, #0
	beq	.La9ca0
	ldr	r3, .La9c68	@ 0x200
	and	r3, r2
	cmp	r3, #0
	beq	.La9ca0
	ldr	r5, [r7]
	cmp	r5, #0
	beq	.La9ca0
	ldr	r0, .La9c6c	@ 0x1ff
	and	r0, r2
	str	r1, [sp]
	bl	_Func_78414
	ldrb	r3, [r0, #2]
	ldr	r1, [sp]
	cmp	r3, #2
	beq	.La9c88
	cmp	r3, #2
	bgt	.La9c7a
	b	.La9c74

	.align	2, 0
.La9c68:
	.word	0x200
.La9c6c:
	.word	0x1ff
	.pool

.La9c74:
	cmp	r3, #1
	beq	.La9c84
	b	.La9c96
.La9c7a:
	cmp	r3, #3
	beq	.La9c8c
	cmp	r3, #4
	beq	.La9c90
	b	.La9c96
.La9c84:
	mov	r3, #0x20
	b	.La9c92
.La9c88:
	mov	r3, #0x50
	b	.La9c92
.La9c8c:
	mov	r3, #0x40
	b	.La9c92
.La9c90:
	mov	r3, #0x30
.La9c92:
	strh	r1, [r5, #6]
	strh	r3, [r5, #8]
.La9c96:
	mov	r0, r5
	str	r1, [sp]
	bl	Func_a17c4
	ldr	r1, [sp]
.La9ca0:
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r3, r8
	add	r7, #4
	cmp	r3, #0
	bge	.La9c36
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a9c18

@ ParkListSprites
@ Takes no arguments. Moves all 32 nodes at state+0x48 to (0xF8, 0xA8) -- off
@ the visible area -- and rewinds each. Hiding by position rather than by the
@ +0x05 state byte, which is what Func_a345c does.
.thumb_func_start Func_a9cbc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r2, #0xf8
	mov	r5, r3
	mov	r8, r2
	add	r5, #0x48
	mov	r7, #0xa8
	mov	r6, #0x1f
.La9cd2:
	ldmia	r5!, {r0}
	cmp	r0, #0
	beq	.La9ce2
	mov	r3, r8
	strh	r3, [r0, #6]
	strh	r7, [r0, #8]
	bl	Func_a17c4
.La9ce2:
	sub	r6, #1
	cmp	r6, #0
	bge	.La9cd2
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a9cbc

@ CreateElementSprites
@ r0 = window. Fills the eight node slots at state+0xC8 with panel sprites from
@ _Func_1eb64 at priority 0xA8, tile source 0xF8. Returns 1.
.thumb_func_start Func_a9cf8
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r2, #0xa8
	mov	r6, r3
	sub	sp, #4
	mov	r7, r0
	mov	r5, #0
	mov	r8, r2
	add	r6, #0xc8
.La9d10:
	mov	r3, r8
	str	r3, [sp]
	mov	r1, r5
	mov	r0, #2
	mov	r2, r7
	mov	r3, #0xf8
	bl	_Func_1eb64
	add	r5, #1
	stmia	r6!, {r0}
	cmp	r5, #7
	ble	.La9d10
	mov	r0, #1
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a9cf8

@ PlaceElementIcons
@ r0 = five flag bytes. Parks the eight state+0xC8 sprites with Func_a9d84, then
@ for each set flag moves one to x 8 with y stepping down from 0x58 by 0x10 and
@ sort order 0xF0. Only as many icons appear as the character has affinities.
.thumb_func_start Func_a9d3c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	mov	r8, r0
	ldr	r5, [r3]
	bl	Func_a9d84
	mov	r6, #0
	add	r5, #0xc8
	mov	r7, #0x58
.La9d52:
	ldmia	r5!, {r0}
	cmp	r0, #0
	beq	.La9d70
	mov	r2, r8
	ldrb	r3, [r2, r6]
	cmp	r3, #0
	beq	.La9d70
	mov	r3, #8
	strh	r3, [r0, #6]
	mov	r3, #0xf0
	strh	r7, [r0, #8]
	strb	r3, [r0, #0xf]
	bl	Func_a17c4
	add	r7, #0x10
.La9d70:
	add	r6, #1
	cmp	r6, #4
	ble	.La9d52
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a9d3c

@ ParkElementSprites
@ Takes no arguments. Moves the five state+0xC8 sprites to (0xF8, 0xA8) with
@ sort order 0xF0 and rewinds each -- the Func_a9cbc of the element icons.
.thumb_func_start Func_a9d84
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r2, #0xf8
	mov	r5, r3
	mov	r8, r2
	add	r5, #0xc8
	mov	r7, #0xa8
	mov	r6, #4
.La9d9a:
	ldmia	r5!, {r0}
	cmp	r0, #0
	beq	.La9dae
	mov	r3, r8
	strh	r3, [r0, #6]
	mov	r3, #0xf0
	strh	r7, [r0, #8]
	strb	r3, [r0, #0xf]
	bl	Func_a17c4
.La9dae:
	sub	r6, #1
	cmp	r6, #0
	bge	.La9d9a
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a9d84

@ LoadElementIcons
@ r0 = five flag bytes. For each set flag, loads panel set 8 into the matching
@ state+0xC8 node with the graphic id its slot selects: 0x10, 1, 2, 0x0F, 7 for
@ slots 0 through 4. Slots past 4 would take 0, but the loop stops at 4.
@ Returns 1.
.thumb_func_start Func_a9dc4
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1f2c
	ldr	r6, [r3]
	mov	r7, r0
	mov	r5, #0
.La9dce:
	ldrb	r3, [r7, r5]
	cmp	r3, #0
	beq	.La9e1c
	cmp	r5, #4
	bhi	.La9e08
	ldr	r3, =.La9de0
	lsl	r2, r5, #2
	ldr	r3, [r2, r3]
	mov	pc, r3
	.align	2, 0
.La9de0:
	.word	.La9df4
	.word	.La9df8
	.word	.La9dfc
	.word	.La9e00
	.word	.La9e04
.La9df4:
	mov	r1, #0x10
	b	.La9e0c
.La9df8:
	mov	r1, #1
	b	.La9e0c
.La9dfc:
	mov	r1, #2
	b	.La9e0c
.La9e00:
	mov	r1, #0xf
	b	.La9e0c
.La9e04:
	mov	r1, #7
	b	.La9e0c
.La9e08:
	mov	r1, #0
	lsl	r2, r5, #2
.La9e0c:
	mov	r3, r2
	add	r3, #0xc8
	ldr	r3, [r6, r3]
	mov	r0, #8
	ldrb	r2, [r3, #0xe]
	mov	r3, #0
	bl	_Func_1bcd4
.La9e1c:
	add	r5, #1
	cmp	r5, #4
	ble	.La9dce
	mov	r0, #1
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a9dc4

@ RestoreStatusPalette
@ Takes no arguments. Func_a22f4 then Func_a2144(0x0D) -- resync the palettes
@ and reload bank 13, which is the status screen's own.
.thumb_func_start Func_a9e34
	push	{lr}
	bl	Func_a22f4
	mov	r0, #0xd
	bl	Func_a2144
	pop	{r0}
	bx	r0
.func_end Func_a9e34

@ NullHandler7
@ An empty `bx lr`.
.thumb_func_start Func_a9e44
	bx	lr
.func_end Func_a9e44

@ UseInventoryItem
@ r0 = inventory slot, r1 = user id, r2 = target. THE function that actually
@ uses an item. It resolves the slot to an ability record, takes the display id
@ from +0x28 (mask 0x3FFF) and applies the effect through Func_a9f10. A -1 from
@ there is passed straight back as the failure code.
@
@ On success the ability record's TARGET KIND at +0x0C decides what happens to
@ the item itself:
@
@     1  it is consumed -- _Func_788c4 takes one unit and the list is recompacted
@     4  it TRANSFORMS: the slot's id is rewritten in place, and id 0xB8
@        specifically becomes 0xB9
@     anything else  the item is unchanged
@
@ Returns 0 on success.
.thumb_func_start Func_a9e48
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r7, r1
	mov	r11, r0
	mov	r0, r7
	sub	sp, #4
	mov	r9, r2
	bl	_Func_77394
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r1, r11
	str	r3, [sp]
	ldr	r2, =0x1ff
	lsl	r3, r1, #1
	mov	r5, r0
	add	r3, #0xd8
	mov	r10, r2
	mov	r8, r3
	ldrh	r3, [r5, r3]
	mov	r1, r10
	and	r1, r3
	mov	r10, r1
	mov	r0, r10
	bl	_Func_78414
	mov	r6, r0
	ldrh	r3, [r6, #0x28]
	ldr	r0, =0x3fff
	mov	r2, r9
	and	r0, r3
	mov	r1, r7
	mov	r3, #1
	bl	Func_a9f10
	mov	r2, #1
	neg	r2, r2
	cmp	r0, r2
	beq	.La9eee
	mov	r3, r8
	ldrh	r0, [r5, r3]
	bl	_Func_78414
	mov	r6, r0
	ldrb	r2, [r6, #0xc]
	mov	r3, r2
	cmp	r3, #1
	bne	.La9ed6
	mov	r1, r11
	mov	r0, r7
	bl	_Func_788c4
	mov	r3, #0xe4
	ldr	r2, [sp]
	lsl	r3, #1
	add	r1, r2, r3
	mov	r0, r5
	mov	r2, #0
	bl	Func_a3ddc
	mov	r2, #0x86
	ldr	r1, [sp]
	lsl	r2, #2
	add	r3, r1, r2
	strb	r0, [r3]
	ldrb	r2, [r6, #0xc]
.La9ed6:
	mov	r3, r2
	cmp	r3, #4
	bne	.La9eec
	mov	r3, r10
	cmp	r3, #0xb8
	bne	.La9ee6
	mov	r1, #0xb9
	mov	r10, r1
.La9ee6:
	mov	r3, r10
	mov	r2, r8
	strh	r3, [r5, r2]
.La9eec:
	mov	r0, #0
.La9eee:
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a9e48

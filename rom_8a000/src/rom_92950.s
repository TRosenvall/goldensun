	.include "macros.inc"

@ ============================================================================
@ Message boxes.
@
@ The id of the line to show lives at iwram_1ebc+0x1D8 and is bumped by one
@ after each box closes, so a run of consecutive lines needs no bookkeeping in
@ the caller. Func_92c40 opens a box positioned against a speaker; Func_93168
@ opens one at explicit screen coordinates. Both clamp into the visible area and
@ flip the box above the speaker when it would fall off the bottom.
@ Every wait honours the fast-forward flag at +0x1CC (see Func_9163c).
@ ============================================================================

@ SetSlotPalette
@ r0=slot, r1=palette index, with bit 8 selecting the mode.
@ Bit 8 set installs Func_92980 as the entity's per-frame hook (+0x6C), so the
@ palette cycles on its own. Bit 8 clear removes the hook and applies r1 once
@ through Func_929d8. Used to make an NPC flash while a script runs and then
@ settle back to a fixed colour.
.thumb_func_start Func_92950
	push	{r5, lr}
	mov	r5, r1
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L92974
	mov	r3, #0x80
	lsl	r3, #1
	and	r3, r5
	cmp	r3, #0
	beq	.L9296c
	ldr	r3, =Func_92980
	str	r3, [r0, #0x6c]
	b	.L92974
.L9296c:
	str	r3, [r0, #0x6c]
	mov	r1, r5
	bl	Func_929d8
.L92974:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_92950

@ PaletteBlinkHook
@ r0=entity. Per-frame hook installed by Func_92950. Picks a palette byte from
@ the 4-entry table .L9ed80 indexed by (iwram_1e40 >> 1) & 3 -- the global frame
@ counter, so the cycle runs at half speed over four steps -- and writes it to
@ the palette field (+0x05) of every part of the actor that has an animation
@ table. Sets the actor's dirty flag at +0x25 so the change is picked up.
@ Ignores entities that are not draw kind 1.
.thumb_func_start Func_92980
	push	{lr}
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.L929cc
	ldr	r3, =iwram_1e40
	ldr	r3, [r3]
	ldr	r0, [r0, #0x50]
	ldr	r1, =L9ed80
	lsr	r3, #1
	mov	r2, #3
	and	r3, r2
	mov	r12, r0
	ldrb	r4, [r1, r3]
	mov	r3, r12
	add	r3, #0x27
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L929c4
	add	r0, #0x28
	mov	r1, r3
.L929b0:
	ldmia	r0!, {r2}
	cmp	r2, #0
	beq	.L929be
	ldr	r3, [r2, #0x10]
	cmp	r3, #0
	beq	.L929be
	strb	r4, [r2, #5]
.L929be:
	sub	r1, #1
	cmp	r1, #0
	bne	.L929b0
.L929c4:
	mov	r2, r12
	add	r2, #0x25
	mov	r3, #1
	strb	r3, [r2]
.L929cc:
	pop	{r0}
	bx	r0
.func_end Func_92980

@ SetActorPartsPalette
@ r0=entity, r1=palette index. Writes r1 to the palette field (+0x05) of every
@ part of the actor that has an animation table, then sets the dirty flag at
@ +0x25. The static counterpart to Func_92980, and what Func_92950 calls when
@ no cycling was requested. Ignores entities that are not draw kind 1.
.thumb_func_start Func_929d8
	push	{lr}
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.L92a18
	ldr	r0, [r0, #0x50]
	mov	r12, r0
	mov	r3, r12
	add	r3, #0x27
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L92a10
	mov	r4, r12
	add	r4, #0x28
	mov	r0, r3
.L929fc:
	ldmia	r4!, {r2}
	cmp	r2, #0
	beq	.L92a0a
	ldr	r3, [r2, #0x10]
	cmp	r3, #0
	beq	.L92a0a
	strb	r1, [r2, #5]
.L92a0a:
	sub	r0, #1
	cmp	r0, #0
	bne	.L929fc
.L92a10:
	mov	r2, r12
	add	r2, #0x25
	mov	r3, #1
	strb	r3, [r2]
.L92a18:
	pop	{r0}
	bx	r0
.func_end Func_929d8

@ SetSlotChaseTarget
@ r0=slot, r1=packed target -- slot index in the low byte, bit 12 a
@ "keep current speed" flag -- r2=script.
@ Stores the target entity in the chaser's script argument slot at +0x68 and
@ installs the script with _Func_c2d8. Unless bit 12 is set it also matches the
@ chaser to its quarry: the turn rate at +0x64 becomes 0x28, the acceleration
@ at +0x34 is twice the target's, the max speed at +0x30 is copied outright, and
@ the collision flags at +0x59 are cleared so the chaser passes through others.
@ Both slots must resolve or nothing happens.
.thumb_func_start Func_92a1c
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	Func_8ba1c
	mov	r5, r0
	mov	r0, #0xff
	and	r0, r6
	bl	Func_8ba1c
	cmp	r5, #0
	beq	.L92a6c
	cmp	r0, #0
	beq	.L92a6c
	mov	r3, #0x80
	lsl	r3, #9
	and	r3, r6
	str	r0, [r5, #0x68]
	cmp	r3, #0
	bne	.L92a5e
	mov	r2, r5
	mov	r3, #0x28
	add	r2, #0x64
	strh	r3, [r2]
	ldr	r3, [r0, #0x34]
	lsl	r3, #1
	str	r3, [r5, #0x34]
	ldr	r3, [r0, #0x30]
	ldr	r1, =0
	str	r3, [r5, #0x30]
	mov	r3, r5
	add	r3, #0x59
	strb	r1, [r3]
.L92a5e:
	mov	r0, r5
	mov	r1, r7
	bl	_Func_c2d8
	b	.L92a6c

	.pool_aligned

.L92a6c:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92a1c

@ StepFacingTowardTarget
@ r0=entity. Moves the facing angle at +0x06 one step toward the goal angle
@ stored at +0x64 and returns the step taken (0 when already there).
@ NOTE the clamp is asymmetric with its own test: a delta larger than 0x1000 is
@ replaced by 0x800, and one below -0x1000 by -0x800, so deltas between 0x800
@ and 0x1000 pass through UNCLAMPED and turn faster than the nominal limit.
@ That looks unintentional but is what the original does.
.thumb_func_start Func_92a74
	push	{lr}
	mov	r2, r0
	mov	r0, #0
	cmp	r2, #0
	beq	.L92aa8
	mov	r3, r2
	add	r3, #0x64
	ldrh	r3, [r3]
	ldrh	r1, [r2, #6]
	sub	r3, r1
	lsl	r3, #16
	asr	r0, r3, #16
	cmp	r0, #0
	beq	.L92aa8
	mov	r3, #0x80
	lsl	r3, #5
	cmp	r0, r3
	ble	.L92a9c
	mov	r0, #0x80
	lsl	r0, #4
.L92a9c:
	ldr	r3, =0xfffff000
	cmp	r0, r3
	bge	.L92aa4
	ldr	r0, =0xfffff800
.L92aa4:
	add	r3, r1, r0
	strh	r3, [r2, #6]
.L92aa8:
	pop	{r1}
	bx	r1
.func_end Func_92a74

@ StopSlotEntity
@ r0=slot. Cancels any move: all three targets (+0x38/+0x3C/+0x40) go back to
@ the 0x80000000 "none" sentinel, the default behaviour is reinstalled with
@ _Func_c4ac, and the idle animation 1 is selected.
.thumb_func_start Func_92ab4
	push	{r5, lr}
	bl	Func_8ba1c
	mov	r5, r0
	cmp	r5, #0
	beq	.L92ad6
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r5, #0x38]
	str	r3, [r5, #0x3c]
	str	r3, [r5, #0x40]
	bl	_Func_c4ac
	mov	r0, r5
	mov	r1, #1
	bl	_Func_c300
.L92ad6:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_92ab4

@ TurnSlotToAngle
@ r0=slot, r1=goal facing angle, r2=frames to wait afterwards. Stores the goal
@ at +0x64, installs the turn script Data_9fc1c so the entity rotates toward it
@ over subsequent frames, then blocks for r2 frames through Func_9163c (so a
@ fast-forwarding player skips the pause).
.thumb_func_start Func_92adc
	push	{r5, r6, lr}
	mov	r5, r1
	mov	r6, r2
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L92afc
	mov	r3, r0
	add	r3, #0x64
	strh	r5, [r3]
	ldr	r1, =Data_9fc1c
	bl	_Func_c2d8
	mov	r0, r6
	bl	Func_9163c
.L92afc:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_92adc

@ SetSlotDrawPriority
@ r0=slot, r1=priority 0-3. Writes bits 2-3 of BOTH actor byte +0x09 and byte
@ +0x15 -- the OAM priority of the main sprite and of its companion/shadow --
@ so the pair stays on the same layer. Also clears bit 0 of the entity's +0x23,
@ dropping the position-correction that would otherwise offset the sprite.
@ Draw kind 1 only.
.thumb_func_start Func_92b08
	push	{r5, r6, lr}
	mov	r5, r1
	bl	Func_8ba1c
	mov	r6, r0
	cmp	r6, #0
	beq	.L92b4e
	mov	r3, r6
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.L92b4e
	ldr	r1, [r6, #0x50]
	mov	r2, #0xd
	ldrb	r0, [r1, #9]
	mov	r3, #3
	neg	r2, r2
	and	r5, r3
	mov	r3, r2
	lsl	r4, r5, #2
	and	r3, r0
	orr	r3, r4
	strb	r3, [r1, #9]
	ldrb	r3, [r1, #0x15]
	and	r2, r3
	orr	r2, r4
	strb	r2, [r1, #0x15]
	mov	r1, r6
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #0xfe
	and	r3, r2
	strb	r3, [r1]
.L92b4e:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_92b08

@ ShareActorTiles
@ r0=destination slot, r1=source slot. Copies the source actor's tile
@ allocation size code (+0x1C) and the low 10 bits of its OAM attr2 (+0x08) --
@ the tile index -- onto the destination actor, so both draw from the same VRAM
@ tiles. Lets a second entity mirror a sprite without a second allocation.
.thumb_func_start Func_92b54
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r8, r0
	mov	r0, r1
	bl	Func_92054
	ldr	r0, [r0, #0x50]
	ldrb	r6, [r0, #0x1c]
	ldrh	r5, [r0, #8]
	mov	r0, r8
	bl	Func_92054
	ldr	r0, [r0, #0x50]
	ldr	r3, =0xfffffc00
	ldrh	r2, [r0, #8]
	lsl	r5, #22
	lsr	r5, #22
	and	r3, r2
	orr	r3, r5
	strb	r6, [r0, #0x1c]
	strh	r3, [r0, #8]
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_92b54

@ Nop -- empty hook.
.thumb_func_start Func_92b90
	bx	lr
.func_end Func_92b90

@ SetActiveMessageId
@ r0=message id. Stores it as a halfword at iwram_1ebc+0x1D8, the id the
@ dialogue system opens next. The interaction handlers in rom_8d5dc.s feed this
@ from a trigger's payload.
.thumb_func_start Func_92b94
	ldr	r3, =iwram_1ebc
	mov	r2, #0xec
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	strh	r0, [r3]
	bx	lr
.func_end Func_92b94

@ GetSlotSpriteId
@ r0=slot (masked to 12 bits). Returns the sprite resource id of that slot's
@ actor -- read from the first part at actor+0x28 -- or -1 when the slot is
@ empty or not draw kind 1. The inverse of Func_92be0.
.thumb_func_start Func_92ba8
	push	{lr}
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	ldr	r3, =0xfff
	and	r3, r0
	lsl	r3, #2
	add	r3, #0x14
	ldr	r2, [r2, r3]
	mov	r1, #1
	neg	r1, r1
	cmp	r2, #0
	beq	.L92bd2
	mov	r3, r2
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L92bd2
	ldr	r3, [r2, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r1, [r3, r2]
.L92bd2:
	mov	r0, r1
	pop	{r1}
	bx	r1
.func_end Func_92ba8

@ FindSlotBySpriteId
@ r0=sprite resource id. Returns the slot holding an actor whose first part has
@ that id, or -1 if none does.
@ Checks slot 8 first as a special case, then scans 9..0x41. Since only the
@ streamed scenery range is searched, party slots 0-7 are never returned.
.thumb_func_start Func_92be0
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	ldr	r4, [r3]
	ldr	r2, [r4, #0x34]
	mov	r5, #1
	neg	r5, r5
	mov	r1, #8
	cmp	r2, #0
	beq	.L92c0c
	mov	r3, r2
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L92c0c
	ldr	r3, [r2, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, r0
	bne	.L92c0c
	mov	r5, #8
	b	.L92c34
.L92c0c:
	add	r1, #1
	cmp	r1, #0x41
	bgt	.L92c34
	lsl	r3, r1, #2
	add	r3, #0x14
	ldr	r2, [r4, r3]
	cmp	r2, #0
	beq	.L92c0c
	mov	r3, r2
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L92c0c
	ldr	r3, [r2, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, r0
	bne	.L92c0c
	mov	r5, r1
.L92c34:
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_92be0

@ OpenMessageBoxForSlot
@ r0=packed speaker: slot in the low 12 bits, style flags in bits 12-15.
@ Opens a message box for the line at iwram_1ebc+0x1D8, positioned against that
@ slot's on-screen sprite, and returns the box handle.
@ Reads the window metrics from iwram_1e8c and resolves the speaker's sprite id
@ with Func_92ba8 so the box can carry the right portrait. The box is placed
@ below the speaker when there is room and flipped above it otherwise, then
@ clamped into the visible area.
@ The ~420-instruction body is characterised structurally; the state block, the
@ message-id source and the flag layout are verified.
.thumb_func_start Func_92c40
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1e8c
	ldr	r1, [r3]
	sub	sp, #0x40
	str	r1, [sp, #0x20]
	ldr	r3, [r3, #0x30]
	mov	r2, #0
	mov	r6, r0
	str	r3, [sp, #0x1c]
	mov	r10, r2
	mov	r9, r2
	bl	Func_92ba8
	mov	r2, #0xf0
	lsl	r2, #8
	mov	r3, #0
	mov	r11, r2
	mov	r1, #4
	str	r3, [sp, #0x14]
	str	r3, [sp, #0x10]
	str	r1, [sp, #0xc]
	mov	r3, r11
	ldr	r1, [sp, #0x1c]
	mov	r2, #0xec
	lsl	r2, #1
	str	r0, [sp, #0x18]
	and	r3, r6
	mov	r11, r3
	add	r3, r1, r2
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	ldr	r3, =0xfff
	and	r6, r3
	mov	r4, #0
	mov	r0, r6
	mov	r8, r1
	str	r4, [sp, #4]
	bl	Func_8ba1c
	mov	r2, #0xfa
	ldr	r1, [sp, #0x1c]
	lsl	r2, #1
	add	r3, r1, r2
	str	r6, [r3]
	sub	r2, #0x28
	add	r3, r1, r2
	ldr	r3, [r3]
	mov	r5, #0
	mov	r7, #0
	ldr	r4, [sp, #4]
	cmp	r3, #0
	beq	.L92cb6
	b	.L92f3c
.L92cb6:
	cmp	r0, #0
	beq	.L92ce6
	sub	r2, #0x2e
	add	r3, r1, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #3
	bne	.L92cde
	add	r5, sp, #0x34
	mov	r1, r5
	add	r0, #8
	bl	Func_5268
	ldr	r3, [r5]
	asr	r4, r3, #3
	ldr	r3, [r5, #4]
	asr	r5, r3, #3
	mov	r7, #1
	sub	r5, #2
	b	.L92d38
.L92cde:
	add	r5, sp, #0x34
	mov	r1, r5
	mov	r0, r6
	b	.L92d24
.L92ce6:
	cmp	r6, #7
	bgt	.L92d38
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	str	r6, [sp, #0x18]
	add	r5, r3, r2
	ldr	r0, [r5]
	bl	Func_8ba1c
	mov	r2, #0xcf
	ldr	r1, [sp, #0x1c]
	lsl	r2, #1
	add	r3, r1, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #3
	bne	.L92d1e
	add	r5, sp, #0x34
	mov	r1, r5
	add	r0, #8
	bl	Func_5268
	ldr	r3, [r5]
	asr	r4, r3, #3
	ldr	r3, [r5, #4]
	mov	r7, #1
	b	.L92d36
.L92d1e:
	ldr	r0, [r5]
	add	r5, sp, #0x34
	mov	r1, r5
.L92d24:
	bl	Func_94154
	mvn	r0, r0
	neg	r3, r0
	orr	r3, r0
	lsr	r7, r3, #31
	ldr	r3, [r5]
	asr	r4, r3, #3
	ldr	r3, [r5, #4]
.L92d36:
	asr	r5, r3, #3
.L92d38:
	cmp	r7, #0
	bne	.L92d44
	mov	r3, #0xf
	str	r3, [sp, #0x30]
	mov	r3, #0xa
	b	.L92d94
.L92d44:
	mov	r3, #0
	add	r0, sp, #0x24
	str	r3, [sp, #0x30]
	str	r3, [sp, #0x2c]
	add	r2, sp, #0x2c
	add	r3, sp, #0x28
	str	r0, [sp]
	add	r1, sp, #0x30
	mov	r0, r8
	str	r4, [sp, #4]
	bl	_Func_187fc
	ldr	r3, [sp, #0x28]
	lsr	r2, r3, #31
	add	r3, r2
	ldr	r4, [sp, #4]
	asr	r3, #1
	sub	r3, r4, r3
	str	r3, [sp, #0x30]
	mov	r3, #0x80
	lsl	r3, #7
	mov	r2, r11
	and	r3, r2
	cmp	r3, #0
	beq	.L92d7e
	ldr	r3, [sp, #0x24]
	sub	r3, r5, r3
	sub	r3, #1
	b	.L92d94
.L92d7e:
	mov	r1, r11
	lsr	r3, r1, #15
	cmp	r3, #0
	bne	.L92d92
	cmp	r5, #8
	bgt	.L92d92
	ldr	r3, [sp, #0x24]
	sub	r3, r5, r3
	sub	r3, #1
	b	.L92d94
.L92d92:
	add	r3, r5, #4
.L92d94:
	str	r3, [sp, #0x2c]
	ldr	r2, [sp, #0x20]
	ldr	r1, =0xea4
	add	r3, r2, r1
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L92da6
	mov	r2, #5
	str	r2, [sp, #0xc]
.L92da6:
	mov	r3, #0x80
	lsl	r3, #5
	mov	r1, r11
	and	r3, r1
	mov	r6, r4
	cmp	r3, #0
	beq	.L92dc2
	ldr	r2, [sp, #0xc]
	sub	r3, r6, r2
	sub	r6, r3, #2
	cmp	r6, #0
	bge	.L92dfe
	mov	r6, #0
	b	.L92dfe
.L92dc2:
	mov	r3, #0x80
	lsl	r3, #6
	mov	r1, r11
	and	r3, r1
	cmp	r3, #0
	beq	.L92dde
	ldr	r2, [sp, #0xc]
	add	r6, #2
	add	r3, r6, r2
	cmp	r3, #0x1d
	ble	.L92dfe
	mov	r3, #0x1d
	sub	r6, r3, r2
	b	.L92dfe
.L92dde:
	cmp	r6, #0xf
	bgt	.L92df0
	ldr	r1, [sp, #0xc]
	sub	r3, r6, r1
	sub	r6, r3, #2
	cmp	r6, #0
	bge	.L92dfe
	add	r6, r4, #2
	b	.L92dfe
.L92df0:
	ldr	r2, [sp, #0xc]
	add	r6, #2
	add	r3, r6, r2
	cmp	r3, #0x1d
	ble	.L92dfe
	sub	r3, r4, r2
	sub	r6, r3, #2
.L92dfe:
	ldr	r0, [sp, #0x18]
	bl	_Func_19d2c
	mov	r3, #1
	neg	r3, r3
	mov	r7, r0
	mov	r10, r3
	cmp	r7, r10
	beq	.L92e80
	mov	r3, sp
	mov	r1, #0x30
	mov	r2, #0x2c
	add	r1, sp
	add	r2, sp
	add	r3, #0x28
	add	r7, sp, #0x24
	mov	r0, r8
	mov	r9, r1
	mov	r11, r2
	str	r3, [sp, #8]
	str	r7, [sp]
	bl	_Func_187fc
	ldr	r2, [sp, #0x2c]
	sub	r1, r2, #5
	mov	r8, r10
	str	r1, [sp, #0x10]
	cmp	r2, r5
	bgt	.L92e3e
	ldr	r3, [sp, #0x24]
	add	r3, r2, r3
	str	r3, [sp, #0x10]
.L92e3e:
	ldr	r3, [sp, #0x10]
	cmp	r3, #0
	bge	.L92e4c
	ldr	r3, [sp, #0x24]
	add	r3, r2, r3
	str	r3, [sp, #0x10]
	b	.L92e58
.L92e4c:
	ldr	r3, [sp, #0x10]
	add	r3, #5
	cmp	r3, #0x13
	ble	.L92e58
	sub	r1, r2, #5
	str	r1, [sp, #0x10]
.L92e58:
	ldr	r3, [sp, #0x10]
	cmp	r2, r3
	bge	.L92ea2
	mov	r0, #1
	mov	r1, r9
	ldr	r3, [sp, #8]
	neg	r0, r0
	mov	r2, r11
	ldr	r5, [sp, #0x24]
	str	r7, [sp]
	bl	_Func_187ac
	ldr	r3, [sp, #0x24]
	mov	r1, #1
	sub	r5, r3
	neg	r1, r1
	add	r5, #1
	mov	r8, r1
	str	r5, [sp, #0x14]
	b	.L92ea2
.L92e80:
	ldr	r3, [sp, #0x2c]
	cmp	r3, r5
	bge	.L92ea2
	add	r0, sp, #0x24
	add	r3, sp, #0x28
	str	r0, [sp]
	add	r1, sp, #0x30
	mov	r0, r8
	add	r2, sp, #0x2c
	ldr	r5, [sp, #0x24]
	bl	_Func_187ac
	ldr	r3, [sp, #0x24]
	sub	r5, r3
	add	r5, #1
	str	r5, [sp, #0x14]
	mov	r8, r7
.L92ea2:
	cmp	r6, #0
	bge	.L92eaa
	mov	r6, #0
	b	.L92eb6
.L92eaa:
	ldr	r2, [sp, #0xc]
	add	r3, r6, r2
	cmp	r3, #0x1d
	ble	.L92eb6
	mov	r3, #0x1d
	sub	r6, r3, r2
.L92eb6:
	ldr	r1, [sp, #0x20]
	ldr	r2, =0xea4
	add	r3, r1, r2
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L92ee6
	mov	r0, #8
	bl	Func_30f8
	ldr	r3, [sp, #0x14]
	cmp	r3, #0
	beq	.L92edc
	ldr	r2, [sp, #0x2c]
	add	r2, r3
	ldr	r1, [sp, #0x30]
	sub	r2, #1
	mov	r0, r8
	mov	r3, #0x12
	b	.L92f10
.L92edc:
	ldr	r1, [sp, #0x30]
	ldr	r2, [sp, #0x2c]
	mov	r0, r8
	mov	r3, #2
	b	.L92f10
.L92ee6:
	ldr	r0, [sp, #0x18]
	bl	Func_915ac
	ldr	r1, [sp, #0x14]
	cmp	r1, #0
	beq	.L92f04
	ldr	r3, [sp, #0x14]
	ldr	r2, [sp, #0x2c]
	add	r2, r3
	lsl	r3, r0, #16
	mov	r0, #0x11
	orr	r3, r0
	ldr	r1, [sp, #0x30]
	sub	r2, #1
	b	.L92f0e
.L92f04:
	lsl	r3, r0, #16
	mov	r0, #1
	orr	r3, r0
	ldr	r1, [sp, #0x30]
	ldr	r2, [sp, #0x2c]
.L92f0e:
	mov	r0, r8
.L92f10:
	bl	_Func_17658
	mov	r10, r0
	ldr	r1, [sp, #0x20]
	ldr	r2, =0xea4
	add	r3, r1, r2
	ldrb	r3, [r3]
	ldr	r0, [sp, #0x18]
	mov	r1, #0
	mov	r2, r6
	ldr	r3, [sp, #0x10]
	bl	_Func_19da8
	mov	r9, r0
	b	.L92f34
.L92f2e:
	mov	r0, #1
	bl	Func_30f8
.L92f34:
	bl	_Func_17364
	cmp	r0, #0
	beq	.L92f2e
.L92f3c:
	ldr	r1, [sp, #0x1c]
	mov	r2, #0xfc
	lsl	r2, #1
	add	r3, r1, r2
	mov	r1, r10
	str	r1, [r3]
	ldr	r2, [sp, #0x1c]
	mov	r1, #0xfe
	lsl	r1, #1
	add	r3, r2, r1
	mov	r2, r9
	str	r2, [r3]
	ldr	r3, [sp, #0x1c]
	sub	r1, #0x24
	add	r2, r3, r1
	ldrh	r3, [r2]
	add	r3, #1
	mov	r0, r10
	strh	r3, [r2]
	add	sp, #0x40
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_92c40

@ ShowMessageAndWait
@ r0=speaker slot, r1=style flags. Opens the box with Func_92c40, lets a frame
@ pass, then resolves the speaker for the portrait: slots 0-7 whose Func_8d394
@ lookup succeeds report themselves, anything else falls back to the sprite id
@ from Func_92ba8. That id goes to _Func_19e48.
@ Blocks until _Func_17394 reports the box closed, polling once per frame with a
@ 0x258 (600) frame cap, and closes any leftover prompt with _Func_19a54.
@ Returns immediately after opening if the fast-forward flag at +0x1CC is set.
.thumb_func_start Func_92f84
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	mov	r7, r0
	mov	r9, r3
	bl	Func_92c40
	mov	r10, r0
	mov	r0, #1
	bl	Func_30f8
	mov	r0, r7
	bl	Func_92ba8
	mov	r5, #0
	mov	r8, r0
	cmp	r7, #7
	bgt	.L92fc0
	ldr	r6, =0xfff
	and	r6, r7
	mov	r0, r6
	bl	Func_8d394
	cmp	r0, #0
	bne	.L92fc0
	mov	r8, r6
.L92fc0:
	mov	r0, r8
	bl	_Func_19e48
	mov	r3, #0xe6
	lsl	r3, #1
	add	r3, r9
	ldr	r3, [r3]
	cmp	r3, #0
	bne	.L9301e
	b	.L93014
.L92fd4:
	mov	r0, #1
	bl	Func_30f8
	mov	r3, #0x96
	add	r5, #1
	lsl	r3, #2
	cmp	r5, r3
	bhi	.L93010
	ldr	r1, =iwram_1ae8
	ldr	r2, [r1]
	mov	r3, #4
	and	r2, r3
	cmp	r2, #0
	beq	.L93014
	ldr	r2, [r1]
	add	r3, #0xfc
	and	r2, r3
	cmp	r2, #0
	beq	.L93014
	ldr	r2, [r1]
	mov	r3, #0x80
	lsl	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.L93014
	ldr	r3, [r1]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L93014
.L93010:
	bl	_Func_19a54
.L93014:
	mov	r0, r10
	bl	_Func_17394
	cmp	r0, #0
	beq	.L92fd4
.L9301e:
	mov	r0, #1
	bl	Func_30f8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_92f84

@ ShowMessageAndPause
@ r0=speaker slot, r1=style flags, r2=frames. Func_92f84 followed by
@ Func_9163c(r2), so the caller gets a skippable pause after the box closes.
.thumb_func_start Func_93040
	push	{r5, lr}
	mov	r5, r2
	bl	Func_92f84
	mov	r0, r5
	bl	Func_9163c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_93040

@ ShowMessageWithPrompt
@ r0=speaker slot, r1=style flags. Returns the player's choice.
@ Opens the box with Func_92c40, puts the yes/no prompt up through
@ Func_91c7c against the player entity, then shows the follow-up line with
@ Func_92f84. The message id at +0x1D8 is advanced by one either way, but the
@ order differs: a non-zero choice advances before the follow-up, a zero choice
@ after -- so the two branches select different lines.
.thumb_func_start Func_93054
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r5, r0
	bl	Func_92c40
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	mov	r1, #0
	bl	Func_91c7c
	mov	r7, r0
	cmp	r7, #0
	bne	.L9308e
	mov	r0, r5
	mov	r1, r6
	bl	Func_92f84
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	b	.L930a6
.L9308e:
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, r5
	mov	r1, r6
	bl	Func_92f84
.L930a6:
	mov	r0, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_93054

@ Nop -- empty hook.
.thumb_func_start Func_930b8
	bx	lr
.func_end Func_930b8

@ ShowMessageAtPositionForSlot
@ r0=packed slot (low 12 bits). Records the slot as the active speaker at
@ iwram_1ebc+0x1F4, then opens a box at an explicit position -- unless the
@ fast-forward flag at +0x1CC is set, in which case the whole box is skipped.
@ Coordinates are clamped the same way everywhere in this file: x into
@ [0x14, 0xDC], y into [8, 0x138], and a y past 0x77 moves the box 0x20 down
@ rather than up so it clears the speaker.
.thumb_func_start Func_930bc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	mov	r8, r3
	ldr	r3, =0xfff
	mov	r5, r0
	and	r5, r3
	mov	r0, r5
	bl	Func_8ba1c
	mov	r3, #0xfa
	lsl	r3, #1
	add	r3, r8
	str	r5, [r3]
	mov	r3, #0xe6
	lsl	r3, #1
	add	r3, r8
	ldr	r3, [r3]
	cmp	r3, #0
	bne	.L9313e
	mov	r2, r7
	mov	r1, r6
	cmp	r2, #0x77
	ble	.L930f4
	add	r2, #0x20
	b	.L930f6
.L930f4:
	sub	r2, #0x20
.L930f6:
	cmp	r1, #8
	bge	.L930fc
	mov	r1, #8
.L930fc:
	mov	r3, #0x9c
	lsl	r3, #1
	cmp	r1, r3
	ble	.L93106
	mov	r1, r3
.L93106:
	cmp	r2, #0x14
	bge	.L9310c
	mov	r2, #0x14
.L9310c:
	cmp	r2, #0xdc
	ble	.L93112
	mov	r2, #0xdc
.L93112:
	mov	r3, #0xec
	lsl	r3, #1
	add	r3, r8
	mov	r4, #0
	ldrsh	r0, [r3, r4]
	mov	r3, #1
	bl	_Func_17658
	mov	r3, #0xfc
	lsl	r3, #1
	mov	r5, r0
	add	r3, r8
	str	r5, [r3]
	b	.L93134
.L9312e:
	mov	r0, #1
	bl	Func_30f8
.L93134:
	mov	r0, r5
	bl	_Func_17394
	cmp	r0, #0
	beq	.L9312e
.L9313e:
	mov	r2, #0xec
	lsl	r2, #1
	add	r2, r8
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_930bc

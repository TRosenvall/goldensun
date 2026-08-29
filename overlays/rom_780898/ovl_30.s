	.include "macros.inc"

@ ============================================================================
@ Overlay 0x780898 -- a map overlay whose first 1125 instructions are the
@ PUSHABLE-LOG PUZZLE, shared verbatim with about seventeen other overlays.
@
@ OvlFunc_30 .. OvlFunc_8c0 are that shared block. Everything from OvlFunc_9dc
@ onward is this map's own content. The block is byte-identical across the
@ family, so the notes here carry over unchanged; see docs/overlays.md for the
@ member list.
@
@ THE PUZZLE. Some maps contain long logs and pillars the player can shoulder
@ one tile at a time. Pushing one has to do three things at once: verify the
@ whole footprint of the log clears the terrain it is moving onto, animate the
@ player and the log together, and rewrite the map's collision attributes so
@ the log blocks movement at its new resting place and stops blocking at the
@ old one. The last part is why this needs map code at all rather than a plain
@ entity script.
@
@ THE TWO TABLES, both at the bottom of the file:
@
@   .L6190  16 words, indexed by the player's facing (the top four bits of the
@           halfword at entity+0x06). Each is a packed one-tile step: the HIGH
@           halfword is the x delta and the LOW halfword the z delta, both in
@           1/16-tile units, applied as `x += v & 0xFFFF0000` and
@           `z += v << 16`. The sixteen facings collapse to four cardinal
@           steps of +-16 units.
@
@   .L61d0  six model ids -- 0xCF, 0xCD, 0xE4, 0xE5, 0x12A, 0x129 -- read from
@           [[entity+0x50]+0x28]. These are the pushable props.
@   .L61e8  the matching six footprints, four s32 each: minX, minZ, maxX, maxZ
@           in 1/16-tile units. They alternate orientation, so the even ids are
@           logs lying along x (-32,-8,32,8) and the odd ids along z
@           (-8,-32,8,32). Index bit 0 therefore doubles as the axis flag, and
@           OvlFunc_34c uses it that way.
@
@ Positions are 16.16 fixed point at entity +0x08 (x), +0x0C (y), +0x10 (z);
@ `asr #20` turns one into a whole tile index, `asr #16` into 1/16 units.
@ ============================================================================

@ Distance3D
@ r0, r1 = two {x, y, z} triples of 16.16 words. Returns the integer distance
@ between them: each axis difference is taken down to 1/16 units with `asr #16`,
@ squared, summed, and passed to the ROM's Func_948 (IntegerSqrt) through a
@ call_via veneer because Func_948 is ARM code.
.thumb_func_start OvlFunc_30
	push	{r5, lr}
	ldmia	r0!, {r5}
	ldmia	r1!, {r3}
	ldmia	r0!, {r4}
	sub	r5, r3
	ldmia	r1!, {r3}
	ldr	r2, [r1]
	sub	r4, r3
	ldr	r3, [r0]
	sub	r3, r2
	asr	r5, #16
	asr	r4, #16
	asr	r3, #16
	mov	r0, r5
	mul	r0, r5
	mov	r2, r4
	mul	r2, r4
	mov	r1, r3
	mul	r1, r3
	add	r0, r2
	mov	r3, r1
	add	r0, r3
	ldr	r3, =Func_948
	bl	_call_via_r3
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_30

@ FindEntityAtPosition
@ r0 = an {x, y, z} triple, r1 = the entity to skip (the caller itself).
@ Scans entity slots 8..0x41 -- the map-object range, above the party slots --
@ from the table at [iwram_1ebc]+0x34, and returns the first whose position
@ matches r0. Returns 0 if nothing is there.
@
@ The comparison is deliberately mismatched between axes: x and z are compared
@ at whole-tile resolution (`asr #20`) but y at 1/16 (`asr #16`, with 0xFFFF
@ added first to round negatives toward zero). So an object counts as "here" if
@ it shares the tile, but only if it is on the same height step -- which is what
@ keeps a log on a ledge from blocking one on the floor below.
.thumb_func_start OvlFunc_6c
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1ebc
	mov	r4, r0
	ldr	r2, [r3]
	ldr	r3, [r4]
	mov	r1, r2
	ldr	r6, =0xffff
	mov	r5, #8
	asr	r7, r3, #20
	add	r1, #0x34
.L80:
	ldmia	r1!, {r0}
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r7, r3
	bne	.Lae
	ldr	r3, [r4, #4]
	cmp	r3, #0
	bge	.L92
	add	r3, r6
.L92:
	asr	r2, r3, #16
	ldr	r3, [r0, #0xc]
	cmp	r3, #0
	bge	.L9c
	add	r3, r6
.L9c:
	asr	r3, #16
	cmp	r2, r3
	bne	.Lae
	ldr	r2, [r4, #8]
	ldr	r3, [r0, #0x10]
	asr	r2, #20
	asr	r3, #20
	cmp	r2, r3
	beq	.Lb6
.Lae:
	add	r5, #1
	cmp	r5, #0x41
	bls	.L80
	mov	r0, #0
.Lb6:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_6c

@ TryPushBlockOneTile
@ No arguments. The interaction handler for a single-tile pushable block, as
@ opposed to the multi-tile logs the rest of this file deals with.
@
@ Takes the player (slot 0) and steps one tile along the facing from .L6190 to
@ find the block. Then it rejects the push unless all three of these hold:
@   - the tile beyond the block is empty (a second OvlFunc_6c one step further,
@     and if something is there, bit 0 of its byte at +0x59 must be clear),
@   - the tile directly above the block is clear too -- the same lookup with
@     y raised by 0x100000, one height step, so a block cannot be shoved under
@     an overhang,
@   - Func_120dc accepts the destination terrain (it returns > 0 to reject) and
@     the block's byte at +0x62 is zero.
@ +0x22 is set to 2 first because Func_120dc reads it to choose the collision
@ layer to sample.
@
@ Once committed: animation 8 on the player, a fifteen-frame beat, sound 0xB9,
@ then both the block and the player are given speed 0x3333 in +0x30/+0x34 and
@ sent to the new tile with Func_d14c. Func_ca6c waits out the slide and
@ Func_9202c ends the looping push cue. The final position is written straight
@ into the block, its velocities at +0x24/+0x2C are cleared, and the player's
@ move targets are reset to the 0x80000000 sentinel before animation 1 returns
@ it to idle.
.thumb_func_start OvlFunc_c4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r0, #0
	sub	sp, #0xc
	bl	__Func_92054
	ldrh	r3, [r0, #6]
	mov	r8, r0
	lsr	r3, #12
	ldr	r0, =.L6190
	lsl	r5, r3, #2
	ldr	r2, =0xffff0000
	ldr	r1, [r0, r5]
	mov	r10, r2
	mov	r3, r10
	mov	r2, r1
	mov	r9, r0
	mov	r0, r8
	and	r2, r3
	ldr	r3, [r0, #8]
	mov	r7, sp
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r0, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r0, #0x10]
	lsl	r1, #16
	add	r3, r1
	str	r3, [r7, #8]
	mov	r0, r7
	mov	r1, r8
	bl	OvlFunc_6c
	mov	r6, r0
	cmp	r6, #0
	bne	.L114
	b	.L226
.L114:
	mov	r2, r9
	ldr	r1, [r2, r5]
	mov	r3, r10
	mov	r2, r1
	and	r2, r3
	ldr	r3, [r6, #8]
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	lsl	r1, #16
	add	r3, r1
	str	r3, [r7, #8]
	mov	r0, r7
	mov	r1, r6
	bl	OvlFunc_6c
	cmp	r0, #0
	beq	.L14a
	mov	r3, r0
	add	r3, #0x59
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	bne	.L226
.L14a:
	ldr	r3, [r6, #8]
	str	r3, [r7]
	mov	r0, #0x80
	ldr	r3, [r6, #0xc]
	lsl	r0, #13
	add	r3, r0
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, r7
	str	r3, [r7, #8]
	mov	r1, r6
	bl	OvlFunc_6c
	cmp	r0, #0
	beq	.L176
	mov	r3, r0
	add	r3, #0x59
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	bne	.L226
.L176:
	mov	r2, r6
	add	r2, #0x22
	mov	r3, #2
	strb	r3, [r2]
	mov	r2, r9
	ldr	r1, [r2, r5]
	mov	r3, r10
	mov	r2, r1
	and	r2, r3
	ldr	r3, [r6, #8]
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
	bgt	.L226
	mov	r3, r6
	add	r3, #0x62
	ldrb	r3, [r3]
	mov	r10, r3
	cmp	r3, #0
	bne	.L226
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
	mov	r0, r8
	str	r5, [r0, #0x30]
	str	r5, [r0, #0x34]
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	bl	__Func_d14c
	mov	r0, r6
	bl	__Func_ca6c
	bl	__Func_9202c
	ldr	r3, [r7]
	str	r3, [r6, #8]
	ldr	r3, [r7, #8]
	mov	r1, r10
	str	r3, [r6, #0x10]
	str	r1, [r6, #0x24]
	str	r1, [r6, #0x2c]
	mov	r3, #0x80
	mov	r2, r8
	lsl	r3, #24
	str	r3, [r2, #0x38]
	str	r3, [r2, #0x40]
	mov	r0, #0xa
	ldrsh	r3, [r2, r0]
	lsl	r3, #16
	str	r1, [r2, #0x24]
	str	r1, [r2, #0x2c]
	str	r3, [r2, #8]
	mov	r1, #0x12
	ldrsh	r3, [r2, r1]
	lsl	r3, #16
	str	r3, [r2, #0x10]
	mov	r0, r8
	mov	r1, #1
	bl	__Func_c300
.L226:
	add	sp, #0xc
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_c4

@ FillMapRectCollisionByte
@ r0 = layer (0..2, anything higher means ewram_10000), r1 = x, r2 = z,
@ r3 = width, [sp+0x0C] = height, [sp+0x10] = the byte to store.
@ Always returns 0.
@
@ Writes one byte into every cell of a rectangle of the map's cell array: the
@ layer base comes from [iwram_1e70] + 0x130 + layer*0x30, rows are 0x200 bytes
@ apart (`lsl #9`) and cells 4 bytes, and the byte written is at offset +2 of
@ the cell -- the collision/attribute byte, not the metatile index.
@
@ This is how a log occupies space. OvlFunc_608 calls it with 0xFF to stamp the
@ footprint solid at the destination and with 0 to clear it at the origin, on
@ both layer 0 and layer 2. A null [iwram_1e70] makes it a no-op.
.thumb_func_start OvlFunc_244
	push	{r5, r6, lr}
	mov	r4, r3
	ldr	r3, [sp, #0xc]
	mov	r12, r3
	ldr	r3, =iwram_1e70
	mov	r6, r1
	mov	r1, r2
	ldr	r2, [r3]
	ldr	r5, [sp, #0x10]
	cmp	r2, #0
	beq	.L298
	cmp	r0, #2
	bhi	.L26e
	lsl	r3, r0, #1
	add	r3, r0
	mov	r0, #0x98
	lsl	r0, #1
	lsl	r3, #4
	add	r3, r0
	ldr	r0, [r2, r3]
	b	.L270
.L26e:
	ldr	r0, =ewram_10000
.L270:
	lsl	r3, r1, #7
	add	r3, r6, r3
	lsl	r3, #2
	mov	r1, #0
	add	r0, r3
	cmp	r1, r12
	bcs	.L298
.L27e:
	lsl	r3, r1, #9
	mov	r2, #0
	add	r3, r0, r3
	cmp	r2, r4
	bcs	.L292
.L288:
	add	r2, #1
	strb	r5, [r3, #2]
	add	r3, #4
	cmp	r2, r4
	bcc	.L288
.L292:
	add	r1, #1
	cmp	r1, r12
	bcc	.L27e
.L298:
	mov	r0, #0
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_244

@ PlayerPushFrameHook
@ r0 = the player entity. Installed at entity+0x6C by OvlFunc_608 for the
@ duration of a push and cleared again afterwards, so it runs once per frame
@ while the player is walking the log forward. Always returns 0.
@
@ It aborts the player's motion the moment the push stops being valid, by
@ zeroing the velocities at +0x24/+0x2C and resetting the move targets at
@ +0x38/+0x40 to the 0x80000000 sentinel. Two conditions trigger that:
@   - the tile ahead holds an entity that is NOT one of the six pushable models
@     in .L61d0 (something walked into the way), or
@   - Func_120dc rejects the terrain ahead.
@ Stopping the player here rather than in OvlFunc_608 means the log keeps its
@ own scripted glide while the player halts, which is what produces the small
@ stumble when a push is interrupted.
.thumb_func_start OvlFunc_2a8
	push	{r5, r6, r7, lr}
	mov	r5, r0
	ldrh	r3, [r5, #6]
	ldr	r2, =.L6190
	lsr	r3, #12
	lsl	r7, r3, #2
	ldr	r1, [r2, r7]
	ldr	r2, =0xffff0000
	ldr	r3, [r5, #8]
	and	r2, r1
	sub	sp, #0xc
	mov	r6, sp
	add	r3, r2
	str	r3, [r6]
	ldr	r3, [r5, #0xc]
	str	r3, [r6, #4]
	ldr	r3, [r5, #0x10]
	lsl	r1, #16
	add	r3, r1
	mov	r0, r6
	mov	r1, r5
	str	r3, [r6, #8]
	bl	OvlFunc_6c
	cmp	r0, #0
	beq	.L302
	ldr	r3, [r0, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	ldr	r2, =.L61d0
	mov	r1, #0
.L2e8:
	ldmia	r2!, {r3}
	cmp	r0, r3
	beq	.L336
	add	r1, #1
	cmp	r1, #5
	bls	.L2e8
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x2c]
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r5, #0x38]
	str	r3, [r5, #0x40]
.L302:
	ldr	r3, =.L6190
	ldr	r2, =0xffff0000
	ldr	r1, [r3, r7]
	ldr	r3, [r5, #8]
	and	r2, r1
	add	r3, r2
	str	r3, [r6]
	ldr	r3, [r5, #0xc]
	str	r3, [r6, #4]
	ldr	r3, [r5, #0x10]
	lsl	r1, #16
	add	r3, r1
	mov	r0, r5
	mov	r1, r6
	str	r3, [r6, #8]
	bl	__Func_120dc
	cmp	r0, #0
	ble	.L336
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x2c]
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r5, #0x38]
	str	r3, [r5, #0x40]
.L336:
	mov	r0, #0
	add	sp, #0xc
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_2a8

@ FindPushableFacingPlayer
@ r0 = out, the player's facing (0..15); r1 = out, the entity slot found;
@ r2 = out, the index into .L61d0/.L61e8 of the model that matched.
@ Returns the log's entity, or 0 if the player is not facing one.
@
@ A double loop: slots 8..0x41 on the outside, the six pushable model ids on the
@ inside. For each candidate whose model id at [[entity+0x50]+0x28] matches, it
@ builds the footprint rectangle from .L61e8 around the candidate's centre
@ (+0x0A and +0x12, the integer halves of x and z) and tests whether the tile
@ one step ahead of the player falls inside it.
@
@ The last check is what makes long logs work. Index bit 0 selects the log's
@ axis, and the player's coordinate ALONG that axis must differ from the log's:
@   even index (log lies along x) -- reject if the tile x matches,
@   odd index  (log lies along z) -- reject if the tile z matches.
@ Standing at the end of a log and facing down its length therefore finds
@ nothing, so a log can only be pushed broadside. Everything is compared at
@ 1/16 resolution (`asr #16` then `asr #4`) rather than whole tiles, because the
@ footprints are half-tile multiples.
.thumb_func_start OvlFunc_34c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x18
	str	r0, [sp, #0x14]
	str	r1, [sp, #0x10]
	str	r2, [sp, #0xc]
	ldr	r3, =iwram_1ebc
	mov	r0, #0
	ldr	r5, [r3]
	bl	__Func_92054
	ldrh	r3, [r0, #6]
	ldr	r1, [sp, #0x14]
	lsr	r3, #12
	mov	r2, #8
	str	r3, [r1]
	add	r5, #0x34
	str	r2, [sp, #8]
	mov	r9, r0
	mov	r11, r5
.L37e:
	mov	r3, r11
	ldr	r3, [r3]
	mov	r10, r3
	ldr	r3, [r3, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	ldr	r3, =.L61d0
	mov	r4, #0
	str	r1, [sp, #4]
	ldr	r0, =.L61e8
	str	r3, [sp]
	mov	r8, r4
.L398:
	ldr	r1, [sp]
	ldmia	r1!, {r3}
	ldr	r2, [sp, #4]
	mov	r4, r1
	str	r4, [sp]
	cmp	r2, r3
	bne	.L436
	ldr	r4, [sp, #0xc]
	mov	r3, r8
	str	r3, [r4]
	ldr	r2, [sp, #0x14]
	ldr	r3, [r2]
	ldr	r4, =.L6190
	lsl	r3, #2
	mov	r1, r9
	ldr	r2, [r4, r3]
	ldr	r1, [r1, #8]
	asr	r3, r2, #16
	mov	r14, r1
	asr	r1, #16
	add	r1, r3
	asr	r7, r1, #4
	mov	r1, r9
	ldr	r1, [r1, #0x10]
	lsl	r2, #16
	asr	r2, #16
	asr	r3, r1, #16
	add	r3, r2
	asr	r5, r3, #4
	mov	r3, r10
	mov	r12, r1
	mov	r2, #0xa
	ldrsh	r1, [r3, r2]
	ldr	r3, [r0]
	add	r3, r1, r3
	asr	r6, r3, #4
	mov	r3, r10
	mov	r4, #0x12
	ldrsh	r2, [r3, r4]
	ldr	r3, [r0, #4]
	add	r3, r2, r3
	asr	r4, r3, #4
	ldr	r3, [r0, #8]
	add	r1, r3
	ldr	r3, [r0, #0xc]
	add	r2, r3
	asr	r1, #4
	asr	r2, #4
	cmp	r6, r7
	bgt	.L436
	cmp	r7, r1
	bge	.L436
	cmp	r4, r5
	bgt	.L436
	cmp	r5, r2
	bge	.L436
	mov	r3, #1
	mov	r1, r8
	and	r3, r1
	cmp	r3, #0
	beq	.L424
	mov	r2, r14
	asr	r3, r2, #20
	cmp	r6, r3
	beq	.L436
	ldr	r3, [sp, #8]
	ldr	r4, [sp, #0x10]
	mov	r0, r10
	str	r3, [r4]
	b	.L452
.L424:
	mov	r1, r12
	asr	r3, r1, #20
	cmp	r4, r3
	beq	.L436
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #0x10]
	mov	r0, r10
	str	r2, [r3]
	b	.L452
.L436:
	mov	r4, #1
	add	r8, r4
	mov	r1, r8
	add	r0, #0x10
	cmp	r1, #5
	bls	.L398
	ldr	r3, [sp, #8]
	mov	r2, #4
	add	r3, #1
	add	r11, r2
	str	r3, [sp, #8]
	cmp	r3, #0x41
	bls	.L37e
	mov	r0, #0
.L452:
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_34c

@ PlanPushDistance
@ r0 = a six-word request block, filled in on the way through:
@       +0x00  out, the .L61e8 model index
@       +0x04  out, the entity slot (written by OvlFunc_34c through r1)
@       +0x08  out, destination x (16.16)
@       +0x0C  out, destination y
@       +0x10  out, destination z
@       +0x14  cleared on entry
@ Returns 1 if the log can move at least one tile, 0 if it cannot move at all
@ or the player is not facing one.
@
@ Having located the log with OvlFunc_34c, it converts the footprint into a
@ width and height in whole tiles -- |minX| + |maxX| and |minZ| + |maxZ|, each
@ `asr #4` -- and then walks outward one step at a time along the facing. At
@ every step it probes EVERY cell of the footprint with Func_120dc, raising y
@ by 0x100000 per row so each height step is sampled where it actually sits.
@
@ Only a return of 2 stops the walk -- "no tile / blocked". A return of 1 or -1,
@ meaning the step up or drop is too large, does not, so a log will happily be
@ pushed down a slope it could not be pushed up. The loop has no upper bound
@ other than that wall, which is why long open floors let a log slide the whole
@ way across.
@
@ The step count lands in [sp+0x0C]; the destination written back is the log's
@ start plus count times the .L6190 delta.
.thumb_func_start OvlFunc_474
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r6, r0
	sub	sp, #0x20
	mov	r3, #0
	add	r0, sp, #0x10
	add	r1, r6, #4
	mov	r2, r6
	str	r3, [r6, #0x14]
	bl	OvlFunc_34c
	mov	r10, r0
	cmp	r0, #0
	bne	.L49e
	mov	r0, #0
	b	.L5e8
.L49e:
	mov	r0, r10
	add	r0, #0x22
	mov	r3, #2
	str	r0, [sp, #4]
	strb	r3, [r0]
	ldr	r3, [r6]
	mov	r1, #0
	str	r1, [sp, #0xc]
	ldr	r5, =.L61e8
	lsl	r1, r3, #4
	add	r3, r1, #4
	ldr	r2, [r5, r3]
	cmp	r2, #0
	bge	.L4bc
	neg	r2, r2
.L4bc:
	mov	r3, r1
	add	r3, #0xc
	ldr	r3, [r5, r3]
	cmp	r3, #0
	bge	.L4c8
	neg	r3, r3
.L4c8:
	add	r3, r2, r3
	asr	r3, #4
	str	r3, [sp, #8]
	ldr	r2, [r5, r1]
	cmp	r2, #0
	bge	.L4d6
	neg	r2, r2
.L4d6:
	mov	r3, r1
	add	r3, #8
	ldr	r3, [r5, r3]
	cmp	r3, #0
	bge	.L4e2
	neg	r3, r3
.L4e2:
	add	r3, r2, r3
	asr	r3, #4
	mov	r9, r3
	ldr	r3, [sp, #0x10]
	ldr	r1, =.L6190
	add	r2, sp, #0x14
	lsl	r3, #2
	mov	r8, r2
	ldr	r2, [r1, r3]
	ldr	r3, =0xffff0000
	mov	r4, r10
	and	r2, r3
	ldr	r3, [r4, #8]
	mov	r0, r8
	add	r3, r2
	str	r3, [r0]
	ldr	r0, [r4, #0xc]
	mov	r2, r8
	str	r0, [r2, #4]
	ldr	r3, [sp, #0x10]
	lsl	r3, #2
	ldr	r2, [r1, r3]
	ldr	r3, [r4, #0x10]
	lsl	r2, #16
	mov	r4, r8
	add	r3, r2
	str	r3, [r4, #8]
	mov	r4, r6
	str	r0, [r6, #0xc]
	add	r4, #8
	mov	r11, r8
.L520:
	ldr	r3, [r6]
	ldr	r0, =.L61e8
	lsl	r3, #4
	add	r3, #4
	ldr	r2, [r0, r3]
	mov	r1, r8
	ldr	r3, [r1, #8]
	lsl	r2, #16
	add	r3, r2
	ldr	r2, [sp, #8]
	mov	r7, #0
	str	r3, [r6, #0x10]
	cmp	r7, r2
	bge	.L586
.L53c:
	ldr	r3, [r6]
	ldr	r0, =.L61e8
	lsl	r3, #4
	ldr	r2, [r0, r3]
	mov	r1, r8
	ldr	r3, [r1]
	lsl	r2, #16
	add	r3, r2
	mov	r5, #0
	str	r3, [r6, #8]
	cmp	r5, r9
	bge	.L574
.L554:
	mov	r1, r4
	mov	r0, r10
	str	r4, [sp]
	bl	__Func_120dc
	ldr	r4, [sp]
	cmp	r0, #2
	beq	.L5b0
	ldr	r3, [r4]
	mov	r2, #0x80
	lsl	r2, #13
	add	r3, r2
	add	r5, #1
	str	r3, [r4]
	cmp	r5, r9
	blt	.L554
.L574:
	ldr	r3, [r6, #0x10]
	mov	r0, #0x80
	lsl	r0, #13
	ldr	r1, [sp, #8]
	add	r3, r0
	add	r7, #1
	str	r3, [r6, #0x10]
	cmp	r7, r1
	blt	.L53c
.L586:
	ldr	r2, [sp, #0xc]
	ldr	r3, [sp, #0x10]
	add	r2, #1
	str	r2, [sp, #0xc]
	ldr	r0, =.L6190
	lsl	r3, #2
	ldr	r2, [r0, r3]
	ldr	r3, =0xffff0000
	mov	r1, r11
	and	r2, r3
	ldr	r3, [r1]
	add	r3, r2
	str	r3, [r1]
	ldr	r3, [sp, #0x10]
	lsl	r3, #2
	ldr	r2, [r0, r3]
	ldr	r3, [r1, #8]
	lsl	r2, #16
	add	r3, r2
	str	r3, [r1, #8]
	b	.L520
.L5b0:
	ldr	r2, [sp, #4]
	mov	r3, #0
	strb	r3, [r2]
	ldr	r3, [sp, #0xc]
	mov	r0, #0
	cmp	r3, #0
	beq	.L5e8
	ldr	r3, [sp, #0x10]
	ldr	r2, =.L6190
	lsl	r3, #2
	ldr	r2, [r2, r3]
	ldr	r3, =0xffff0000
	ldr	r4, [sp, #0xc]
	and	r3, r2
	mov	r1, r4
	mul	r1, r3
	lsl	r2, #16
	mul	r2, r4
	mov	r0, r10
	ldr	r3, [r0, #8]
	add	r3, r1
	str	r3, [r6, #8]
	ldr	r3, [r0, #0xc]
	str	r3, [r6, #0xc]
	ldr	r3, [r0, #0x10]
	add	r3, r2
	str	r3, [r6, #0x10]
	mov	r0, #1
.L5e8:
	add	sp, #0x20
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_474

@ RunPushCutscene
@ r0 = the .L61e8 model index, r1 = the log entity, r2 = destination x,
@ r3 = destination y, [sp+0x58] = destination z, [sp+0x5C] = an optional
@ callback invoked at the midpoint (0 for none). Returns nothing useful.
@
@ The whole push, start to finish. Note the unusual prologue -- `sub sp, #0x10`
@ BEFORE the register push, matched by `add sp, #0x10` after popping the return
@ address into r3 -- which is how the six stacked arguments are addressed.
@
@ Order of operations:
@   1. Clear the log's old footprint: OvlFunc_244 with byte 0 over the width
@      and height derived from .L61e8, so the player can walk into the space it
@      is about to vacate.
@   2. Player speed 0x8000 / accel 0x1999, animation 8, fifteen-frame beat.
@   3. Func_9228c nudges the player half a tile into the log, then
@      OvlFunc_2a8 is installed at the player's +0x6C as the per-frame guard.
@   4. Animation 3 if the facing is 6..13, otherwise 2 -- the two shoulder
@      poses, chosen by which side of the log the player is on.
@   5. Sound 0xEF, and Func_d14c launches the log toward its destination.
@   6. Speed drops to 0x4CCC / 0x1999 and the player follows for half the
@      remaining distance (`asr #1` on each delta, with the usual round-toward-
@      zero correction from `lsr #31`).
@   7. The optional callback fires here, through a call_via veneer.
@   8. The frame hook is cleared, Func_ca6c waits the log out, sounds 0x120
@      and 0xD5 close the cue, and the log's final position is written directly
@      with its velocities zeroed.
@   9. Func_10704 copies the map attributes over the new footprint and
@      OvlFunc_244 stamps 0xFF into layers 0 and 2, making the log solid again;
@      the same pair then runs at the destination and finally clears the origin.
@      Func_9202c ends the looping push sound.
.thumb_func_start OvlFunc_608
	sub	sp, #0x10
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x28
	str	r0, [sp, #0x48]
	str	r1, [sp, #0x4c]
	str	r2, [sp, #0x50]
	str	r3, [sp, #0x54]
	ldr	r3, =iwram_1e70
	ldr	r3, [r3]
	mov	r0, #0
	str	r3, [sp, #0xc]
	bl	__Func_92054
	ldrh	r3, [r0, #6]
	ldr	r0, [sp, #0x4c]
	lsr	r3, #12
	mov	r8, r3
	bl	__Func_92054
	ldr	r3, [sp, #0x48]
	ldr	r4, =.L61e8
	lsl	r1, r3, #4
	add	r3, r1, #4
	ldr	r2, [r4, r3]
	mov	r7, r0
	cmp	r2, #0
	bge	.L64c
	neg	r2, r2
.L64c:
	mov	r3, r1
	add	r3, #0xc
	ldr	r3, [r4, r3]
	cmp	r3, #0
	bge	.L658
	neg	r3, r3
.L658:
	add	r3, r2, r3
	asr	r3, #4
	str	r3, [sp, #8]
	ldr	r2, [r4, r1]
	cmp	r2, #0
	bge	.L666
	neg	r2, r2
.L666:
	mov	r3, r1
	add	r3, #8
	ldr	r3, [r4, r3]
	cmp	r3, #0
	bge	.L672
	neg	r3, r3
.L672:
	add	r3, r2, r3
	asr	r3, #4
	mov	r9, r3
	ldr	r5, =0x1999
	ldr	r3, [r7, #8]
	mov	r6, #0x80
	add	r0, sp, #0x1c
	lsl	r6, #8
	str	r6, [r7, #0x30]
	str	r5, [r7, #0x34]
	str	r3, [r0]
	ldr	r3, [r7, #0x10]
	str	r3, [r0, #8]
	ldr	r2, [sp, #0x48]
	lsl	r2, #4
	ldr	r3, [r4, r2]
	ldr	r1, [r7, #8]
	lsl	r3, #16
	add	r1, r3
	mov	r11, r0
	add	r0, sp, #0x10
	str	r1, [r0]
	add	r2, #4
	ldr	r3, [r4, r2]
	ldr	r2, [r7, #0x10]
	lsl	r3, #16
	add	r2, r3
	asr	r1, #20
	asr	r2, #20
	str	r1, [r0]
	str	r2, [r0, #8]
	ldr	r3, [sp, #8]
	str	r3, [sp]
	mov	r3, #0
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r3, r9
	bl	OvlFunc_244
	mov	r2, r5
	mov	r0, #0
	mov	r1, r6
	bl	__Func_92064
	mov	r1, #8
	mov	r0, #0
	bl	__Func_924d4
	mov	r0, #0xf
	bl	__Func_9163c
	mov	r4, r11
	ldr	r2, [sp, #0x50]
	ldr	r3, [r4]
	sub	r1, r2, r3
	cmp	r1, #0
	bge	.L6e8
	ldr	r0, =0x1ffff
	add	r1, r0
.L6e8:
	mov	r4, r11
	ldr	r2, [sp, #0x58]
	ldr	r3, [r4, #8]
	sub	r2, r3
	asr	r1, #17
	cmp	r2, #0
	bge	.L6fa
	ldr	r0, =0x1ffff
	add	r2, r0
.L6fa:
	asr	r2, #17
	mov	r0, #0
	bl	__Func_9228c
	mov	r0, #0
	bl	__Func_92054
	ldr	r3, =OvlFunc_2a8
	str	r3, [r0, #0x6c]
	mov	r0, #4
	bl	__Func_9163c
	mov	r3, r8
	sub	r3, #6
	cmp	r3, #7
	bhi	.L724
	mov	r0, r7
	mov	r1, #3
	bl	__Func_c300
	b	.L72c
.L724:
	mov	r0, r7
	mov	r1, #2
	bl	__Func_c300
.L72c:
	mov	r0, #0xef
	bl	__Func_f9080
	mov	r0, r7
	ldr	r1, [sp, #0x50]
	ldr	r2, [sp, #0x54]
	ldr	r3, [sp, #0x58]
	bl	__Func_d14c
	mov	r0, #0
	bl	__Func_923c4
	mov	r0, #0
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0
	ldr	r1, =0x4ccc
	ldr	r2, =0x1999
	bl	__Func_92064
	ldr	r2, =.L6190
	mov	r1, r8
	lsl	r3, r1, #2
	ldr	r0, [r2, r3]
	asr	r3, r0, #16
	lsl	r3, #16
	lsl	r0, #16
	asr	r1, r3, #16
	asr	r2, r0, #16
	lsr	r3, #31
	lsr	r0, #31
	add	r1, r3
	add	r2, r0
	asr	r1, #1
	asr	r2, #1
	mov	r0, #0
	bl	__Func_9228c
	ldr	r3, [sp, #0x5c]
	cmp	r3, #0
	beq	.L784
	bl	_call_via_r3
.L784:
	mov	r0, #0
	bl	__Func_923c4
	mov	r1, #1
	mov	r0, #0
	bl	__Func_924d4
	mov	r0, #0
	bl	__Func_92054
	mov	r2, #0
	str	r2, [r0, #0x6c]
	mov	r0, r7
	bl	__Func_ca6c
	mov	r0, #0x90
	lsl	r0, #1
	bl	__Func_f9080
	mov	r0, #0xd5
	bl	__Func_f9080
	ldr	r3, [sp, #0x50]
	str	r3, [r7, #8]
	ldr	r3, [sp, #0x58]
	str	r3, [r7, #0x10]
	mov	r3, #0
	str	r3, [r7, #0x24]
	str	r3, [r7, #0x2c]
	mov	r0, r7
	mov	r1, #1
	bl	__Func_c300
	ldr	r2, [sp, #0x48]
	ldr	r4, =.L61e8
	lsl	r2, #4
	ldr	r3, [r4, r2]
	ldr	r0, [sp, #0x50]
	lsl	r3, #16
	add	r2, #4
	add	r0, r3
	ldr	r3, [r4, r2]
	ldr	r1, [sp, #0x58]
	lsl	r3, #16
	add	r1, r3
	ldr	r2, [sp, #0xc]
	asr	r0, #20
	asr	r1, #20
	mov	r10, r4
	mov	r4, #0x9e
	str	r0, [sp, #0x50]
	str	r1, [sp, #0x58]
	lsl	r4, #1
	add	r3, r2, r4
	ldr	r3, [r3]
	mov	r8, r3
	mov	r2, r8
	asr	r2, #20
	ldr	r4, [sp, #0xc]
	mov	r8, r2
	mov	r2, #0xa0
	lsl	r2, #1
	add	r3, r4, r2
	ldr	r6, [r3]
	mov	r4, r8
	asr	r6, #20
	add	r3, r4, r0
	add	r2, r6, r1
	str	r3, [sp]
	str	r2, [sp, #4]
	ldr	r3, [sp, #8]
	mov	r2, r9
	bl	__Func_10704
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #0x50]
	ldr	r2, [sp, #0x58]
	str	r0, [sp]
	mov	r5, #0xff
	mov	r3, r9
	mov	r0, #0
	str	r5, [sp, #4]
	bl	OvlFunc_244
	ldr	r3, [sp, #8]
	ldr	r1, [sp, #0x50]
	ldr	r2, [sp, #0x58]
	str	r3, [sp]
	mov	r0, #2
	mov	r3, r9
	str	r5, [sp, #4]
	bl	OvlFunc_244
	ldr	r2, [sp, #0x48]
	mov	r4, r10
	lsl	r2, #4
	ldr	r3, [r4, r2]
	mov	r0, r11
	ldr	r1, [r0]
	lsl	r3, #16
	add	r2, #4
	add	r1, r3
	ldr	r3, [r4, r2]
	ldr	r2, [r0, #8]
	lsl	r3, #16
	add	r2, r3
	asr	r1, #20
	asr	r2, #20
	str	r1, [r0]
	str	r2, [r0, #8]
	add	r8, r1
	add	r6, r2
	str	r1, [sp]
	str	r2, [sp, #4]
	ldr	r3, [sp, #8]
	mov	r0, r8
	mov	r1, r6
	mov	r2, r9
	bl	__Func_10704
	ldr	r3, [sp, #8]
	mov	r2, r11
	ldr	r1, [r2]
	mov	r4, #0
	ldr	r2, [r2, #8]
	mov	r0, #2
	str	r3, [sp]
	mov	r3, r9
	str	r4, [sp, #4]
	bl	OvlFunc_244
	bl	__Func_9202c
	add	sp, #0x28
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r3}
	add	sp, #0x10
	bx	r3
.func_end OvlFunc_608

@ StampPlayerFootprintSolid
@ No arguments. Returns 1 if the player is standing as one of the six pushable
@ models, 0 otherwise.
@
@ Called on map entry. The player entity here is not the party leader but the
@ log itself -- these maps place the log in slot 0 during setup -- so this reads
@ its model id at [[entity+0x50]+0x28], finds it in .L61d0, and marks the map
@ cells it covers solid before the player can reach them. Without it a log would
@ start the map passable and only become an obstacle after the first push.
@
@ The search loop leaves index 7 in the slot when nothing matches (`mov r6, #7`
@ stored each pass), which the `cmp r2, #6 / bls` test then rejects. Index 0 is
@ handled by a separate branch before the loop, since the loop writes its
@ counter only after the first increment.
@
@ The work is the same closing pair as OvlFunc_608: Func_10704 to repaint the
@ attributes, then OvlFunc_244 with 0xFF on layer 0 and layer 2.
.thumb_func_start OvlFunc_8c0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1e70
	ldr	r3, [r3]
	sub	sp, #0x20
	mov	r10, r3
	bl	__Func_92054
	ldr	r3, [r0, #0x50]
	ldr	r3, [r3, #0x28]
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r1, =.L61d0
	mov	r5, #0
	ldr	r3, [r1, r5]
	cmp	r2, r3
	bne	.L8ea
	add	r7, sp, #8
	b	.L90c
.L8ea:
	add	r7, sp, #8
	mov	r12, r7
	mov	r6, #7
	mov	r4, r1
.L8f2:
	mov	r3, r12
	add	r5, #1
	str	r6, [r3]
	cmp	r5, #5
	bhi	.L90e
	ldr	r3, [r0, #0x50]
	ldr	r3, [r3, #0x28]
	add	r4, #4
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, [r4]
	cmp	r2, r3
	bne	.L8f2
.L90c:
	str	r5, [r7]
.L90e:
	ldr	r2, [r7]
	cmp	r2, #6
	bls	.L918
	mov	r0, #0
	b	.L9c2
.L918:
	ldr	r3, [r0, #8]
	str	r3, [r7, #8]
	mov	r12, r3
	ldr	r3, [r0, #0xc]
	str	r3, [r7, #0xc]
	ldr	r0, [r0, #0x10]
	lsl	r1, r2, #4
	str	r0, [r7, #0x10]
	ldr	r4, =.L61e8
	add	r5, r1, #4
	ldr	r2, [r4, r5]
	mov	r14, r0
	cmp	r2, #0
	bge	.L936
	neg	r2, r2
.L936:
	mov	r3, r1
	add	r3, #0xc
	ldr	r3, [r4, r3]
	cmp	r3, #0
	bge	.L942
	neg	r3, r3
.L942:
	add	r3, r2, r3
	ldr	r0, [r4, r1]
	asr	r3, #4
	mov	r8, r3
	mov	r6, r0
	cmp	r0, #0
	bge	.L952
	neg	r6, r0
.L952:
	mov	r3, r1
	add	r3, #8
	ldr	r3, [r4, r3]
	cmp	r3, #0
	bge	.L95e
	neg	r3, r3
.L95e:
	lsl	r0, #16
	add	r0, r12
	str	r0, [r7, #8]
	ldr	r1, [r4, r5]
	lsl	r1, #16
	add	r1, r14
	asr	r0, #20
	asr	r1, #20
	add	r6, r3
	mov	r3, #0x9e
	str	r0, [r7, #8]
	str	r1, [r7, #0x10]
	lsl	r3, #1
	add	r3, r10
	ldr	r3, [r3]
	asr	r5, r3, #20
	mov	r3, #0xa0
	lsl	r3, #1
	add	r3, r10
	ldr	r3, [r3]
	asr	r3, #20
	add	r2, r5, r0
	add	r3, r1
	asr	r6, #4
	str	r2, [sp]
	str	r3, [sp, #4]
	mov	r2, r6
	mov	r3, r8
	bl	__Func_10704
	mov	r3, r8
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0x10]
	mov	r5, #0xff
	str	r3, [sp]
	mov	r0, #0
	mov	r3, r6
	str	r5, [sp, #4]
	bl	OvlFunc_244
	mov	r3, r8
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0x10]
	mov	r0, #2
	str	r3, [sp]
	mov	r3, r6
	str	r5, [sp, #4]
	bl	OvlFunc_244
	mov	r0, #1
.L9c2:
	add	sp, #0x20
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_8c0

@ ----------------------------------------------------------------------------
@ THIS MAP'S OWN CONTENT starts here. The shared push-log block above ends at
@ OvlFunc_8c0.
@
@ The six export slots, resolved from the overlay header (slot N's target is at
@ __start_overlay + N*8 + 4):
@
@   slot 0  OvlFunc_2a54   main entry (code)
@   slot 1  OvlFunc_9dc    edge-transition table  -> .L6708
@   slot 2  OvlFunc_9e8    map event list         -> .L6870
@   slot 3  OvlFunc_9f0    read after slot 4      -> one of four, see below
@   slot 4  OvlFunc_aa4    map object table       -> one of three
@   slot 5  OvlFunc_9e4    interaction table      -> none (returns 0)
@
@ Slots 3 and 4 are not constants: this map has three story states, selected by
@ two save bits, plus one case keyed on how the player arrived. They must agree,
@ because slot 3's table is indexed against the object list slot 4 returns.
@
@   condition                                 slot 4     slot 3
@   entrance id (ewram_240+0x1C2) == 0x10     --         .L6e48
@   save bit 0x87A set                        .L7334     .L6cc8
@   save bit 0x815 set                        .L7100     .L6ab8
@   neither                                   .L6F38     .L68a8
@
@ Save bit 0x815 also gates most of the NPC dialogue below, and 0x87A a later
@ revision of it, so the two bits are this map's before/during/after markers.
@ ----------------------------------------------------------------------------

@ Slot 1: the edge-transition table. Constant for this map.
.thumb_func_start OvlFunc_9dc
	ldr	r0, =.L6708
	bx	lr
.func_end OvlFunc_9dc

@ Slot 5: the interaction table. This map has none.
.thumb_func_start OvlFunc_9e4
	mov	r0, #0
	bx	lr
.func_end OvlFunc_9e4

@ Slot 2: the map event list. Constant for this map.
.thumb_func_start OvlFunc_9e8
	ldr	r0, =.L6870
	bx	lr
.func_end OvlFunc_9e8

@ Slot 3: picks the table matching the map's story state. Checked in priority
@ order -- the entrance id at ewram_240+0x1C2 first, then the two save bits
@ newest-first -- so a later state wins over an earlier one. Must stay in step
@ with slot 4 (OvlFunc_aa4).
.thumb_func_start OvlFunc_9f0
	push	{lr}
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x10
	bne	.La06
	ldr	r0, =.L6e48
	b	.La24
.La06:
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	beq	.La14
	ldr	r0, =.L6cc8
	b	.La24
.La14:
	ldr	r0, =0x815
	bl	__Func_79338
	cmp	r0, #0
	beq	.La22
	ldr	r0, =.L6ab8
	b	.La24
.La22:
	ldr	r0, =.L68a8
.La24:
	pop	{r1}
	bx	r1
.func_end OvlFunc_9f0

@ 18 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, PlaceSlotAt, SetSaveBit, UpdateObjectProximity
@   ApplyFieldItemOrAbility, EndCutscene
@ sets 0xfd0.
.thumb_func_start OvlFunc_a44
	push	{lr}
	bl	__Func_916b0
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x1a
	bl	__Func_923e4
	mov	r0, #0xfd
	lsl	r0, #4
	bl	__Func_79358
	mov	r0, #0xb5
	mov	r1, #3
	bl	__Func_8f1c0
	mov	r1, #0
	mov	r0, #0xb5
	bl	__Func_91a58
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_a44

@ 18 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, PlaceSlotAt, SetSaveBit, UpdateObjectProximity
@   ApplyFieldItemOrAbility, EndCutscene
@ sets 0xfd0.
.thumb_func_start OvlFunc_a74
	push	{lr}
	bl	__Func_916b0
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x14
	bl	__Func_923e4
	mov	r0, #0xfd
	lsl	r0, #4
	bl	__Func_79358
	mov	r0, #0xb5
	mov	r1, #3
	bl	__Func_8f1c0
	mov	r1, #0
	mov	r0, #0xb5
	bl	__Func_91a58
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_a74

@ Slot 4: the map object table, in the same three story states as slot 3 but
@ without the ewram_240+0x1C2 case -- that state reuses whichever object list
@ the save bits already selected.
.thumb_func_start OvlFunc_aa4
	push	{lr}
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lab4
	ldr	r0, =.L7334
	b	.Lac4
.Lab4:
	ldr	r0, =0x815
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lac2
	ldr	r0, =.L7100
	b	.Lac4
.Lac2:
	ldr	r0, =.L6f38
.Lac4:
	pop	{r1}
	bx	r1
.func_end OvlFunc_aa4

@ Talk: lines 0xf81, 0x11cc, shown.
@ Which line is chosen by save bit 0x815.
@ Also turns to face the player.
.thumb_func_start OvlFunc_adc
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x815
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lafc
	ldr	r0, =0x11cc
	bl	__Func_92b94
	mov	r0, #0xa
	mov	r1, #0
	bl	__Func_92f84
	b	.Lb14
.Lafc:
	ldr	r0, =0xf81
	bl	__Func_92b94
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #4
	bl	__Func_92848
	mov	r0, #0xa
	mov	r1, #0
	bl	__Func_93054
.Lb14:
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_adc

@ Talk: lines 0xf7c, 0xf7e, 0x11c9, shown.
@ Which line is chosen by save bits 0x806, 0x815.
@ Also turns to face the player.
@ Sets save bit 0x806.
.thumb_func_start OvlFunc_b28
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x815
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lb48
	ldr	r0, =0x11c9
	bl	__Func_92b94
	mov	r0, #0xe
	mov	r1, #0
	bl	__Func_92f84
	b	.Lb8a
.Lb48:
	ldr	r0, =0x806
	bl	__Func_79338
	cmp	r0, #0
	bne	.Lb72
	ldr	r0, =0x806
	bl	__Func_79358
	ldr	r0, =0xf7c
	bl	__Func_92b94
	mov	r0, #0xe
	mov	r1, #0
	mov	r2, #4
	bl	__Func_92848
	mov	r0, #0xe
	mov	r1, #0
	bl	__Func_93054
	b	.Lb8a
.Lb72:
	ldr	r0, =0xf7e
	bl	__Func_92b94
	mov	r0, #0xe
	mov	r1, #0
	mov	r2, #4
	bl	__Func_92848
	mov	r0, #0xe
	mov	r1, #0
	bl	__Func_92f84
.Lb8a:
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_b28

@ Cutscene: roughly 66 instructions of straight-line script --
@ 1 turn, 0 animation changes, 4 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xf63, 0xf66.
@ Reads save bit 0x807.
@ Sets save bit 0x807.
.thumb_func_start OvlFunc_ba8
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x807
	bl	__Func_79338
	cmp	r0, #0
	bne	.Lc2e
	ldr	r0, =0x807
	bl	__Func_79358
	ldr	r0, =0xf63
	bl	__Func_92b94
	mov	r0, #0x12
	ldr	r1, =0x103
	mov	r2, #0
	bl	__Func_937b8
	mov	r0, #0
	mov	r1, #0x12
	mov	r2, #0x14
	bl	__Func_92848
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #6
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #0x12
	lsl	r1, #8
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r0, #0x12
	mov	r1, #2
	mov	r2, #0x14
	bl	__Func_92560
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #6
	bl	__Func_93040
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_92848
	mov	r0, #0x12
	ldr	r1, =0x103
	mov	r2, #0
	bl	__Func_937b8
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0x3c
	bl	__Func_937b8
	b	.Lc48
.Lc2e:
	ldr	r1, =0x103
	mov	r2, #0
	mov	r0, #0x12
	bl	__Func_937b8
	ldr	r0, =0xf66
	bl	__Func_92b94
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
.Lc48:
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_ba8

@ Talk: lines 0xf68, 0xf69, shown.
@ Which line is chosen by save bit 0x202.
.thumb_func_start OvlFunc_c60
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x202
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lc78
	ldr	r0, =0xf68
	bl	__Func_92b94
	b	.Lc7e
.Lc78:
	ldr	r0, =0xf69
	bl	__Func_92b94
.Lc7e:
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_c60

@ Talk: line 0x1c8d, shown.
@ Also turns to face the player.
@ Sets save bit 0x81f.
.thumb_func_start OvlFunc_c9c
	push	{lr}
	bl	__Func_916b0
	mov	r2, #0x14
	mov	r1, #0xa
	mov	r0, #0
	bl	__Func_92848
	ldr	r0, =0x1c8d
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0xa
	bl	__Func_92f84
	ldr	r0, =0x81f
	bl	__Func_79358
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_c9c

@ 33 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetMapTransition, Func_30f8, SetMapTransition
@   SetActiveMessageId, ShowMessageAndPause, FaceEntityInstant, ShowMessageAndWait
@   EndCutscene
@ message id 0x1c9a.
.thumb_func_start OvlFunc_cd0
	push	{lr}
	bl	__Func_916b0
	mov	r0, #0xc0
	mov	r1, #0xc0
	mov	r2, #0x80
	lsl	r1, #10
	lsl	r2, #9
	lsl	r0, #10
	bl	__Func_12330
	mov	r0, #0xa
	bl	__Func_30f8
	mov	r0, #1
	mov	r1, #1
	neg	r1, r1
	ldr	r2, =0xe666
	neg	r0, r0
	bl	__Func_12330
	ldr	r0, =0x1c9a
	bl	__Func_92b94
	mov	r0, #0x11
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r2, #0x14
	mov	r0, #0x11
	mov	r1, #0
	bl	__Func_9280c
	mov	r0, #0x11
	mov	r1, #0
	bl	__Func_92f84
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_cd0

@ Talk: line 0x1c9d, asked as a question.
@ Also turns to face the player, re-forms the followers.
@ Sets save bit 0x307.
.thumb_func_start OvlFunc_d2c
	push	{lr}
	bl	__Func_916b0
	mov	r1, #2
	mov	r0, #0x13
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r1, #0
	mov	r0, #0x13
	bl	__Func_9280c
	ldr	r0, =0x1c9d
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x13
	bl	__Func_93054
	ldr	r0, =0x307
	bl	__Func_79358
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_d2c

@ Map edit: 1 attribute copy.
@ Attributes only, so the artwork is already correct and only
@ collision or priority changes.
@ Records it with save bit 0x210.
.thumb_func_start OvlFunc_d70
	push	{lr}
	mov	r0, #0x84
	lsl	r0, #2
	sub	sp, #8
	bl	__Func_79358
	mov	r3, #0xa
	mov	r2, #0x54
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x28
	mov	r1, #0x54
	mov	r2, #7
	mov	r3, #4
	bl	__Func_10704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_d70

@ Map edit: 1 attribute copy.
@ Attributes only, so the artwork is already correct and only
@ collision or priority changes.
@ Clears save bit 0x210.
.thumb_func_start OvlFunc_d98
	push	{lr}
	mov	r0, #0x84
	lsl	r0, #2
	sub	sp, #8
	bl	__Func_79374
	mov	r3, #0xa
	mov	r2, #0x54
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x28
	mov	r1, #0x59
	mov	r2, #7
	mov	r3, #4
	bl	__Func_10704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_d98

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_dc0
	push	{lr}
	mov	r0, #0xbc
	bl	__Func_f9080
	ldr	r0, =.L7544
	mov	r1, #0x2d
	mov	r2, #0xb
	bl	__Func_10560
	mov	r2, #0xd2
	mov	r0, #0
	ldr	r1, =0x101
	lsl	r2, #1
	bl	__Func_9218c
	mov	r0, #0xb
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_dc0

@ 13 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, OvlFunc_fec, PlaySound, SetPendingMessageId
@ reads save bit 0x801.
.thumb_func_start OvlFunc_df0
	push	{lr}
	ldr	r0, =0x801
	bl	__Func_79338
	cmp	r0, #0
	bne	.Le02
	bl	OvlFunc_fec
	b	.Le0e
.Le02:
	mov	r0, #0x7b
	bl	__Func_f9080
	mov	r0, #1
	bl	__Func_91e9c
.Le0e:
	pop	{r0}
	bx	r0
.func_end OvlFunc_df0

@ 7 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, SetPendingMessageId
.thumb_func_start OvlFunc_e18
	push	{lr}
	mov	r0, #0x7b
	bl	__Func_f9080
	mov	r0, #3
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_e18

@ 7 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, SetPendingMessageId
.thumb_func_start OvlFunc_e2c
	push	{lr}
	mov	r0, #0x7b
	bl	__Func_f9080
	mov	r0, #4
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_e2c

@ 7 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, SetPendingMessageId
.thumb_func_start OvlFunc_e40
	push	{lr}
	mov	r0, #0x7b
	bl	__Func_f9080
	mov	r0, #2
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_e40

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_e54
	push	{lr}
	mov	r0, #0x9e
	bl	__Func_f9080
	ldr	r0, =.L755a
	mov	r1, #0x36
	mov	r2, #0x20
	bl	__Func_10560
	mov	r1, #0xcb
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x2d7
	bl	__Func_9218c
	mov	r0, #5
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_e54

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_e84
	push	{lr}
	mov	r0, #0x9e
	bl	__Func_f9080
	ldr	r0, =.L7570
	mov	r1, #0x2d
	mov	r2, #0x27
	bl	__Func_10560
	mov	r1, #0x83
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x325
	bl	__Func_9218c
	mov	r0, #6
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_e84

@ 59 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit x2, BeginCutscene, SetActiveMessageId, OpenMessageBoxForSlot
@   RunFieldAbilityPrompt, ShowMessageAndPause, ShowMessageAndWait, DialogueWait
@   ShowMessageAndWait, EndCutscene, PlaySound, PlayMapRectAnimation
@   WalkSlotTo, SetPendingMessageId
@ message id 0x11b6; reads save bits 0x815, 0x87a.
.thumb_func_start OvlFunc_eb4
	push	{lr}
	ldr	r0, =0x815
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lf20
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	bne	.Lf20
	bl	__Func_916b0
	ldr	r0, =0x11b6
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	bne	.Lefc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x3c
	bl	__Func_93040
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
	b	.Lf1a
.Lefc:
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #2
	mov	r0, #0x28
	strh	r3, [r2]
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92f84
.Lf1a:
	bl	__Func_91750
	b	.Lf44
.Lf20:
	mov	r0, #0x9e
	bl	__Func_f9080
	ldr	r0, =.L755a
	mov	r1, #0x32
	mov	r2, #0x2c
	bl	__Func_10560
	mov	r1, #0xaa
	mov	r2, #0xde
	mov	r0, #0
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_9218c
	mov	r0, #7
	bl	__Func_91e9c
.Lf44:
	pop	{r0}
	bx	r0
.func_end OvlFunc_eb4

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_f5c
	push	{lr}
	mov	r0, #0x9e
	bl	__Func_f9080
	ldr	r0, =.L7570
	mov	r1, #0x31
	mov	r2, #0x45
	bl	__Func_10560
	mov	r1, #0xa3
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x466
	bl	__Func_9218c
	mov	r0, #8
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_f5c

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_f8c
	push	{lr}
	mov	r0, #0x9e
	bl	__Func_f9080
	ldr	r0, =.L7586
	mov	r1, #0x34
	mov	r2, #0x4c
	bl	__Func_10560
	mov	r1, #0xbb
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x4d6
	bl	__Func_9218c
	mov	r0, #9
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_f8c

@ 15 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, PlayMapRectAnimation, WalkSlotTo, SetPendingMessageId
.thumb_func_start OvlFunc_fbc
	push	{lr}
	mov	r0, #0x9e
	bl	__Func_f9080
	ldr	r0, =.L755a
	mov	r1, #0x23
	mov	r2, #0x4a
	bl	__Func_10560
	mov	r0, #0
	mov	r1, #0x66
	ldr	r2, =0x4b6
	bl	__Func_9218c
	mov	r0, #0xa
	bl	__Func_91e9c
	pop	{r0}
	bx	r0
.func_end OvlFunc_fbc

@ Cutscene: roughly 88 instructions of straight-line script --
@ 0 turns, 2 animation changes, 2 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0xf39.
.thumb_func_start OvlFunc_fec
	push	{r5, r6, lr}
	mov	r0, #0
	bl	__Func_92054
	mov	r6, r0
	mov	r0, #5
	bl	__Func_92054
	mov	r5, r0
	bl	__Func_916b0
	ldr	r3, [r6, #8]
	str	r3, [r5, #8]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #0xc]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #0x10]
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r5, #0x38]
	str	r3, [r5, #0x3c]
	str	r3, [r5, #0x40]
	mov	r3, #0
	str	r3, [r5, #0x24]
	str	r3, [r5, #0x28]
	str	r3, [r5, #0x2c]
	ldr	r3, [r6, #0xc]
	mov	r0, #1
	str	r3, [r5, #0x14]
	bl	__Func_30f8
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r0, #5
	mov	r1, #0x6e
	ldr	r2, =0x11b
	bl	__Func_921c4
	mov	r2, #2
	mov	r0, #0
	mov	r1, #5
	bl	__Func_92848
	ldr	r0, =0xf39
	bl	__Func_92b94
	ldr	r2, [r6, #8]
	ldr	r3, [r5, #8]
	cmp	r2, r3
	bge	.L1066
	ldr	r0, =0xa005
	mov	r1, #0
	mov	r2, #2
	bl	__Func_93040
	b	.L1070
.L1066:
	ldr	r0, =0x8005
	mov	r1, #0
	mov	r2, #2
	bl	__Func_93040
.L1070:
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #2
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0
	bl	__Func_92054
	cmp	r0, #0
	beq	.L109e
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #5
	bl	__Func_92128
.L109e:
	mov	r0, #5
	bl	__Func_923c4
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r0, #0
	mov	r1, #0x6e
	ldr	r2, =0x12f
	bl	__Func_921c4
	bl	__Func_91750
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_fec

@ 97 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, BeginCutscene, SetSlotEntitySpeed, SetSlotAnimation
@   DialogueWait, SetActiveMessageId, ShowMessageAndPause x2, GetSlotEntityChecked
@   DialogueWait, UpdateMapView, DialogueWait, RunTextBoxModal
@   DialogueWait x2, UpdateMapView
@   ... and 3 more
@ message id 0xf4d; reads save bit 0x808.
.thumb_func_start OvlFunc_10d8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r0, =0x808
	sub	sp, #0xc
	bl	__Func_79338
	cmp	r0, #0
	bne	.L11b2
	ldr	r3, =iwram_1e70
	ldr	r3, [r3]
	mov	r8, r3
	bl	__Func_916b0
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	mov	r0, #0
	lsl	r1, #9
	bl	__Func_92064
	mov	r1, #1
	mov	r0, #0
	bl	__Func_924d4
	mov	r0, #2
	bl	__Func_9163c
	ldr	r0, =0xf4d
	bl	__Func_92b94
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #2
	bl	__Func_93040
	mov	r2, #2
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0
	bl	__Func_92054
	ldr	r3, [r0, #8]
	mov	r7, sp
	str	r3, [r7]
	ldr	r3, [r0, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r0, #0x10]
	mov	r2, r8
	ldr	r2, [r2]
	str	r3, [r7, #8]
	mov	r3, r8
	str	r7, [r3]
	mov	r10, r2
	mov	r6, #0
	mov	r5, r7
.L114e:
	ldr	r3, [r5, #8]
	mov	r2, #0x80
	lsl	r2, #10
	add	r3, r2
	str	r3, [r5, #8]
	mov	r0, #1
	add	r6, #1
	bl	__Func_9163c
	bl	__Func_fe9c
	cmp	r6, #0x28
	bne	.L114e
	mov	r0, #0x3c
	bl	__Func_9163c
	ldr	r0, =0xf4f
	mov	r1, #1
	bl	__Func_1776c
	mov	r0, #6
	bl	__Func_9163c
	mov	r6, #0
	mov	r5, r7
.L1180:
	ldr	r3, [r5, #8]
	ldr	r2, =0xfffe0000
	add	r3, r2
	str	r3, [r5, #8]
	mov	r0, #1
	add	r6, #1
	bl	__Func_9163c
	bl	__Func_fe9c
	cmp	r6, #0x28
	bne	.L1180
	mov	r3, r10
	mov	r2, r8
	str	r3, [r2]
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0x46
	ldr	r2, =0x2e5
	bl	__Func_921c4
	bl	__Func_91750
.L11b2:
	add	sp, #0xc
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_10d8

@ 37 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit, BeginCutscene, SetSlotEntitySpeed, SetActiveMessageId
@   ShowMessageAndPause x2, RunTextBoxModal, DialogueWait, WalkSlotToAndWait
@   EndCutscene
@ message id 0xf4d; reads save bit 0x808.
.thumb_func_start OvlFunc_11d8
	push	{r5, lr}
	ldr	r0, =0x808
	bl	__Func_79338
	cmp	r0, #0
	bne	.L1230
	bl	__Func_916b0
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #9
	lsl	r2, #8
	mov	r0, #0
	bl	__Func_92064
	ldr	r5, =0xf4d
	mov	r0, r5
	bl	__Func_92b94
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #2
	bl	__Func_93040
	add	r5, #2
	mov	r2, #2
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #1
	mov	r0, r5
	bl	__Func_1776c
	mov	r0, #6
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0x45
	ldr	r2, =0x366
	bl	__Func_921c4
	bl	__Func_91750
.L1230:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_11d8

@ 24 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, CopyMapRectAttributes, OvlFunc_32b0, SetSaveBit
@   EndCutscene
@ sets 0x204.
.thumb_func_start OvlFunc_1244
	push	{lr}
	sub	sp, #8
	bl	__Func_916b0
	mov	r3, #0x14
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x31
	mov	r1, #0x35
	mov	r2, #8
	mov	r3, #4
	bl	__Func_10704
	mov	r1, #0xa
	mov	r2, #0xb
	mov	r3, #1
	mov	r0, #0
	bl	OvlFunc_32b0
	mov	r0, #0x81
	lsl	r0, #2
	bl	__Func_79358
	bl	__Func_91750
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_1244

@ 24 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, OvlFunc_3380, ClearSaveBit, CopyMapRectAttributes
@   EndCutscene
@ clears 0x204.
.thumb_func_start OvlFunc_1280
	push	{lr}
	sub	sp, #8
	bl	__Func_916b0
	mov	r1, #0xd
	mov	r2, #0xa
	mov	r3, #1
	mov	r0, #0
	bl	OvlFunc_3380
	mov	r0, #0x81
	lsl	r0, #2
	bl	__Func_79374
	mov	r3, #0x14
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x31
	mov	r1, #0x2e
	mov	r2, #8
	mov	r3, #4
	bl	__Func_10704
	bl	__Func_91750
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_1280

@ Cutscene: roughly 146 instructions of straight-line script --
@ 5 turns, 2 animation changes, 1 dialogue line, 4 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xfce.
@ Sets save bit 0x823.
.thumb_func_start OvlFunc_12bc
	push	{r5, r6, lr}
	mov	r0, #0x16
	bl	__Func_92054
	mov	r5, r0
	bl	__Func_916b0
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #10
	lsl	r2, #10
	bl	__Func_92064
	mov	r0, #0
	mov	r1, #5
	mov	r2, #0
	bl	__Func_92560
	add	r5, #0x5a
	mov	r0, #0
	mov	r1, #0xd7
	ldr	r2, =0x193
	bl	__Func_9218c
	ldrb	r3, [r5]
	mov	r6, #1
	orr	r3, r6
	mov	r1, #0xa6
	strb	r3, [r5]
	mov	r0, #0x16
	lsl	r1, #16
	ldr	r2, =0x1770000
	bl	__Func_923e4
	mov	r1, #0x80
	mov	r0, #0x16
	lsl	r1, #6
	mov	r2, #0x14
	bl	__Func_92adc
	ldrb	r3, [r5]
	mov	r1, #0xa0
	eor	r3, r6
	mov	r2, #0xa0
	strb	r3, [r5]
	mov	r0, #0x16
	lsl	r1, #10
	lsl	r2, #10
	bl	__Func_92064
	mov	r0, #0x16
	mov	r1, #4
	mov	r2, #0
	bl	__Func_92560
	ldr	r2, =0x18b
	mov	r0, #0x16
	mov	r1, #0xca
	bl	__Func_921c4
	mov	r1, #1
	mov	r0, #0
	bl	__Func_924d4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xb0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0x16
	lsl	r1, #6
	mov	r2, #0x18
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, #2
	mov	r0, #0
	bl	__Func_9259c
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r2, #0x80
	lsl	r2, #9
	mov	r0, #0x16
	lsl	r1, #9
	bl	__Func_92064
	ldr	r1, =.L759c
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0
	mov	r0, #0x16
	ldr	r1, =0x103
	bl	__Func_937b8
	ldr	r1, =.L75ec
	mov	r0, #0x16
	bl	__Func_9207c
	mov	r0, #0
	bl	__Func_920e8
	mov	r1, #0x80
	mov	r2, #0xed
	mov	r0, #0
	lsl	r1, #1
	lsl	r2, #1
	bl	__Func_921c4
	mov	r1, #0xc0
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #0
	bl	__Func_92adc
	mov	r0, #0x16
	bl	__Func_920e8
	mov	r1, #0x80
	mov	r2, #0xe4
	lsl	r2, #1
	mov	r0, #0x16
	lsl	r1, #1
	bl	__Func_921c4
	mov	r0, #0
	mov	r1, #1
	bl	__Func_924d4
	mov	r1, #0x80
	mov	r2, #0x14
	mov	r0, #0x16
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0x16
	bl	__Func_9259c
	mov	r0, #0x14
	bl	__Func_9163c
	ldr	r0, =0xfce
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x16
	bl	__Func_93054
	mov	r0, #0x16
	bl	__Func_92054
	ldr	r3, =OvlFunc_572c
	ldr	r1, =.L6248
	str	r3, [r0, #0x6c]
	mov	r0, #0x16
	bl	__Func_9207c
	ldr	r0, =0x823
	bl	__Func_79358
	bl	__Func_91750
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_12bc

@ 20 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, OvlFunc_1490
@ reads save bit 0x823.
.thumb_func_start OvlFunc_1454
	push	{r5, lr}
	mov	r0, #0x16
	bl	__Func_92054
	mov	r5, r0
	ldr	r0, =0x823
	bl	__Func_79338
	cmp	r0, #0
	beq	.L147c
	mov	r3, r5
	add	r3, #0x64
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #1
	bne	.L147c
	ldr	r0, =.L763c
	ldr	r1, =.L76cc
	bl	OvlFunc_1490
.L147c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_1454

@ Cutscene: roughly 70 instructions of straight-line script --
@ 0 turns, 0 animation changes, 1 dialogue line, 2 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xfce.
.thumb_func_start OvlFunc_1490
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r5, r0
	mov	r0, #0x16
	mov	r8, r1
	bl	__Func_92054
	mov	r6, r0
	bl	__Func_916b0
	mov	r0, #0x16
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #0x80
	mov	r2, #0x14
	mov	r0, #0x16
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #0
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #0x81
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, r5
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0
	mov	r0, #0x16
	ldr	r1, =0x103
	bl	__Func_937b8
	mov	r1, r8
	mov	r0, #0x16
	bl	__Func_920fc
	mov	r0, #0
	bl	__Func_920e8
	mov	r5, #0x80
	mov	r0, #0x14
	bl	__Func_9163c
	lsl	r5, #9
	mov	r1, #2
	mov	r0, #0x16
	bl	__Func_925cc
	str	r5, [r6, #0x18]
	str	r5, [r6, #0x1c]
	mov	r0, #0
	bl	__Func_92054
	str	r5, [r0, #0x18]
	str	r5, [r0, #0x1c]
	ldr	r0, =0xfce
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #0x16
	bl	__Func_93054
	mov	r0, #0x16
	bl	__Func_92054
	ldr	r3, =OvlFunc_572c
	ldr	r1, =.L6248
	str	r3, [r0, #0x6c]
	mov	r0, #0x16
	bl	__Func_9207c
	bl	__Func_91750
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_1490

@ 20 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, OvlFunc_1490
@ reads save bit 0x823.
.thumb_func_start OvlFunc_1554
	push	{r5, lr}
	mov	r0, #0x16
	bl	__Func_92054
	mov	r5, r0
	ldr	r0, =0x823
	bl	__Func_79338
	cmp	r0, #0
	beq	.L157c
	mov	r3, r5
	add	r3, #0x64
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #2
	bne	.L157c
	ldr	r0, =.L7748
	ldr	r1, =.L77c4
	bl	OvlFunc_1490
.L157c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_1554

@ 26 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, OvlFunc_1490 x2
@ reads save bit 0x823.
.thumb_func_start OvlFunc_1590
	push	{r5, lr}
	mov	r0, #0x16
	bl	__Func_92054
	mov	r5, r0
	ldr	r0, =0x823
	bl	__Func_79338
	cmp	r0, #0
	beq	.L15c6
	mov	r3, r5
	add	r3, #0x64
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	cmp	r0, #1
	bne	.L15ba
	ldr	r0, =.L7748
	ldr	r1, =.L76cc
	bl	OvlFunc_1490
	b	.L15c6
.L15ba:
	cmp	r0, #2
	bne	.L15c6
	ldr	r0, =.L7748
	ldr	r1, =.L77c4
	bl	OvlFunc_1490
.L15c6:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_1590

@ Cutscene: roughly 1996 instructions of straight-line script --
@ 74 turns, 56 animation changes, 46 dialogue lines, 90 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xf03, 0xf0a, 0xf0e, 0xf27.
@ Sets save bit 0x202.
.thumb_func_start OvlFunc_15dc
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x24
	bl	__Func_916b0
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r1, r1
	neg	r2, r2
	mov	r3, #0
	neg	r0, r0
	bl	__Func_933f8
	bl	__Func_93554
	mov	r1, #0
	mov	r6, r0
	mov	r8, r1
	mov	r3, r6
	add	r3, #0x55
	mov	r2, r8
	mov	r1, #0xa0
	lsl	r1, #16
	strb	r2, [r3]
	ldr	r0, =0x17f0000
	ldr	r2, =0x36d0000
	mov	r3, #0
	bl	__Func_933f8
	mov	r0, #1
	bl	__Func_9163c
	bl	__Func_fe9c
	mov	r3, #0x14
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x31
	mov	r1, #0x29
	mov	r2, #7
	mov	r3, #3
	bl	__Func_10704
	mov	r3, #2
	str	r3, [sp]
	mov	r5, #1
	mov	r9, r3
	mov	r0, #2
	mov	r1, #0x66
	mov	r2, #0x54
	mov	r3, #0x29
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #1
	mov	r1, #0x66
	mov	r2, #0x53
	mov	r3, #0x29
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r1, #0x67
	mov	r2, #0x52
	mov	r3, #0x2a
	mov	r0, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #0
	bl	__Func_92054
	mov	r6, r0
	mov	r1, r6
	add	r1, #0x55
	str	r1, [sp, #0x1c]
	ldrb	r2, [r1]
	mov	r3, r8
	str	r2, [sp, #0x20]
	mov	r0, #0
	strb	r3, [r1]
	ldr	r2, =0x2b20000
	ldr	r1, =0x1970000
	bl	__Func_923e4
	mov	r1, #0xc4
	mov	r2, #0xe0
	mov	r0, #0x15
	lsl	r1, #17
	lsl	r2, #18
	bl	__Func_923e4
	mov	r1, #0x95
	mov	r2, #0xb8
	mov	r0, #1
	lsl	r1, #17
	lsl	r2, #18
	bl	__Func_923e4
	mov	r1, #0x95
	mov	r2, #0xbe
	mov	r0, #5
	lsl	r1, #17
	lsl	r2, #18
	bl	__Func_923e4
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	ldr	r1, =.L6590
	mov	r0, #0
	mov	r10, r1
	bl	__Func_9207c
	mov	r0, #0x17
	mov	r1, #2
	mov	r2, #1
	bl	OvlFunc_345c
	ldr	r2, =iwram_1ebc
	mov	r1, #0xe0
	mov	r11, r2
	ldr	r2, [r2]
	lsl	r1, #1
	add	r3, r2, r1
	mov	r1, r8
	str	r1, [r3]
	mov	r3, #0xe4
	lsl	r3, #1
	add	r2, r3
	mov	r3, #0x20
	str	r3, [r2]
	bl	__Func_91dc8
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #8
	lsl	r2, #7
	bl	__Func_92064
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #7
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92064
	ldr	r1, =.L6614
	mov	r0, #5
	bl	__Func_9207c
	ldr	r1, =.L65cc
	mov	r0, #1
	bl	__Func_9207c
	mov	r7, #0x80
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	lsl	r7, #9
	mov	r1, #0xb0
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #8
	str	r7, [r6, #0x18]
	str	r7, [r6, #0x1c]
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xca
	lsl	r1, #1
	ldr	r2, =0x34b
	mov	r0, #0
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r2, #0x1e
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #1
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #2
	bl	__Func_92950
	mov	r0, #0x17
	mov	r1, #2
	bl	__Func_92950
	mov	r0, #0x17
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xc3
	mov	r2, #0xd0
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #0x17
	bl	__Func_92158
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc9
	mov	r2, #0xcf
	lsl	r2, #2
	lsl	r1, #1
	mov	r0, #0x17
	bl	__Func_92158
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92950
	mov	r0, #0x17
	mov	r1, #0
	bl	__Func_92950
	mov	r1, #0xc3
	ldr	r2, =0x34a0000
	mov	r0, #0x17
	lsl	r1, #17
	bl	__Func_923e4
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	mov	r1, r10
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0xc8
	bl	__Func_9163c
	mov	r1, r9
	str	r1, [sp]
	mov	r3, #0x29
	mov	r2, #0x54
	mov	r0, #7
	mov	r1, #0x66
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	mov	r1, #1
	mov	r0, #0
	str	r7, [r6, #0x18]
	str	r7, [r6, #0x1c]
	bl	__Func_924d4
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	ldr	r1, =0x179
	ldr	r2, =0x34b
	mov	r0, #0
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r2, #0x14
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92adc
	mov	r0, #0
	mov	r1, #2
	bl	__Func_92950
	mov	r0, #0x17
	mov	r1, #2
	bl	__Func_92950
	mov	r0, #0x17
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xc3
	mov	r2, #0xd0
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #0x17
	bl	__Func_92158
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0xcf
	lsl	r2, #2
	ldr	r1, =0x179
	mov	r0, #0x17
	bl	__Func_92158
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92950
	mov	r0, #0x17
	mov	r1, #0
	bl	__Func_92950
	mov	r2, #0
	mov	r0, #0x17
	mov	r1, #0
	bl	__Func_923e4
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	mov	r1, r10
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0xc8
	bl	__Func_9163c
	mov	r3, #0x29
	mov	r2, #0x53
	mov	r0, #6
	mov	r1, #0x66
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	mov	r1, #1
	mov	r0, #0
	str	r7, [r6, #0x18]
	str	r7, [r6, #0x1c]
	bl	__Func_924d4
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0xb4
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x357
	bl	__Func_921c4
	mov	r1, #0xb0
	mov	r0, #0x15
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r2, #0x14
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0
	mov	r1, #2
	bl	__Func_92950
	mov	r0, #0x18
	mov	r1, #2
	bl	__Func_92950
	mov	r0, #0x18
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xc3
	mov	r2, #0xd0
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #0x18
	bl	__Func_92158
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xb4
	ldr	r2, =0x34a
	lsl	r1, #1
	mov	r0, #0x18
	bl	__Func_92158
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92950
	mov	r0, #0x18
	mov	r1, #0
	bl	__Func_92950
	mov	r2, #0
	mov	r0, #0x18
	mov	r1, #0
	bl	__Func_923e4
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	mov	r1, r10
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0xc8
	bl	__Func_9163c
	b	.L1a50

	.pool_aligned

.L1a50:
	mov	r3, #0x2a
	mov	r2, #0x52
	mov	r0, #5
	mov	r1, #0x67
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r1, #1
	mov	r0, #0
	bl	__Func_9207c
	ldr	r0, =0xf03
	str	r7, [r6, #0x18]
	str	r7, [r6, #0x1c]
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #2
	mov	r2, #0x14
	bl	__Func_92560
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #5
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r3, #0
	mov	r0, #0x15
	mov	r1, #5
	mov	r2, #6
	bl	OvlFunc_32b0
	mov	r0, #0x15
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r2, #0xd0
	ldr	r1, =0x18d
	lsl	r2, #2
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x3c
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xba
	mov	r2, #0xd0
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r2, #0x1e
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	bne	.L1ba2
	mov	r3, r11
	ldr	r2, [r3]
	mov	r1, #0xec
	lsl	r1, #1
	add	r2, r1
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L1ba2:
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0
	mov	r2, #0x14
	mov	r0, #0x15
	bl	__Func_93040
	ldr	r0, =0xf0a
	bl	__Func_92b94
	mov	r1, #0xc1
	lsl	r1, #1
	ldr	r2, =0x349
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xd0
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #1
	bne	.L1c26
	mov	r3, r11
	ldr	r2, [r3]
	mov	r1, #0xec
	lsl	r1, #1
	add	r2, r1
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L1c26:
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xd0
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	ldr	r0, =0xf0e
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xc1
	ldr	r2, =0x339
	lsl	r1, #1
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x3c
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xba
	mov	r2, #0xd0
	mov	r0, #0x15
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_933d4
	mov	r1, #0xa0
	mov	r2, #0xd7
	mov	r3, #1
	ldr	r0, =0x1790000
	lsl	r1, #16
	lsl	r2, #18
	bl	__Func_933f8
	mov	r2, #0x80
	mov	r1, r7
	mov	r0, #5
	lsl	r2, #8
	bl	__Func_92064
	mov	r2, #0x80
	mov	r1, r7
	mov	r0, #1
	lsl	r2, #8
	bl	__Func_92064
	mov	r2, #0xe2
	mov	r0, #1
	ldr	r1, =0x171
	lsl	r2, #2
	bl	__Func_9218c
	mov	r1, #0xc4
	mov	r2, #0xe2
	lsl	r2, #2
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_921c4
	mov	r0, #1
	mov	r1, #1
	bl	__Func_924d4
	mov	r3, #0
	mov	r0, #5
	mov	r1, #0xa
	mov	r2, #0xb
	bl	OvlFunc_32b0
	mov	r1, #0xa0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0xa
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #5
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r0, #5
	mov	r1, #4
	mov	r2, #0
	bl	__Func_92560
	mov	r1, #0xc4
	mov	r0, #5
	lsl	r1, #1
	ldr	r2, =0x34b
	bl	__Func_921c4
	mov	r1, #0x90
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r3, #0
	mov	r0, #1
	mov	r1, #0xa
	mov	r2, #0xb
	b	.L1e8c

	.pool_aligned

.L1e8c:
	bl	OvlFunc_32b0
	mov	r0, #5
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r0, #1
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xc4
	lsl	r1, #1
	ldr	r2, =0x34b
	mov	r0, #1
	bl	__Func_9218c
	mov	r0, #5
	bl	__Func_92054
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r7, #0xfe
	mov	r3, r7
	and	r3, r2
	mov	r1, #0xcc
	strb	r3, [r0]
	lsl	r1, #1
	ldr	r2, =0x34b
	mov	r0, #5
	bl	__Func_921c4
	mov	r0, #1
	bl	__Func_9163c
	mov	r0, #5
	bl	__Func_92054
	add	r0, #0x5a
	ldrb	r3, [r0]
	mov	r2, #1
	orr	r3, r2
	mov	r1, #0x80
	strb	r3, [r0]
	mov	r2, #0
	lsl	r1, #8
	mov	r0, #5
	bl	__Func_92adc
	mov	r0, #1
	bl	__Func_923c4
	mov	r0, #1
	mov	r1, #1
	bl	__Func_924d4
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #4
	mov	r2, #0x1e
	bl	__Func_92560
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0x1e
	mov	r0, #0
	mov	r1, #2
	bl	__Func_92560
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0
	bl	__Func_93874
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r2, #0x28
	mov	r0, #0x15
	lsl	r1, #6
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r0, #0x15
	ldr	r1, =0x101
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x64
	bl	__Func_9163c
	mov	r2, #0x1e
	mov	r0, #5
	mov	r1, #1
	bl	__Func_92848
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0x15
	ldr	r1, =0x105
	mov	r2, #0x3c
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_9259c
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	ldr	r0, =0x9999
	ldr	r1, =0x1333
	bl	__Func_933d4
	mov	r1, #0xa0
	mov	r3, #1
	ldr	r0, =0x1750000
	lsl	r1, #16
	ldr	r2, =0x3450000
	bl	__Func_933f8
	mov	r1, #0xb6
	mov	r2, #0xcc
	mov	r0, #0x15
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_93040
	mov	r2, #0x1e
	mov	r0, #5
	mov	r1, #1
	bl	__Func_92848
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x1e
	bl	__Func_93040
	mov	r2, #0x3c
	mov	r0, #0
	ldr	r1, =0x105
	bl	__Func_937b8
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	mov	r5, #0
	bl	__Func_91c7c
	cmp	r0, #1
	bne	.L21b0
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L21b0:
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r1, #0
	mov	r2, #0x14
	mov	r0, #0x15
	bl	__Func_93040
	ldr	r0, =0xf27
	bl	__Func_92b94
	mov	r2, #0
	mov	r0, #0x15
	ldr	r1, =0x103
	bl	__Func_937b8
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r2, #0
	mov	r0, #0x15
	mov	r1, #4
	bl	__Func_92560
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_925cc
	mov	r1, #7
	mov	r0, #0x15
	bl	__Func_924d4
	mov	r0, #5
	bl	__Func_9163c
	mov	r3, #0xe
	mov	r1, #1
	mov	r0, #0xa
	mov	r4, #4
	str	r1, [sp, #4]
	str	r0, [sp, #8]
	str	r3, [sp, #0xc]
	str	r3, [sp, #0x14]
	mov	r2, #2
	mov	r1, #0xe
	mov	r3, #0x18
	mov	r0, #0x15
	str	r2, [sp]
	str	r5, [sp, #0x18]
	str	r4, [sp, #0x10]
	bl	__Func_931ec
	mov	r0, #0x15
	bl	__Func_92054
	mov	r6, r0
	ldr	r3, [r6, #0x50]
	mov	r1, r6
	add	r1, #0x5a
	ldrb	r2, [r1]
	add	r3, #0x26
	strb	r5, [r3]
	mov	r3, r7
	and	r3, r2
	strb	r3, [r1]
	mov	r2, #0xc0
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_92064
	mov	r1, #0xb6
	mov	r0, #0x15
	lsl	r1, #1
	ldr	r2, =0x32f
	bl	__Func_921c4
	mov	r0, #4
	bl	__Func_9163c
	mov	r5, #0
.L2266:
	ldr	r3, [r6, #0x10]
	mov	r1, #0xc0
	lsl	r1, #9
	add	r3, r1
	str	r3, [r6, #0x10]
	ldr	r2, =0xffffe667
	ldr	r3, [r6, #0x1c]
	add	r3, r2
	str	r3, [r6, #0x1c]
	mov	r0, #1
	add	r5, #1
	bl	__Func_9163c
	b	.L22bc

	.pool_aligned

.L22bc:
	cmp	r5, #4
	bne	.L2266
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #1
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_92064
	mov	r0, #1
	mov	r1, #6
	mov	r2, #0
	bl	__Func_92560
	mov	r1, #0xbb
	ldr	r2, =0x33b
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_921c4
	mov	r0, #5
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0xb0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #5
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #0x80
	mov	r2, #0xa
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #1
	mov	r1, #0xd
	bl	__Func_924d4
	mov	r0, #1
	mov	r1, #2
	mov	r2, #5
	bl	__Func_92560
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #11
	lsl	r2, #9
	bl	__Func_12330
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r2, #0x53
	mov	r3, #0x29
	mov	r1, #0x66
	mov	r0, #1
	bl	__Func_10424
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r1, #0xd0
	mov	r2, #0xa
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #3
	bl	__Func_9259c
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_12330
	bl	__Func_12350
	mov	r1, #0x81
	mov	r2, #0x50
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #8
	bl	__Func_924d4
	mov	r3, #0x80
	lsl	r3, #8
	mov	r1, #0xb6
	str	r3, [r6, #0x1c]
	mov	r0, #0x15
	lsl	r1, #17
	ldr	r2, =0x32b0000
	bl	__Func_923e4
	mov	r5, #0
.L23ae:
	ldr	r3, [r6, #0x1c]
	ldr	r1, =0x1999
	add	r3, r1
	str	r3, [r6, #0x1c]
	mov	r0, #1
	add	r5, #1
	bl	__Func_9163c
	cmp	r5, #5
	bne	.L23ae
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #1
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, #0xa0
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	ldr	r0, =0x4ccc
	ldr	r1, =0x999
	bl	__Func_933d4
	mov	r0, #0xba
	mov	r1, #0xa0
	mov	r3, #1
	lsl	r0, #17
	lsl	r1, #16
	ldr	r2, =0x35b0000
	bl	__Func_933f8
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #0x15
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_92064
	mov	r0, #0x15
	mov	r1, #6
	mov	r2, #0
	bl	__Func_92560
	ldr	r1, =0x167
	ldr	r2, =0x343
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x1e
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r0, #0x15
	bl	__Func_92054
	mov	r6, r0
	mov	r1, r6
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r5, #0xfe
	mov	r3, r5
	and	r3, r2
	strb	r3, [r1]
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x50
	bl	__Func_93040
	mov	r0, #0x15
	ldr	r1, =0x101
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r2, #0x3c
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_925cc
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0x15
	bl	__Func_93874
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #0xa0
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0x81
	mov	r2, #0x50
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #1
	bl	__Func_c528
	mov	r2, #0
	mov	r0, #1
	mov	r1, #6
	bl	__Func_92560
	mov	r0, #1
	mov	r1, #1
	bl	__Func_924d4
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #11
	lsl	r2, #10
	mov	r0, #1
	bl	__Func_92064
	mov	r0, #1
	bl	__Func_92054
	mov	r6, r0
	mov	r2, r6
	add	r2, #0x5a
	ldrb	r3, [r2]
	and	r5, r3
	strb	r5, [r2]
	mov	r0, #1
	ldr	r2, =0x33b
	ldr	r1, =0x193
	bl	__Func_92128
	mov	r1, #0x81
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_93874
	mov	r1, #0xc0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r1, #0
	mov	r2, #1
	mov	r0, #5
	bl	__Func_93040
	mov	r0, #1
	bl	__Func_923c4
	mov	r1, #0xa0
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #1
	mov	r1, #0xd
	bl	__Func_924d4
	mov	r2, #5
	mov	r1, #2
	mov	r0, #1
	bl	__Func_92560
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r3, #2
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #0x29
	mov	r0, #2
	mov	r1, #0x66
	mov	r2, #0x54
	bl	__Func_10424
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	mov	r0, #0
	lsl	r1, #11
	bl	__Func_12330
	mov	r0, #1
	mov	r1, #3
	bl	__Func_925cc
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_12330
	bl	__Func_12350
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0x1e
	bl	__Func_937b8
	mov	r0, #5
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xcc
	ldr	r2, =0x357
	lsl	r1, #1
	mov	r0, #5
	bl	__Func_921c4
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #1
	mov	r1, #2
	bl	__Func_925cc
	mov	r2, #0x3c
	mov	r0, #0x15
	ldr	r1, =0x105
	bl	__Func_937b8
	mov	r0, #5
	mov	r1, #3
	bl	__Func_9259c
	mov	r1, #3
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xb0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x3c
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x50
	bl	__Func_92adc
	mov	r0, #0x15
	ldr	r1, =0x105
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #0
	b	.L2714

	.pool_aligned

.L2714:
	mov	r2, #0x3c
	bl	__Func_93040
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r0, #0
	ldr	r1, =0x101
	mov	r2, #0
	bl	__Func_937b8
	mov	r0, #5
	ldr	r1, =0x101
	mov	r2, #0
	bl	__Func_937b8
	mov	r2, #0x3c
	mov	r0, #1
	ldr	r1, =0x101
	bl	__Func_937b8
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r2, #0x46
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x3c
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r2, #0x1e
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r2, #0x1e
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92adc
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x15
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r2, #0x3c
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_925cc
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	lsl	r1, #9
	mov	r0, #1
	bl	__Func_92064
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #1
	mov	r1, #4
	mov	r2, #0
	bl	__Func_92560
	mov	r1, #0xc7
	mov	r2, #0xcf
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #1
	bl	__Func_921c4
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r2, #0x3c
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #1
	bl	__Func_92054
	mov	r6, r0
	mov	r2, r6
	add	r2, #0x5a
	ldrb	r3, [r2]
	mov	r5, #1
	orr	r3, r5
	strb	r3, [r2]
	mov	r0, #5
	bl	__Func_92054
	mov	r6, r0
	mov	r2, r6
	add	r2, #0x5a
	ldrb	r3, [r2]
	orr	r3, r5
	strb	r3, [r2]
	mov	r0, #0
	bl	__Func_92054
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r6, r0
	lsl	r1, #9
	mov	r0, #1
	lsl	r2, #8
	bl	__Func_92064
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92adc
	mov	r2, #0xa
	ldrsh	r1, [r6, r2]
	mov	r0, #5
	mov	r3, #0x12
	ldrsh	r2, [r6, r3]
	add	r1, #0x10
	bl	__Func_9218c
	mov	r2, #0xa
	ldrsh	r1, [r6, r2]
	mov	r3, #0x12
	ldrsh	r2, [r6, r3]
	add	r1, #0x10
	sub	r2, #0x10
	mov	r0, #1
	bl	__Func_921c4
	mov	r0, #1
	bl	__Func_923c4
	mov	r1, #0xa0
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #5
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0xa
	ldrsh	r1, [r6, r2]
	mov	r0, #5
	mov	r3, #0x12
	ldrsh	r2, [r6, r3]
	bl	__Func_921c4
	mov	r2, #0
	mov	r0, #5
	mov	r1, #0
	bl	__Func_923e4
	mov	r2, #0xa
	ldrsh	r1, [r6, r2]
	mov	r0, #1
	mov	r3, #0x12
	ldrsh	r2, [r6, r3]
	bl	__Func_921c4
	mov	r2, #0
	mov	r0, #1
	mov	r1, #0
	bl	__Func_923e4
	mov	r0, #1
	mov	r1, #5
	bl	__Func_917f4
	mov	r1, #0xa0
	ldr	r0, =0x1790000
	lsl	r1, #16
	ldr	r2, =0x3770000
	mov	r3, #1
	bl	__Func_933f8
	mov	r3, #0
	mov	r0, #0
	mov	r1, #0xd
	mov	r2, #0xa
	bl	OvlFunc_3380
	mov	r1, #0xbc
	mov	r2, #0xe4
	mov	r0, #0
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xc0
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #0
	bl	__Func_92adc
	mov	r0, #0x15
	bl	__Func_92054
	add	r0, #0x5a
	ldrb	r3, [r0]
	orr	r5, r3
	strb	r5, [r0]
	mov	r3, #0
	mov	r0, #0x15
	mov	r1, #6
	mov	r2, #5
	bl	OvlFunc_3380
	mov	r0, #0x15
	ldr	r1, =0x175
	ldr	r2, =0x377
	bl	__Func_921c4
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #1
	mov	r0, #0
	bl	__Func_9335c
	bl	__Func_93530
	mov	r0, #0x64
	bl	__Func_9163c
	ldr	r0, =0x202
	bl	__Func_79358
	ldr	r0, =0x12f
	bl	__Func_79374
	add	r1, sp, #0x20
	ldr	r2, [sp, #0x1c]
	ldrb	r1, [r1]
	strb	r1, [r2]
	bl	__Func_91750
	add	sp, #0x24
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_15dc

@ Cutscene: roughly 229 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x109, 0x204, 0x210, 0x308.
@ Sets save bit 0x308.
.thumb_func_start OvlFunc_2a54
	push	{r5, lr}
	ldr	r2, =ewram_240
	mov	r1, #0xe1
	lsl	r1, #1
	add	r3, r2, r1
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	sub	sp, #8
	cmp	r3, #0x10
	bne	.L2a7e
	ldr	r1, =0x205
	add	r3, r2, r1
	add	r1, #1
	ldrb	r0, [r3]
	add	r3, r2, r1
	ldrb	r1, [r3]
	bl	__Func_1ccc0
	bl	OvlFunc_34c8
	b	.L2c6e
.L2a7e:
	mov	r0, #0xfd
	lsl	r0, #4
	bl	__Func_79338
	cmp	r0, #0
	bne	.L2aa2
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	bne	.L2a9c
	mov	r0, #0x1a
	bl	OvlFunc_5b48
	b	.L2aa2
.L2a9c:
	mov	r0, #0x14
	bl	OvlFunc_5b48
.L2aa2:
	mov	r3, #2
	str	r3, [sp]
	mov	r5, #1
	mov	r0, #2
	mov	r1, #0x66
	mov	r2, #0x54
	mov	r3, #0x29
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r1, #0x66
	mov	r2, #0x53
	mov	r3, #0x29
	mov	r0, #1
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	ldr	r0, =0x87a
	bl	__Func_79338
	mov	r3, r0
	neg	r0, r3
	orr	r0, r3
	lsr	r0, #31
	add	r0, #0x14
	bl	__Func_92054
	mov	r1, #0
	mov	r5, r0
	bl	__Func_c528
	mov	r0, #0xc5
	lsl	r0, #2
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2af2
	mov	r3, #0xb5
	b	.L2b02
.L2af2:
	ldr	r0, =0x316
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2b00
	mov	r3, #0xc5
	b	.L2b02
.L2b00:
	mov	r3, #0xbd
.L2b02:
	lsl	r3, #17
	str	r3, [r5, #8]
	mov	r3, #0x92
	lsl	r3, #18
	str	r3, [r5, #0x10]
	mov	r3, #0xc0
	lsl	r3, #16
	str	r3, [r5, #0xc]
	bl	OvlFunc_5950
	mov	r1, r5
	add	r1, #0x22
	mov	r3, #3
	strb	r3, [r1]
	mov	r3, r5
	mov	r2, #0
	add	r3, #0x55
	mov	r1, #0xc8
	strb	r2, [r3]
	ldr	r0, =OvlFunc_5a94
	lsl	r1, #4
	bl	__Func_41d8
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	bne	.L2c24
	ldr	r0, =0x815
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2b5e
	mov	r0, #0x15
	bl	__Func_92054
	mov	r5, r0
	mov	r0, #0x15
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	ldr	r3, =0x28f
	str	r3, [r5, #0x18]
	str	r3, [r5, #0x1c]
.L2b5e:
	ldr	r0, =0x808
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2b86
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r0, #0x11
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
.L2b86:
	ldr	r0, =0x815
	bl	__Func_79338
	mov	r5, r0
	cmp	r5, #0
	bne	.L2bfc
	ldr	r0, =0x109
	bl	__Func_79338
	cmp	r0, #0
	bne	.L2bc8
	ldr	r0, =0x823
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2bd8
	mov	r1, #0x80
	mov	r2, #0xe4
	lsl	r1, #17
	mov	r0, #0x16
	lsl	r2, #17
	bl	__Func_923e4
	mov	r0, #0x16
	bl	__Func_92054
	ldr	r3, =OvlFunc_572c
	ldr	r1, =.L6248
	str	r3, [r0, #0x6c]
	mov	r0, #0x16
	bl	__Func_9207c
	b	.L2bd8
.L2bc8:
	mov	r0, #0x16
	bl	__Func_92054
	add	r0, #0x5b
	strb	r5, [r0]
	ldr	r0, =0x241
	bl	__Func_79374
.L2bd8:
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #0x10
	beq	.L2bfc
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	bne	.L2bfc
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_5a40
	lsl	r1, #4
	bl	__Func_41d8
.L2bfc:
	mov	r0, #0xc2
	lsl	r0, #2
	bl	__Func_79338
	cmp	r0, #0
	bne	.L2c24
	ldr	r3, =ewram_240
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #0x11
	bne	.L2c24
	bl	OvlFunc_3fb0
	mov	r0, #0xc2
	lsl	r0, #2
	bl	__Func_79358
.L2c24:
	ldr	r0, =0x109
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2c5e
	mov	r0, #0x81
	lsl	r0, #2
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2c4e
	mov	r3, #0x14
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x31
	mov	r1, #0x35
	mov	r2, #8
	mov	r3, #4
	bl	__Func_10704
.L2c4e:
	mov	r0, #0x84
	lsl	r0, #2
	bl	__Func_79338
	cmp	r0, #0
	beq	.L2c5e
	bl	OvlFunc_d70
.L2c5e:
	mov	r0, #0xaa
	bl	__Func_91ff0
	bl	__Func_fe9c
	mov	r0, #1
	bl	__Func_30f8
.L2c6e:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_2a54

@ Cutscene: roughly 228 instructions of straight-line script --
@ 6 turns, 10 animation changes, 12 dialogue lines, 4 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x1c45.
@ Sets save bit 0x303.
.thumb_func_start OvlFunc_2cb0
	push	{r5, lr}
	bl	__Func_916b0
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92950
	mov	r2, #0x14
	mov	r1, #0
	mov	r0, #8
	bl	__Func_9280c
	ldr	r5, =0x1c45
	mov	r0, r5
	bl	__Func_92b94
	mov	r0, #8
	mov	r1, #2
	bl	__Func_9259c
	mov	r2, #0x14
	mov	r0, #8
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #9
	lsl	r1, #6
	bl	__Func_933d4
	mov	r0, #0xc7
	mov	r1, #1
	mov	r3, #1
	lsl	r0, #17
	neg	r1, r1
	ldr	r2, =0x2460000
	bl	__Func_933f8
	mov	r0, #0
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__Func_92064
	mov	r0, #1
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__Func_92064
	mov	r1, #0xd2
	mov	r2, #0x98
	mov	r0, #0
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xa0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #8
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_92adc
	mov	r0, #0
	bl	__Func_92054
	cmp	r0, #0
	beq	.L2d4c
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #1
	bl	__Func_923e4
.L2d4c:
	mov	r1, #0xc9
	mov	r2, #0x98
	mov	r0, #1
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xd0
	mov	r2, #0x14
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92adc
	ldr	r0, =0x1001
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #8
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #8
	mov	r1, #3
	bl	__Func_92548
	ldr	r0, =0x4008
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #0
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #8
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, #0
	ldr	r0, =0x4008
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #1
	bne	.L2dd4
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, #8
	mov	r1, #1
	bl	__Func_9259c
.L2dd4:
	ldr	r0, =0x4008
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_93040
	ldr	r1, =0x105
	mov	r2, #0x3c
	mov	r0, #8
	bl	__Func_937b8
	add	r0, r5, #6
	bl	__Func_92b94
	mov	r2, #0x14
	ldr	r0, =0x4008
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #1
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0x28
	ldr	r0, =0x1001
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #8
	mov	r1, #1
	bl	__Func_925cc
	mov	r1, #0xd0
	mov	r2, #0x14
	mov	r0, #8
	lsl	r1, #8
	bl	__Func_92adc
	ldr	r0, =0x4008
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #1
	mov	r1, #3
	bl	__Func_92548
	ldr	r0, =0x1001
	mov	r1, #0
	mov	r2, #0x78
	bl	__Func_93040
	ldr	r0, =0x4008
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r0, #1
	ldr	r1, =0x105
	mov	r2, #0x28
	bl	__Func_937b8
	mov	r2, #0x28
	ldr	r0, =0x1001
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #8
	mov	r1, #4
	bl	__Func_92548
	mov	r2, #0x14
	ldr	r0, =0x4008
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r1, #0xa0
	mov	r0, #8
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r2, #0xa
	ldr	r0, =0x4008
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #8
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #1
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0
	bl	__Func_92054
	cmp	r0, #0
	beq	.L2ed0
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__Func_92128
.L2ed0:
	mov	r0, #1
	bl	__Func_923c4
	mov	r1, #0
	mov	r2, #0
	mov	r0, #1
	bl	__Func_923e4
	ldr	r0, =0x303
	bl	__Func_79358
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_2cb0

@ Cutscene: roughly 254 instructions of straight-line script --
@ 13 turns, 10 animation changes, 13 dialogue lines, 2 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x1c53, 0x1c60.
@ Sets save bit 0x304.
.thumb_func_start OvlFunc_2f14
	push	{lr}
	bl	__Func_916b0
	mov	r1, #1
	mov	r3, #1
	ldr	r0, =0x1650000
	neg	r1, r1
	ldr	r2, =0x2e20000
	bl	__Func_933f8
	mov	r0, #0
	ldr	r1, =0x16f
	ldr	r2, =0x2e9
	bl	__Func_921c4
	mov	r1, #0xa0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r0, #0
	bl	__Func_92054
	cmp	r0, #0
	beq	.L2f52
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #1
	bl	__Func_923e4
.L2f52:
	mov	r1, #0xad
	mov	r0, #1
	lsl	r1, #1
	ldr	r2, =0x2e9
	bl	__Func_921c4
	mov	r1, #0xd0
	mov	r2, #0x14
	lsl	r1, #8
	mov	r0, #1
	bl	__Func_92adc
	ldr	r0, =0x1c53
	bl	__Func_92b94
	mov	r0, #1
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #9
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, #0x80
	mov	r0, #9
	lsl	r1, #1
	mov	r2, #0
	bl	__Func_937b8
	mov	r1, #0xc0
	mov	r0, #9
	lsl	r1, #6
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r1, #0xa0
	mov	r0, #9
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #9
	lsl	r1, #6
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r2, #0x14
	mov	r0, #9
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #1
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #9
	mov	r1, #1
	bl	__Func_925cc
	mov	r1, #0xa0
	mov	r0, #9
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r2, #0x14
	mov	r0, #9
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #1
	bl	__Func_9259c
	mov	r0, #1
	ldr	r1, =0x103
	mov	r2, #0x28
	bl	__Func_937b8
	mov	r2, #0xa
	mov	r0, #1
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #9
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #1
	lsl	r1, #5
	bl	__Func_92adc
	mov	r0, #9
	mov	r1, #4
	bl	__Func_92548
	mov	r0, #9
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0xb0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r2, #0xa
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0xa
	mov	r0, #1
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #9
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__Func_937b8
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r2, #0x14
	mov	r0, #1
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #9
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #5
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r1, #0
	mov	r0, #1
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	bne	.L30e0
	mov	r0, #1
	ldr	r1, =0x105
	mov	r2, #0x3c
	bl	__Func_937b8
	b	.L30f0
.L30e0:
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L30f0:
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xd0
	mov	r2, #0xa
	lsl	r1, #8
	mov	r0, #1
	bl	__Func_92adc
	ldr	r0, =0x1c60
	bl	__Func_92b94
	mov	r0, #1
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #9
	mov	r1, #3
	bl	__Func_92548
	mov	r2, #0x14
	mov	r0, #9
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x14
	mov	r0, #1
	lsl	r1, #5
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #1
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0
	bl	__Func_92054
	cmp	r0, #0
	beq	.L3170
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__Func_92128
.L3170:
	mov	r0, #1
	bl	__Func_923c4
	mov	r1, #0
	mov	r2, #0
	mov	r0, #1
	bl	__Func_923e4
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__Func_79358
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_2f14

@ Cutscene: roughly 89 instructions of straight-line script --
@ 2 turns, 3 animation changes, 2 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x1c90.
@ Sets save bit 0x305.
.thumb_func_start OvlFunc_31b4
	push	{r5, lr}
	bl	__Func_916b0
	mov	r0, #0xc
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xd
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xe
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xc
	mov	r1, #0
	bl	__Func_924d4
	mov	r0, #0xd
	mov	r1, #0
	bl	__Func_924d4
	mov	r1, #0
	mov	r0, #0xe
	bl	__Func_924d4
	mov	r0, #0x14
	bl	__Func_30f8
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	lsl	r0, #10
	lsl	r1, #10
	bl	__Func_12330
	ldr	r5, =.L665c
	mov	r0, #0xc
	mov	r1, r5
	bl	__Func_9207c
	mov	r0, #0xa
	bl	__Func_30f8
	mov	r1, r5
	mov	r0, #0xd
	bl	__Func_9207c
	mov	r0, #1
	mov	r1, #1
	ldr	r2, =0xe666
	neg	r1, r1
	neg	r0, r0
	bl	__Func_12330
	mov	r0, #0x14
	bl	__Func_30f8
	mov	r1, r5
	mov	r0, #0xe
	bl	__Func_920fc
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #0xb
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #0xb
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, #0xd0
	lsl	r1, #8
	mov	r2, #0xa
	mov	r0, #0xb
	bl	__Func_92adc
	ldr	r0, =0x1c90
	bl	__Func_92b94
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_93040
	mov	r2, #0x14
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_9280c
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0x80
	lsl	r1, #8
	mov	r2, #0xa
	mov	r0, #0xb
	bl	__Func_92adc
	ldr	r0, =0x305
	bl	__Func_79358
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_31b4

@ Cutscene: roughly 80 instructions of straight-line script --
@ 1 turn, 3 animation changes, 0 dialogue lines, 2 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_32b0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r0
	mov	r9, r3
	mov	r8, r1
	mov	r10, r2
	bl	__Func_92054
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r6, r0
	lsl	r1, #9
	mov	r0, r7
	lsl	r2, #8
	ldr	r5, [r6, #0x50]
	bl	__Func_92064
	mov	r1, #0xc4
	mov	r0, r7
	lsl	r1, #1
	ldr	r2, =0x376
	bl	__Func_921c4
	mov	r1, #0xc0
	mov	r2, #0xa
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	add	r5, #0x26
	mov	r3, #0
	add	r6, #0x55
	strb	r3, [r6]
	strb	r3, [r5]
	mov	r0, r7
	mov	r1, r8
	bl	__Func_924d4
	mov	r0, r7
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xc4
	ldr	r2, =0x36b
	lsl	r1, #1
	mov	r0, r7
	bl	__Func_92158
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, r7
	mov	r1, r10
	bl	__Func_924d4
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, r7
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r1, #0xc4
	mov	r0, r7
	lsl	r1, #1
	ldr	r2, =0x35b
	bl	__Func_92158
	mov	r3, #1
	strb	r3, [r5]
	mov	r3, r9
	cmp	r3, #0
	beq	.L334e
	mov	r3, #3
	strb	r3, [r6]
.L334e:
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, r7
	mov	r1, #1
	bl	__Func_924d4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_32b0

@ Cutscene: roughly 87 instructions of straight-line script --
@ 1 turn, 3 animation changes, 0 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_3380
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r9, r3
	mov	r8, r1
	mov	r10, r2
	bl	__Func_92054
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r6, r0
	lsl	r1, #9
	mov	r0, r7
	lsl	r2, #8
	ldr	r5, [r6, #0x50]
	bl	__Func_92064
	mov	r1, #0xc4
	mov	r0, r7
	lsl	r1, #1
	ldr	r2, =0x35b
	bl	__Func_921c4
	mov	r1, #0xc0
	mov	r0, r7
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r2, #0x55
	mov	r3, #0
	add	r2, r6
	add	r5, #0x26
	strb	r3, [r2]
	strb	r3, [r5]
	mov	r0, r7
	mov	r1, r8
	mov	r11, r2
	bl	__Func_924d4
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, r7
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r1, #0xc4
	lsl	r1, #1
	ldr	r2, =0x36b
	mov	r0, r7
	bl	__Func_92158
	mov	r0, #0xa
	bl	__Func_9163c
	ldr	r2, =0x2666
	mov	r0, r7
	ldr	r1, =0x4ccc
	bl	__Func_92064
	mov	r0, r7
	mov	r1, r10
	bl	__Func_924d4
	mov	r1, #0xc4
	mov	r0, r7
	lsl	r1, #1
	ldr	r2, =0x37a
	bl	__Func_92158
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r6, #0x28]
	mov	r3, #1
	strb	r3, [r5]
	mov	r3, r9
	cmp	r3, #0
	beq	.L342e
	mov	r3, #3
	mov	r2, r11
	strb	r3, [r2]
.L342e:
	mov	r0, r7
	mov	r1, #1
	bl	__Func_924d4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_3380

@ 48 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, SetEntityActorOptions, PlaceSlotAt
.thumb_func_start OvlFunc_345c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r8, r1
	mov	r6, r0
	cmp	r2, #0
	bne	.L34a0
	mov	r7, #0
	cmp	r7, r8
	bcs	.L34b8
.L3470:
	mov	r0, r6
	bl	__Func_92054
	mov	r5, r0
	mov	r2, r5
	mov	r3, #0
	add	r2, #0x55
	strb	r3, [r2]
	mov	r1, #0
	bl	__Func_c528
	mov	r3, #0xc3
	lsl	r3, #17
	str	r3, [r5, #8]
	mov	r3, #0xa0
	lsl	r3, #16
	str	r3, [r5, #0xc]
	ldr	r3, =0x34a0000
	add	r7, #1
	str	r3, [r5, #0x10]
	add	r6, #1
	cmp	r7, r8
	bcc	.L3470
	b	.L34b8
.L34a0:
	mov	r7, #0
	cmp	r7, r8
	bcs	.L34b8
.L34a6:
	mov	r0, r6
	mov	r1, #0
	mov	r2, #0
	add	r7, #1
	bl	__Func_923e4
	add	r6, #1
	cmp	r7, r8
	bcc	.L34a6
.L34b8:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_345c

@ Cutscene: roughly 1024 instructions of straight-line script --
@ 16 turns, 23 animation changes, 24 dialogue lines, 47 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_34c8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r0, #0
	sub	sp, #0x1c
	bl	__Func_92054
	mov	r9, r0
	mov	r0, #0xe
	bl	__Func_92054
	mov	r6, r0
	bl	__Func_916b0
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r2, r2
	mov	r3, #0
	neg	r1, r1
	neg	r0, r0
	bl	__Func_933f8
	mov	r0, #1
	bl	__Func_30f8
	bl	__Func_93554
	mov	r7, #0
	add	r0, #0x55
	mov	r1, #0
	strb	r7, [r0]
	mov	r0, #1
	mov	r10, r1
	bl	__Func_30f8
	mov	r3, #0x14
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x31
	mov	r1, #0x35
	mov	r2, #8
	mov	r3, #4
	bl	__Func_10704
	mov	r3, #2
	str	r3, [sp]
	mov	r5, #1
	mov	r0, #2
	mov	r1, #0x66
	mov	r2, #0x54
	mov	r3, #0x29
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #1
	mov	r1, #0x66
	mov	r2, #0x53
	mov	r3, #0x29
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r1, #0x67
	mov	r2, #0x52
	mov	r3, #0x2a
	mov	r0, #0
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #0xb
	bl	__Func_92054
	mov	r7, r0
	mov	r3, r7
	mov	r2, r10
	add	r3, #0x55
	strb	r2, [r3]
	mov	r3, #0xa0
	lsl	r3, #16
	mov	r8, r3
	str	r3, [r7, #0xc]
	mov	r5, #0xc2
	mov	r3, #0xd2
	lsl	r5, #17
	lsl	r3, #18
	str	r3, [r7, #0x10]
	str	r5, [r7, #8]
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xc
	bl	__Func_92054
	mov	r7, r0
	mov	r3, r7
	mov	r1, r10
	add	r3, #0x55
	strb	r1, [r3]
	mov	r3, #0xd3
	mov	r2, r8
	lsl	r3, #18
	str	r2, [r7, #0xc]
	str	r3, [r7, #0x10]
	str	r5, [r7, #8]
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xd
	bl	__Func_92054
	mov	r7, r0
	mov	r3, r7
	mov	r1, r10
	add	r3, #0x55
	strb	r1, [r3]
	mov	r3, #0xd4
	lsl	r3, #18
	mov	r2, r8
	str	r3, [r7, #0x10]
	str	r2, [r7, #0xc]
	str	r5, [r7, #8]
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xb
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0xc
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	ldr	r7, =.L6590
	mov	r0, #0
	mov	r1, r7
	bl	__Func_9207c
	bl	__Func_c5b4
	ldr	r5, =0xee8
	mov	r1, #0
	mov	r0, r5
	mov	r2, #0
	bl	__Func_19aa0
	bl	__Func_c5fc
	ldr	r2, =0x4950000
	mov	r3, #0
	mov	r1, r8
	ldr	r0, =0x1530000
	bl	__Func_933f8
	bl	__Func_fe9c
	mov	r0, #1
	bl	__Func_30f8
	ldr	r0, =0x547a
	ldr	r1, =0xa8f
	bl	__Func_933d4
	mov	r0, #0x94
	mov	r3, #1
	lsl	r0, #17
	mov	r1, r8
	ldr	r2, =0x3990000
	bl	__Func_933f8
	ldr	r1, =0x1990000
	ldr	r2, =0x46e0000
	mov	r0, #5
	bl	__Func_923e4
	mov	r0, #1
	bl	__Func_30f8
	mov	r0, #5
	ldr	r1, =0xb333
	ldr	r2, =0x5999
	bl	__Func_92064
	mov	r1, #0xd2
	mov	r0, #5
	lsl	r1, #1
	ldr	r2, =0x42c
	bl	__Func_9218c
	bl	__Func_8acc4
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xe4
	lsl	r3, #1
	mov	r10, r3
	mov	r1, r10
	mov	r3, #0x3c
	str	r3, [r2, r1]
	bl	__Func_91dc8
	mov	r0, #5
	bl	__Func_923c4
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r2, #0x85
	mov	r0, #5
	ldr	r1, =0x155
	lsl	r2, #3
	bl	__Func_921c4
	mov	r0, #5
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__Func_92064
	mov	r0, #5
	ldr	r1, =0x167
	ldr	r2, =0x409
	bl	__Func_921c4
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #8
	lsl	r1, #8
	lsl	r2, #7
	bl	__Func_92064
	mov	r1, #0x9f
	ldr	r2, =0x3b3
	mov	r0, #8
	lsl	r1, #1
	bl	__Func_9218c
	mov	r0, #8
	mov	r1, #2
	bl	__Func_924d4
	mov	r1, #0xce
	mov	r0, #5
	lsl	r1, #1
	ldr	r2, =0x409
	bl	__Func_921c4
	mov	r1, #0xce
	mov	r0, #5
	lsl	r1, #1
	ldr	r2, =0x3fb
	bl	__Func_921c4
	mov	r1, #0xbb
	mov	r2, #0xfc
	mov	r0, #5
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r0, #5
	ldr	r1, =0x15b
	ldr	r2, =0x3bb
	bl	__Func_921c4
	mov	r1, #0x9f
	mov	r0, #8
	lsl	r1, #1
	ldr	r2, =0x3b3
	bl	__Func_921c4
	mov	r2, #0x28
	mov	r0, #5
	mov	r1, #8
	bl	__Func_92848
	mov	r0, #8
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	ldr	r2, =0x3f9
	mov	r0, #8
	ldr	r1, =0x17b
	bl	__Func_9218c
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #8
	lsl	r1, #5
	bl	__Func_933d4
	mov	r2, #0xe6
	mov	r0, #5
	ldr	r1, =0x14d
	lsl	r2, #2
	bl	__Func_921c4
	mov	r2, #0xe7
	ldr	r1, =0x12b
	lsl	r2, #2
	mov	r0, #5
	bl	__Func_921c4
	bl	__Func_93530
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xf0
	mov	r2, #0x1e
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #10
	lsl	r1, #7
	bl	__Func_933d4
	mov	r3, #1
	ldr	r0, =0x1830000
	mov	r1, r8
	ldr	r2, =0x3620000
	bl	__Func_933f8
	add	r5, #1
	bl	__Func_93530
	mov	r1, #2
	mov	r2, #0x14
	mov	r0, #0xa
	bl	__Func_92560
	mov	r0, r5
	bl	__Func_92b94
	ldr	r0, =0x100a
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r3, #0x80
	lsl	r3, #9
	mov	r2, r9
	str	r3, [r2, #0x18]
	str	r3, [r2, #0x1c]
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	mov	r2, #0x28
	mov	r0, #0xa
	mov	r1, #0
	bl	__Func_92848
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_93874
	mov	r1, #2
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r1, #2
	mov	r0, #0xa
	bl	__Func_9259c
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0x28
	ldr	r0, =0x100a
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	mov	r0, #0
	mov	r1, r7
	bl	__Func_9207c
	mov	r1, #1
	mov	r0, #5
	bl	__Func_9335c
	bl	__Func_93530
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #3
	bl	__Func_92548
	mov	r1, #0xd0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r1, #0x9c
	mov	r0, #5
	lsl	r1, #1
	ldr	r2, =0x2f7
	bl	__Func_921c4
	mov	r2, #0xbe
	ldr	r1, =0x169
	lsl	r2, #2
	mov	r0, #5
	bl	__Func_921c4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_92adc
	ldr	r0, =0x6001
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #1
	mov	r2, #0
	bl	__Func_937b8
	mov	r0, #5
	mov	r1, #4
	mov	r2, #0x28
	bl	__Func_92560
	mov	r1, #0xc0
	mov	r2, #0x1e
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0x1e
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #11
	lsl	r1, #8
	bl	__Func_933d4
	mov	r0, #0xc6
	mov	r1, #1
	mov	r2, #0x93
	mov	r3, #1
	lsl	r0, #17
	neg	r1, r1
	lsl	r2, #18
	bl	__Func_933f8
	mov	r1, r10
	ldr	r2, =0x2e3
	mov	r0, #5
	bl	__Func_921c4
	bl	__Func_93530
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x28
	mov	r0, #1
	ldr	r1, =0x105
	bl	__Func_937b8
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	bl	OvlFunc_5594
	mov	r0, #1
	mov	r1, #0x11
	bl	__Func_924d4
	ldr	r0, =0x2001
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r0, #0x83
	bl	__Func_f9080
	mov	r5, #0
.L3922:
	mov	r0, #1
	bl	__Func_92054
	bl	OvlFunc_5c20
	add	r5, #1
	mov	r0, #1
	bl	__Func_30f8
	cmp	r5, #0x3b
	bls	.L3922
	mov	r0, #1
	mov	r1, #1
	bl	__Func_92b08
	ldr	r3, =OvlFunc_55b0
	mov	r1, #0xc8
	mov	r10, r3
	mov	r0, r10
	lsl	r1, #4
	bl	__Func_41d8
	ldr	r1, =OvlFunc_55d0
	mov	r8, r1
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r8
	bl	__Func_41d8
	mov	r0, #0xe
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r2, #0
	mov	r9, r2
	mov	r3, r6
	mov	r1, r9
	add	r3, #0x55
	strb	r1, [r3]
	mov	r3, #0xd6
	lsl	r3, #17
	str	r3, [r6, #8]
	mov	r3, #0x80
	lsl	r3, #8
	mov	r11, r3
	mov	r2, #0xd0
	ldr	r3, =OvlFunc_575c
	mov	r5, #0x92
	lsl	r5, #18
	mov	r1, r11
	lsl	r2, #16
	str	r3, [r6, #0x6c]
	str	r2, [r6, #0xc]
	strh	r1, [r6, #6]
	str	r5, [r6, #0x10]
	mov	r0, #4
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xe
	lsl	r1, #10
	lsl	r2, #10
	bl	__Func_92064
	mov	r1, #0xcc
	mov	r2, #0xd0
	mov	r3, r5
	mov	r0, r6
	lsl	r1, #17
	lsl	r2, #16
	bl	__Func_d14c
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #9
	ldr	r1, =0x2666
	ldr	r2, =0x1333
	bl	__Func_92064
	ldr	r1, =0x2666
	ldr	r2, =0x1333
	mov	r0, #0xe
	bl	__Func_92064
	mov	r0, #9
	bl	__Func_92054
	mov	r1, #0xc4
	mov	r2, #0xd0
	mov	r3, r5
	mov	r0, r6
	lsl	r1, #17
	lsl	r2, #16
	b	.L3a88

	.pool_aligned

.L3a88:
	bl	__Func_d14c
	mov	r1, #0xbd
	mov	r2, #0x92
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #9
	bl	__Func_92158
	mov	r0, #0x14
	bl	__Func_9163c
	ldr	r0, =0x2005
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r2, r9
	mov	r1, #2
	str	r2, [r6, #0x6c]
	mov	r0, #1
	bl	__Func_92b08
	mov	r0, #1
	bl	__Func_92054
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r0]
	mov	r0, r10
	bl	__Func_4278
	mov	r0, r8
	bl	__Func_4278
	mov	r0, #1
	bl	__Func_30f8
	mov	r0, #1
	mov	r1, #0
	bl	__Func_92950
	mov	r0, #9
	mov	r1, #0
	bl	__Func_92950
	mov	r1, #0xc0
	mov	r2, #0
	mov	r0, #1
	lsl	r1, #6
	bl	__Func_92adc
	mov	r1, #1
	mov	r0, #1
	bl	__Func_924d4
	mov	r0, r6
	bl	OvlFunc_57fc
	bl	OvlFunc_55a4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xd4
	mov	r2, #0x9c
	mov	r0, #5
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r2, #0x3c
	mov	r0, #1
	mov	r1, #5
	bl	__Func_92848
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x14
	ldr	r0, =0x6001
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x81
	mov	r2, #0x28
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r2, #0x50
	mov	r0, #1
	ldr	r1, =0x101
	bl	__Func_937b8
	mov	r1, #1
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x81
	mov	r2, #0x50
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xc2
	mov	r2, #0x97
	mov	r0, #5
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x14
	ldr	r0, =0x1005
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x28
	mov	r0, #1
	ldr	r1, =0x105
	bl	__Func_937b8
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, r11
	mov	r0, #1
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r3, #0xe
	str	r3, [sp, #0xc]
	str	r3, [sp, #0x14]
	mov	r1, #5
	mov	r3, r9
	mov	r0, #0xa
	mov	r4, #4
	str	r1, [sp, #4]
	str	r0, [sp, #8]
	str	r3, [sp, #0x18]
	mov	r2, #2
	mov	r3, #0x19
	mov	r1, #1
	mov	r0, #1
	str	r2, [sp]
	str	r4, [sp, #0x10]
	bl	__Func_931ec
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r1, #0x81
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_93874
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #1
	bl	__Func_93874
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r0, #5
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__Func_937b8
	mov	r2, #0x14
	ldr	r0, =0x1005
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #1
	bl	__Func_93874
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #5
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__Func_937b8
	mov	r2, #0x14
	ldr	r0, =0x1005
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x50
	mov	r0, #5
	ldr	r1, =0x101
	bl	__Func_937b8
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	ldr	r0, =0x1005
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #5
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r2, #0x28
	mov	r0, #5
	mov	r1, #1
	bl	__Func_9280c
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xbe
	mov	r2, #0x9b
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #5
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	ldr	r0, =0x1005
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #1
	bl	__Func_93874
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #2
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x14
	ldr	r0, =0x6001
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x50
	mov	r0, #1
	ldr	r1, =0x101
	bl	__Func_937b8
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #1
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__Func_92064
	mov	r1, #0xce
	mov	r2, #0x97
	mov	r0, #1
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #1
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	ldr	r0, =0x1005
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r0, #1
	ldr	r1, =0x101
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r0, #5
	mov	r1, #4
	mov	r2, #0x1e
	bl	__Func_92560
	mov	r2, #0x14
	ldr	r0, =0x1005
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0
	mov	r2, #0x14
	ldr	r0, =0x6001
	bl	__Func_93040
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r1, #0xe0
	mov	r2, #0x28
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #5
	mov	r1, #3
	bl	__Func_92548
	ldr	r0, =0x1005
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r0, #1
	ldr	r1, =0x101
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r0, #5
	ldr	r1, =0x103
	mov	r2, #0x28
	bl	__Func_937b8
	mov	r0, #5
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__Func_92064
	mov	r1, #0xd6
	mov	r2, #0x9d
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #5
	bl	__Func_9218c
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r2, #0
	lsl	r1, #6
	mov	r0, #1
	bl	__Func_92adc
	mov	r0, #5
	bl	__Func_923c4
	ldr	r0, =0x5001
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #1
	mov	r0, #5
	bl	__Func_924d4
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #0xb0
	mov	r2, #0x1e
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #1
	b	.L3ec0

	.pool_aligned

.L3ec0:
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, #0xd6
	mov	r2, #0x9d
	mov	r0, #5
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xb0
	mov	r2, #0x14
	mov	r0, #5
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x14
	ldr	r0, =0x2005
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #1
	bl	__Func_93874
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0x14
	ldr	r0, =0x5001
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x80
	mov	r1, r11
	mov	r0, #5
	lsl	r2, #7
	bl	__Func_92064
	mov	r2, #0x80
	mov	r1, r11
	mov	r0, #1
	lsl	r2, #7
	bl	__Func_92064
	mov	r1, #0xe1
	mov	r0, #5
	lsl	r1, #1
	ldr	r2, =0x2ee
	bl	__Func_9218c
	mov	r1, #0xe1
	lsl	r1, #1
	ldr	r2, =0x2ee
	mov	r0, #1
	bl	__Func_9218c
	mov	r0, #0x3c
	bl	__Func_9163c
	ldr	r3, =iwram_1ebc
	mov	r1, #0xe4
	ldr	r3, [r3]
	lsl	r1, #1
	add	r3, r1
	mov	r2, #0x3c
	str	r2, [r3]
	bl	__Func_91df4
	bl	__Func_91e20
	mov	r0, #0xc
	bl	__Func_91e9c
	bl	__Func_91750
	add	sp, #0x1c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_34c8

@ Cutscene: roughly 2126 instructions of straight-line script --
@ 70 turns, 63 animation changes, 46 dialogue lines, 63 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0xf03, 0xf0a, 0xf0e, 0xf27.
@ Sets save bit 0x202.
.thumb_func_start OvlFunc_3fb0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r0, #0
	sub	sp, #0x1c
	bl	__Func_92054
	mov	r11, r0
	bl	__Func_916b0
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r1, r1
	neg	r2, r2
	mov	r3, #0
	neg	r0, r0
	bl	__Func_933f8
	mov	r0, #1
	bl	__Func_30f8
	mov	r3, #0x14
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x31
	mov	r1, #0x35
	mov	r2, #8
	mov	r3, #4
	bl	__Func_10704
	mov	r3, #2
	str	r3, [sp]
	mov	r5, #1
	mov	r0, #2
	mov	r1, #0x66
	mov	r2, #0x54
	mov	r3, #0x29
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #1
	mov	r1, #0x66
	mov	r2, #0x53
	mov	r3, #0x29
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r3, #0x2a
	mov	r0, #0
	mov	r1, #0x67
	mov	r2, #0x52
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r1, #0xc4
	mov	r2, #0xe0
	lsl	r1, #17
	lsl	r2, #18
	mov	r0, #0x15
	bl	__Func_923e4
	mov	r0, #0x15
	bl	__Func_92054
	mov	r3, #0xc0
	lsl	r3, #8
	ldr	r2, =0
	mov	r9, r3
	mov	r8, r2
	mov	r2, r9
	strh	r2, [r0, #6]
	mov	r1, #0x95
	mov	r2, #0xb8
	lsl	r1, #17
	lsl	r2, #18
	mov	r0, #1
	bl	__Func_923e4
	mov	r0, #1
	bl	__Func_92054
	mov	r3, #0x80
	lsl	r3, #7
	mov	r10, r3
	mov	r2, r10
	strh	r2, [r0, #6]
	mov	r1, #0x95
	mov	r2, #0xbe
	lsl	r2, #18
	lsl	r1, #17
	mov	r0, #5
	bl	__Func_923e4
	mov	r0, #5
	bl	__Func_92054
	b	.L4088

	.pool_aligned

.L4088:
	mov	r3, r10
	strh	r3, [r0, #6]
	mov	r1, #0xb
	mov	r0, #0
	bl	__Func_924d4
	ldr	r1, =.L6590
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0x17
	bl	__Func_92054
	mov	r7, r0
	mov	r3, r7
	mov	r2, r8
	add	r3, #0x55
	strb	r2, [r3]
	mov	r5, #0xc2
	mov	r6, #0xa0
	mov	r3, #0xd2
	lsl	r5, #17
	lsl	r6, #16
	lsl	r3, #18
	str	r3, [r7, #0x10]
	str	r5, [r7, #8]
	mov	r1, #0
	str	r6, [r7, #0xc]
	bl	__Func_c528
	mov	r0, #0x18
	bl	__Func_92054
	mov	r7, r0
	mov	r3, r7
	mov	r2, r8
	add	r3, #0x55
	strb	r2, [r3]
	mov	r3, #0xd3
	lsl	r3, #18
	str	r3, [r7, #0x10]
	str	r5, [r7, #8]
	mov	r1, #0
	str	r6, [r7, #0xc]
	bl	__Func_c528
	mov	r0, #0x19
	bl	__Func_92054
	mov	r7, r0
	mov	r3, r7
	mov	r2, r8
	add	r3, #0x55
	strb	r2, [r3]
	mov	r3, #0xd4
	lsl	r3, #18
	mov	r1, #0
	str	r3, [r7, #0x10]
	str	r5, [r7, #8]
	str	r6, [r7, #0xc]
	bl	__Func_c528
	bl	__Func_93554
	mov	r3, r8
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #1
	bl	__Func_30f8
	mov	r1, r6
	ldr	r2, =0x36d0000
	mov	r3, #0
	ldr	r0, =0x17f0000
	bl	__Func_933f8
	bl	__Func_fe9c
	mov	r0, #1
	bl	__Func_30f8
	ldr	r3, =iwram_1ebc
	mov	r2, #0xe4
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0x20
	str	r2, [r3]
	bl	__Func_91dc8
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, r10
	bl	__Func_92064
	mov	r1, #0x80
	mov	r2, r10
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92064
	ldr	r1, =.L6614
	mov	r0, #5
	bl	__Func_9207c
	ldr	r1, =.L65cc
	mov	r0, #1
	bl	__Func_9207c
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	mov	r3, #0x80
	lsl	r3, #9
	mov	r2, r11
	mov	r1, #0xb0
	str	r3, [r2, #0x18]
	str	r3, [r2, #0x1c]
	mov	r0, #0
	mov	r2, #0x28
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xc8
	mov	r2, #0xd2
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #0
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r2, #0x1e
	mov	r0, #0
	mov	r1, r9
	bl	__Func_92adc
	mov	r1, #1
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	bl	OvlFunc_5594
	mov	r0, #0
	mov	r1, #0x11
	bl	__Func_924d4
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_5a08
	lsl	r1, #4
	bl	__Func_41d8
	mov	r5, #0
.L41f0:
	mov	r0, r11
	bl	OvlFunc_5c20
	add	r5, #1
	mov	r0, #1
	bl	__Func_30f8
	cmp	r5, #0x27
	bls	.L41f0
	mov	r0, #0
	mov	r1, #1
	bl	__Func_92b08
	ldr	r6, =OvlFunc_55c0
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r6
	bl	__Func_41d8
	ldr	r5, =OvlFunc_55e0
	mov	r1, #0xc8
	mov	r0, r5
	lsl	r1, #4
	bl	__Func_41d8
	mov	r0, #0x17
	ldr	r1, =0x3333
	ldr	r2, =0x1999
	bl	__Func_92064
	mov	r1, #0xc3
	mov	r2, #0xd0
	mov	r0, #0x17
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_92158
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc8
	lsl	r1, #1
	ldr	r2, =0x33a
	mov	r0, #0x17
	bl	__Func_92158
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0
	bl	__Func_92054
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	mov	r1, #1
	strb	r3, [r0]
	mov	r0, #0
	bl	__Func_924d4
	ldr	r3, =OvlFunc_5a08
	mov	r8, r3
	mov	r0, r8
	bl	__Func_4278
	mov	r0, r6
	bl	__Func_4278
	mov	r0, r5
	bl	__Func_4278
	mov	r0, #1
	bl	__Func_30f8
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92950
	mov	r0, #0x17
	mov	r1, #0
	bl	__Func_92950
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x17
	bl	__Func_923e4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	ldr	r1, =.L6590
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0x78
	bl	__Func_9163c
	mov	r3, #2
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #0x29
	mov	r2, #0x54
	mov	r0, #7
	mov	r1, #0x66
	bl	__Func_10424
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	mov	r3, #0x80
	lsl	r3, #9
	mov	r2, r11
	str	r3, [r2, #0x18]
	str	r3, [r2, #0x1c]
	mov	r1, #1
	mov	r0, #0
	bl	__Func_924d4
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0
	ldr	r1, =0x179
	ldr	r2, =0x34b
	bl	__Func_921c4
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r2, #0x14
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92adc
	mov	r0, #0
	mov	r1, #0x11
	bl	__Func_924d4
	mov	r1, #0xc8
	mov	r0, r8
	lsl	r1, #4
	bl	__Func_41d8
	mov	r5, #0
.L4332:
	mov	r0, r11
	bl	OvlFunc_5c20
	add	r5, #1
	mov	r0, #1
	bl	__Func_30f8
	cmp	r5, #0x27
	bls	.L4332
	mov	r0, #0
	mov	r1, #1
	bl	__Func_92b08
	ldr	r6, =OvlFunc_55c0
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r6
	bl	__Func_41d8
	ldr	r5, =OvlFunc_55f0
	mov	r1, #0xc8
	mov	r0, r5
	lsl	r1, #4
	bl	__Func_41d8
	mov	r0, #0x18
	ldr	r1, =0x3333
	ldr	r2, =0x1999
	bl	__Func_92064
	mov	r1, #0xc3
	mov	r2, #0xd0
	mov	r0, #0x18
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_92158
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0xcf
	ldr	r1, =0x179
	lsl	r2, #2
	mov	r0, #0x18
	bl	__Func_92158
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0
	bl	__Func_92054
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	mov	r1, #1
	strb	r3, [r0]
	mov	r0, #0
	bl	__Func_924d4
	ldr	r3, =OvlFunc_5a08
	mov	r8, r3
	mov	r0, r8
	bl	__Func_4278
	mov	r0, r6
	bl	__Func_4278
	mov	r0, r5
	bl	__Func_4278
	mov	r0, #1
	bl	__Func_30f8
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92950
	mov	r0, #0x18
	mov	r1, #0
	bl	__Func_92950
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x18
	bl	__Func_923e4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	ldr	r1, =.L6590
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0x78
	bl	__Func_9163c
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r2, #0x53
	mov	r3, #0x29
	mov	r0, #6
	mov	r1, #0x66
	bl	__Func_10424
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	mov	r3, #0x80
	lsl	r3, #9
	mov	r2, r11
	str	r3, [r2, #0x18]
	str	r3, [r2, #0x1c]
	mov	r1, #1
	mov	r0, #0
	bl	__Func_924d4
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #3
	bl	__Func_92548
	mov	r1, #0xb4
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x357
	bl	__Func_921c4
	mov	r1, #0xb0
	mov	r0, #0x15
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r2, #0x14
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0
	mov	r1, #0x11
	bl	__Func_924d4
	mov	r1, #0xc8
	mov	r0, r8
	lsl	r1, #4
	bl	__Func_41d8
	mov	r5, #0
	b	.L44cc

	.pool_aligned

.L44cc:
	mov	r0, r11
	bl	OvlFunc_5c20
	add	r5, #1
	mov	r0, #1
	bl	__Func_30f8
	cmp	r5, #0x27
	bls	.L44cc
	mov	r0, #0
	mov	r1, #1
	bl	__Func_92b08
	ldr	r6, =OvlFunc_55c0
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r6
	bl	__Func_41d8
	ldr	r5, =OvlFunc_5600
	mov	r1, #0xc8
	lsl	r1, #4
	mov	r0, r5
	bl	__Func_41d8
	mov	r0, #0x19
	ldr	r1, =0x3333
	ldr	r2, =0x1999
	bl	__Func_92064
	mov	r1, #0xc3
	mov	r2, #0xd0
	mov	r0, #0x19
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_92158
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xb4
	lsl	r1, #1
	ldr	r2, =0x345
	mov	r0, #0x19
	bl	__Func_92158
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0
	bl	__Func_92054
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r0]
	mov	r1, #1
	mov	r0, #0
	bl	__Func_924d4
	ldr	r0, =OvlFunc_5a08
	bl	__Func_4278
	mov	r0, r6
	bl	__Func_4278
	mov	r0, r5
	bl	__Func_4278
	mov	r0, #1
	bl	__Func_30f8
	mov	r0, #0
	mov	r1, #0
	bl	__Func_92950
	mov	r0, #0x19
	mov	r1, #0
	bl	__Func_92950
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x19
	bl	__Func_923e4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0
	mov	r1, #0xb
	bl	__Func_924d4
	ldr	r1, =.L6590
	mov	r0, #0
	bl	__Func_9207c
	mov	r0, #0x78
	bl	__Func_9163c
	bl	OvlFunc_55a4
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r2, #0x52
	mov	r3, #0x2a
	mov	r0, #5
	mov	r1, #0x67
	bl	__Func_10424
	mov	r0, #0
	mov	r1, #1
	bl	__Func_9207c
	mov	r3, #0x80
	lsl	r3, #9
	mov	r2, r11
	str	r3, [r2, #0x18]
	str	r3, [r2, #0x1c]
	mov	r1, #2
	mov	r2, #0x14
	mov	r0, #0x15
	bl	__Func_92560
	ldr	r0, =0xf03
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #5
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r3, #0
	mov	r0, #0x15
	mov	r1, #5
	mov	r2, #6
	bl	OvlFunc_32b0
	mov	r0, #0x15
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r2, #0xd0
	ldr	r1, =0x18d
	lsl	r2, #2
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x3c
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0xba
	mov	r2, #0xd0
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #4
	bl	__Func_92548
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	bne	.L471c
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	b	.L4724

	.pool_aligned

.L471c:
	mov	r0, #0x15
	mov	r1, #4
	bl	__Func_92548
.L4724:
	mov	r1, #0
	mov	r2, #0x14
	mov	r0, #0x15
	bl	__Func_93040
	ldr	r0, =0xf0a
	bl	__Func_92b94
	mov	r1, #0xc1
	lsl	r1, #1
	ldr	r2, =0x349
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xd0
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #1
	bne	.L4794
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L4794:
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xd0
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	ldr	r0, =0xf0e
	bl	__Func_92b94
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xc1
	ldr	r2, =0x339
	lsl	r1, #1
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x3c
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0xba
	mov	r2, #0xd0
	mov	r0, #0x15
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xa0
	mov	r2, #0xa
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #0
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #2
	bl	__Func_925cc
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_933d4
	mov	r1, #0xa0
	mov	r2, #0xd7
	mov	r3, #1
	ldr	r0, =0x1790000
	lsl	r1, #16
	lsl	r2, #18
	bl	__Func_933f8
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #1
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r2, #0xe2
	mov	r0, #1
	ldr	r1, =0x171
	lsl	r2, #2
	bl	__Func_9218c
	mov	r1, #0xc4
	mov	r2, #0xe2
	lsl	r2, #2
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_921c4
	mov	r0, #1
	mov	r1, #1
	bl	__Func_924d4
	mov	r3, #0
	mov	r0, #5
	mov	r1, #0xa
	mov	r2, #0xb
	bl	OvlFunc_32b0
	mov	r1, #0xa0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0xa
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #5
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r0, #5
	mov	r1, #4
	mov	r2, #0
	bl	__Func_92560
	mov	r1, #0xc4
	mov	r0, #5
	lsl	r1, #1
	ldr	r2, =0x34b
	bl	__Func_921c4
	mov	r1, #0x90
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r2, #0x14
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #5
	mov	r1, #3
	bl	__Func_92548
	mov	r2, #0xa
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #0
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r3, #0
	mov	r0, #1
	mov	r1, #0xa
	mov	r2, #0xb
	bl	OvlFunc_32b0
	mov	r0, #5
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r0, #1
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xc4
	lsl	r1, #1
	ldr	r2, =0x34b
	mov	r0, #1
	bl	__Func_9218c
	mov	r0, #5
	bl	__Func_92054
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r5, #0xfe
	mov	r3, r5
	and	r3, r2
	mov	r1, #0xcc
	strb	r3, [r0]
	lsl	r1, #1
	ldr	r2, =0x34b
	mov	r0, #5
	bl	__Func_921c4
	mov	r0, #1
	bl	__Func_9163c
	mov	r0, #5
	bl	__Func_92054
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	mov	r1, #0x80
	strb	r3, [r0]
	mov	r2, #0
	lsl	r1, #8
	mov	r0, #5
	bl	__Func_92adc
	mov	r0, #1
	bl	__Func_923c4
	mov	r0, #1
	mov	r1, #1
	bl	__Func_924d4
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #4
	mov	r2, #0x1e
	bl	__Func_92560
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0x1e
	mov	r0, #0
	mov	r1, #2
	bl	__Func_92560
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0
	bl	__Func_93874
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #0xc0
	mov	r2, #0x28
	mov	r0, #0x15
	lsl	r1, #6
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r0, #0x15
	ldr	r1, =0x101
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0x3c
	bl	__Func_937b8
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #1
	bl	__Func_92548
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r2, #0x1e
	mov	r0, #5
	mov	r1, #1
	bl	__Func_92848
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0x15
	ldr	r1, =0x105
	mov	r2, #0x3c
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	b	.L4b5c

	.pool_aligned

.L4b5c:
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0xa
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0xa
	mov	r0, #5
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_9259c
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #4
	bl	__Func_92548
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #5
	mov	r1, #3
	bl	__Func_92548
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_92548
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #2
	bl	__Func_925cc
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	ldr	r0, =0x9999
	ldr	r1, =0x1333
	bl	__Func_933d4
	mov	r1, #0xa0
	mov	r3, #1
	ldr	r0, =0x1750000
	lsl	r1, #16
	ldr	r2, =0x3450000
	bl	__Func_933f8
	mov	r1, #0xb6
	mov	r2, #0xcc
	mov	r0, #0x15
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #6
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_93040
	mov	r2, #0x1e
	mov	r0, #5
	mov	r1, #1
	bl	__Func_92848
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r1, #0xa0
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r2, #0x3c
	mov	r0, #0
	ldr	r1, =0x105
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #4
	bl	__Func_92548
	mov	r1, #0
	mov	r0, #0x15
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	mov	r6, #0
	bl	__Func_91c7c
	cmp	r0, #1
	bne	.L4cda
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L4cda:
	mov	r1, #0
	mov	r2, #0x14
	mov	r0, #0x15
	bl	__Func_93040
	ldr	r0, =0xf27
	bl	__Func_92b94
	mov	r2, #0
	mov	r0, #0x15
	ldr	r1, =0x103
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_925cc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r2, #0
	mov	r0, #0x15
	mov	r1, #4
	bl	__Func_92560
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_925cc
	mov	r1, #7
	mov	r0, #0x15
	bl	__Func_924d4
	mov	r0, #5
	bl	__Func_9163c
	mov	r3, #0xe
	mov	r1, #1
	mov	r0, #0xa
	mov	r4, #4
	str	r1, [sp, #4]
	str	r0, [sp, #8]
	str	r3, [sp, #0xc]
	str	r3, [sp, #0x14]
	mov	r2, #2
	mov	r1, #0xe
	mov	r3, #0x18
	mov	r0, #0x15
	str	r2, [sp]
	str	r4, [sp, #0x10]
	str	r6, [sp, #0x18]
	bl	__Func_931ec
	mov	r0, #0xa1
	bl	__Func_f9080
	mov	r0, #0x15
	bl	__Func_92054
	mov	r7, r0
	ldr	r3, [r7, #0x50]
	mov	r1, r7
	add	r1, #0x5a
	ldrb	r2, [r1]
	add	r3, #0x26
	strb	r6, [r3]
	mov	r3, r5
	and	r3, r2
	strb	r3, [r1]
	mov	r2, #0xc0
	mov	r1, #0xc0
	mov	r0, #0x15
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_92064
	mov	r1, #0xb6
	mov	r0, #0x15
	lsl	r1, #1
	ldr	r2, =0x32f
	bl	__Func_921c4
	mov	r0, #4
	bl	__Func_9163c
	mov	r5, #0
.L4d8a:
	ldr	r3, [r7, #0x10]
	mov	r2, #0xc0
	lsl	r2, #9
	add	r3, r2
	str	r3, [r7, #0x10]
	ldr	r2, =0xffffe667
	ldr	r3, [r7, #0x1c]
	add	r3, r2
	str	r3, [r7, #0x1c]
	mov	r0, #1
	add	r5, #1
	bl	__Func_9163c
	cmp	r5, #4
	bne	.L4d8a
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #1
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_92064
	mov	r0, #1
	mov	r1, #6
	mov	r2, #0
	bl	__Func_92560
	mov	r1, #0xbb
	ldr	r2, =0x33b
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_921c4
	mov	r0, #5
	mov	r1, #0
	bl	__Func_92f84
	mov	r1, #0xb0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #5
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #0x80
	mov	r2, #0xa
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #1
	mov	r1, #0xd
	bl	__Func_924d4
	mov	r1, #2
	mov	r2, #5
	mov	r0, #1
	bl	__Func_92560
	mov	r0, #0x8f
	bl	__Func_f9080
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #11
	lsl	r2, #9
	bl	__Func_12330
	mov	r3, #1
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r2, #0x53
	mov	r3, #0x29
	mov	r1, #0x66
	mov	r0, #1
	bl	__Func_10424
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r1, #0xd0
	mov	r2, #0xa
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #3
	bl	__Func_9259c
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_12330
	bl	__Func_12350
	mov	r1, #0x81
	mov	r2, #0x50
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #8
	bl	__Func_924d4
	mov	r3, #0x80
	lsl	r3, #8
	mov	r1, #0xb6
	str	r3, [r7, #0x1c]
	mov	r0, #0x15
	lsl	r1, #17
	ldr	r2, =0x32b0000
	bl	__Func_923e4
	mov	r5, #0
.L4e9c:
	ldr	r3, [r7, #0x1c]
	ldr	r2, =0x1999
	add	r3, r2
	str	r3, [r7, #0x1c]
	mov	r0, #1
	add	r5, #1
	bl	__Func_9163c
	cmp	r5, #5
	bne	.L4e9c
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #1
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, #0xa0
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	ldr	r0, =0x4ccc
	ldr	r1, =0x999
	bl	__Func_933d4
	mov	r0, #0xba
	mov	r1, #0xa0
	mov	r3, #1
	lsl	r0, #17
	lsl	r1, #16
	ldr	r2, =0x35b0000
	bl	__Func_933f8
	mov	r1, #0xc0
	mov	r2, #0xc0
	mov	r0, #0x15
	lsl	r1, #10
	lsl	r2, #9
	bl	__Func_92064
	mov	r0, #0x15
	mov	r1, #6
	mov	r2, #0
	bl	__Func_92560
	ldr	r1, =0x167
	ldr	r2, =0x343
	mov	r0, #0x15
	bl	__Func_921c4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x14
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #2
	bl	__Func_925cc
	mov	r1, r7
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r5, #0xfe
	mov	r3, r5
	and	r3, r2
	strb	r3, [r1]
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x50
	bl	__Func_93040
	mov	r0, #0x15
	ldr	r1, =0x101
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r2, #0x3c
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_925cc
	mov	r2, #0xa
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0x15
	bl	__Func_93874
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #0xa0
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r1, #0x81
	mov	r2, #0x50
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #1
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r2, #0x14
	mov	r0, #1
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #1
	bl	__Func_c528
	mov	r2, #0
	mov	r0, #1
	mov	r1, #6
	b	.L504c

	.pool_aligned

.L504c:
	bl	__Func_92560
	mov	r0, #1
	mov	r1, #1
	bl	__Func_924d4
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #11
	lsl	r2, #10
	mov	r0, #1
	bl	__Func_92064
	mov	r0, #1
	bl	__Func_92054
	mov	r7, r0
	mov	r2, r7
	add	r2, #0x5a
	ldrb	r3, [r2]
	and	r5, r3
	strb	r5, [r2]
	mov	r0, #1
	ldr	r2, =0x33b
	ldr	r1, =0x193
	bl	__Func_92128
	mov	r1, #0x81
	mov	r0, #5
	lsl	r1, #1
	bl	__Func_93874
	mov	r1, #0xc0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r1, #0
	mov	r2, #1
	mov	r0, #5
	bl	__Func_93040
	mov	r0, #1
	bl	__Func_923c4
	mov	r1, #0xa0
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #1
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #1
	mov	r1, #0xd
	bl	__Func_924d4
	mov	r2, #5
	mov	r1, #2
	mov	r0, #1
	bl	__Func_92560
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r3, #2
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #0x29
	mov	r1, #0x66
	mov	r2, #0x54
	mov	r0, #2
	bl	__Func_10424
	mov	r0, #0x8f
	bl	__Func_f9080
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	mov	r0, #0
	lsl	r1, #11
	bl	__Func_12330
	mov	r0, #1
	mov	r1, #3
	bl	__Func_925cc
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_12330
	bl	__Func_12350
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0x1e
	bl	__Func_937b8
	mov	r0, #5
	ldr	r1, =0x4ccc
	ldr	r2, =0x2666
	bl	__Func_92064
	mov	r1, #0xcc
	ldr	r2, =0x357
	lsl	r1, #1
	mov	r0, #5
	bl	__Func_921c4
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #1
	mov	r1, #2
	bl	__Func_925cc
	mov	r2, #0x3c
	mov	r0, #0x15
	ldr	r1, =0x105
	bl	__Func_937b8
	mov	r0, #5
	mov	r1, #3
	bl	__Func_9259c
	mov	r1, #3
	mov	r0, #0
	bl	__Func_925cc
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r1, #4
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x50
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r1, #0xb0
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x3c
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0x3c
	mov	r0, #0x15
	lsl	r1, #7
	bl	__Func_92adc
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x50
	bl	__Func_92adc
	mov	r0, #0x15
	ldr	r1, =0x105
	mov	r2, #0x50
	bl	__Func_937b8
	mov	r0, #0x15
	mov	r1, #0
	mov	r2, #0x3c
	bl	__Func_93040
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r0, #0
	ldr	r1, =0x101
	mov	r2, #0
	bl	__Func_937b8
	mov	r0, #5
	ldr	r1, =0x101
	mov	r2, #0
	bl	__Func_937b8
	mov	r2, #0x3c
	mov	r0, #1
	ldr	r1, =0x101
	bl	__Func_937b8
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r2, #0x3c
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #2
	bl	__Func_9259c
	mov	r1, #2
	mov	r0, #5
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #5
	lsl	r1, #8
	mov	r2, #0x3c
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_92adc
	mov	r2, #0x1e
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #5
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r2, #0x1e
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_92adc
	mov	r1, #4
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #3
	mov	r0, #5
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #1
	bl	__Func_925cc
	mov	r0, #0xa
	bl	__Func_9163c
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x15
	lsl	r1, #1
	bl	__Func_937b8
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_925cc
	mov	r0, #0x1e
	bl	__Func_9163c
	mov	r2, #0x3c
	mov	r0, #0x15
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #1
	mov	r1, #3
	bl	__Func_925cc
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #8
	lsl	r1, #9
	mov	r0, #1
	bl	__Func_92064
	mov	r0, #1
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	mov	r0, #1
	mov	r1, #4
	mov	r2, #0
	bl	__Func_92560
	mov	r1, #0xc7
	mov	r2, #0xcf
	lsl	r1, #1
	lsl	r2, #2
	mov	r0, #1
	bl	__Func_921c4
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r2, #0x3c
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r1, #3
	mov	r0, #0x15
	bl	__Func_92548
	mov	r0, #0x3c
	bl	__Func_9163c
	mov	r0, #1
	bl	__Func_92054
	mov	r7, r0
	mov	r2, r7
	add	r2, #0x5a
	ldrb	r3, [r2]
	mov	r5, #1
	orr	r3, r5
	strb	r3, [r2]
	mov	r0, #5
	bl	__Func_92054
	mov	r7, r0
	mov	r2, r7
	add	r2, #0x5a
	ldrb	r3, [r2]
	orr	r3, r5
	strb	r3, [r2]
	mov	r0, #0
	bl	__Func_92054
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r7, r0
	lsl	r1, #9
	mov	r0, #1
	lsl	r2, #8
	bl	__Func_92064
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #5
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_92adc
	mov	r3, #0xa
	ldrsh	r1, [r7, r3]
	mov	r3, #0x12
	ldrsh	r2, [r7, r3]
	add	r1, #0x10
	mov	r0, #5
	bl	__Func_9218c
	mov	r2, #0xa
	ldrsh	r1, [r7, r2]
	mov	r3, #0x12
	ldrsh	r2, [r7, r3]
	add	r1, #0x10
	sub	r2, #0x10
	mov	r0, #1
	bl	__Func_921c4
	mov	r0, #1
	bl	__Func_923c4
	mov	r1, #0xa0
	mov	r2, #0x1e
	mov	r0, #1
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #1
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #5
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r2, #0xa
	ldrsh	r1, [r7, r2]
	mov	r0, #5
	mov	r3, #0x12
	ldrsh	r2, [r7, r3]
	bl	__Func_921c4
	mov	r2, #0
	mov	r0, #5
	mov	r1, #0
	bl	__Func_923e4
	mov	r2, #0xa
	ldrsh	r1, [r7, r2]
	mov	r0, #1
	mov	r3, #0x12
	ldrsh	r2, [r7, r3]
	bl	__Func_921c4
	mov	r2, #0
	mov	r0, #1
	mov	r1, #0
	bl	__Func_923e4
	mov	r0, #1
	mov	r1, #5
	bl	__Func_917f4
	b	.L5494

	.pool_aligned

.L5494:
	mov	r1, #0xa0
	ldr	r0, =0x1790000
	lsl	r1, #16
	ldr	r2, =0x3770000
	mov	r3, #1
	bl	__Func_933f8
	mov	r3, #0
	mov	r0, #0
	mov	r1, #0xd
	mov	r2, #0xa
	bl	OvlFunc_3380
	mov	r1, #0xbc
	mov	r2, #0xe4
	mov	r0, #0
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_921c4
	mov	r1, #0xc0
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #0
	bl	__Func_92adc
	mov	r0, #0x15
	bl	__Func_92054
	add	r0, #0x5a
	ldrb	r3, [r0]
	orr	r5, r3
	strb	r5, [r0]
	mov	r3, #0
	mov	r0, #0x15
	mov	r1, #6
	mov	r2, #5
	bl	OvlFunc_3380
	mov	r0, #0x15
	ldr	r1, =0x175
	ldr	r2, =0x377
	bl	__Func_921c4
	mov	r1, #0x80
	mov	r0, #0x15
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xc0
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0x15
	mov	r1, #3
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #0
	bl	__Func_92548
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #1
	mov	r0, #0
	bl	__Func_9335c
	bl	__Func_93530
	mov	r0, #0x64
	bl	__Func_9163c
	mov	r3, #0x14
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0x2e
	mov	r2, #8
	mov	r3, #4
	mov	r0, #0x31
	bl	__Func_10704
	ldr	r0, =0x202
	bl	__Func_79358
	ldr	r0, =0x12f
	bl	__Func_79374
	mov	r2, r11
	add	r2, #0x55
	mov	r3, #3
	strb	r3, [r2]
	mov	r3, #0xa0
	mov	r2, r11
	lsl	r3, #16
	str	r3, [r2, #0xc]
	mov	r3, #0x80
	lsl	r3, #24
	mov	r6, #0
	str	r3, [r2, #0x3c]
	str	r6, [r2, #0x28]
	bl	__Func_91750
	add	sp, #0x1c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_3fb0

@ 6 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   ApplyRevealToScene
.thumb_func_start OvlFunc_5594
	push	{lr}
	mov	r0, #0x8c
	mov	r1, #0
	bl	__Func_96fb0
	pop	{r0}
	bx	r0
.func_end OvlFunc_5594

@ 4 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   RunLiftAbility
.thumb_func_start OvlFunc_55a4
	push	{lr}
	bl	__Func_97194
	pop	{r0}
	bx	r0
.func_end OvlFunc_55a4

@ 6 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_5c5c
.thumb_func_start OvlFunc_55b0
	push	{lr}
	mov	r0, #1
	bl	__Func_92054
	bl	OvlFunc_5c5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_55b0

@ 6 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_5c5c
.thumb_func_start OvlFunc_55c0
	push	{lr}
	mov	r0, #0
	bl	__Func_92054
	bl	OvlFunc_5c5c
	pop	{r0}
	bx	r0
.func_end OvlFunc_55c0

@ 6 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_5c98
.thumb_func_start OvlFunc_55d0
	push	{lr}
	mov	r0, #9
	bl	__Func_92054
	bl	OvlFunc_5c98
	pop	{r0}
	bx	r0
.func_end OvlFunc_55d0

@ 6 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_5c98
.thumb_func_start OvlFunc_55e0
	push	{lr}
	mov	r0, #0x17
	bl	__Func_92054
	bl	OvlFunc_5c98
	pop	{r0}
	bx	r0
.func_end OvlFunc_55e0

@ 6 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_5c98
.thumb_func_start OvlFunc_55f0
	push	{lr}
	mov	r0, #0x18
	bl	__Func_92054
	bl	OvlFunc_5c98
	pop	{r0}
	bx	r0
.func_end OvlFunc_55f0

@ 6 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_5c98
.thumb_func_start OvlFunc_5600
	push	{lr}
	mov	r0, #0x19
	bl	__Func_92054
	bl	OvlFunc_5c98
	pop	{r0}
	bx	r0
.func_end OvlFunc_5600

@ Distance3D
@ Two {x, y, z} triples of 16.16 words. Each axis difference is taken down to
@ 1/16 units with `asr #16`, squared and summed -- the same helper documented
@ in overlays/rom_780898/ovl_30.s.
.thumb_func_start OvlFunc_5610
	push	{r5, lr}
	ldmia	r0!, {r5}
	ldmia	r1!, {r3}
	ldmia	r0!, {r4}
	sub	r5, r3
	ldmia	r1!, {r3}
	ldr	r2, [r1]
	sub	r4, r3
	ldr	r3, [r0]
	sub	r3, r2
	asr	r5, #16
	asr	r4, #16
	asr	r3, #16
	mov	r0, r5
	mul	r0, r5
	mov	r2, r4
	mul	r2, r4
	mov	r1, r3
	mul	r1, r3
	add	r0, r2
	mov	r3, r1
	add	r0, r3
	ldr	r3, =Func_948
	bl	_call_via_r3
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_5610

@ 105 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetEntityAnimation, OvlFunc_5610, Atan2, SetEntityAnimation x2
.thumb_func_start OvlFunc_564c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #8
	str	r2, [sp, #4]
	mov	r2, #0
	mov	r5, r0
	mov	r6, r3
	str	r2, [sp]
	mov	r3, #0x5b
	add	r3, r5
	mov	r10, r3
	ldrb	r3, [r3]
	mov	r11, r1
	cmp	r3, #1
	bne	.L568a
	mov	r2, #0x62
	add	r2, r5
	ldrb	r3, [r2]
	mov	r9, r2
	cmp	r3, #0
	bne	.L5690
	mov	r1, #1
	bl	__Func_c300
	mov	r0, #1
	b	.L5716
.L568a:
	mov	r3, #0x62
	add	r3, r5
	mov	r9, r3
.L5690:
	mov	r2, #8
	add	r2, r5
	mov	r7, r11
	mov	r8, r2
	add	r7, #8
	mov	r0, r7
	mov	r1, r8
	bl	OvlFunc_5610
	ldr	r3, [sp, #4]
	cmp	r0, r3
	blt	.L56ac
	cmp	r6, #0
	beq	.L5704
.L56ac:
	mov	r2, r11
	ldr	r0, [r2, #0x10]
	ldr	r3, [r5, #0x10]
	mov	r2, r8
	sub	r0, r3
	ldr	r1, [r7]
	ldr	r3, [r2]
	sub	r1, r3
	bl	__Func_44d0
	ldr	r3, =0xfffff000
	lsl	r0, #16
	mov	r2, #0x80
	lsr	r0, #16
	lsl	r2, #5
	add	r4, r0, r3
	add	r1, r0, r2
	mov	r3, #0xf0
	ldrh	r2, [r5, #6]
	lsl	r3, #8
	and	r4, r3
	and	r1, r3
	and	r0, r3
	and	r3, r2
	cmp	r0, r3
	beq	.L56ec
	cmp	r1, r3
	beq	.L56ec
	cmp	r4, r3
	beq	.L56ec
	cmp	r6, #0
	beq	.L5704
.L56ec:
	mov	r3, #1
	mov	r2, r10
	strb	r3, [r2]
	mov	r0, r5
	mov	r1, #1
	bl	__Func_c300
	mov	r3, #1
	mov	r2, r9
	str	r3, [sp]
	strb	r3, [r2]
	b	.L5714
.L5704:
	mov	r3, r10
	strb	r6, [r3]
	mov	r0, r5
	mov	r1, #2
	bl	__Func_c300
	mov	r2, r9
	strb	r6, [r2]
.L5714:
	ldr	r0, [sp]
.L5716:
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_564c

@ 22 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, OvlFunc_564c
.thumb_func_start OvlFunc_572c
	push	{r5, lr}
	mov	r5, r0
	mov	r0, #0
	bl	__Func_92054
	mov	r3, #0x80
	ldr	r2, [r5, #0x38]
	lsl	r3, #24
	mov	r1, r0
	cmp	r2, r3
	bne	.L574a
	ldr	r3, [r5, #0x40]
	mov	r0, #0
	cmp	r3, r2
	beq	.L5756
.L574a:
	mov	r0, r5
	mov	r2, #0x12
	mov	r3, #0
	bl	OvlFunc_564c
	mov	r0, #0
.L5756:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_572c

@ 63 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random x3, RotateVector, SpawnEntity, SetEntityActorOptions
@   SetEntityAnimation, SetEntityScript, PlaySound
.thumb_func_start OvlFunc_575c
	push	{r5, r6, lr}
	mov	r6, r0
	sub	sp, #0xc
	ldr	r3, [r6, #8]
	mov	r5, sp
	str	r3, [r5]
	bl	__Func_4458
	ldr	r3, [r6, #0xc]
	ldr	r2, =0xfff80000
	lsl	r0, #4
	sub	r3, r0
	add	r3, r2
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	str	r3, [r5, #8]
	bl	__Func_4458
	mov	r6, r0
	bl	__Func_4458
	mov	r1, r0
	lsl	r0, r6, #1
	add	r0, r6
	mov	r2, r5
	lsl	r0, #4
	bl	__Func_447c
	ldr	r1, [r5]
	ldr	r2, [r5, #4]
	ldr	r3, [r5, #8]
	ldr	r0, =0x11d
	bl	__Func_c150
	mov	r5, r0
	cmp	r5, #0
	beq	.L57de
	mov	r2, r5
	add	r2, #0x55
	mov	r3, #2
	strb	r3, [r2]
	ldr	r3, =0x1999
	add	r2, #9
	str	r3, [r5, #0x48]
	mov	r3, #0xc
	strh	r3, [r2]
	mov	r1, #0
	bl	__Func_c528
	mov	r0, r5
	mov	r1, #0
	bl	__Func_c300
	ldr	r1, =.L66e0
	mov	r0, r5
	bl	__Func_c2d8
	ldr	r1, [r5, #0x50]
	mov	r3, #0xd
	ldrb	r2, [r1, #9]
	neg	r3, r3
	and	r3, r2
	mov	r2, #4
	orr	r3, r2
	strb	r3, [r1, #9]
.L57de:
	mov	r0, #0x8a
	bl	__Func_f9080
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_575c

@ 102 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, Func_30f8, SpawnEntity, SetEntityActorOptions
@   SetEntityScript, Random x5, OvlFunc_58f0, PlaySound
.thumb_func_start OvlFunc_57fc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r0, #0x9a
	bl	__Func_f9080
	ldr	r5, =0xfffff800
	mov	r2, #0x1e
	mov	r8, r2
.L5810:
	ldr	r3, [r7, #0xc]
	mov	r2, #0x80
	lsl	r2, #9
	add	r3, r2
	str	r3, [r7, #0xc]
	mov	r2, #0x80
	ldrh	r3, [r7, #6]
	lsl	r2, #6
	add	r3, r2
	strh	r3, [r7, #6]
	ldr	r3, [r7, #0x18]
	add	r3, r5
	str	r3, [r7, #0x18]
	ldr	r3, [r7, #0x1c]
	add	r3, r5
	str	r3, [r7, #0x1c]
	mov	r0, #1
	bl	__Func_30f8
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r2, r8
	cmp	r2, #0
	bge	.L5810
	mov	r3, #7
	mov	r8, r3
.L5846:
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0xc]
	ldr	r3, [r7, #0x10]
	ldr	r0, =0x11d
	bl	__Func_c150
	mov	r6, r0
	cmp	r6, #0
	beq	.L58ac
	mov	r1, #0
	bl	__Func_c528
	ldr	r1, =.L66e4
	mov	r0, r6
	bl	__Func_c2d8
	bl	__Func_4458
	mov	r3, #0x80
	lsl	r3, #9
	mov	r2, r6
	add	r2, #0x55
	add	r0, r3
	str	r3, [r6, #0x34]
	mov	r3, #2
	str	r0, [r6, #0x30]
	strb	r3, [r2]
	ldr	r3, =0xa3d
	str	r3, [r6, #0x48]
	bl	__Func_4458
	mov	r5, r0
	bl	__Func_4458
	sub	r5, r0
	str	r5, [r6, #0x28]
	bl	__Func_4458
	lsl	r5, r0, #1
	add	r5, r0
	mov	r2, #0x80
	lsl	r2, #12
	lsl	r5, #3
	add	r5, r2
	bl	__Func_4458
	mov	r1, r5
	mov	r2, r0
	mov	r0, r6
	bl	OvlFunc_58f0
.L58ac:
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r2, r8
	cmp	r2, #0
	bge	.L5846
	mov	r0, #0x83
	bl	__Func_f9080
	mov	r3, #0x80
	mov	r2, #0
	lsl	r3, #24
	str	r2, [r7, #8]
	str	r2, [r7, #0xc]
	str	r2, [r7, #0x10]
	str	r3, [r7, #0x38]
	str	r3, [r7, #0x3c]
	str	r3, [r7, #0x40]
	str	r2, [r7, #0x24]
	str	r2, [r7, #0x28]
	str	r2, [r7, #0x2c]
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_57fc

@ 25 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   RotateVector, SetEntityMoveTarget
.thumb_func_start OvlFunc_58f0
	push	{r5, r6, lr}
	mov	r6, r0
	sub	sp, #0xc
	mov	r0, r1
	mov	r1, r2
	cmp	r6, #0
	beq	.L591e
	ldr	r3, [r6, #8]
	mov	r5, sp
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	mov	r2, r5
	str	r3, [r5, #8]
	bl	__Func_447c
	ldr	r1, [r5]
	ldr	r2, [r5, #4]
	ldr	r3, [r5, #8]
	mov	r0, r6
	bl	__Func_d14c
.L591e:
	add	sp, #0xc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_58f0

@ 16 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectAttributes, OvlFunc_c4, OvlFunc_5950
.thumb_func_start OvlFunc_5928
	push	{lr}
	sub	sp, #8
	mov	r3, #0x16
	mov	r2, #0x24
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x11
	mov	r1, #0
	mov	r2, #3
	mov	r3, #1
	bl	__Func_10704
	bl	OvlFunc_c4
	bl	OvlFunc_5950
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_5928

@ 73 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectAttributes, TestSaveBit, GetSlotEntityChecked, ClearSaveBit x3
@   CopyMapRectAttributes, SetSaveBit, CopyMapRectAttributes, SetSaveBit
@   CopyMapRectAttributes, SetSaveBit
@ reads save bit 0x87a; sets 0x314, 0x315, 0x316; clears 0x314, 0x315, 0x316.
.thumb_func_start OvlFunc_5950
	push	{r5, lr}
	sub	sp, #8
	mov	r3, #0x16
	mov	r2, #0x24
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x11
	mov	r1, #0
	mov	r2, #3
	mov	r3, #1
	bl	__Func_10704
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	beq	.L5976
	mov	r0, #0x15
	b	.L5978
.L5976:
	mov	r0, #0x14
.L5978:
	bl	__Func_92054
	mov	r5, r0
	cmp	r5, #0
	beq	.L59f2
	mov	r0, #0xc5
	lsl	r0, #2
	bl	__Func_79374
	ldr	r0, =0x315
	bl	__Func_79374
	ldr	r0, =0x316
	bl	__Func_79374
	ldr	r3, [r5, #8]
	asr	r0, r3, #20
	cmp	r0, #0x16
	bne	.L59ba
	mov	r3, #0x24
	str	r0, [sp]
	str	r3, [sp, #4]
	mov	r0, #0x11
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	mov	r0, #0xc5
	lsl	r0, #2
	bl	__Func_79358
	b	.L59f2
.L59ba:
	cmp	r0, #0x17
	bne	.L59d8
	mov	r3, #0x24
	str	r0, [sp]
	str	r3, [sp, #4]
	mov	r0, #0x11
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	ldr	r0, =0x315
	bl	__Func_79358
	b	.L59f2
.L59d8:
	mov	r3, #0x18
	mov	r2, #0x24
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x11
	mov	r1, #1
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
	ldr	r0, =0x316
	bl	__Func_79358
.L59f2:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5950

@ 11 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound
.thumb_func_start OvlFunc_5a08
	push	{lr}
	ldr	r3, =iwram_1e40
	ldr	r3, [r3]
	mov	r2, #0xf
	and	r3, r2
	cmp	r3, #0
	bne	.L5a1c
	mov	r0, #0x83
	bl	__Func_f9080
.L5a1c:
	pop	{r0}
	bx	r0
.func_end OvlFunc_5a08

@ Text box: string 0xee4 through Func_1776c, option bits 0x1.
@ No speaker and no staging -- a sign, notice or narration line.
.thumb_func_start OvlFunc_5a24
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0xee4
	mov	r1, #1
	bl	__Func_1776c
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_5a24

@ 33 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   TestSaveBit x2, GetSlotEntityChecked, ClearSaveBit, TestSaveBit
@   GetSlotEntityChecked, SetSaveBit
@ reads save bits 0x106, 0x241; sets 0x241; clears 0x241.
.thumb_func_start OvlFunc_5a40
	push	{r5, lr}
	ldr	r0, =0x241
	bl	__Func_79338
	cmp	r0, #0
	beq	.L5a6c
	mov	r0, #0x83
	lsl	r0, #1
	bl	__Func_79338
	mov	r5, r0
	cmp	r5, #0
	bne	.L5a8a
	mov	r0, #0x16
	bl	__Func_92054
	add	r0, #0x5b
	strb	r5, [r0]
	ldr	r0, =0x241
	bl	__Func_79374
	b	.L5a8a
.L5a6c:
	mov	r0, #0x83
	lsl	r0, #1
	bl	__Func_79338
	cmp	r0, #0
	beq	.L5a8a
	mov	r0, #0x16
	bl	__Func_92054
	mov	r3, #1
	add	r0, #0x5b
	strb	r3, [r0]
	ldr	r0, =0x241
	bl	__Func_79358
.L5a8a:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5a40

@ 31 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, TestSaveBit, GetSlotEntityChecked x2
@ reads save bit 0x87a.
.thumb_func_start OvlFunc_5a94
	push	{r5, lr}
	mov	r0, #0
	bl	__Func_92054
	mov	r5, r0
	ldr	r0, =0x87a
	bl	__Func_79338
	cmp	r0, #0
	beq	.L5ab0
	mov	r0, #0x15
	bl	__Func_92054
	b	.L5ab6
.L5ab0:
	mov	r0, #0x14
	bl	__Func_92054
.L5ab6:
	cmp	r0, #0
	beq	.L5ad4
	mov	r2, #0xc8
	ldr	r3, [r5, #0xc]
	lsl	r2, #16
	cmp	r3, r2
	ble	.L5acc
	mov	r2, r0
	add	r2, #0x23
	mov	r3, #3
	b	.L5ad2
.L5acc:
	mov	r2, r0
	add	r2, #0x23
	mov	r3, #1
.L5ad2:
	strb	r3, [r2]
.L5ad4:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5a94

@ 46 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Sin, Cos x2, Random x2
.thumb_func_start OvlFunc_5ae0
	push	{r5, r6, r7, lr}
	mov	r6, r0
	ldr	r0, [r6, #0x30]
	ldr	r7, [r6, #0x50]
	bl	__Func_2322
	lsl	r5, r0, #1
	cmp	r5, #0
	ble	.L5af4
	neg	r5, r5
.L5af4:
	ldr	r0, [r6, #0x30]
	bl	__Func_231c
	ldr	r3, [r6, #0x38]
	lsl	r0, #1
	add	r3, r0
	str	r3, [r6, #8]
	ldr	r0, [r6, #0x30]
	ldr	r3, [r6, #0x3c]
	mov	r2, #0x80
	lsl	r2, #8
	add	r3, r5
	add	r0, r2
	str	r3, [r6, #0xc]
	bl	__Func_231c
	cmp	r0, #0
	bge	.L5b1a
	add	r0, #7
.L5b1a:
	asr	r3, r0, #3
	strh	r3, [r7, #0x1e]
	bl	__Func_4458
	mov	r5, r0
	bl	__Func_4458
	lsl	r5, #9
	lsl	r0, #9
	ldr	r3, [r6, #0x30]
	lsr	r0, #16
	lsr	r5, #16
	add	r5, r0
	mov	r2, #0x80
	add	r3, r5
	lsl	r2, #3
	add	r3, r2
	str	r3, [r6, #0x30]
	mov	r0, #0
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_5ae0

@ 97 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, SetEntityActorOptions, TestSaveBit, Func_48b0
@   SetPortraitPointer, AllocObjTiles, Func_2dd8
@ reads save bit 0x109.
.thumb_func_start OvlFunc_5b48
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	bl	__Func_92054
	mov	r7, r0
	ldr	r6, [r7, #0x50]
	mov	r2, #0xd
	ldrb	r3, [r6, #9]
	neg	r2, r2
	and	r2, r3
	mov	r3, #4
	ldrb	r1, [r6, #5]
	orr	r2, r3
	mov	r3, #0x21
	neg	r3, r3
	and	r3, r1
	strb	r3, [r6, #5]
	mov	r3, #0xf
	and	r2, r3
	strb	r2, [r6, #9]
	mov	r2, #0
	mov	r8, r2
	mov	r3, r6
	add	r3, #0x27
	mov	r2, r8
	strb	r2, [r3]
	mov	r1, #0
	bl	__Func_c528
	mov	r3, #0x5c
	add	r3, r7
	mov	r2, r8
	strb	r2, [r3]
	mov	r10, r3
	mov	r3, r7
	add	r3, #0x55
	strb	r2, [r3]
	ldr	r0, =0x109
	bl	__Func_79338
	cmp	r0, #0
	bne	.L5bac
	ldr	r3, [r7, #0xc]
	mov	r2, #0x80
	lsl	r2, #14
	add	r3, r2
	str	r3, [r7, #0xc]
.L5bac:
	mov	r1, r7
	add	r1, #0x23
	ldrb	r2, [r1]
	mov	r3, #0xfe
	and	r3, r2
	mov	r2, #1
	strb	r3, [r1]
	mov	r9, r2
	mov	r3, r7
	mov	r2, r9
	add	r3, #0x61
	mov	r1, #0xc1
	strb	r2, [r3]
	lsl	r1, #3
	mov	r0, #0x11
	bl	__Func_48b0
	mov	r5, r0
	mov	r0, #0xb5
	bl	__Func_1a370
	mov	r3, #0x80
	lsl	r3, #3
	add	r5, r3
	mov	r2, r5
	mov	r1, #0x80
	ldrb	r0, [r6, #0x1c]
	bl	__Func_3fa4
	mov	r0, #0x11
	bl	__Func_2dd8
	ldr	r3, [r7, #8]
	str	r3, [r7, #0x38]
	ldr	r3, [r7, #0xc]
	mov	r2, r8
	str	r2, [r7, #0x30]
	str	r3, [r7, #0x3c]
	mov	r2, r10
	mov	r3, r9
	strb	r3, [r2]
	ldr	r3, =OvlFunc_5ae0
	str	r3, [r7, #0x6c]
	mov	r3, r7
	add	r3, #0x56
	mov	r2, r8
	strb	r2, [r3]
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5b48

@ 25 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetEntityPalette x2, OvlFunc_5d68
.thumb_func_start OvlFunc_5c20
	push	{r5, lr}
	ldr	r3, =iwram_1e40
	ldr	r3, [r3]
	mov	r2, #2
	and	r3, r2
	mov	r5, r0
	cmp	r3, #0
	beq	.L5c38
	mov	r1, #7
	bl	__Func_c598
	b	.L5c40
.L5c38:
	mov	r0, r5
	mov	r1, #0
	bl	__Func_c598
.L5c40:
	ldr	r3, =iwram_1e40
	ldr	r3, [r3]
	mov	r2, #0xf
	and	r3, r2
	cmp	r3, #0
	bne	.L5c52
	mov	r0, r5
	bl	OvlFunc_5d68
.L5c52:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5c20

@ 25 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   UnsignedRem, SetEntityPalette, OvlFunc_5d68
.thumb_func_start OvlFunc_5c5c
	push	{r5, r6, lr}
	ldr	r5, =iwram_1e40
	ldr	r3, [r5]
	mov	r2, #1
	and	r3, r2
	mov	r6, r0
	cmp	r3, #0
	beq	.L5c7e
	ldr	r0, [r5]
	mov	r1, #6
	lsr	r0, #1
	bl	_Func_b50
	mov	r1, r0
	mov	r0, r6
	bl	__Func_c598
.L5c7e:
	ldr	r3, [r5]
	mov	r2, #0xf
	and	r3, r2
	cmp	r3, #0
	bne	.L5c8e
	mov	r0, r6
	bl	OvlFunc_5d68
.L5c8e:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5c5c

@ 18 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   UnsignedRem, SetEntityPalette
.thumb_func_start OvlFunc_5c98
	push	{r5, lr}
	mov	r5, r0
	ldr	r0, =iwram_1e40
	ldr	r3, [r0]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L5cba
	ldr	r0, [r0]
	mov	r1, #6
	lsr	r0, #1
	bl	_Func_b50
	mov	r1, r0
	mov	r0, r5
	bl	__Func_c598
.L5cba:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5c98

@ 38 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   DestroyEntity, Sin
.thumb_func_start OvlFunc_5cc4
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r2, r5
	add	r2, #0x64
	ldrh	r3, [r2]
	add	r3, #1
	ldr	r6, [r5, #0x68]
	strh	r3, [r2]
	lsl	r3, #16
	asr	r0, r3, #16
	cmp	r0, #0x1f
	ble	.L5ce4
	mov	r0, r5
	bl	__Func_c0f4
	b	.L5d0e
.L5ce4:
	lsl	r0, #10
	bl	__Func_2322
	str	r0, [r5, #0x18]
	str	r0, [r5, #0x1c]
	ldr	r3, [r6, #8]
	mov	r1, #0x80
	str	r3, [r5, #8]
	ldr	r3, [r5, #0xc]
	lsl	r1, #9
	add	r3, r1
	str	r3, [r5, #0xc]
	sub	r1, r0
	ldr	r3, [r6, #0x10]
	lsl	r2, r1, #2
	add	r2, r1
	add	r3, r2
	mov	r2, #0x80
	lsl	r2, #12
	add	r3, r2
	str	r3, [r5, #0x10]
.L5d0e:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5cc4

@ 39 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   DestroyEntity, Sin
.thumb_func_start OvlFunc_5d14
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r2, r5
	add	r2, #0x64
	ldrh	r3, [r2]
	add	r3, #1
	ldr	r6, [r5, #0x68]
	strh	r3, [r2]
	lsl	r3, #16
	asr	r0, r3, #16
	cmp	r0, #0x1f
	ble	.L5d34
	mov	r0, r5
	bl	__Func_c0f4
	b	.L5d60
.L5d34:
	lsl	r0, #10
	bl	__Func_2322
	neg	r3, r0
	str	r0, [r5, #0x18]
	str	r3, [r5, #0x1c]
	ldr	r3, [r6, #8]
	mov	r1, #0x80
	str	r3, [r5, #8]
	ldr	r3, [r5, #0xc]
	lsl	r1, #9
	add	r3, r1
	str	r3, [r5, #0xc]
	sub	r1, r0
	ldr	r3, [r6, #0x10]
	lsl	r2, r1, #2
	add	r2, r1
	sub	r3, r2
	mov	r2, #0x80
	lsl	r2, #13
	add	r3, r2
	str	r3, [r5, #0x10]
.L5d60:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5d14

@ 126 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SpawnEntity, SetActorAnimation, FreeObjTiles
.thumb_func_start OvlFunc_5d68
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f30
	ldr	r3, [r3]
	sub	sp, #8
	mov	r1, #0x3f
	mov	r6, r0
	mov	r11, r3
	mov	r7, #0
	mov	r10, sp
	mov	r9, r1
.L5d88:
	ldr	r2, [r6, #0xc]
	ldr	r3, [r6, #0x10]
	ldr	r1, [r6, #8]
	mov	r0, #0x1a
	bl	__Func_c150
	lsl	r3, r7, #2
	mov	r2, r10
	str	r0, [r3, r2]
	cmp	r0, #0
	beq	.L5e34
	ldr	r3, [r6, #0x14]
	str	r3, [r0, #0x14]
	mov	r3, r0
	ldr	r5, [r0, #0x50]
	add	r3, #0x55
	mov	r2, #0
	ldr	r1, .L5dbc	@ 0
	strb	r2, [r3]
	add	r3, #0xf
	strh	r2, [r3]
	mov	r8, r1
	str	r6, [r0, #0x68]
	cmp	r5, #0
	beq	.L5e34
	b	.L5dc4

	.align	2, 0
.L5dbc:
	.word	0
	.pool

.L5dc4:
	mov	r1, #0
	mov	r0, r5
	bl	__Func_ba30
	mov	r3, r5
	add	r3, #0x26
	mov	r2, r8
	strb	r2, [r3]
	ldrb	r0, [r5, #0x1c]
	bl	__Func_3f3c
	mov	r3, r11
	add	r3, #0x46
	ldrh	r3, [r3]
	strb	r3, [r5, #0x1c]
	ldrb	r3, [r5, #0x1d]
	mov	r2, #1
	orr	r3, r2
	strb	r3, [r5, #0x1d]
	ldrb	r3, [r5, #0x1c]
	ldr	r2, =iwram_1b10
	lsl	r3, #2
	add	r3, r2
	ldrh	r1, [r3, #2]
	ldr	r2, .L5e2c	@ 0xfffffc00
	ldrh	r3, [r5, #8]
	lsl	r1, #17
	lsr	r1, #22
	and	r3, r2
	orr	r3, r1
	mov	r1, #0x21
	neg	r1, r1
	strh	r3, [r5, #8]
	ldrb	r3, [r5, #5]
	mov	r2, r1
	and	r3, r2
	mov	r2, r9
	and	r3, r2
	mov	r2, #0x40
	orr	r3, r2
	ldrb	r2, [r5, #7]
	strb	r3, [r5, #5]
	mov	r3, r9
	and	r3, r2
	mov	r2, #0x80
	orr	r3, r2
	strb	r3, [r5, #7]
	ldr	r3, [r5, #0x28]
	mov	r1, r8
	strb	r1, [r3, #0x16]
	b	.L5e34

	.align	2, 0
.L5e2c:
	.word	0xfffffc00
	.pool

.L5e34:
	add	r7, #1
	cmp	r7, #1
	ble	.L5d88
	ldr	r2, [sp]
	ldr	r3, =OvlFunc_5d14
	ldr	r0, [r2, #0x50]
	str	r3, [r2, #0x6c]
	mov	r2, #0xd
	ldrb	r1, [r0, #9]
	neg	r2, r2
	mov	r3, r2
	mov	r4, #4
	and	r3, r1
	orr	r3, r4
	strb	r3, [r0, #9]
	mov	r3, r10
	ldr	r1, [r3, #4]
	ldr	r0, [r1, #0x50]
	ldrb	r3, [r0, #9]
	and	r2, r3
	ldr	r3, =OvlFunc_5cc4
	orr	r2, r4
	str	r3, [r1, #0x6c]
	add	r1, #0x23
	mov	r3, #2
	strb	r2, [r0, #9]
	strb	r3, [r1]
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_5d68

	.section .data

.L6190:
	.incbin "overlays/rom_780898/orig.bin", 0x6190, (0x61d0-0x6190)
.L61d0:
	.incbin "overlays/rom_780898/orig.bin", 0x61d0, (0x61e8-0x61d0)
.L61e8:
	.incbin "overlays/rom_780898/orig.bin", 0x61e8, (0x6248-0x61e8)
.L6248:
	.incbin "overlays/rom_780898/orig.bin", 0x6248, (0x6590-0x6248)
.L6590:
	.incbin "overlays/rom_780898/orig.bin", 0x6590, (0x65cc-0x6590)
.L65cc:
	.incbin "overlays/rom_780898/orig.bin", 0x65cc, (0x6614-0x65cc)
.L6614:
	.incbin "overlays/rom_780898/orig.bin", 0x6614, (0x665c-0x6614)
.L665c:
	.incbin "overlays/rom_780898/orig.bin", 0x665c, (0x66e0-0x665c)
.L66e0:
	.incbin "overlays/rom_780898/orig.bin", 0x66e0, (0x66e4-0x66e0)
.L66e4:
	.incbin "overlays/rom_780898/orig.bin", 0x66e4, (0x6708-0x66e4)
.L6708:
	.incbin "overlays/rom_780898/orig.bin", 0x6708, (0x6870-0x6708)
.L6870:
	.incbin "overlays/rom_780898/orig.bin", 0x6870, (0x68a8-0x6870)
.L68a8:
	.incbin "overlays/rom_780898/orig.bin", 0x68a8, (0x6ab8-0x68a8)
.L6ab8:
	.incbin "overlays/rom_780898/orig.bin", 0x6ab8, (0x6cc8-0x6ab8)
.L6cc8:
	.incbin "overlays/rom_780898/orig.bin", 0x6cc8, (0x6e48-0x6cc8)
.L6e48:
	.incbin "overlays/rom_780898/orig.bin", 0x6e48, (0x6f38-0x6e48)
.L6f38:
	.incbin "overlays/rom_780898/orig.bin", 0x6f38, (0x7100-0x6f38)
.L7100:
	.incbin "overlays/rom_780898/orig.bin", 0x7100, (0x7334-0x7100)
.L7334:
	.incbin "overlays/rom_780898/orig.bin", 0x7334, (0x7544-0x7334)
.L7544:
	.incbin "overlays/rom_780898/orig.bin", 0x7544, (0x755a-0x7544)
.L755a:
	.incbin "overlays/rom_780898/orig.bin", 0x755a, (0x7570-0x755a)
.L7570:
	.incbin "overlays/rom_780898/orig.bin", 0x7570, (0x7586-0x7570)
.L7586:
	.incbin "overlays/rom_780898/orig.bin", 0x7586, (0x759c-0x7586)
.L759c:
	.incbin "overlays/rom_780898/orig.bin", 0x759c, (0x75ec-0x759c)
.L75ec:
	.incbin "overlays/rom_780898/orig.bin", 0x75ec, (0x763c-0x75ec)
.L763c:
	.incbin "overlays/rom_780898/orig.bin", 0x763c, (0x76cc-0x763c)
.L76cc:
	.incbin "overlays/rom_780898/orig.bin", 0x76cc, (0x7748-0x76cc)
.L7748:
	.incbin "overlays/rom_780898/orig.bin", 0x7748, (0x77c4-0x7748)
.L77c4:
	.incbin "overlays/rom_780898/orig.bin", 0x77c4

	.include "macros.inc"

@ ============================================================================
@ Map transitions and surface effects.
@
@ The destination of a transition is looked up from map data rather than named
@ directly: .L9c610 is a table of 14 halfwords per map group, indexed by
@ (group * 14 + entrance) * 2 + 4 (Func_8b05c). ewram_240 holds the pending
@ transition -- +0x1C0/+0x1C2 the target map and entrance, +0x1D6 the current
@ map id -- and Func_8b320 commits it.
@ The surface routines at the top resolve what the player is standing on
@ (through _Func_122c8, the rom_9000 footstep lookup) and play the matching
@ step sound.
@ ============================================================================

@ ApplyMapEntry
@ Takes no arguments. Runs after a map load to place the player at the entrance
@ recorded in ewram_240+0x1C0: resolves the entrance record, writes the spawn
@ position and facing into the scene block, and applies any entry script the
@ entrance carries. The ~110-instruction body is characterised structurally.
.thumb_func_start Func_8ace0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	ldr	r2, =ewram_240
	ldr	r3, [r3]
	mov	r1, #0xe0
	lsl	r1, #1
	mov	r10, r3
	add	r3, r2, r1
	mov	r1, #0
	ldrsh	r4, [r3, r1]
	mov	r11, r4
	mov	r4, #0xe1
	lsl	r4, #1
	add	r3, r2, r4
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	mov	r6, #0xd0
	sub	sp, #8
	lsl	r6, #1
	mov	r3, #0
	mov	r9, r1
	add	r6, r10
	ldr	r5, =.L9d170
	mov	r7, #0
	str	r3, [sp, #4]
	cmp	r0, #0
	beq	.L8ad80
	mov	r1, #1
	mov	r4, #0
	ldrsh	r3, [r5, r4]
	neg	r1, r1
	ldrh	r2, [r5]
	cmp	r3, r1
	beq	.L8ad80
	mov	r8, r1
	ldr	r1, =0x7fff
.L8ad34:
	lsl	r3, r2, #16
	asr	r3, #16
	cmp	r3, r11
	bne	.L8ad74
	mov	r0, #2
	ldrsh	r3, [r5, r0]
	cmp	r3, r8
	beq	.L8ad48
	cmp	r3, r9
	bne	.L8ad74
.L8ad48:
	ldrh	r0, [r5, #4]
	mov	r3, r1
	and	r3, r0
	cmp	r3, r1
	beq	.L8ad62
	lsl	r0, #17
	asr	r0, #17
	str	r1, [sp]
	bl	_Func_79338
	ldr	r1, [sp]
	cmp	r0, #0
	bne	.L8ad74
.L8ad62:
	ldrb	r3, [r5, #5]
	lsl	r3, #24
	asr	r3, #31
	lsl	r3, #16
	asr	r3, #16
	mov	r1, #6
	ldrsh	r7, [r5, r1]
	str	r3, [sp, #4]
	b	.L8ad80
.L8ad74:
	add	r5, #8
	mov	r4, #0
	ldrsh	r3, [r5, r4]
	ldrh	r2, [r5]
	cmp	r3, r8
	bne	.L8ad34
.L8ad80:
	mov	r3, #0
	strb	r3, [r6]
	add	r6, #1
	mov	r3, #0
.L8ad88:
	strb	r7, [r6]
	add	r6, #1
	cmp	r7, #0
	beq	.L8ad92
	add	r7, #1
.L8ad92:
	add	r3, #1
	cmp	r3, #6
	bls	.L8ad88
	ldr	r0, [sp, #4]
	cmp	r0, #0
	beq	.L8adb0
	ldr	r0, =0x1a1
	mov	r2, #0xd0
	add	r0, r10
	ldrb	r3, [r0]
	lsl	r2, #1
	add	r2, r10
	mov	r1, #0
	strb	r3, [r2]
	strb	r1, [r0]
.L8adb0:
	mov	r2, #0xd4
	lsl	r2, #1
	add	r2, r10
	mov	r3, #0
	str	r3, [r2]
	mov	r2, #0xd6
	lsl	r2, #1
	mov	r3, #0x80
	add	r2, r10
	lsl	r3, #13
	str	r3, [r2]
	bl	Func_8b25c
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8ace0

@ GetSurfaceTypeAtPosition
@ r0=position vec3. Returns the surface type index under that point.
@ Calls _Func_122c8 (the rom_9000 footstep lookup, which combines the terrain
@ material with the tile's type bits) and maps its result through the table at
@ .L9d7a8 to the smaller set of surface classes this module uses for sounds.
.thumb_func_start Func_8adf0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	sub	sp, #4
	mov	r1, sp
	ldr	r5, =.L9d7a8
	bl	_Func_122c8
	mov	r1, #0
	mov	r10, r1
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	ldr	r1, =0xffff0000
	mov	r6, r0
	asr	r0, r1, #16
	ldrh	r2, [r5]
	cmp	r3, r0
	beq	.L8ae56
	mov	r7, r1
	mov	r8, r0
.L8ae1a:
	lsl	r3, r2, #16
	ldr	r2, [sp]
	asr	r3, #16
	cmp	r3, r2
	bne	.L8ae4a
	mov	r2, #2
	ldrsh	r3, [r5, r2]
	asr	r2, r7, #16
	cmp	r3, r2
	beq	.L8ae32
	cmp	r3, r6
	bne	.L8ae4a
.L8ae32:
	mov	r3, #4
	ldrsh	r0, [r5, r3]
	cmp	r0, r2
	beq	.L8ae42
	bl	_Func_79338
	cmp	r0, #0
	bne	.L8ae4a
.L8ae42:
	mov	r2, #6
	ldrsh	r1, [r5, r2]
	mov	r10, r1
	b	.L8ae56
.L8ae4a:
	add	r5, #8
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	ldrh	r2, [r5]
	cmp	r3, r8
	bne	.L8ae1a
.L8ae56:
	mov	r0, r6
	bl	Func_8b2b0
	mov	r0, r10
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8adf0

@ PlaySurfaceStepEffect
@ r0=surface type, r1=variant. Plays the footstep sound and any splash/dust
@ effect for that surface. Event flag 0x15F gates the whole thing, so the
@ effects can be suppressed during cutscenes.
@ The ~110-instruction body dispatches per surface type; the flag gate and the
@ argument roles are verified, the individual arms are not enumerated.
.thumb_func_start Func_8ae74
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #8
	str	r0, [sp, #4]
	str	r1, [sp]
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	ldr	r0, =0x15f
	mov	r11, r3
	bl	_Func_79338
	cmp	r0, #0
	beq	.L8ae9a
	b	.L8afa0
.L8ae9a:
	mov	r0, #0xb0
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	bne	.L8af26
	ldr	r0, =0x161
	bl	_Func_79338
	cmp	r0, #0
	bne	.L8af26
	ldr	r2, [sp, #4]
	mov	r0, #0
	cmp	r2, #0
	bne	.L8aeba
	b	.L8affc
.L8aeba:
	ldr	r3, =ewram_240
	mov	r2, #0x93
	lsl	r2, #2
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0
	beq	.L8aecc
	b	.L8affc
.L8aecc:
	ldr	r2, [sp, #4]
	lsl	r3, r2, #3
	sub	r3, r2
	ldr	r2, =.L9c610
	lsl	r3, #2
	add	r3, r2
	ldrh	r7, [r3]
	mov	r9, r3
	cmp	r7, #0
	bne	.L8aee2
	b	.L8affc
.L8aee2:
	mov	r0, #5
	bl	_Func_79338
	cmp	r0, #0
	beq	.L8aefe
	mov	r0, #5
	bl	_Func_77394
	mov	r3, #0x92
	lsl	r3, #1
	add	r0, r3
	ldr	r0, [r0]
	cmp	r0, #0x82
	bgt	.L8af26
.L8aefe:
	bl	_Func_77348
	mov	r2, r9
	ldrh	r3, [r2, #2]
	sub	r0, r3
	cmp	r0, #0
	bge	.L8af0e
	mov	r0, #0
.L8af0e:
	cmp	r0, #5
	ble	.L8af14
	mov	r0, #5
.L8af14:
	cmp	r0, #0
	ble	.L8af2a
	ldr	r3, =ewram_240
	mov	r2, #0x91
	lsl	r2, #2
	add	r3, r2
	ldr	r3, [r3]
	cmp	r3, #0
	beq	.L8af2a
.L8af26:
	mov	r0, #0
	b	.L8affc
.L8af2a:
	lsl	r3, r0, #2
	add	r3, r0
	add	r7, r3
	mov	r3, #0xd4
	lsl	r3, #1
	add	r3, r11
	ldr	r5, [r3]
	mov	r10, r3
	cmp	r5, #0
	bne	.L8af66
	bl	Func_4458
	mov	r5, r0
	bl	Func_4458
	mov	r8, r0
	bl	Func_4458
	mov	r6, r0
	bl	Func_4458
	mov	r2, r8
	sub	r5, r2
	add	r5, r6
	sub	r5, r0
	lsr	r3, r5, #31
	add	r5, r3
	asr	r5, #1
	mov	r3, r10
	str	r5, [r3]
.L8af66:
	lsl	r3, r7, #4
	sub	r3, #0x10
	mul	r3, r5
	lsl	r0, r7, #20
	mov	r1, #0x80
	add	r0, r3
	lsl	r1, #13
	ldr	r3, =Func_8ac
	bl	_call_via_r3
	ldr	r3, =Func_888
	ldr	r1, [sp]
	.call_via r3
	ldr	r3, =ewram_240
	mov	r2, #0x8e
	lsl	r2, #2
	add	r3, r2
	ldr	r2, [r3]
	add	r2, r0
	str	r2, [r3]
	mov	r3, #0xd6
	lsl	r3, #1
	add	r3, r11
	ldr	r3, [r3]
	mov	r0, #0
	cmp	r2, r3
	blt	.L8affc
.L8afa0:
	mov	r2, #0xd4
	lsl	r2, #1
	add	r2, r11
	mov	r3, #0
	str	r3, [r2]
	mov	r2, r9
	mov	r5, #0
	add	r2, #0x14
	mov	r1, #7
.L8afb2:
	ldrb	r3, [r2]
	sub	r1, #1
	add	r2, #1
	add	r5, r3
	cmp	r1, #0
	bge	.L8afb2
	mov	r0, #0
	cmp	r5, #0
	beq	.L8affc
	bl	Func_4458
	mov	r3, r5
	mul	r3, r0
	mov	r2, r9
	lsr	r0, r3, #16
	ldrb	r3, [r2, #0x14]
	sub	r0, r3
	mov	r1, #0
	cmp	r0, #0
	blt	.L8afec
	add	r2, #0x14
.L8afdc:
	add	r1, #1
	cmp	r1, #7
	bgt	.L8afec
	add	r2, #1
	ldrb	r3, [r2]
	sub	r0, r3
	cmp	r0, #0
	bge	.L8afdc
.L8afec:
	lsl	r3, r1, #1
	add	r3, #4
	mov	r2, r9
	ldrh	r5, [r2, r3]
	ldr	r0, [sp, #4]
	bl	Func_8b320
	mov	r0, r5
.L8affc:
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8ae74

@ PlayStepEffectForSlot
@ r0=slot. Reads that slot's cached surface type from the byte array at
@ iwram_1ebc+0x1A0 and plays its step effect through Func_8ae74.
.thumb_func_start Func_8b02c
	push	{lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xd0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	ldrb	r0, [r3, r0]
	bl	Func_8ae74
	pop	{r1}
	bx	r1
.func_end Func_8b02c

@ PlayStepEffectAtPosition
@ r0=position vec3, r1=variant. Func_8adf0 to classify the surface, then
@ Func_8ae74 to play it -- the one-shot form used when no slot is involved.
.thumb_func_start Func_8b048
	push	{r5, lr}
	mov	r5, r1
	bl	Func_8adf0
	mov	r1, r5
	bl	Func_8ae74
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_8b048

@ LookupMapDestination
@ r0=map group, r1=entrance index. Returns the destination map id from the
@ table at .L9c610, which stores 14 halfwords per group; the entry read is at
@ (group * 14 + entrance) * 2 + 4. No range check -- callers are expected to
@ pass indices that came from map data.
.thumb_func_start Func_8b05c
	lsl	r3, r0, #3
	sub	r3, r0
	lsl	r3, #1
	add	r3, r1
	ldr	r2, =.L9c610
	lsl	r3, #1
	add	r3, #4
	ldrh	r0, [r2, r3]
	bx	lr
.func_end Func_8b05c

@ LookupMapDestinationByIndex
@ r0=transition index. Reads a {group, entrance} pair from the 4-byte records at
@ .L9d8b0 and resolves it through Func_8b05c. Lets map scripts refer to a
@ destination by a single number.
.thumb_func_start Func_8b074
	push	{lr}
	ldr	r3, =.L9d8b0
	lsl	r0, #2
	add	r0, r3
	ldrh	r3, [r0]
	ldrh	r1, [r0, #2]
	mov	r0, r3
	bl	Func_8b05c
	pop	{r1}
	bx	r1
.func_end Func_8b074

@ PrepareMapTransition
@ Takes no arguments. Reads the pending destination from ewram_240+0x1C0 and
@ sets up the transition: resolves the target map, picks the fade style, and
@ stages the spawn position for the incoming map. The ~100-instruction body is
@ characterised structurally.
.thumb_func_start Func_8b090
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r4, =ewram_240
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r4, r2
	mov	r1, #0x12
	mov	r10, r1
	mov	r2, #0
	ldrsh	r1, [r3, r2]
	mov	r11, r1
	mov	r1, #0xe1
	lsl	r1, #1
	ldr	r5, =.L9d9f0
	add	r3, r4, r1
	add	r1, #0xa
	mov	r2, #0
	ldrsh	r7, [r3, r2]
	ldrh	r0, [r5]
	add	r3, r4, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	mov	r1, r0
	mov	r9, r2
	lsl	r3, r1, #16
	mov	r2, #1
	asr	r3, #16
	neg	r2, r2
	cmp	r3, r2
	beq	.L8b132
	ldr	r6, =0x7fff
	mov	r8, r2
.L8b0da:
	ldrb	r2, [r5, #3]
	mov	r3, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.L8b0ee
	lsl	r3, r1, #16
	asr	r3, #16
	cmp	r3, r11
	bne	.L8b122
	b	.L8b0f6
.L8b0ee:
	lsl	r3, r0, #16
	asr	r3, #16
	cmp	r3, r9
	bne	.L8b122
.L8b0f6:
	ldrh	r2, [r5, #2]
	mov	r3, r6
	and	r3, r2
	cmp	r3, r6
	beq	.L8b108
	lsl	r3, r2, #17
	asr	r3, #17
	cmp	r3, r7
	bne	.L8b122
.L8b108:
	mov	r2, #4
	ldrsh	r0, [r5, r2]
	cmp	r0, r8
	beq	.L8b118
	bl	_Func_79338
	cmp	r0, #0
	beq	.L8b122
.L8b118:
	mov	r1, #6
	ldrsh	r3, [r5, r1]
	ldr	r4, =ewram_240
	mov	r10, r3
	b	.L8b132
.L8b122:
	add	r5, #8
	ldrh	r1, [r5]
	lsl	r3, r1, #16
	asr	r3, #16
	mov	r0, r1
	cmp	r3, r8
	bne	.L8b0da
	ldr	r4, =ewram_240
.L8b132:
	mov	r2, #0xf8
	lsl	r2, #1
	add	r3, r4, r2
	mov	r1, r10
	strh	r1, [r3]
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8b090

@ FindMapEntryRecord
@ r0=map id, r1=entrance. Searches the table at .L9ddd8 for the matching entry
@ record, using Func_8a8d0 to resolve the group. Returns the record, or a
@ negative result when the pair is not listed.
.thumb_func_start Func_8b158
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r3, #0
	mov	r6, r1
	ldr	r5, =.L9ddd8
	mov	r8, r3
	mov	r7, r0
	bl	Func_8a8d0
	ldrh	r4, [r5]
	mov	r1, r4
	lsl	r3, r1, #16
	mov	r2, #1
	asr	r3, #16
	neg	r2, r2
	mov	r12, r0
	cmp	r3, r2
	beq	.L8b1c4
	ldr	r0, =0x7fff
	mov	r14, r2
.L8b182:
	ldrb	r2, [r5, #3]
	mov	r3, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.L8b196
	lsl	r3, r1, #16
	asr	r3, #16
	cmp	r3, r7
	bne	.L8b1b6
	b	.L8b19e
.L8b196:
	lsl	r3, r4, #16
	asr	r3, #16
	cmp	r3, r12
	bne	.L8b1b6
.L8b19e:
	ldrh	r2, [r5, #2]
	mov	r3, r0
	and	r3, r2
	cmp	r3, r0
	beq	.L8b1b0
	lsl	r3, r2, #17
	asr	r3, #17
	cmp	r3, r6
	bne	.L8b1b6
.L8b1b0:
	ldr	r5, [r5, #4]
	mov	r8, r5
	b	.L8b1c4
.L8b1b6:
	add	r5, #8
	ldrh	r1, [r5]
	lsl	r3, r1, #16
	asr	r3, #16
	mov	r4, r1
	cmp	r3, r14
	bne	.L8b182
.L8b1c4:
	mov	r0, r8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8b158

@ FindAreaRecord
@ Takes no arguments. Looks the current map and entrance (ewram_240+0x1C0 and
@ +0x1C2) up in the area table at .L9e1d8 and returns the matching record --
@ the area name, music and encounter set that go with this location.
.thumb_func_start Func_8b1d8
	push	{r5, r6, r7, lr}
	ldr	r0, =ewram_240
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r0, r1
	mov	r4, #0xe1
	lsl	r4, #1
	mov	r2, #0
	ldrsh	r6, [r3, r2]
	ldr	r2, =.L9e1d8
	add	r3, r0, r4
	mov	r7, #0
	ldrsh	r5, [r3, r7]
	mov	r4, #0
	ldrsh	r3, [r2, r4]
	mov	r4, #1
	neg	r4, r4
	ldrh	r1, [r2]
	cmp	r3, r4
	beq	.L8b238
	mov	r7, #0xe2
	mov	r3, #0xe3
	lsl	r7, #1
	lsl	r3, #1
	mov	r12, r4
	add	r4, r0, r7
	add	r0, r3
.L8b20e:
	lsl	r3, r1, #16
	asr	r3, #16
	cmp	r3, r6
	bne	.L8b22c
	mov	r7, #2
	ldrsh	r3, [r2, r7]
	cmp	r3, r12
	beq	.L8b222
	cmp	r3, r5
	bne	.L8b22c
.L8b222:
	ldrh	r3, [r2, #4]
	strh	r3, [r4]
	ldrh	r3, [r2, #6]
	strh	r3, [r0]
	b	.L8b238
.L8b22c:
	add	r2, #8
	mov	r7, #0
	ldrsh	r3, [r2, r7]
	ldrh	r1, [r2]
	cmp	r3, r12
	bne	.L8b20e
.L8b238:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8b1d8

@ GetCurrentMapId
@ Returns the signed halfword at ewram_240+0x1D6, the id of the map currently
@ loaded.
.thumb_func_start Func_8b248
	ldr	r3, =ewram_240
	mov	r2, #0xeb
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	bx	lr
.func_end Func_8b248

@ FindMapInSpecialList
@ Takes no arguments. Scans the zero-terminated list at .L9e270 for the current
@ map id (ewram_240+0x1C0) and reports whether it is present -- the list of maps
@ that get special handling on entry.
.thumb_func_start Func_8b25c
	push	{r5, r6, lr}
	ldr	r2, =ewram_240
	mov	r3, #0xe0
	mov	r12, r2
	lsl	r3, #1
	ldr	r4, =.L9e270
	add	r3, r12
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	ldmia	r4!, {r2}
	cmp	r2, #0
	beq	.L8b294
	cmp	r2, r0
	beq	.L8b294
	mov	r6, #0x80
	ldr	r5, =0xffff
	lsl	r6, #24
.L8b27e:
	mov	r3, r2
	and	r3, r6
	cmp	r3, #0
	beq	.L8b28a
	mov	r1, r5
	and	r1, r2
.L8b28a:
	ldmia	r4!, {r2}
	cmp	r2, #0
	beq	.L8b294
	cmp	r2, r0
	bne	.L8b27e
.L8b294:
	mov	r3, #0xeb
	lsl	r3, #1
	add	r3, r12
	strh	r1, [r3]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8b25c

@ GetTransitionStyle
@ r0=transition kind 1..7. Returns the fade/wipe style constant for that kind
@ via the jump table at .L8b2c0. Kinds outside 1..7 fall through to the default
@ at .L8b2f0.
.thumb_func_start Func_8b2b0
	push	{lr}
	sub	r0, #1
	cmp	r0, #6
	bhi	.L8b2f0
	ldr	r2, =.L8b2c0
	lsl	r3, r0, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L8b2c0:
	.word	.L8b2dc
	.word	.L8b2e0
	.word	.L8b2e4
	.word	.L8b2e8
	.word	.L8b2ec
	.word	.L8b2ec
	.word	.L8b2e8
.L8b2dc:
	ldr	r2, =0x38
	b	.L8b2f2
.L8b2e0:
	ldr	r2, =0x3a
	b	.L8b2f2
.L8b2e4:
	ldr	r2, =0x3c
	b	.L8b2f2
.L8b2e8:
	ldr	r2, =0x36
	b	.L8b2f2
.L8b2ec:
	ldr	r2, =0x37
	b	.L8b2f2
.L8b2f0:
	ldr	r2, =0x39
.L8b2f2:
	ldr	r3, =ewram_240
	mov	r1, #0xeb
	lsl	r1, #1
	add	r3, r1
	strh	r2, [r3]
	pop	{r0}
	bx	r0
.func_end Func_8b2b0

@ StartMapTransition
@ r0=map group, r1=entrance. Commits the transition: forms the packed
@ destination as (group << 4) | entrance, then picks the transition record from
@ .L9e488. Event flag 0x16C selects an alternate style (0x12) -- the
@ post-progress variant -- otherwise the style is looked up normally.
@ This is what Func_91eb0 and Func_91f14 in rom_91584.s ultimately call.
.thumb_func_start Func_8b320
	push	{r5, r6, r7, lr}
	lsl	r0, #4
	add	r0, r1
	lsl	r0, #16
	asr	r7, r0, #16
	mov	r0, #0xb6
	lsl	r0, #1
	ldr	r5, =.L9e488
	bl	_Func_79338
	cmp	r0, #0
	beq	.L8b33c
	mov	r6, #0x12
	b	.L8b37a
.L8b33c:
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	lsl	r2, r3, #16
	lsr	r1, r2, #16
	add	r5, #2
	cmp	r1, #0
	beq	.L8b37a
	lsl	r3, r7, #16
	lsr	r0, r3, #16
	cmp	r1, r0
	beq	.L8b37a
	mov	r4, #0x80
	ldr	r1, =0xfff
	lsl	r4, #8
	mov	r12, r0
.L8b35a:
	lsr	r2, #16
	mov	r3, r2
	and	r3, r4
	cmp	r3, #0
	beq	.L8b368
	mov	r6, r2
	and	r6, r1
.L8b368:
	mov	r2, #0
	ldrsh	r3, [r5, r2]
	lsl	r2, r3, #16
	lsr	r3, r2, #16
	add	r5, #2
	cmp	r3, #0
	beq	.L8b37a
	cmp	r3, r12
	bne	.L8b35a
.L8b37a:
	ldr	r3, =ewram_240
	mov	r2, #0xf7
	lsl	r2, #1
	add	r3, r2
	strh	r6, [r3]
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8b320

@ RemapAreaIdForProgress
@ r0=area id. Returns the id to actually use, accounting for story progress:
@ once event flag 0x20 is set, areas 0 and 1 become 0x12 and 0x13 -- the
@ post-event versions of the same locations. Ids above 8 pass through
@ unchanged.
.thumb_func_start Func_8b398
	push	{r5, lr}
	mov	r5, r0
	cmp	r5, #8
	bgt	.L8b3c8
	mov	r0, #0x20
	bl	_Func_79338
	cmp	r0, #0
	beq	.L8b3b8
	cmp	r5, #0
	bne	.L8b3b0
	mov	r5, #0x12
.L8b3b0:
	cmp	r5, #1
	bne	.L8b3c8
	mov	r5, #0x13
	b	.L8b3c8
.L8b3b8:
	mov	r0, #0x21
	bl	_Func_79338
	cmp	r0, #0
	beq	.L8b3c8
	cmp	r5, #0
	bne	.L8b3c8
	mov	r5, #0x11
.L8b3c8:
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_8b398

@ RemapAreaId
@ r0=area id, r1=non-zero to apply the remap. Func_8b398 without the event-flag
@ lookup: the caller supplies the condition. Same 0 -> 0x12, 1 -> 0x13 mapping
@ for ids at or below 8.
.thumb_func_start Func_8b3d0
	push	{lr}
	cmp	r0, #8
	bgt	.L8b3e6
	cmp	r1, #0
	beq	.L8b3e6
	cmp	r0, #0
	bne	.L8b3e0
	mov	r0, #0x12
.L8b3e0:
	cmp	r0, #1
	bne	.L8b3e6
	mov	r0, #0x13
.L8b3e6:
	pop	{r1}
	bx	r1
.func_end Func_8b3d0

@ BindRecordToSlot
@ r0=scene record, r1=slot. Instantiates the record into the slot: spawns its
@ entity, applies the record's position, facing, sprite and behaviour script,
@ and stores the entity in the slot table at iwram_1ebc+0x14.
@ This is the workhorse behind Func_917ac and Func_8b674. The
@ ~250-instruction body is characterised structurally; the slot binding and
@ record source are verified.
.thumb_func_start Func_8b3ec
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ebc
	ldr	r3, [r3]
	mov	r9, r3
	ldr	r3, [r3]
	mov	r7, r0
	mov	r11, r1
	sub	sp, #8
	mov	r1, #0
	cmp	r3, r7
	beq	.L8b42e
	cmp	r3, #0
	bne	.L8b418
	mov	r0, r9
	str	r7, [r0]
	b	.L8b42e
.L8b418:
	add	r1, #1
	cmp	r1, #3
	bgt	.L8b42e
	lsl	r2, r1, #2
	mov	r0, r9
	ldr	r3, [r0, r2]
	cmp	r3, r7
	beq	.L8b42e
	cmp	r3, #0
	bne	.L8b418
	str	r7, [r0, r2]
.L8b42e:
	mov	r0, #1
	mov	r1, #0
	ldrsh	r3, [r7, r1]
	neg	r0, r0
	ldrh	r2, [r7]
	b	.L8b618
.L8b43a:
	lsl	r3, r2, #16
	asr	r3, #16
	cmp	r3, #7
	bgt	.L8b446
	str	r3, [sp, #4]
	b	.L8b454
.L8b446:
	ldr	r2, =0x2705
	cmp	r3, r2
	bgt	.L8b454
	mov	r3, r11
	mov	r0, #1
	str	r3, [sp, #4]
	add	r11, r0
.L8b454:
	mov	r1, #2
	ldrsh	r5, [r7, r1]
	mov	r0, r5
	bl	Func_8d428
	cmp	r0, #0
	bne	.L8b464
	b	.L8b60a
.L8b464:
	mov	r3, r5
	sub	r3, #0x30
	cmp	r3, #0x4f
	bhi	.L8b488
	mov	r3, #0xcf
	lsl	r3, #1
	add	r3, r9
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	beq	.L8b488
	mov	r0, r5
	add	r0, #0x50
	bl	Func_8d428
	cmp	r0, #0
	bne	.L8b488
	b	.L8b60a
.L8b488:
	mov	r3, #0
	ldrsh	r0, [r7, r3]
	bl	Func_8b398
	mov	r10, r0
	ldr	r0, [sp, #4]
	bl	Func_8ba1c
	mov	r6, r0
	cmp	r6, #0
	bne	.L8b512
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0xc]
	ldr	r3, [r7, #0x10]
	mov	r0, r10
	bl	_Func_c150
	ldrb	r2, [r7, #0x17]
	mov	r3, #1
	and	r3, r2
	mov	r6, r0
	mov	r1, #1
	cmp	r3, #0
	beq	.L8b4f6
	ldr	r0, [sp, #4]
	sub	r0, #1
	str	r1, [sp]
	bl	Func_8ba1c
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	ldr	r1, [sp]
	cmp	r3, #1
	bne	.L8b4f6
	mov	r3, r6
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L8b4f6
	ldr	r0, [r0, #0x50]
	ldrb	r3, [r0, #0x1d]
	orr	r3, r1
	strb	r3, [r0, #0x1d]
	ldrb	r5, [r0, #0x1c]
	ldr	r0, [r6, #0x50]
	ldrb	r3, [r0, #0x1d]
	orr	r3, r1
	mov	r8, r0
	strb	r3, [r0, #0x1d]
	ldrb	r0, [r0, #0x1c]
	bl	Func_3f3c
	mov	r1, r8
	strb	r5, [r1, #0x1c]
.L8b4f6:
	mov	r0, #0x21
	bl	_Func_79338
	cmp	r0, #0
	beq	.L8b528
	mov	r3, r10
	sub	r3, #0x12
	cmp	r3, #1
	bhi	.L8b528
	mov	r0, r6
	mov	r1, #0xe2
	bl	_Func_c430
	b	.L8b528
.L8b512:
	ldr	r0, =0x109
	bl	_Func_79338
	cmp	r0, #0
	bne	.L8b528
	ldr	r1, [r7, #8]
	ldr	r2, [r7, #0xc]
	ldr	r3, [r7, #0x10]
	mov	r0, r6
	bl	_Func_d130
.L8b528:
	cmp	r6, #0
	beq	.L8b600
	mov	r0, r6
	mov	r1, #1
	bl	_Func_c300
	mov	r3, r6
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.L8b556
	ldr	r2, [r6, #0x50]
	mov	r8, r2
	cmp	r2, #0
	beq	.L8b556
	bl	Func_4458
	mov	r1, #0x1e
	bl	Func_b50_from_thumb
	mov	r3, r8
	add	r3, #0x24
	strb	r0, [r3]
.L8b556:
	ldrh	r3, [r7, #0x14]
	mov	r2, r6
	strh	r3, [r6, #6]
	add	r2, #0x59
	mov	r3, #1
	strb	r3, [r2]
	ldr	r1, [r7, #4]
	mov	r0, r6
	bl	Func_93a6c
	mov	r0, r6
	mov	r1, #1
	bl	_Func_c300
	ldr	r2, [r6, #8]
	cmp	r2, #0
	bge	.L8b57c
	ldr	r3, =0xffff
	add	r2, r3
.L8b57c:
	mov	r3, r6
	add	r3, #0x64
	asr	r2, #16
	strh	r2, [r3]
	ldr	r3, [r6, #0x10]
	cmp	r3, #0
	bge	.L8b58e
	ldr	r0, =0xffff
	add	r3, r0
.L8b58e:
	mov	r2, r6
	asr	r3, #16
	add	r2, #0x66
	strh	r3, [r2]
	ldr	r3, [r6, #0xc]
	cmp	r3, #0
	beq	.L8b5ac
	mov	r3, #4
	sub	r2, #0x11
	strb	r3, [r2]
	mov	r1, #0x80
	ldr	r3, [r6, #0xc]
	lsl	r1, #8
	add	r3, r1
	str	r3, [r6, #0xc]
.L8b5ac:
	mov	r3, #0xcf
	lsl	r3, #1
	add	r3, r9
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L8b5e6
	mov	r1, r6
	add	r1, #0x55
	ldrb	r3, [r1]
	mov	r2, #0xfe
	and	r2, r3
	strb	r2, [r1]
	mov	r0, #0x21
	bl	_Func_79338
	cmp	r0, #0
	bne	.L8b5f8
	mov	r1, r8
	ldr	r0, [r1, #0x18]
	mov	r1, #0xc0
	ldr	r3, =Func_888
	lsl	r1, #8
	.call_via r3
	mov	r2, r8
	str	r0, [r2, #0x18]
	b	.L8b5f8
.L8b5e6:
	ldr	r1, [r6, #8]
	ldr	r2, [r6, #0x10]
	mov	r0, #0
	bl	_Func_11f54
	ldr	r3, [r6, #0xc]
	add	r3, r0
	str	r0, [r6, #0x14]
	str	r3, [r6, #0xc]
.L8b5f8:
	mov	r2, r6
	add	r2, #0x23
	mov	r3, #1
	strb	r3, [r2]
.L8b600:
	ldr	r0, [sp, #4]
	lsl	r3, r0, #2
	add	r3, #0x14
	mov	r1, r9
	str	r6, [r1, r3]
.L8b60a:
	add	r7, #0x18
	ldrh	r3, [r7]
	mov	r2, r3
	lsl	r3, r2, #16
	mov	r0, #1
	asr	r3, #16
	neg	r0, r0
.L8b618:
	cmp	r3, r0
	beq	.L8b624
	mov	r1, r11
	cmp	r1, #0x41
	bgt	.L8b624
	b	.L8b43a
.L8b624:
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8b3ec

@ ClearSceneSlot
@ r0=slot. Destroys the slot's entity with _Func_c0f4 and zeroes the slot
@ pointer at iwram_1ebc + 0x14 + slot * 4. The single-slot counterpart of
@ Func_8b98c.
.thumb_func_start Func_8b64c
	push	{r5, r6, lr}
	ldr	r3, =iwram_1ebc
	mov	r5, r0
	ldr	r6, [r3]
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L8b668
	bl	_Func_c0f4
	lsl	r3, r5, #2
	add	r3, #0x14
	mov	r2, #0
	str	r2, [r6, r3]
.L8b668:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8b64c

	.section .rodata

.L9c610:
	.incrom 0x9c610, 0x9d170
.L9d170:
	.incrom 0x9d170, 0x9d7a8
.L9d7a8:
	.incrom 0x9d7a8, 0x9d8b0
.L9d8b0:
	.incrom 0x9d8b0, 0x9d9f0
.L9d9f0:
	.incrom 0x9d9f0, 0x9ddd8
.L9ddd8:
	.incrom 0x9ddd8, 0x9e1d8
.L9e1d8:
	.incrom 0x9e1d8, 0x9e270
.L9e270:
	.incrom 0x9e270, 0x9e488
.L9e488:
	.incrom 0x9e488, 0x9e4ce

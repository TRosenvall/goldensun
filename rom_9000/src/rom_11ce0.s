	.include "macros.inc"

@ ============================================================================
@ Terrain height sampling.
@
@ .L134fc at the bottom of this file is a 16-entry dispatch table: one height
@ sampler per tile shape. Every sampler has the same signature
@
@   r0 = pointer to the tile material's corner-height bytes (ewram_2c001 + id*4)
@   r1 = sub-tile x, 0..15
@   r2 = sub-tile z, 0..15
@   -> height as 16.16
@
@ Corner bytes are heights in units of 8 pixels, which is why every sampler
@ finishes with `lsl #19` (<<16 to reach 16.16, <<3 for the unit).
@
@ Callers (Func_11f54, Func_120dc) resolve a world position to a tile, read the
@ material id from tile byte +3, take the low nibble of ewram_2c000[id*4] as the
@ shape index, and invoke the corresponding sampler.
@ ============================================================================

@ TileHeight_RampX (shape 1)
@ Linear ramp along x between corner bytes 0 and 1, interpolated as
@ c0 + (c1 - c0) * x / 16. The +0xF before the >>4 rounds toward zero for
@ negative slopes.
.thumb_func_start Func_11cec
	push	{lr}
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r2, r3, #19
	mov	r3, #1
	ldrsb	r3, [r0, r3]
	lsl	r3, #19
	sub	r3, r2
	mov	r0, r1
	mul	r0, r3
	cmp	r0, #0
	bge	.L11d06
	add	r0, #0xf
.L11d06:
	asr	r0, #4
	add	r0, r2, r0
	pop	{r1}
	bx	r1
.func_end Func_11cec

@ TileHeight_RampZ (shape 2)
@ Linear ramp along z between corner bytes 0 and 1 -- Func_11cec with the other
@ axis.
.thumb_func_start Func_11d10
	push	{lr}
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r1, r3, #19
	mov	r3, #1
	ldrsb	r3, [r0, r3]
	lsl	r3, #19
	sub	r3, r1
	mov	r0, r2
	mul	r0, r3
	cmp	r0, #0
	bge	.L11d2a
	add	r0, #0xf
.L11d2a:
	asr	r0, #4
	add	r0, r1, r0
	pop	{r1}
	bx	r1
.func_end Func_11d10

@ TileHeight_StepAntiDiagonal (shape 3)
@ Two flat halves split by the anti-diagonal x + z = 15: below it corner 0,
@ above it corner 1, and exactly on it max(c0, c1) so the ridge line sits at the
@ higher of the two.
.thumb_func_start Func_11d34
	push	{lr}
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r4, r3, #19
	mov	r3, #1
	ldrsb	r3, [r0, r3]
	lsl	r0, r3, #19
	mov	r3, r4
	cmp	r0, r4
	ble	.L11d4a
	mov	r3, r0
.L11d4a:
	add	r1, r2
	cmp	r1, #0xf
	bne	.L11d54
	mov	r0, r3
	b	.L11d5a
.L11d54:
	cmp	r1, #0xe
	bhi	.L11d5a
	mov	r0, r4
.L11d5a:
	pop	{r1}
	bx	r1
.func_end Func_11d34

@ TileHeight_StepDiagonal (shape 4)
@ As Func_11d34 but split by the main diagonal z - x = 0: negative side corner
@ 0, positive side corner 1, max(c0, c1) on the line itself.
.thumb_func_start Func_11d60
	push	{r5, lr}
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r4, r3, #19
	mov	r3, #1
	ldrsb	r3, [r0, r3]
	lsl	r0, r3, #19
	mov	r5, r2
	mov	r2, r4
	cmp	r0, r4
	ble	.L11d78
	mov	r2, r0
.L11d78:
	sub	r3, r5, r1
	mov	r1, r3
	add	r1, #0xf
	cmp	r1, #0xf
	bne	.L11d86
	mov	r0, r2
	b	.L11d8c
.L11d86:
	cmp	r1, #0xe
	bhi	.L11d8c
	mov	r0, r4
.L11d8c:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_11d60

@ TileHeight_RampAntiDiagonal2 (shape 5)
@ Two-segment ramp along the anti-diagonal using three corner bytes. For
@ x + z <= 14 it interpolates corner 0 -> corner 1, past 15 it interpolates
@ corner 1 -> corner 2, and at exactly 15 it returns corner 1. Division is by
@ 15 via Func_af0_from_thumb rather than a shift.
.thumb_func_start Func_11d94
	push	{r5, r6, lr}
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	add	r0, #1
	lsl	r6, r3, #19
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r5, r3, #19
	mov	r3, #1
	ldrsb	r3, [r0, r3]
	add	r1, r2
	lsl	r3, #19
	mov	r0, r5
	cmp	r1, #0xf
	beq	.L11dd6
	cmp	r1, #0xe
	bhi	.L11dc6
	sub	r3, r5, r6
	mov	r0, r1
	mul	r0, r3
	mov	r1, #0xf
	bl	Func_af0_from_thumb
	add	r0, r6, r0
	b	.L11dd6
.L11dc6:
	sub	r1, #0xf
	sub	r3, r5
	mov	r0, r1
	mul	r0, r3
	mov	r1, #0xf
	bl	Func_af0_from_thumb
	add	r0, r5, r0
.L11dd6:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_11d94

@ TileHeight_RampDiagonal2 (shape 6)
@ Three-corner two-segment ramp along z - x, mirroring Func_11d94. Uses the
@ +0xF / >>4 rounding rather than a real divide.
.thumb_func_start Func_11ddc
	push	{r5, lr}
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	add	r0, #1
	lsl	r5, r3, #19
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r4, r3, #19
	mov	r3, #1
	ldrsb	r3, [r0, r3]
	sub	r0, r2, r1
	mov	r1, r0
	add	r1, #0xf
	lsl	r3, #19
	cmp	r1, #0xf
	bne	.L11e00
	mov	r0, r4
	b	.L11e24
.L11e00:
	cmp	r1, #0xe
	bhi	.L11e16
	sub	r3, r4, r5
	mov	r0, r1
	mul	r0, r3
	cmp	r0, #0
	bge	.L11e10
	add	r0, #0xf
.L11e10:
	asr	r0, #4
	add	r0, r5, r0
	b	.L11e24
.L11e16:
	sub	r3, r4
	mul	r0, r3
	cmp	r0, #0
	bge	.L11e20
	add	r0, #0xf
.L11e20:
	asr	r0, #4
	add	r0, r4, r0
.L11e24:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_11ddc

@ TileHeight_WeightMap (shape 7)
@ Blends corner 0 and corner 1 using a per-sub-tile weight byte from the 16x16
@ table .L132fc, indexed by z * 16 + x: height = c0 + weight * (c1 - c0).
@ NOTE there is no normalising shift after the multiply, unlike every other
@ sampler here, so the table bytes must already be the final scale factor.
.thumb_func_start Func_11e2c
	mov	r3, r0
	mov	r0, #0
	ldrsb	r0, [r3, r0]
	mov	r4, #1
	ldrsb	r4, [r3, r4]
	lsl	r2, #4
	ldr	r3, =.L132fc
	add	r1, r2
	lsl	r0, #19
	ldrb	r3, [r3, r1]
	lsl	r4, #19
	sub	r4, r0
	mul	r3, r4
	add	r0, r3
	bx	lr
.func_end Func_11e2c

@ TileHeight_HalfStepX (shape 8)
@ Flat on each half of the tile: corner 0 for x <= 7, corner 1 for x >= 8.
.thumb_func_start Func_11e50
	push	{lr}
	cmp	r1, #7
	bhi	.L11e5e
	ldrb	r0, [r0]
	lsl	r0, #24
	asr	r0, #24
	b	.L11e64
.L11e5e:
	ldrb	r0, [r0, #1]
	lsl	r0, #24
	asr	r0, #24
.L11e64:
	lsl	r0, #19
	pop	{r1}
	bx	r1
.func_end Func_11e50

@ TileHeight_HalfStepZ (shape 9)
@ Flat on each half of the tile along z: corner 0 for z <= 7, corner 1 for
@ z >= 8.
.thumb_func_start Func_11e6c
	push	{lr}
	cmp	r2, #7
	bhi	.L11e7a
	ldrb	r0, [r0]
	lsl	r0, #24
	asr	r0, #24
	b	.L11e80
.L11e7a:
	ldrb	r0, [r0, #1]
	lsl	r0, #24
	asr	r0, #24
.L11e80:
	lsl	r0, #19
	pop	{r1}
	bx	r1
.func_end Func_11e6c

@ TileHeight_RampX2 (shape 10)
@ Three-corner two-segment ramp along x: corner 0 -> corner 1 over the first
@ half (divided by 8), then corner 1 -> corner 2 over the second.
.thumb_func_start Func_11e88
	push	{lr}
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	add	r0, #1
	lsl	r2, r3, #19
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	add	r0, #1
	lsl	r4, r3, #19
	cmp	r1, #7
	bhi	.L11eb0
	sub	r3, r4, r2
	mov	r0, r1
	mul	r0, r3
	cmp	r0, #0
	bge	.L11eaa
	add	r0, #7
.L11eaa:
	asr	r0, #3
	add	r0, r2, r0
	b	.L11eca
.L11eb0:
	mov	r3, #0
	ldrsb	r3, [r0, r3]
	lsl	r2, r3, #19
	mov	r3, r1
	sub	r2, r4
	sub	r3, #8
	mov	r0, r3
	mul	r0, r2
	cmp	r0, #0
	bge	.L11ec6
	add	r0, #7
.L11ec6:
	asr	r0, #3
	add	r0, r4, r0
.L11eca:
	pop	{r1}
	bx	r1
.func_end Func_11e88

@ TileHeight_RampZ2 (shape 11)
@ Three-corner two-segment ramp along z, the counterpart to Func_11e88.
@ NOTE this sampler loads its corner bytes with `ldrb`, not `ldrsb` -- so unlike
@ every other entry in the table it treats them as UNSIGNED and cannot express a
@ negative height.
.thumb_func_start Func_11ed0
	push	{lr}
	ldrb	r3, [r0]
	add	r0, #1
	mov	r4, r2
	lsl	r2, r3, #19
	ldrb	r3, [r0]
	add	r0, #1
	lsl	r1, r3, #19
	cmp	r4, #7
	bhi	.L11ef6
	sub	r3, r1, r2
	mov	r0, r4
	mul	r0, r3
	cmp	r0, #0
	bge	.L11ef0
	add	r0, #7
.L11ef0:
	asr	r0, #3
	add	r0, r2, r0
	b	.L11f0e
.L11ef6:
	ldrb	r3, [r0]
	lsl	r2, r3, #19
	mov	r3, r4
	sub	r2, r1
	sub	r3, #8
	mov	r0, r3
	mul	r0, r2
	cmp	r0, #0
	bge	.L11f0a
	add	r0, #7
.L11f0a:
	asr	r0, #3
	add	r0, r1, r0
.L11f0e:
	pop	{r1}
	bx	r1
.func_end Func_11ed0

@ TileHeight_LookupTable (shape 12)
@ Per-sub-tile corner selection: .L133fc (a 16x16 byte table) indexed by
@ z * 16 + x yields an offset into the corner-byte array, and that byte is the
@ height. Lets a tile carry an arbitrary sculpted surface rather than a ramp.
.thumb_func_start Func_11f14
	ldr	r3, =.L133fc
	lsl	r2, #4
	add	r1, r2
	ldrb	r3, [r3, r1]
	ldrsb	r0, [r0, r3]
	lsl	r0, #19
	bx	lr
.func_end Func_11f14

@ TileHeight_LookupTableMirroredX (shape 13)
@ Func_11f14 with the x axis mirrored: the table index is z * 16 - x + 15.
.thumb_func_start Func_11f28
	lsl	r2, #4
	ldr	r3, =.L133fc
	sub	r2, r1
	add	r2, #0xf
	ldrb	r3, [r3, r2]
	ldrsb	r0, [r0, r3]
	lsl	r0, #19
	bx	lr
.func_end Func_11f28

@ TileHeight_Flat2 (shape 14)
@ Constant height from corner byte 0. Byte-for-byte the same behaviour as
@ Func_11ce0, kept as a separate table entry.
.thumb_func_start Func_11f3c
	ldrb	r0, [r0]
	lsl	r0, #24
	asr	r0, #24
	lsl	r0, #19
	bx	lr
.func_end Func_11f3c

@ TileHeight_Flat3 (shape 15)
@ Constant height from corner byte 0; third identical flat entry.
.thumb_func_start Func_11f48
	ldrb	r0, [r0]
	lsl	r0, #24
	asr	r0, #24
	lsl	r0, #19
	bx	lr
.func_end Func_11f48

@ GetTerrainHeight
@ r0=layer selector, r1=world x (16.16), r2=world z (16.16). Returns the ground
@ height at that point as 16.16.
@ Selects the map layer from [iwram_1e70] + 0x130 + (r0 & 3) * 0x30, falling
@ back to ewram_10000 when no map is loaded. Tiles are 16 pixels and the grid is
@ 128 wide with a 4-byte record each; byte +3 of the record is the material id.
@ The material record at ewram_2c000 + id * 4 supplies the shape index in the
@ low nibble of its first byte, and its remaining bytes (ewram_2c001 + id * 4)
@ are the corner heights handed to the sampler along with (x & 15, z & 15).
.thumb_func_start Func_11f54
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1e70
	mov	r5, r1
	ldr	r1, [r3]
	mov	r6, r2
	asr	r5, #16
	asr	r6, #16
	ldr	r2, =ewram_10000
	cmp	r1, #0
	beq	.L11f7a
	mov	r2, #3
	and	r2, r0
	lsl	r3, r2, #1
	add	r3, r2
	mov	r2, #0x98
	lsl	r2, #1
	lsl	r3, #4
	add	r3, r2
	ldr	r2, [r1, r3]
.L11f7a:
	mov	r3, r5
	cmp	r5, #0
	bge	.L11f82
	add	r3, #0xf
.L11f82:
	asr	r1, r3, #4
	mov	r3, r6
	cmp	r6, #0
	bge	.L11f8c
	add	r3, #0xf
.L11f8c:
	asr	r3, #4
	lsl	r3, #7
	add	r3, r1, r3
	lsl	r3, #2
	add	r2, r3
	ldrb	r1, [r2, #3]
	ldr	r3, =ewram_2c000
	lsl	r1, #2
	add	r0, r1, r3
	mov	r2, #0xf
	ldrb	r0, [r0]
	mov	r3, r2
	ldr	r4, =.L134fc
	and	r3, r0
	ldr	r7, =ewram_2c001
	and	r5, r2
	and	r6, r2
	lsl	r3, #2
	add	r0, r1, r7
	ldr	r3, [r4, r3]
	mov	r1, r5
	mov	r2, r6
	bl	_call_via_r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_11f54

@ GetTerrainSurfaceType
@ r0=layer selector, r1=world x (16.16), r2=world z (16.16). Resolves the tile
@ exactly as Func_11f54 does but stops at the material byte, returning its low
@ nibble -- the shape/surface index -- without sampling a height.
.thumb_func_start Func_11fd8
	push	{r5, r6, lr}
	ldr	r3, =iwram_1e70
	ldr	r5, [r3]
	mov	r4, r2
	mov	r6, r0
	asr	r1, #16
	asr	r4, #16
	ldr	r0, =ewram_10000
	cmp	r5, #0
	beq	.L11ffe
	mov	r2, #3
	and	r2, r6
	lsl	r3, r2, #1
	add	r3, r2
	mov	r2, #0x98
	lsl	r3, #4
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r5, r3]
.L11ffe:
	cmp	r1, #0
	bge	.L12004
	add	r1, #0xf
.L12004:
	mov	r2, r4
	asr	r1, #4
	cmp	r2, #0
	bge	.L1200e
	add	r2, #0xf
.L1200e:
	asr	r3, r2, #4
	lsl	r3, #7
	add	r3, r1, r3
	lsl	r3, #2
	add	r0, r3
	ldrb	r3, [r0, #3]
	ldr	r2, =ewram_2c000
	lsl	r3, #2
	add	r3, r2
	ldrb	r3, [r3]
	mov	r0, #0xf
	and	r0, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_11fd8

@ GetTileFlags
@ r0=layer selector, r1=world x, r2=world z. Returns byte +2 of the tile record,
@ the passability/type field. The coordinates are shifted by 20 rather than 16,
@ so they are taken straight to whole tiles. A value of 0xFF marks a blocked or
@ absent tile (see Func_120dc).
.thumb_func_start Func_12038
	push	{r5, lr}
	ldr	r3, =iwram_1e70
	mov	r5, r0
	ldr	r0, [r3]
	mov	r4, r2
	asr	r1, #20
	asr	r4, #20
	ldr	r2, =ewram_10000
	cmp	r0, #0
	beq	.L1205e
	mov	r2, #3
	and	r2, r5
	lsl	r3, r2, #1
	add	r3, r2
	mov	r2, #0x98
	lsl	r2, #1
	lsl	r3, #4
	add	r3, r2
	ldr	r2, [r0, r3]
.L1205e:
	lsl	r3, r4, #7
	add	r3, r1, r3
	lsl	r3, #2
	add	r2, r3
	ldrb	r0, [r2, #2]
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_12038

@ SetTileFlags
@ r0=layer selector, r1=world x, r2=world z, r3=value. The write counterpart to
@ Func_12038: stores r3 into byte +2 of the addressed tile record. Silently does
@ nothing when no map is loaded.
.thumb_func_start Func_12078
	push	{r5, r6, lr}
	mov	r6, r3
	ldr	r3, =iwram_1e70
	mov	r5, r0
	ldr	r0, [r3]
	mov	r4, r2
	asr	r1, #20
	asr	r4, #20
	cmp	r0, #0
	beq	.L120a8
	mov	r2, #3
	and	r2, r5
	lsl	r3, r2, #1
	add	r3, r2
	mov	r2, #0x98
	lsl	r2, #1
	lsl	r3, #4
	add	r3, r2
	ldr	r2, [r0, r3]
	lsl	r3, r4, #7
	add	r3, r1, r3
	lsl	r3, #2
	add	r2, r3
	strb	r6, [r2, #2]
.L120a8:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_12078

@ GetTileZone
@ r0=x, r1=z, both already in pixels. Returns the top two bits of byte +1 of the
@ tile record. Always reads the base map at ewram_10000 -- unlike the functions
@ above it takes no layer selector.
.thumb_func_start Func_120b4
	push	{lr}
	cmp	r0, #0
	bge	.L120bc
	add	r0, #0xf
.L120bc:
	asr	r0, #4
	cmp	r1, #0
	bge	.L120c4
	add	r1, #0xf
.L120c4:
	asr	r3, r1, #4
	lsl	r3, #7
	add	r3, r0, r3
	ldr	r2, =ewram_10000
	lsl	r3, #2
	add	r3, r2
	ldrb	r0, [r3, #1]
	lsr	r0, #6
	pop	{r1}
	bx	r1
.func_end Func_120b4

@ CheckTerrainStep
@ r0=entity, r1=candidate position. Returns 0 if the entity may stand there,
@ and non-zero if the move must be rejected:
@     2 = no tile / blocked (tile flags byte is 0xFF)
@     1 = the step up exceeds 8.0
@    -1 = the drop exceeds 12.0
@ Reads the candidate's integer x and z from the high halfwords at +0x02 and
@ +0x0A, picks the layer from the entity's tile-type byte at +0x22 (values above
@ 2 fall back to ewram_10000), samples the height through the same shape
@ dispatch as Func_11f54, and compares it against the ground height cached in
@ the entity at +0x14.
@ This is the terrain half of movement validation; Func_d924 is the entity half.
.thumb_func_start Func_120dc
	push	{r5, r6, r7, lr}
	mov	r3, #0xa
	ldrsh	r6, [r1, r3]
	ldr	r3, =iwram_1e70
	mov	r2, #2
	ldrsh	r5, [r1, r2]
	ldr	r1, [r3]
	mov	r7, r0
	mov	r0, #0
	cmp	r1, #0
	beq	.L1217c
	mov	r2, r7
	add	r2, #0x22
	ldrb	r3, [r2]
	cmp	r3, #2
	bhi	.L1210e
	mov	r2, r3
	lsl	r3, r2, #1
	add	r3, r2
	mov	r2, #0x98
	lsl	r2, #1
	lsl	r3, #4
	add	r3, r2
	ldr	r2, [r1, r3]
	b	.L12110
.L1210e:
	ldr	r2, =ewram_10000
.L12110:
	mov	r3, r5
	cmp	r5, #0
	bge	.L12118
	add	r3, #0xf
.L12118:
	asr	r1, r3, #4
	mov	r3, r6
	cmp	r6, #0
	bge	.L12122
	add	r3, #0xf
.L12122:
	asr	r3, #4
	lsl	r3, #7
	add	r3, r1, r3
	lsl	r3, #2
	add	r2, r3
	ldrb	r3, [r2, #2]
	mov	r0, #2
	cmp	r3, #0xff
	beq	.L1217c
	ldrb	r1, [r2, #3]
	ldr	r3, =ewram_2c000
	lsl	r1, #2
	add	r0, r1, r3
	mov	r2, #0xf
	ldrb	r0, [r0]
	mov	r3, r2
	and	r3, r0
	lsl	r3, #2
	mov	r12, r3
	ldr	r4, =.L134fc
	ldr	r3, =ewram_2c001
	and	r5, r2
	and	r6, r2
	mov	r2, r12
	add	r0, r1, r3
	ldr	r3, [r4, r2]
	mov	r1, r5
	mov	r2, r6
	bl	_call_via_r3
	ldr	r3, [r7, #0x14]
	sub	r0, r3
	mov	r3, #0x80
	lsl	r3, #12
	cmp	r0, r3
	ble	.L1216e
	mov	r0, #1
	b	.L1217c
.L1216e:
	ldr	r2, =0xfff40000
	cmp	r0, r2
	bge	.L1217a
	mov	r0, #1
	neg	r0, r0
	b	.L1217c
.L1217a:
	mov	r0, #0
.L1217c:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_120dc

	.section .rodata

@ .L132fc -- 16x16 weight table used by TileHeight_WeightMap (Func_11e2c).
.L132fc:
	.incrom 0x132fc, 0x133fc
@ .L133fc -- 16x16 corner-index table used by Func_11f14 / Func_11f28.
.L133fc:
	.incrom 0x133fc, 0x134fc
@ .L134fc -- tile-shape dispatch table, indexed 0-15 by the low nibble of the
@ material byte. Order matches the shape numbers noted on each sampler above.
.L134fc:
	.word	Func_11ce0
	.word	Func_11cec
	.word	Func_11d10
	.word	Func_11d34
	.word	Func_11d60
	.word	Func_11d94
	.word	Func_11ddc
	.word	Func_11e2c
	.word	Func_11e50
	.word	Func_11e6c
	.word	Func_11e88
	.word	Func_11ed0
	.word	Func_11f14
	.word	Func_11f28
	.word	Func_11f3c
	.word	Func_11f48

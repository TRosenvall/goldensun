	.include "macros.inc"
	.include "gba.inc"

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
@ PROMOTED: referenced from rom_11ce0.s across the split
	.global	L132fc
L132fc:
.L132fc:
	.incrom 0x132fc, 0x133fc
@ .L133fc -- 16x16 corner-index table used by Func_11f14 / Func_11f28.
@ PROMOTED: referenced from rom_11ce0.s across the split
	.global	L133fc
L133fc:
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

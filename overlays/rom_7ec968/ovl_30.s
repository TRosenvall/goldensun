	.include "macros.inc"

@ ============================================================================
@ Overlay 0x7ec968 -- serves THREE AREAS (0xA9, 0xAA, 0xAB) and is unusual in
@ how much of its state lives in ewram_240 rather than in save bits.
@
@ Slot 0  OvlFunc_124  map-load entry
@ Slot 1  OvlFunc_40   edge transitions   -> 0xAA: .La40, 0xAB: .Lad0, else .L998
@ Slot 2  OvlFunc_84   map event list     -> .Lb48 (the only constant slot)
@ Slot 3  OvlFunc_8c   read after slot 4
@ Slot 4  OvlFunc_e4   map objects        -> 0xAA: .Lddc, 0xAB: .Le54, else .Ld10
@ Slot 5  OvlFunc_80   interactions       -> none (returns 0)
@
@ The save bits here come in a matched pair, 0x8FB and 0x8FC, which record
@ WHICH OF TWO ROUTES the player took. The map-load entry reads them back and
@ writes a destination into ewram_240+0x240 and a mode into +0x242, so leaving
@ this map sends the player somewhere that depends on how they arrived.
@ OvlFunc_2f8 clears both, resetting the choice.
@ ============================================================================

@ Trigger: loads map 0x1A via slot 0x0D.
.thumb_func_start OvlFunc_30
	push	{lr}
	mov	r0, #0xd
	mov	r1, #0x1a
	bl	__Func_91f14
	pop	{r0}
	bx	r0
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
	ldr	r3, =0xaa
	cmp	r2, r3
	bne	.L58
	ldr	r0, =.La40
	b	.L64
.L58:
	ldr	r3, =0xab
	cmp	r2, r3
	bne	.L62
	ldr	r0, =.Lad0
	b	.L64
.L62:
	ldr	r0, =.L998
.L64:
	pop	{r1}
	bx	r1
.func_end OvlFunc_40

@ Slot 5: interaction table -- none.
.thumb_func_start OvlFunc_80
	mov	r0, #0
	bx	lr
.func_end OvlFunc_80

@ Slot 2: map event list. Shared by all three areas.
.thumb_func_start OvlFunc_84
	ldr	r0, =.Lb48
	bx	lr
.func_end OvlFunc_84

@ Slot 3: read after slot 4.
@ Area 0xAA -> .Lba8. Area 0xA9 splits on save bit 0x96F -- .Lc98 once set,
@ .Lc50 before. Anything else -> .Lb90. Note the area tested here (0xA9) is
@ NOT one of the two slot 4 distinguishes (0xAA, 0xAB), so the two slots key on
@ different areas; they are not simply parallel.
.thumb_func_start OvlFunc_8c
	push	{lr}
	ldr	r3, =ewram_240
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xaa
	cmp	r2, r3
	bne	.La4
	ldr	r0, =.Lba8
	b	.Lbe
.La4:
	ldr	r3, =0xa9
	cmp	r2, r3
	bne	.Lbc
	ldr	r0, =0x96f
	bl	__Func_79338
	cmp	r0, #0
	beq	.Lb8
	ldr	r0, =.Lc98
	b	.Lbe
.Lb8:
	ldr	r0, =.Lc50
	b	.Lbe
.Lbc:
	ldr	r0, =.Lb90
.Lbe:
	pop	{r1}
	bx	r1
.func_end OvlFunc_8c

@ Slot 4: the map object table, one per area.
.thumb_func_start OvlFunc_e4
	push	{lr}
	ldr	r3, =ewram_240
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xaa
	cmp	r2, r3
	bne	.Lfc
	ldr	r0, =.Lddc
	b	.L108
.Lfc:
	ldr	r3, =0xab
	cmp	r2, r3
	bne	.L106
	ldr	r0, =.Le54
	b	.L108
.L106:
	ldr	r0, =.Ld10
.L108:
	pop	{r1}
	bx	r1
.func_end OvlFunc_e4

@ Slot 0: map-load entry.
@
@ Save bit 0x89F stages a destination up front -- ewram_240+0x1C4 = 0x69 and
@ +0x1C6 = 0x0A -- regardless of area.
@
@ Area 0xA9:
@   - save bit 0x897 teleports slot 0x0A to the origin with Func_923e4,
@   - entrance 3 reads the route pair back: bit 0x8FB writes mode 1 and bit
@     0x8FC mode 5 into ewram_240+0x242, with the area id going to +0x240, then
@     clears save bit 0x12F,
@   - entrance 1 SETS 0x8FB, and unless 0x96F is already set repaints a 2x1
@     attribute cell from (6, 0) to (0x1B, 8),
@   - entrance 5 sets 0x8FC.
@   So the two entrances record which way in the player came, and entrance 3 --
@   the exit -- turns that into where they go next.
@
@ Area 0xAA: five slots are put into standing animations (8, 9, 0x0B into 4;
@ 0x0A, 0x0C into 3), slot 0x0F gets motion rate 0x19999 at +0x1C, and a
@ 0x66 x 0x38 attribute block is copied from (0x6C, 0x26).
.thumb_func_start OvlFunc_124
	push	{r5, r6, lr}
	ldr	r0, =0x89f
	sub	sp, #8
	bl	__Func_79338
	cmp	r0, #0
	beq	.L148
	ldr	r1, =ewram_240
	mov	r0, #0xe2
	ldr	r3, =0x69
	lsl	r0, #1
	add	r2, r1, r0
	strh	r3, [r2]
	mov	r3, #0xe3
	lsl	r3, #1
	add	r2, r1, r3
	mov	r3, #0xa
	strh	r3, [r2]
.L148:
	ldr	r5, =ewram_240
	mov	r0, #0xe0
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r6, [r3, r2]
	ldr	r3, =0xa9
	cmp	r6, r3
	bne	.L200
	ldr	r0, =0x897
	bl	__Func_79338
	cmp	r0, #0
	beq	.L16e
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
.L16e:
	mov	r0, #0xe1
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L1b6
	ldr	r0, =0x8fb
	bl	__Func_79338
	cmp	r0, #0
	beq	.L196
	mov	r0, #0x90
	lsl	r0, #2
	add	r3, r5, r0
	strh	r6, [r3]
	ldr	r3, =0x242
	add	r2, r5, r3
	mov	r3, #1
	strh	r3, [r2]
.L196:
	ldr	r0, =0x8fc
	bl	__Func_79338
	cmp	r0, #0
	beq	.L1b0
	mov	r0, #0x90
	lsl	r0, #2
	add	r3, r5, r0
	strh	r6, [r3]
	ldr	r3, =0x242
	add	r2, r5, r3
	mov	r3, #5
	strh	r3, [r2]
.L1b0:
	ldr	r0, =0x12f
	bl	__Func_79374
.L1b6:
	ldr	r5, =ewram_240
	mov	r0, #0xe1
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #1
	bne	.L1ea
	ldr	r0, =0x8fb
	bl	__Func_79358
	ldr	r0, =0x96f
	bl	__Func_79338
	cmp	r0, #0
	bne	.L1ea
	mov	r3, #8
	mov	r2, #0x1b
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #6
	mov	r1, #0
	mov	r2, #2
	mov	r3, #1
	bl	__Func_10704
.L1ea:
	mov	r0, #0xe1
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #5
	bne	.L24c
	ldr	r0, =0x8fc
	bl	__Func_79358
	b	.L24c
.L200:
	ldr	r3, =0xaa
	cmp	r6, r3
	bne	.L24c
	mov	r0, #8
	mov	r1, #4
	bl	__Func_924d4
	mov	r0, #9
	mov	r1, #4
	bl	__Func_924d4
	mov	r0, #0xa
	mov	r1, #3
	bl	__Func_924d4
	mov	r0, #0xb
	mov	r1, #4
	bl	__Func_924d4
	mov	r1, #3
	mov	r0, #0xc
	bl	__Func_924d4
	mov	r0, #0xf
	bl	__Func_92054
	ldr	r3, =0x19999
	mov	r2, #0x38
	str	r3, [r0, #0x1c]
	mov	r3, #0x66
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x6c
	mov	r1, #0x26
	mov	r2, #1
	mov	r3, #1
	bl	__Func_10704
.L24c:
	mov	r0, #0
	add	sp, #8
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_124

@ CrossThreshold
@ Takes no arguments. Clears the player's flag byte +0x55, plays sound 0x9E, and
@ animates a doorway with two Func_10424 metatile copies four frames apart --
@ source rows 0x42 then 0x44 into the 8x2 block at (0x24, 0x47). Then
@ Func_92208 nudges the player by (3, -0x10) and the interaction target
@ halfword at [iwram_1ebc]+0x16C is left pending as the next message.
.thumb_func_start OvlFunc_288
	push	{r5, r6, lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xb6
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0
	sub	sp, #8
	mov	r2, #0
	ldrsh	r6, [r3, r2]
	bl	__Func_92054
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #0x9e
	bl	__Func_f9080
	mov	r5, #2
	mov	r1, #0x24
	mov	r2, #0x47
	mov	r3, #8
	mov	r0, #0x42
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #4
	bl	__Func_30f8
	mov	r3, #8
	mov	r1, #0x24
	mov	r2, #0x47
	mov	r0, #0x44
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_10424
	mov	r0, #4
	bl	__Func_30f8
	mov	r2, #0x10
	mov	r1, #3
	neg	r2, r2
	mov	r0, #0
	bl	__Func_92208
	mov	r0, r6
	bl	__Func_91e9c
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_288

@ ResetRouteChoice
@ Takes no arguments. Sound 0x7B, then clears BOTH route bits 0x8FB and 0x8FC
@ so the next pass through records afresh, and leaves the interaction target
@ pending as the next message.
.thumb_func_start OvlFunc_2f8
	push	{r5, lr}
	ldr	r3, =iwram_1ebc
	mov	r2, #0xb6
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0x7b
	mov	r2, #0
	ldrsh	r5, [r3, r2]
	bl	__Func_f9080
	ldr	r0, =0x8fb
	bl	__Func_79374
	ldr	r0, =0x8fc
	bl	__Func_79374
	mov	r0, r5
	bl	__Func_91e9c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_2f8

@ Cutscene: a short staged scene, roughly 60 instructions.
.thumb_func_start OvlFunc_334
	push	{r5, lr}
	sub	sp, #8
	bl	__Func_916b0
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #8
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #9
	lsl	r1, #9
	lsl	r2, #8
	bl	__Func_92064
	mov	r2, #0xc0
	mov	r0, #8
	mov	r1, #0x88
	lsl	r2, #1
	bl	__Func_9218c
	mov	r2, #0xc0
	mov	r0, #9
	mov	r1, #0x98
	lsl	r2, #1
	bl	__Func_921c4
	mov	r1, #0x80
	mov	r0, #8
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #9
	lsl	r1, #7
	bl	__Func_92adc
	mov	r0, #8
	mov	r1, #1
	bl	__Func_924d4
	mov	r3, #0x1b
	str	r3, [sp, #4]
	mov	r5, #7
	mov	r0, #6
	mov	r1, #0x1b
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	mov	r3, #0x1a
	str	r3, [sp, #4]
	mov	r0, #9
	mov	r1, #0x1a
	mov	r2, #2
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	bl	__Func_91750
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_334

@ Cutscene: the overlay's main scene, roughly 350 instructions.
@ Two actors are placed and walked into position (Func_923e4 then Func_921c4 at
@ speed 0x19999/0xCCCC), then a conversation runs through eleven Func_93040
@ lines from message base 0x2654, with Func_92adc turns between beats,
@ Func_9259c / Func_925cc formation changes, and two Func_93874 effect
@ sequences. It ends on a Func_93054 question whose answer is checked against a
@ save bit.
.thumb_func_start OvlFunc_3c4
	push	{r5, lr}
	sub	sp, #8
	bl	__Func_916b0
	mov	r0, #0
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__Func_92064
	mov	r2, #0xdb
	mov	r0, #0
	mov	r1, #0x78
	lsl	r2, #1
	bl	__Func_921c4
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r0, #0
	bl	__Func_92054
	cmp	r0, #0
	beq	.L402
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0xb
	bl	__Func_923e4
.L402:
	mov	r0, #1
	bl	__Func_30f8
	mov	r0, #0xb
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__Func_92064
	mov	r0, #0xb
	mov	r1, #0x6c
	ldr	r2, =0x1af
	bl	__Func_921c4
	mov	r1, #0xd0
	mov	r0, #0xb
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r1, #0x80
	mov	r0, #0xb
	lsl	r1, #1
	mov	r2, #0x14
	bl	__Func_937b8
	mov	r1, #0xd0
	mov	r0, #0xb
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r1, #0xd0
	mov	r0, #0xb
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_92adc
	mov	r2, #0x14
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_92adc
	mov	r1, #2
	mov	r0, #0xb
	bl	__Func_9259c
	ldr	r0, =0x2654
	bl	__Func_92b94
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_93040
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #8
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #8
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r2, #0xd2
	mov	r0, #0xb
	mov	r1, #0x84
	lsl	r2, #1
	bl	__Func_921c4
	mov	r1, #0xd0
	mov	r0, #0xb
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r1, #0xe0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0xd0
	mov	r0, #0xb
	mov	r1, #0x8a
	lsl	r2, #1
	bl	__Func_921c4
	mov	r1, #0xb0
	mov	r2, #0xa
	mov	r0, #0xb
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #0xb
	mov	r1, #2
	bl	__Func_9259c
	mov	r2, #0x28
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #8
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_93040
	mov	r1, #0x80
	mov	r2, #0x14
	mov	r0, #9
	lsl	r1, #1
	bl	__Func_937b8
	mov	r0, #9
	mov	r1, #2
	bl	__Func_925cc
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_92adc
	mov	r2, #0xd2
	lsl	r2, #1
	mov	r1, #0x90
	mov	r0, #0xb
	bl	__Func_921c4
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #9
	mov	r1, #2
	bl	__Func_925cc
	mov	r2, #0x14
	mov	r0, #9
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #0x81
	mov	r0, #9
	lsl	r1, #1
	bl	__Func_93874
	mov	r0, #9
	mov	r1, #3
	bl	__Func_9259c
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r0, #0xb
	lsl	r1, #7
	mov	r2, #0x14
	bl	__Func_92adc
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_93054
	mov	r0, #0x9b
	lsl	r0, #4
	bl	__Func_79338
	cmp	r0, #0
	beq	.L5a8
	mov	r1, #0xd0
	mov	r2, #0x28
	mov	r0, #0xb
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0xb
	bl	__Func_93874
	mov	r0, #0x28
	bl	__Func_9163c
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	b	.L5b8
.L5a8:
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.L5b8:
	mov	r1, #0xa0
	mov	r0, #0xb
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_93040
	mov	r1, #0x80
	mov	r0, #0xb
	lsl	r1, #1
	mov	r2, #0x28
	bl	__Func_937b8
	mov	r1, #0xb0
	mov	r0, #0xb
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r2, #0xd0
	mov	r0, #0xb
	mov	r1, #0x8a
	lsl	r2, #1
	bl	__Func_921c4
	mov	r1, #0xb0
	mov	r2, #0x14
	mov	r0, #0xb
	lsl	r1, #8
	bl	__Func_92adc
	mov	r0, #8
	mov	r1, #2
	bl	__Func_9259c
	mov	r2, #0xa
	mov	r0, #8
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #0x81
	mov	r0, #0xb
	lsl	r1, #1
	bl	__Func_93874
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0x14
	mov	r0, #0xb
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #2
	mov	r0, #9
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r1, #0xe0
	mov	r2, #0xa
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_92adc
	mov	r1, #1
	mov	r0, #9
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r2, #0xa
	mov	r0, #9
	mov	r1, #0
	bl	__Func_93040
	mov	r0, #8
	mov	r1, #2
	bl	__Func_925cc
	mov	r2, #0x14
	mov	r0, #8
	mov	r1, #0
	bl	__Func_93040
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_925cc
	mov	r0, #0x14
	bl	__Func_9163c
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_93040
	mov	r1, #0xa0
	mov	r0, #0xb
	lsl	r1, #7
	mov	r2, #0xa
	bl	__Func_92adc
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r0, #0xb
	mov	r1, #2
	bl	__Func_924d4
	mov	r0, #0
	bl	__Func_92054
	cmp	r0, #0
	beq	.L6ce
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0xb
	bl	__Func_92128
.L6ce:
	mov	r0, #0xb
	bl	__Func_923c4
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__Func_923e4
	mov	r3, #0x1b
	str	r3, [sp, #4]
	mov	r5, #7
	mov	r0, #6
	mov	r1, #0x1b
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_10704
	mov	r3, #0x1a
	str	r3, [sp, #4]
	mov	r1, #0x1a
	mov	r2, #2
	mov	r3, #1
	mov	r0, #9
	str	r5, [sp]
	bl	__Func_10704
	ldr	r0, =0x89f
	bl	__Func_79358
	bl	__Func_91750
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_3c4

@ Cutscene: a shorter follow-up scene, roughly 50 instructions.
.thumb_func_start OvlFunc_730
	push	{lr}
	bl	__Func_916b0
	ldr	r0, =0x89f
	bl	__Func_79338
	cmp	r0, #0
	beq	.L748
	ldr	r0, =0x2668
	bl	__Func_92b94
	b	.L772
.L748:
	ldr	r0, =0x264e
	bl	__Func_92b94
	mov	r1, #0
	mov	r0, #9
	bl	__Func_92c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_91c7c
	cmp	r0, #0
	bne	.L77c
	mov	r0, #9
	mov	r1, #0
	bl	__Func_92f84
	mov	r0, #9
	mov	r1, #4
	bl	__Func_92548
.L772:
	mov	r0, #9
	mov	r1, #0
	bl	__Func_92f84
	b	.L794
.L77c:
	ldr	r3, =iwram_1ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #2
	strh	r3, [r2]
	mov	r0, #9
	mov	r1, #0
	bl	__Func_92f84
.L794:
	bl	__Func_91750
	pop	{r0}
	bx	r0
.func_end OvlFunc_730

@ Trigger: a short handler closing the overlay's scene set.
.thumb_func_start OvlFunc_7ac
	push	{r5, lr}
	bl	__Func_916b0
	ldr	r0, =0x266d
	bl	__Func_92b94
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_93040
	mov	r5, #0
.L7c4:
	mov	r1, #0
	mov	r0, #0xa
	bl	__Func_92950
	mov	r0, #0xa
	bl	__Func_92054
	mov	r1, #1
	bl	__Func_c528
	mov	r0, #4
	bl	__Func_30f8
	mov	r1, #0xf
	mov	r0, #0xa
	bl	__Func_92950
	mov	r0, #0xa
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	add	r5, #1
	mov	r0, #4
	bl	__Func_30f8
	cmp	r5, #5
	bls	.L7c4
	mov	r5, #0
.L800:
	mov	r1, #0
	mov	r0, #0xa
	bl	__Func_92950
	mov	r0, #0xa
	bl	__Func_92054
	mov	r1, #1
	bl	__Func_c528
	mov	r0, #2
	bl	__Func_30f8
	mov	r1, #0xf
	mov	r0, #0xa
	bl	__Func_92950
	mov	r0, #0xa
	bl	__Func_92054
	mov	r1, #0
	bl	__Func_c528
	add	r5, #1
	mov	r0, #2
	bl	__Func_30f8
	cmp	r5, #0xb
	bls	.L800
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0xa
	bl	__Func_923e4
	ldr	r0, =0x897
	bl	__Func_79358
	bl	__Func_91750
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_7ac

	.section .data

	.incbin "overlays/rom_7ec968/orig.bin", 0x974, (0x998-0x974)
.L998:
	.incbin "overlays/rom_7ec968/orig.bin", 0x998, (0xa40-0x998)
.La40:
	.incbin "overlays/rom_7ec968/orig.bin", 0xa40, (0xad0-0xa40)
.Lad0:
	.incbin "overlays/rom_7ec968/orig.bin", 0xad0, (0xb48-0xad0)
.Lb48:
	.incbin "overlays/rom_7ec968/orig.bin", 0xb48, (0xb90-0xb48)
.Lb90:
	.incbin "overlays/rom_7ec968/orig.bin", 0xb90, (0xba8-0xb90)
.Lba8:
	.incbin "overlays/rom_7ec968/orig.bin", 0xba8, (0xc50-0xba8)
.Lc50:
	.incbin "overlays/rom_7ec968/orig.bin", 0xc50, (0xc98-0xc50)
.Lc98:
	.incbin "overlays/rom_7ec968/orig.bin", 0xc98, (0xd10-0xc98)
.Ld10:
	.incbin "overlays/rom_7ec968/orig.bin", 0xd10, (0xddc-0xd10)
.Lddc:
	.incbin "overlays/rom_7ec968/orig.bin", 0xddc, (0xe54-0xddc)
.Le54:
	.incbin "overlays/rom_7ec968/orig.bin", 0xe54

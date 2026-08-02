	.include "macros.inc"

@ Slot 3: read after slot 4.
@ Area 0xAA -> .Lba8. Area 0xA9 splits on save bit 0x96F -- .Lc98 once set,
@ .Lc50 before. Anything else -> .Lb90. Note the area tested here (0xA9) is
@ NOT one of the two slot 4 distinguishes (0xAA, 0xAB), so the two slots key on
@ different areas; they are not simply parallel.
.thumb_func_start OvlFunc_963_200808c
	push	{lr}
	ldr	r3, =gState
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
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb8
	ldr	r0, =.Lc98
	b	.Lbe
.Lb8:
	ldr	r0, =gOvl_02008c50
	b	.Lbe
.Lbc:
	ldr	r0, =.Lb90
.Lbe:
	pop	{r1}
	bx	r1
.func_end OvlFunc_963_200808c

@ Slot 4: the map object table, one per area.
.thumb_func_start OvlFunc_963_20080e4
	push	{lr}
	ldr	r3, =gState
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
.func_end OvlFunc_963_20080e4

@ Slot 0: map-load entry.
@
@ Save bit 0x89F stages a destination up front -- ewram_240+0x1C4 = 0x69 and
@ +0x1C6 = 0x0A -- regardless of area.
@
@ Area 0xA9:
@   - save bit 0x897 teleports slot 0x0A to the origin with MapActor_SetPos,
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
.thumb_func_start OvlFunc_963_2008124
	push	{r5, r6, lr}
	ldr	r0, =0x89f
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	beq	.L148
	ldr	r1, =gState
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
	ldr	r5, =gState
	mov	r0, #0xe0
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r6, [r3, r2]
	ldr	r3, =0xa9
	cmp	r6, r3
	bne	.L200
	ldr	r0, =0x897
	bl	__GetFlag
	cmp	r0, #0
	beq	.L16e
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L16e:
	mov	r0, #0xe1
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #3
	bne	.L1b6
	ldr	r0, =0x8fb
	bl	__GetFlag
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
	bl	__GetFlag
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
	bl	__ClearFlag
.L1b6:
	ldr	r5, =gState
	mov	r0, #0xe1
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #1
	bne	.L1ea
	ldr	r0, =0x8fb
	bl	__SetFlag
	ldr	r0, =0x96f
	bl	__GetFlag
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
	bl	__Func_8010704
.L1ea:
	mov	r0, #0xe1
	lsl	r0, #1
	add	r3, r5, r0
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #5
	bne	.L24c
	ldr	r0, =0x8fc
	bl	__SetFlag
	b	.L24c
.L200:
	ldr	r3, =0xaa
	cmp	r6, r3
	bne	.L24c
	mov	r0, #8
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r0, #9
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0xc
	bl	__MapActor_SetAnim
	mov	r0, #0xf
	bl	__MapActor_GetActor
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
	bl	__Func_8010704
.L24c:
	mov	r0, #0
	add	sp, #8
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_963_2008124

@ CrossThreshold
@ Takes no arguments. Clears the player's flag byte +0x55, plays sound 0x9E, and
@ animates a doorway with two CopyMapTiles metatile copies four frames apart --
@ source rows 0x42 then 0x44 into the 8x2 block at (0x24, 0x47). Then
@ Func_92208 nudges the player by (3, -0x10) and the interaction target
@ halfword at [iwram_1ebc]+0x16C is left pending as the next message.
.thumb_func_start OvlFunc_963_2008288
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xb6
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0
	sub	sp, #8
	mov	r2, #0
	ldrsh	r6, [r3, r2]
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #0x9e
	bl	__PlaySound
	mov	r5, #2
	mov	r1, #0x24
	mov	r2, #0x47
	mov	r3, #8
	mov	r0, #0x42
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #4
	bl	__WaitFrames
	mov	r3, #8
	mov	r1, #0x24
	mov	r2, #0x47
	mov	r0, #0x44
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r0, #4
	bl	__WaitFrames
	mov	r2, #0x10
	mov	r1, #3
	neg	r2, r2
	mov	r0, #0
	bl	__Func_8092208
	mov	r0, r6
	bl	__Func_8091e9c
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_963_2008288


	.include "macros.inc"

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
.thumb_func_start OvlFunc_900_20081e4
	push	{r5, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x49
	str	r2, [r3]
	ldr	r3, =gState
	sub	r2, #0x47
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #2
	bne	.L20a
	ldr	r0, =0x12f
	bl	__ClearFlag
	b	.L24c
.L20a:
	cmp	r3, #0xa
	bne	.L220
	mov	r0, #8
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r2, [r0]
	mov	r3, #0x14
	orr	r3, r2
	strb	r3, [r0]
	b	.L24c
.L220:
	mov	r0, #8
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	mov	r5, #0x14
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #9
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #0xa
	bl	__MapActor_GetActor
	add	r0, #0x59
	ldrb	r3, [r0]
	orr	r5, r3
	strb	r5, [r0]
.L24c:
	mov	r0, #0
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_900_20081e4

	.section .data
	.global gOvl_0200835c
	.global MapEntrance_ARRAY_919__0200835c
	.global .L3bc
	.global .L3ec
	.global MapEntrance_ARRAY_937__020084a0
	.global gOvl_020082d0
	.global MapEntrance_ARRAY_900__020082d0
gOvl_020082d0:
MapEntrance_ARRAY_900__020082d0:
	.incbin "overlays/rom_797740/orig.bin", 0x2d0, (0x348-0x2d0)
	.global gOvl_02008348
gOvl_02008348:
	.incbin "overlays/rom_797740/orig.bin", 0x348, (0x35c-0x348)
gOvl_0200835c:
MapEntrance_ARRAY_919__0200835c:
	.incbin "overlays/rom_797740/orig.bin", 0x35c, (0x3bc-0x35c)
.L3bc:
	.incbin "overlays/rom_797740/orig.bin", 0x3bc, (0x3ec-0x3bc)
.L3ec:
	.incbin "overlays/rom_797740/orig.bin", 0x3ec, (0x4a0-0x3ec)
MapEntrance_ARRAY_937__020084a0:
	.incbin "overlays/rom_797740/orig.bin", 0x4a0
